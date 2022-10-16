###############################################################################
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Renato Araujo Oliveira Filho <renato.araujo@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

from conans import ConanFile, CMake, tools

class ECMConan(ConanFile):
    name = "ECM"
    version = "5.64.0"
    license = ""
    url = "git://anongit.kde.org/extra-cmake-modules.git"
    description = "The Extra CMake Modules package,."
    generators = "cmake"

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        git.checkout("v%s"%self.version)

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
