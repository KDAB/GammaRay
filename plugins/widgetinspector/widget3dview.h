/*
  widget3dview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef WIDGET3DVIEW_H
#define WIDGET3DVIEW_H

#include <QWidget>

namespace GammaRay {
class Widget3DWindow;
class Widget3DSelectionHelper;

class Widget3DView : public QWidget
{
    Q_OBJECT
public:
    explicit Widget3DView(QWidget *parent = nullptr);
    ~Widget3DView();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;


private:
    void showContextMenu(const QPoint &pos);
    void selectCurrentObject();

    Widget3DWindow *mRenderWindow;
    Widget3DSelectionHelper *mSelectionHelper;

    QPoint mLastRightClick;
};

}

#endif // WIDGET3DVIEW_H
