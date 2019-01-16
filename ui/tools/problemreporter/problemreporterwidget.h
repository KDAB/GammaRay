/*
  problemreporterwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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
