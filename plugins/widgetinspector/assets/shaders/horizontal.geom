#version 150 core

/*
  horizontal.geom

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VertexData {
    vec3 vertexPosition;
} gs_in[];


struct Widget {
  float explosionFactor;
};

uniform mat4 mvp;
uniform Widget widget;

void main()
{
    vec4 pos = vec4(gs_in[0].vertexPosition, 1.0);

    pos.z -= 0.5; // half thickness of the widget cuboid
    gl_Position = mvp * pos;
    EmitVertex();

    pos.z -= widget.explosionFactor - 1.0;
    gl_Position = mvp * pos;
    EmitVertex();

    EndPrimitive();
}

