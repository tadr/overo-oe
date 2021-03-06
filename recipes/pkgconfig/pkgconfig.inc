SECTION = "console/utils"
DESCRIPTION = "pkg-config is a system for managing library \
compile/link flags that works with automake and autoconf. \
It replaces the ubiquitous *-config scripts you may have \
seen with a single tool."
HOMEPAGE = "http://pkg-config.freedesktop.org/wiki/"
LICENSE = "GPLv2+"
INC_PR = "r8"

SRC_URI = "http://pkgconfig.freedesktop.org/releases/pkg-config-${PV}.tar.gz \
           file://autofoo.patch \
           file://sysrootfix.patch \
           file://glibconfig-sysdefs.h"

S = "${WORKDIR}/pkg-config-${PV}/"

inherit autotools

acpaths = "-I ."
do_configure_prepend () {
	install -m 0644 ${WORKDIR}/glibconfig-sysdefs.h glib-1.2.*/
}

do_install_append() {
	install -d -m 0755 ${D}${libdir}/pkgconfig
}
