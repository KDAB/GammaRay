# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
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
