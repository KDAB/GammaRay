# qmake -> cmake wrapper for the Qt CI system
#
# DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING!
#
# This only supports a tiny subset of the CMake build system,
# hardcoded for the needs of integration with the Qt CI system.
#
# If you are building GammaRay yourself, please use CMake as
# described in Install. txt

requires(linux:!android|win32:!winphone:!winrt|osx)
requires(!integrity)
requires(!cross_compile)

win32: requires(!contains(QMAKE_HOST.version_string,  $$re_escape(8.1)))

message("QMake wrapper for the GammaRay build system is only supported for the Qt CI.")
message("If you are seeing this and you are not the Qt CI, please refer to Install.txt!")

# additional indirection is needed as the above requires conditions don't work correctly
# in aux templates, on some platforms
TEMPLATE = subdirs
SUBDIRS += qt-ci-wrapper

# forward non-built-in targets too
QMAKE_EXTRA_TARGETS += gammaray_build_docs gammaray_build_qch_docs
gammaray_build_docs.CONFIG = recursive
gammaray_build_docs.recurse_target = docs
gammaray_build_docs.target = docs
gammaray_build_qch_docs.target = qch_docs
gammaray_build_qch_docs.depends = docs

# add dummy qmake targets for doc build - only qch_docs triggers the build
DOC_TARGETS = \
    prepare_docs generate_docs html_docs \
    install_html_docs uninstall_html_docs \
    install_qch_docs uninstall_qch_docs \
    install_docs uninstall_docs

QMAKE_EXTRA_TARGETS += $$DOC_TARGETS
