/*
  actionvalidator.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "actionvalidator.h"

#include <QAction>
#include <QMutexLocker>
#include <QWidget>

#include <core/objectdataprovider.h>
#include <core/probe.h>
#include <core/problemcollector.h>
#include <common/problem.h>

using namespace GammaRay;

QT_BEGIN_NAMESPACE
uint qHash(const QKeySequence &sequence)
{
    return qHash(sequence.toString(QKeySequence::PortableText));
}

QT_END_NAMESPACE

ActionValidator::ActionValidator(QObject *parent)
    : QObject(parent)
{
}

QList<QAction *> ActionValidator::actions() const
{
    return m_shortcutActionMap.values();
}

QList<QAction *> ActionValidator::actions(const QKeySequence &sequence) const
{
    return m_shortcutActionMap.values(sequence);
}

void ActionValidator::setActions(const QList<QAction *> &actions)
{
    clearActions();

    m_shortcutActionMap.reserve(actions.size());
    for (QAction *action : actions) {
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
        if (m_shortcutActionMap.values(sequence).contains(action))
            continue;

        m_shortcutActionMap.insert(sequence, action);
    }

    // also track object destruction
    connect(action, &QObject::destroyed,
            this, &ActionValidator::handleActionDestroyed);
}



void ActionValidator::remove(QAction *action)
{
    Q_ASSERT(action);

    safeRemove(action);
}

void ActionValidator::safeRemove(QAction *action)
{
    for (auto it = m_shortcutActionMap.cbegin(); it != m_shortcutActionMap.cend(); ++it) {
        const QKeySequence &sequence = it.key();
        if (!m_shortcutActionMap.values(sequence).contains(action))
            continue;

        QList<QAction *> oldValues = m_shortcutActionMap.values(sequence);
        const bool success = oldValues.removeOne(action);
        Q_UNUSED(success);
        Q_ASSERT(success);
        m_shortcutActionMap.replace(sequence, action);
    }
}

void ActionValidator::handleActionDestroyed(QObject *object)
{
    QAction *action = static_cast<QAction *>(object);

    safeRemove(action);
}

bool ActionValidator::hasAmbiguousShortcut(const QAction *action) const
{
    QList<QKeySequence> shortcuts = action->shortcuts();
    return std::any_of(shortcuts.constBegin(), shortcuts.constEnd(),
                       [action, this](const QKeySequence &seq) { return isAmbigous(action, seq); });
}

QVector<QKeySequence> GammaRay::ActionValidator::findAmbiguousShortcuts(const QAction *action) const
{
    QVector<QKeySequence> shortcuts;


    if (!action)
        return shortcuts;

    Q_FOREACH (const QKeySequence &sequence, action->shortcuts()) {
        if (isAmbigous(action, sequence)) {
            shortcuts.push_back(sequence);
        }
    }
    return shortcuts;
}

bool GammaRay::ActionValidator::isAmbigous(const QAction *action, const QKeySequence &sequence) const
{
    Q_ASSERT(action);
    QMutexLocker lock(Probe::objectLock());
    if (!Probe::instance()->isValidObject(action)) {
        return false;
    }

    Q_FOREACH (const QAction *other, m_shortcutActionMap.values(sequence)) {
        if (!other || other == action || !Probe::instance()->isValidObject(other)) {
            continue;
        }
        if (action->shortcutContext() == Qt::ApplicationShortcut
            || other->shortcutContext() == Qt::ApplicationShortcut)
            return true;
        if (action->shortcutContext() == Qt::WindowShortcut || other->shortcutContext() == Qt::WindowShortcut) {
            Q_FOREACH (QWidget *w1, action->associatedWidgets()) {
                Q_FOREACH (QWidget *w2, other->associatedWidgets()) {
                    if (w1->window() == w2->window())
                        return true;
                }
            }
        }
        if (action->shortcutContext() == Qt::WidgetWithChildrenShortcut) {
            Q_FOREACH (QWidget *w1, action->associatedWidgets()) {
                Q_FOREACH (QWidget *w2, other->associatedWidgets()) {
                    for (QWidget *ancestor = w2; ancestor; ancestor = ancestor->parentWidget()) {
                        if (w1 == ancestor)
                            return true;
                    }
                }
            }
        }
        if (other->shortcutContext() == Qt::WidgetWithChildrenShortcut) {
            Q_FOREACH (QWidget *w1, other->associatedWidgets()) {
                Q_FOREACH (QWidget *w2, action->associatedWidgets()) {
                    for (QWidget *ancestor = w2; ancestor; ancestor = ancestor->parentWidget()) {
                        if (w1 == ancestor)
                            return true;
                    }
                }
            }
        }
        if (action->shortcutContext() == Qt::WidgetShortcut && other->shortcutContext() == Qt::WidgetShortcut) {
            Q_FOREACH (QWidget *w1, action->associatedWidgets()) {
                Q_FOREACH (QWidget *w2, other->associatedWidgets()) {
                    if (w1 == w2)
                        return true;
                }
            }
        }
    }
    return false;
}
