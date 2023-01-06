# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

requires(linux:!android|win32:!winphone:!winrt|osx)
requires(!integrity)
requires(!cross_compile)

win32: requires(!contains(QMAKE_HOST.version_string,  $$re_escape(8.1)))

message("QMake wrapper for the GammaRay build system is only supported for the Qt CI.")
message("If you are seeing this and you are not the Qt CI, please refer to INSTALL.md!")

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
