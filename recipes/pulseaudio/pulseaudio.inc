DESCRIPTION = "Pulseaudio is a sound server for Linux and Unix-like operating systems."
HOMEPAGE = "http://www.pulseaudio.org"
AUTHOR = "Lennart Poettering"
SECTION = "libs/multimedia"
LICENSE = "LGPL"
DEPENDS = "bluez4 libatomics-ops liboil avahi libsamplerate0 libsndfile1 libtool hal virtual/libx11"
# optional
DEPENDS += "alsa-lib glib-2.0 dbus consolekit hal openssl"
INC_PR = "r8"

SRC_URI = "http://0pointer.de/lennart/projects/pulseaudio/pulseaudio-${PV}.tar.gz \
           file://gcc4-compile-fix.patch;patch=1 \
           file://volatiles.04_pulse"

inherit autotools pkgconfig

EXTRA_OECONF = "\
		--disable-lynx \
		--enable-bluez \
		--disable-polkit \
		--with-x \
		--without-jack \
		--with-glib \
		--with-alsa \
		--with-oss \
		--with-hal \
"

PARALLEL_MAKE = ""

export TARGET_PFPU = "${TARGET_FPU}"

# TODO: Use more fine granular version
#OE_LT_RPATH_ALLOW=":${libdir}/pulse-0.9:"
OE_LT_RPATH_ALLOW = "any"
OE_LT_RPATH_ALLOW[export]="1"

do_stage() {
	autotools_stage_all
}

do_install_append() {
	install -d ${D}${sysconfdir}/default/volatiles
	install -m 0644 ${WORKDIR}/volatiles.04_pulse  ${D}${sysconfdir}/default/volatiles/04_pulse
	
	if [ "x${TARGET_PFPU}" == "xsoft" ] ; then 
	     sed -i -e s:\;\ resample-method\ =\ sinc-fastest:resample-method\ =\ trivial: ${D}${sysconfdir}/pulse/daemon.conf
	fi
}

PACKAGES =+ "libpulsecore libpulsecommon libpulsedsp libpulse libpulse-simple libpulse-browse libpulse-mainloop-glib \
             pulseaudio-server pulseaudio-misc pulseaudio-gconf-helper"

#upgrade path:
RREPLACES_pulseaudio-server = "libpulse-bin libpulse-conf"

PACKAGES_DYNAMIC = "pulseaudio-lib* pulseaudio-module* libpulse-lib* libpulse-module* "

FILES_libpulsecore = "${libdir}/libpulsecore*.so"
FILES_libpulsecommon = "${libdir}/libpulsecommon*.so"
FILES_libpulsedsp = "${libdir}/libpulsedsp*.so"
FILES_libpulse = "${libdir}/libpulse.so.*"
FILES_libpulse-simple = "${libdir}/libpulse-simple.so.*"
FILES_libpulse-browse = "${libdir}/libpulse-browse.so.*"
FILES_libpulse-mainloop-glib = "${libdir}/libpulse-mainloop-glib.so.*"
				   	 
FILES_${PN}-dbg += "${libexecdir}/pulse/.debug \
                    ${libdir}/pulse-${PV}/modules/.debug"
FILES_${PN}-dev += "${libdir}/pulse-${PV}/modules/*.la"		    
FILES_${PN}-conf = "${sysconfdir}"
FILES_${PN}-bin = "${bindir}/* \
                   ${sysconfdir}/default/volatiles/volatiles.04_pulse"
FILES_${PN}-server = "${bindir}/pulseaudio ${bindir}/start-* ${sysconfdir} ${bindir}/pactl"
FILES_${PN}-gconf-helper = "${libexecdir}/pulse/gconf-helper"
FILES_${PN}-misc = "${bindir}/*"

# Allow the pulseaudio package to be created empty as a placeholder (-dbg and -dev depend on it)
ALLOW_EMPTY_${PN} = "1"

CONFFILES_pulseaudio-server = "\ 
  ${sysconfdir}/pulse/default.pa \
  ${sysconfdir}/pulse/daemon.conf \
  ${sysconfdir}/pulse/client.conf \
  "

pkg_postinst_${PN}-server() {
        # can't do this offline
        if [ "x$D" != "x" ]; then
                exit 1
        fi
        grep -q pulse: /etc/group || addgroup pulse
        grep -q pulse: /etc/passwd || \
            adduser --disabled-password --home=/var/run/pulse --system \
                    --ingroup pulse --no-create-home -g "Pulse audio daemon" pulse
        addgroup pulse audio
        /etc/init.d/populate-volatile.sh update
}

pkg_postrm_${PN}-server() {
        deluser pulse || true
}

python populate_packages_prepend() {
        #bb.data.setVar('PKG_pulseaudio', 'pulseaudio', d)

        plugindir = bb.data.expand('${libdir}/pulse-${PV}/modules/', d)
        do_split_packages(d, plugindir, '^module-(.*)\.so$', 'pulseaudio-module-%s', 'PulseAudio module for %s', extra_depends='' )
        do_split_packages(d, plugindir, '^lib(.*)\.so$', 'pulseaudio-lib-%s', 'PulseAudio library for %s', extra_depends='' )
}

RDEPENDS_pulseaudio-module-console-kit =+ "consolekit"
RDEPENDS_pulseaudio-module-hal_detect =+ "hal"
RDEPENDS_pulseaudio-server = " \
    pulseaudio-module-device-restore \
    pulseaudio-module-stream-restore \
    pulseaudio-module-card-restore \
    pulseaudio-module-augment-properties \
    pulseaudio-module-detect \
    pulseaudio-module-alsa-sink \
    pulseaudio-module-alsa-source \
    pulseaudio-module-alsa-card \
    pulseaudio-module-native-protocol-unix \
    pulseaudio-module-hal-detect \
    pulseaudio-module-default-device-restore \
    pulseaudio-module-rescue-streams \
    pulseaudio-module-always-sink \
    pulseaudio-module-suspend-on-idle \
    pulseaudio-module-console-kit \
    pulseaudio-module-position-event-sounds \
    pulseaudio-module-cork-music-on-phone "
