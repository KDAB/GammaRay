# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# put targets in their own prefix, so we can test if GammaRay is properly loaded when installed elsewhere
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testbin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testlib)

add_executable(
    minimalcoreapplication
    minimalcoreapplication.cpp
)
target_link_libraries(
    minimalcoreapplication Qt::Core
)

if(TARGET Qt::Widgets)
    add_executable(minimalwidgetapplication minimalwidgetapplication.cpp)
    target_link_libraries(minimalwidgetapplication Qt::Gui Qt::Widgets)
endif()
