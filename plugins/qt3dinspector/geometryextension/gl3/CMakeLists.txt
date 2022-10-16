# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
if(Glslang_FOUND)
    glslang_validate_program(passthrough.vert normals.geom normals.frag)
    glslang_validate_program(wireframe.vert wireframe.geom wireframe.frag)
    glslang_validate_program(skybox.vert skybox.frag)
endif()
