SUMMARY = "GammaRay Qt introspection probe"
HOMEPAGE = "https://www.kdab.com/gammaray"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE.GPL.txt;md5=2abfe5daa817fd4346b6d55293941415"

inherit cmake_qt5

SRC_URI = "git://github.com/KDAB/GammaRay;branch=master"

SRCREV = "1728dff87163c370fa6903a6d739d1a132b4c62f"
PV = "2.7.0+git${SRCPV}"

DEPENDS = "qtdeclarative"

S = "${WORKDIR}/git"

EXTRA_OECMAKE += " -DGAMMARAY_BUILD_UI=OFF"

FILES_${PN}-dev += " \
    /usr/lib/cmake/* \
    /usr/mkspecs/modules/* \
"
FILES_${PN}-dbg += " \
    /usr/lib/.debug/* \
    /usr/lib/gammaray/*/*/.debug \
    /usr/lib/gammaray/*/*/styles/.debug \
"

