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
set(WIDGET3D_QML_FILES
    main.qml
    CheckBox.qml
    Horizontals.qml
    WidgetDelegate.qml
    WidgetInfo.qml
    WidgetMaterial.qml
)

qml_lint(${WIDGET3D_QML_FILES})
