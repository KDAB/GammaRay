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
if(Qt5Quick_VERSION GREATER 5.5)
    add_subdirectory(quick-batching)
    add_subdirectory(quick-event-handling)
    add_subdirectory(state-machine)
endif()

if(TARGET Qt::Widgets)
    add_subdirectory(signal-slot)
    add_subdirectory(timer)
    add_subdirectory(widget-layouting)
endif()

if(Qt53DExtras_FOUND)
    add_subdirectory(qt3d-geometry)
endif()
