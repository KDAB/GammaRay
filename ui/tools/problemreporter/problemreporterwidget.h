/*
  problemreporterwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBLEMREPORTERWIDGET_H
#define GAMMARAY_PROBLEMREPORTERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class ProblemReporterWidget;
}
class ProblemClientModel;

class ProblemReporterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProblemReporterWidget(QWidget *parent = nullptr);
    ~ProblemReporterWidget() override;

private slots:
    void problemViewContextMenu(const QPoint &p);
    void updateFilter(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    QScopedPointer<Ui::ProblemReporterWidget> ui;
    UIStateManager m_stateManager;
    QAbstractItemModel *m_availableCheckersModel;
    ProblemClientModel *m_problemsModel;
};

}

#endif // GAMMARAY_PROBLEMREPORTERWIDGET_H
