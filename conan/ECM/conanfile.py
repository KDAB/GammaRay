###############################################################################
##  This file is part of GammaRay, the Qt application inspection and
##  manipulation tool.
##
##  Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
##  Author: Renato Araujo Oliveira Filho <renato.araujo@kdab.com>
##
##  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
##  accordance with GammaRay Commercial License Agreement provided with the Software.
##
##  Contact info@kdab.com if any conditions of this licensing are not clear to you.
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

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
