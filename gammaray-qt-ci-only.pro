# qmake -> cmake wrapper for the Qt CI system
#
# DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING!
#
# This only supports a tiny subset of the CMake build system,
# hardcoded for the needs of integration with the Qt CI system.
#
# If you are building GammaRay yourself, please use CMake as
# described in Install. txt

requires(linux:!android|win32:!winphone:!winrt)
requires(!cross_compile)

message("QMake wrapper for the GammaRay build system is only supported for the Qt CI.")
message("If you are seeing this and you are not the Qt CI, please refer to Install.txt!")

# additional indirection is needed as the above requires conditions don't work correctly
# in aux templates, on some platforms
TEMPLATE = subdirs
SUBDIRS += qt-ci-wrapper

# forward non-built-in targets too
QMAKE_EXTRA_TARGETS += gammaray_build_docs
gammaray_build_docs.CONFIG = recursive
gammaray_build_docs.recurse_target = docs
gammaray_build_docs.target = docs
