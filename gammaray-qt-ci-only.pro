# qmake -> cmake wrapper for the Qt CI system
#
# DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING!
#
# This only supports a tiny subset of the CMake build system,
# hardcoded for the needs of integration with the Qt CI system.
#
# If you are building GammaRay yourself, please use CMake as
# described in Install. txt

requires(linux:!android|win32)
requires(!cross_compile)

message("QMake wrapper for the GammaRay build system is only supported for the Qt CI.")
message("If you are seeing this and you are not the Qt CI, please refer to Install.txt!")

TEMPLATE = aux

gammaray_configure.target = gammaray_configure
gammaray_configure.commands = \
    mkdir -p build; \
    cd build; \
    cmake -DCMAKE_INSTALL_PREFIX=$$[QT_INSTALL_PREFIX] -DCMAKE_PREFIX_PATH=$$[QT_INSTALL_PREFIX] $$PWD

gammaray_build.target = gammaray_build
gammaray_build.depends += gammaray_configure
gammaray_build.commands = $(MAKE) -C build

gammaray_test.target = check
gammaray_test.depends += gammaray_configure
gammaray_test.commands = $(MAKE) -C build test

gammaray_online_docs.target = docs
gammaray_online_docs.depends += gammaray_configure
gammaray_online_docs.commands = $(MAKE) -C build online-docs

gammaray_install.target = install
gammaray_install.depends += gammaray_configure
gammaray_install.commands = $(MAKE) DESTDIR=$(INSTALL_ROOT) -C build install

QMAKE_EXTRA_TARGETS += \
    gammaray_configure \
    gammaray_build \
    gammaray_test \
    gammaray_online_docs \
    gammaray_install

TARGET = gammaray
PRE_TARGETDEPS += gammaray_build
