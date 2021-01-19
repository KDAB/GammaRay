# Backward compatibility helpers with ancient Qt5 versions

# Copyright (c) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# All rights reserved.
#
# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

macro(_create_private_target LIB)
    if (NOT TARGET Qt::${LIB}Private AND NOT TARGET Qt5::${LIB}Private AND DEFINED Qt5${LIB}_PRIVATE_INCLUDE_DIRS)
        # HACK to work around broken Qt cmake configurations in older Qt version (up to 5.7 at least)
        if(NOT "${Qt5${LIB}_PRIVATE_INCLUDE_DIRS}" MATCHES "/Qt${LIB}/")
            string(REPLACE "/QtCore" "/Qt${LIB}" replaceme "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
            list(APPEND Qt5${module}_PRIVATE_INCLUDE_DIRS ${replaceme})
            list(REMOVE_DUPLICATES Qt5${LIB}_PRIVATE_INCLUDE_DIRS)
            set(Qt5${module}_PRIVATE_INCLUDE_DIRS ${Qt5${LIB}_PRIVATE_INCLUDE_DIRS} PARENT_SCOPE)
        endif()

        add_library(Qt5::${LIB}Private INTERFACE IMPORTED)
        set_target_properties(Qt5::${LIB}Private PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Qt5${LIB}_PRIVATE_INCLUDE_DIRS}"
        )
        if (DEFINED ARGN)
            set_target_properties(Qt5::${LIB}Private PROPERTIES
                INTERFACE_LINK_LIBRARIES ${ARGN}
            )
        endif()
    endif()
endmacro()

_create_private_target(Core)
_create_private_target(Gui Core)
_create_private_target(IviCore Core)
_create_private_target(Qml Gui)
_create_private_target(Quick Qml)
_create_private_target(Scxml Core)
_create_private_target(Widgets Gui)
