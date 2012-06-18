/*
  actionvalidator.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONVALIDATOR_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONVALIDATOR_H

#include <QObject>

#include <QHash>
#include <QKeySequence>

class QAction;

namespace GammaRay {

class ActionValidator : public QObject
{
  Q_OBJECT

  public:
    ActionValidator(QObject *parent = 0);

    QList<QAction*> actions() const;
    QList<QAction*> actions(const QKeySequence &sequence) const;

    void setActions(const QList<QAction*> &actions);
    void clearActions();

    void insert(QAction *action);
    void remove(QAction *action);

    /// helper method to find out if action has an ambiguous shortcut
    bool hasAmbiguousShortcut(const QAction *action) const;

  private Q_SLOTS:
    void handleActionDestroyed(QObject *object);

  private:
    /// Does not deref the action pointer
    void safeRemove(QAction *action);

    // Multi-Map
    QHash<QKeySequence, QAction*> m_shortcutActionMap;
};

}

#endif // GAMMARAY_ACTIONVALIDATOR_H
