/*
  actionvalidator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONVALIDATOR_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONVALIDATOR_H

#include <QObject>

#include <QHash>
#include <QKeySequence>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace GammaRay {
class ActionValidator : public QObject
{
    Q_OBJECT

public:
    explicit ActionValidator(QObject *parent = nullptr);

    QList<QAction *> actions() const;
    QList<QAction *> actions(const QKeySequence &sequence) const;

    void setActions(const QList<QAction *> &actions);
    void clearActions();

    void insert(QAction *action);
    void remove(QAction *action);

    /// helper method to find out if action has an ambiguous shortcut
    bool hasAmbiguousShortcut(const QAction *action) const;
    bool isAmbigous(const QAction *action, const QKeySequence &sequence) const;
    QVector<QKeySequence> findAmbiguousShortcuts(const QAction *action) const;

private Q_SLOTS:
    void handleActionDestroyed(QObject *object);

private:
    /// Does not deref the action pointer
    void safeRemove(QAction *action);

    // Multi-Map
    QMultiHash<QKeySequence, QAction *> m_shortcutActionMap;
};
}

#endif // GAMMARAY_ACTIONVALIDATOR_H
