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

class GammaRayConan(ConanFile):
    name = "GammaRay"
    version = "2.11.50"
    license = ("https://raw.githubusercontent.com/KDAB/GammaRay/{0}/LICENSE.txt,"
               "https://raw.githubusercontent.com/KDAB/GammaRay/{0}/LICENSE.GPL.txt,"
               "https://raw.githubusercontent.com/KDAB/GammaRay/{0}/LICENSE.BSD2.txt").format(version)
    author = "Klaralvdalens Datakonsult AB (KDAB) info@kdab.com"
    url = "https://github.com/KDAB/GammaRay.git"
    description = "GammaRay is a software introspection tool for Qt applications developed by KDAB."
    generators = "cmake"
    options = dict({
        "build_ui": [True, False],
        "probe_only_build": [True, False],
        "client_only_build": [True, False],
        "enable_gpl_only_features": [True, False],
        "install_qt_layout": [True, False],
        "build_cli_injector": [True, False],
        "multi_build": [True, False],
        "static_probe": [True, False],
        "build_docs": [True, False],
        "enforce_qt_asserts": [True, False],
    })
    default_options = dict({
        "build_ui": True,
        "probe_only_build": False,
        "client_only_build": False,
        "enable_gpl_only_features": False,
        "install_qt_layout": False,
        "build_cli_injector": True,
        "multi_build": True,
        "static_probe": False,
        "build_docs": True,
        "enforce_qt_asserts": False,
    })

    def requirements(self):
        self.requires("qt/5.13.2@kdab/stable")
        self.requires("KDStateMachineEditor/1.2@kdab/stable")
        self.requires("KSyntaxHighlighting/5.64.0@kdab/stable")

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        git.checkout("%s"%self.version)

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
        self.cmake.definitions["WITH_INTERNAL_GRAPHVIZ"] = True
        self.cmake.definitions["GAMMARAY_BUILD_UI"] = self.options.build_ui
        self.cmake.definitions["GAMMARAY_PROBE_ONLY_BUILD"] = self.options.probe_only_build
        self.cmake.definitions["GAMMARAY_CLIENT_ONLY_BUILD"] = self.options.client_only_build
        self.cmake.definitions["GAMMARAY_ENABLE_GPL_ONLY_FEATURES"] = self.options.enable_gpl_only_features
        self.cmake.definitions["GAMMARAY_INSTALL_QT_LAYOUT"] = self.options.install_qt_layout
        self.cmake.definitions["GAMMARAY_BUILD_CLI_INJECTOR"] = self.options.build_cli_injector
        self.cmake.definitions["GAMMARAY_MULTI_BUILD"] = self.options.multi_build
        self.cmake.definitions["GAMMARAY_BUILD_DOCS"] = self.options.build_docs
        self.cmake.definitions["GAMMARAY_ENFORCE_QT_ASSERTS"] = self.options.enforce_qt_asserts
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
