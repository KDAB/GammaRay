# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

set(widget_layouting_srcs widget-layouting.cpp)

add_executable(
    example-widget-layouting
    ${widget_layouting_srcs}
)
target_link_libraries(
    example-widget-layouting Qt::Widgets
)
