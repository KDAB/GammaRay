# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
