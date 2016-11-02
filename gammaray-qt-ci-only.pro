# qmake -> cmake wrapper for the Qt CI system
#
# DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING!
#
# This only supports a tiny subset of the CMake build system,
# hardcoded for the needs of integration with the Qt CI system.
#
# If you are building GammaRay yourself, please use CMake as
# described in Install. txt

message("QMake wrapper for the GammaRay build system is only supported for the Qt CI.")
message("If you are seeing this and you are not the Qt CI, please refer to Install.txt!")

TEMPLATE = aux

gammaray_build.target = gammaray_build
gammaray_build.commands = \
    mkdir -p build; \
    cd build; \
    cmake -DCMAKE_INSTALL_PREFIX=$$[QT_INSTALL_PREFIX] -DCMAKE_PREFIX_PATH=$$[QT_INSTALL_PREFIX] $$PWD; \
    $(MAKE) $(MAKEFLAGS)

gammaray_online_docs.target = docs
gammaray_online_docs.commands = \
    mkdir -p build; \
    cd build; \
    cmake -DCMAKE_INSTALL_PREFIX=$$[QT_INSTALL_PREFIX] -DCMAKE_PREFIX_PATH=$$[QT_INSTALL_PREFIX] $$PWD; \
    $(MAKE) online-docs

gammaray_install.target = install
gammaray_install.commands = \
    mkdir -p build; \
    cd build; \
    cmake -DCMAKE_INSTALL_PREFIX=$$[QT_INSTALL_PREFIX] -DCMAKE_PREFIX_PATH=$$[QT_INSTALL_PREFIX] $$PWD; \
    $(MAKE) $(MAKEFLAGS); \
    $(MAKE) install

QMAKE_EXTRA_TARGETS += gammaray_build gammaray_online_docs gammaray_install

TARGET = gammaray
PRE_TARGETDEPS += gammaray_build
