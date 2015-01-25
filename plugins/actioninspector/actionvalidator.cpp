/*
  actionvalidator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "actionvalidator.h"

#include <QAction>

using namespace GammaRay;

uint qHash(const QKeySequence &sequence)
{
  return qHash(sequence.toString(QKeySequence::PortableText));
}

ActionValidator::ActionValidator(QObject *parent)
  : QObject(parent)
{
}

QList<QAction*> ActionValidator::actions() const
{
  return m_shortcutActionMap.values();
}

QList<QAction*> ActionValidator::actions(const QKeySequence& sequence) const
{
  return m_shortcutActionMap.values(sequence);
}

void ActionValidator::setActions(const QList<QAction*>& actions)
{
  clearActions();

  m_shortcutActionMap.reserve(actions.size());
  Q_FOREACH (QAction *action, actions) {
    insert(action);
  }
}

void ActionValidator::clearActions()
{
  m_shortcutActionMap.clear();
}

void ActionValidator::insert(QAction *action)
{
  Q_ASSERT(action);

  Q_FOREACH (const QKeySequence &sequence, action->shortcuts()) {
    if (m_shortcutActionMap.values(sequence).contains(action)) {
      continue;
    }

    m_shortcutActionMap.insertMulti(sequence, action);
  }

  // also track object destruction
  connect(action, SIGNAL(destroyed(QObject*)),
          SLOT(handleActionDestroyed(QObject*)));
}

void ActionValidator::remove(QAction *action)
{
  Q_ASSERT(action);

  safeRemove(action);
}

void ActionValidator::safeRemove(QAction *action)
{
  Q_FOREACH (const QKeySequence &sequence, m_shortcutActionMap.keys()) { //krazy:exclude=foreach
    if (!m_shortcutActionMap.values(sequence).contains(action)) {
      continue;
    }

    QList<QAction*> oldValues = m_shortcutActionMap.values(sequence);
    const bool success = oldValues.removeOne(action);
    Q_UNUSED(success);
    Q_ASSERT(success);
    m_shortcutActionMap[sequence] = action;
  }
}

void ActionValidator::handleActionDestroyed(QObject *object)
{
  QAction *action = static_cast<QAction*>(object);

  safeRemove(action);
}

bool ActionValidator::hasAmbiguousShortcut(const QAction *action) const
{
  if (!action) {
    return false;
  }

  Q_FOREACH (const QKeySequence &sequence, action->shortcuts()) {
    if (m_shortcutActionMap.values(sequence).size() > 1) {
      return true;
    }
  }
  return false;
}
