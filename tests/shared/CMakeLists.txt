# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

#changingpropertyobject doesn't have any symbols. Quiet MSVC warnings.
if(MSVC)
    set(CMAKE_STATIC_LINKER_FLAGS "/ignore:4221 ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(gammaray_shared_test_srcs variantpropertyobject.cpp changingpropertyobject.cpp propertytestobject.cpp)

add_library(
    gammaray_shared_test_data STATIC
    ${gammaray_shared_test_srcs}
)
target_compile_features(gammaray_shared_test_data PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})
target_link_libraries(
    gammaray_shared_test_data Qt::Core
)
