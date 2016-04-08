#version 330 core

/*
  widget.geom

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} gs_in[];

out VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} gs_out;

uniform mat4 viewportMatrix;
uniform mat4 mvp;


void main()
{
    for (int i = 0; i < gl_in.length(); i++) {
        vec4 pos = vec4(gs_in[i].position, 1);
        gl_Position = mvp * pos;
        gs_out.position = gs_in[i].position;
        gs_out.normal = gs_in[i].normal;
        gs_out.texCoord = gs_in[i].texCoord;

        EmitVertex();
    }
    EndPrimitive();
}
