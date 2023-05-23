SUMMARY = "GammaRay Qt introspection probe"
HOMEPAGE = "https://www.kdab.com/gammaray"

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://LICENSES/GPL-2.0-or-later.txt;md5=3d26203303a722dedc6bf909d95ba815"

inherit qt6-cmake

SRC_URI = "git://github.com/KDAB/GammaRay;branch=master"

SRCREV = "29dc942640d1666f6f11c13fa77d574ed004a450"

PV = "master_${SRCPV}"

DEPENDS = "qtdeclarative qtdeclarative-native"

S = "${WORKDIR}/git"

EXTRA_OECMAKE += " -DGAMMARAY_BUILD_UI=OFF -DQT_VERSION_MAJOR=6"

FILES_${PN} += "/usr/share/zsh/site-functions/_gammaray"

FILES_${PN}-dev += " \
    /usr/lib/cmake/* \
    /usr/mkspecs/modules/* \
"
FILES_${PN}-dbg += " \
    /usr/lib/.debug/* \
    /usr/lib/gammaray/*/*/.debug \
    /usr/lib/gammaray/*/*/styles/.debug \
"
