# implementation detail of the qmake -> cmake wrapper for the Qt CI system
#
# DO NOT USE THIS DIRECTLY!

TEMPLATE = aux

# convert path and command separators to platform format
SHELL_INSTALL_PREFIX = $$shell_path($$[QT_INSTALL_PREFIX])
SHELL_PWD = $$shell_path($$PWD/..)
SHELL_SEP = ;
win32: SHELL_SEP = &

# platform specific cmake arguments
win32-msvc*: CMAKE_PLATFORM_ARGS = -G\"NMake Makefiles\"
win32-g++:  CMAKE_PLATFORM_ARGS = -G\"MinGW Makefiles\"
!qtHaveModule(widgets): GAMMARAY_EXTRA_ARGS = -DGAMMARAY_BUILD_UI=OFF

gammaray_configure.target = gammaray_configure
gammaray_configure.commands = \
    ($$sprintf($$QMAKE_MKDIR_CMD, "build")) $$SHELL_SEP \
    cd build $$SHELL_SEP \
    cmake $$CMAKE_PLATFORM_ARGS \
        -DCMAKE_INSTALL_PREFIX=$$SHELL_INSTALL_PREFIX \
        -DCMAKE_PREFIX_PATH=$$SHELL_INSTALL_PREFIX \
        -DGAMMARAY_PROBE_ONLY_BUILD=TRUE \
        -DGAMMARAY_INSTALL_QT_LAYOUT=TRUE \
        $$GAMMARAY_EXTRA_ARGS $$SHELL_PWD

gammaray_configure_docs.target = gammaray_configure_docs
gammaray_configure_docs.commands = \
    ($$sprintf($$QMAKE_MKDIR_CMD, "build")) $$SHELL_SEP \
    cd build $$SHELL_SEP \
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
gammaray_online_docs.recurse = true
# doc build disabled temporarily until we can run this without requiring a full qtbase install,
# which the Qt CI doesn't provide at this point
#gammaray_online_docs.depends += gammaray_configure_docs
#gammaray_online_docs.commands = cd build $$SHELL_SEP $(MAKE) online-docs

gammaray_install.target = install
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
