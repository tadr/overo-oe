require opkg.inc

DEPENDS = "curl-native"
PR = "${INC_PR}"

SRC_URI += "file://opkg-libdir.patch;patch=1"

target_libdir := "${libdir}"

inherit native

# The nogpg version isn't getting much love and has an unused variable which trips up -Werror
do_configure_prepend() {
        sed -i -e s:-Werror::g ${S}/libopkg/Makefile.am
}


EXTRA_OECONF += "--with-opkglibdir=${target_libdir} --disable-gpg"