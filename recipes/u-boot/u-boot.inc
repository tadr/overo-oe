DESCRIPTION = "U-Boot - the Universal Boot Loader"
HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
PRIORITY = "optional"
LICENSE = "GPL"
PROVIDES = "virtual/bootloader"

DEPENDS = "mtd-utils"

PACKAGE_ARCH = "${MACHINE_ARCH}"
PARALLEL_MAKE=""

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX}"

UBOOT_MACHINE ?= "${MACHINE}_config"
UBOOT_IMAGE ?= "u-boot-${MACHINE}-${PV}-${PR}.bin"
UBOOT_SYMLINK ?= "u-boot-${MACHINE}.bin"

do_compile () {
	unset LDFLAGS
	unset CFLAGS
	unset CPPFLAGS
	oe_runmake ${UBOOT_MACHINE}
	oe_runmake all
	oe_runmake env
}

do_deploy () {
	install -d ${DEPLOY_DIR_IMAGE}
	install ${S}/u-boot.bin ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
	package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}

	cd ${DEPLOY_DIR_IMAGE}
	rm -f ${UBOOT_SYMLINK}
	ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
	package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}
}
do_deploy[dirs] = "${S}"
addtask deploy before do_build after do_compile

do_stage() {
	install -d ${STAGING_BINDIR_NATIVE}
	install -m 755 tools/mkimage ${STAGING_BINDIR_NATIVE}/
}

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${S}/tools/env/fw_printenv ${D}${sbindir}
	ln -s /usr/sbin/fw_printenv ${D}${sbindir}/fw_setenv
	install -d ${D}${sysconfdir}
	install -m 0644 ${S}/tools/env/fw_env.config ${D}${sysconfdir}
}

PACKAGES = "u-boot-tools-env"
FILES_u-boot-tools-env = "${sbindir}/fw_printenv"
FILES_u-boot-tools-env += "${sbindir}/fw_setenv"
FILES_u-boot-tools-env += "${sysconfdir}/fw_env.config"
INSANE_SKIP_u-boot-tools-env = True

PACKAGES += "u-boot-tools-env-dbg"
FILES_u-boot-tools-env-dbg = "${sbindir}/.debug/fw_printenv"

