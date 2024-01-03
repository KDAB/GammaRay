/*
  deferredtreeview_p.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
