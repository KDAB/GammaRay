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

# convert path and command separators to platform format
SHELL_INSTALL_PREFIX = $$shell_path($$[QT_INSTALL_PREFIX])
SHELL_PWD = $$shell_path($$PWD)
SHELL_SEP = ;
win32: SHELL_SEP = &

# platform specific cmake arguments
win32: CMAKE_PLATFORM_ARGS = -G\"NMake Makefiles\"

gammaray_configure.target = gammaray_configure
gammaray_configure.commands = \
    ($$sprintf($$QMAKE_MKDIR_CMD, "build")) $$SHELL_SEP \
    cd build $$SHELL_SEP \
    cmake $$CMAKE_PLATFORM_ARGS \
        -DCMAKE_INSTALL_PREFIX=$$SHELL_INSTALL_PREFIX \
        -DCMAKE_PREFIX_PATH=$$SHELL_INSTALL_PREFIX \
        -DGAMMARAY_PROBE_ONLY_BUILD=TRUE \
        -DGAMMARAY_INSTALL_QT_LAYOUT=TRUE \
        $$SHELL_PWD

gammaray_configure_docs.target = gammaray_configure_docs
gammaray_configure_docs.commands = \
    ($$sprintf($$QMAKE_MKDIR_CMD, "build")) $$SHELL_SEP \
    cd build $SHELL_SEP \
    cmake $$CMAKE_PLATFORM_ARGS \
        -DCMAKE_INSTALL_PREFIX=$$SHELL_INSTALL_PREFIX \
        -DCMAKE_PREFIX_PATH=$$SHELL_INSTALL_PREFIX \
        -DGAMMARAY_PROBE_ONLY_BUILD=FALSE \
        $$SHELL_PWD

gammaray_build.target = gammaray_build
gammaray_build.depends += gammaray_configure
gammaray_build.commands = cd build $$SHELL_SEP $(MAKE)

gammaray_test.target = check
gammaray_test.depends += gammaray_configure
gammaray_test.commands = cd build $$SHELL_SEP $(MAKE) test

gammaray_online_docs.target = docs
gammaray_online_docs.depends += gammaray_configure_docs
gammaray_online_docs.commands = cd build $$SHELL_SEP $(MAKE) online-docs

gammaray_install.target = install
gammaray_install.depends += gammaray_configure
gammaray_install.commands = cd build $$SHELL_SEP $(MAKE) DESTDIR=$(INSTALL_ROOT) install

QMAKE_EXTRA_TARGETS += \
    gammaray_configure \
    gammaray_configure_docs \
    gammaray_build \
    gammaray_test \
    gammaray_online_docs \
    gammaray_install

TARGET = gammaray
PRE_TARGETDEPS += gammaray_build
