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

class KSyntaxHighlightingConan(ConanFile):
    name = "KSyntaxHighlighting"
    version = "5.64.0"
    license = "https://cgit.kde.org/syntax-highlighting.git/plain/COPYING"
    url = "git://anongit.kde.org/syntax-highlighting.git"
    description = "This is a stand-alone implementation of the Kate syntax highlighting engine."
    generators = "cmake"

    def requirements(self):
        self.requires("qt/5.13.2@kdab/stable")
        self.requires("ECM/5.64.0@kdab/stable")

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        git.checkout("v%s"%self.version)

    def configure(self):
        # Use kdab flags to match qt package hash
        # ~$ conan create -ks -o qt:qttools=True -o qt:qtsvg=True -o qt:qtdeclarative=True -o qt:qtremoteobjects=True -o qt:qtscxml=True . 5.13.2@kdab/stable
        self.options["qt"].qtsvg = True
        self.options["qt"].qtdeclarative = True
        self.options["qt"].qtremoteobjects = True
        self.options["qt"].qtscxml = True
        self.options["qt"].qttools = True

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
