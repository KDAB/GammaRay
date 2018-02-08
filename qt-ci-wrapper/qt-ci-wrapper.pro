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

gammaray_configure.target = gammaray_configure
gammaray_configure.commands = \
    ($$sprintf($$QMAKE_MKDIR_CMD, "build")) $$SHELL_SEP \
    cd build $$SHELL_SEP \
    cmake $$CMAKE_PLATFORM_ARGS \
        -DCMAKE_INSTALL_PREFIX=$$SHELL_INSTALL_PREFIX \
        -DCMAKE_PREFIX_PATH=$$SHELL_INSTALL_PREFIX \
        -DGAMMARAY_INSTALL_QT_LAYOUT=TRUE \
        -DGAMMARAY_BUILD_DOCS=FALSE \
        -DGAMMARAY_DISABLE_FEEDBACK=TRUE \
        -DGAMMARAY_BUILD_UI=FALSE \
        $$GAMMARAY_EXTRA_ARGS $$SHELL_PWD

gammaray_build.target = gammaray_build
gammaray_build.depends += gammaray_configure
gammaray_build.commands = cd build $$SHELL_SEP $(MAKE)

gammaray_test.target = check
gammaray_test.depends += gammaray_configure
gammaray_test.commands = cd build $$SHELL_SEP $(MAKE) test

gammaray_install.target = install
gammaray_install.commands = cd build $$SHELL_SEP $(MAKE) DESTDIR=$(INSTALL_ROOT) install

QMAKE_EXTRA_TARGETS += \
    gammaray_configure \
    gammaray_build \
    gammaray_test \
    gammaray_install

# build documentation directly, the Qt CI can't run CMake at this stage
QMAKE_DOCS = $$PWD/../docs/manual/gammaray-manual.qdocconf
load(qt_docs)
docs.commands = $(MAKE) -f $(MAKEFILE) html_docs && $(MAKE) -f $(MAKEFILE) qch_docs
QMAKE_EXTRA_TARGETS += docs

DOC_TARGETS = \
    install_html_docs uninstall_html_docs \
    install_qch_docs uninstall_qch_docs \
    install_docs uninstall_docs \
    qch_docs prepare_docs generate_docs \
    html_docs
QMAKE_EXTRA_TARGETS += $$DOC_TARGETS

# default target
TARGET = gammaray
PRE_TARGETDEPS += gammaray_build
