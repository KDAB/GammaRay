#version 150 core

/*
  widget.frag

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


in VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;
uniform float highlightFactor;

void main(void)
{
    vec2 coords = vec2(fs_in.texCoord.x, 1 - fs_in.texCoord.y);
    // front face
    if (fs_in.normal.z > 0) {
        fragColor = texture(frontTexture, coords);
    // back face
    } else if (fs_in.normal.z < 0) {
        fragColor = texture(backTexture, coords);
    // side face
    } else {
        fragColor = vec4(0.0, 0.4, 0.0, 1.0);
    }

    fragColor = mix(fragColor, vec4(0.5, 0.5, 0.5, 1.0), highlightFactor);
}
