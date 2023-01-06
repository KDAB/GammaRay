/*
  deferredtreeview_p.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_DEFERREDTREEVIEW_P_H
#define GAMMARAY_DEFERREDTREEVIEW_P_H

#include <QHeaderView>

namespace GammaRay {
class HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    enum State
    {
        NoState,
        ResizeSection,
        MoveSection,
        SelectSections,
        NoClear
    }; // Copied from QHVPrivate

    explicit HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

    bool isState(State state) const;
};
}

#endif
