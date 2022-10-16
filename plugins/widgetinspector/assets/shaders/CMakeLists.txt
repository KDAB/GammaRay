# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
set(WIDGET3D_SHADERS_HORIZONTALS_PROGRAM horizontal.frag horizontal.geom horizontal.vert)
set(WIDGET3D_SHADERS_WIDGET_PROGRAM widget.frag widget.vert)
set(WIDGET3D_SHADERS_WIDGET_WIREFRAME_PROGRAM widget_wireframe.frag widget_wireframe.geom widget_wireframe.vert)
if(Glslang_FOUND)
    glslang_validate_program(${WIDGET3D_SHADERS_HORIZONTALS_PROGRAM})
    glslang_validate_program(${WIDGET3D_SHADERS_WIDGET_PROGRAM})
    glslang_validate_program(${WIDGET3D_SHADERS_WIDGET_WIREFRAME_PROGRAM})
endif()
