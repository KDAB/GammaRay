/*
  sgwireframewidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef SGWIREFRAMEWIDGET_H
#define SGWIREFRAMEWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QAbstractItemModel;

namespace GammaRay {

class SGWireframeWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit SGWireframeWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~SGWireframeWidget();

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *m_model);

  protected:
    virtual void paintEvent(QPaintEvent* );

  private slots:
    void onModelDataChanged(const QModelIndex & topLeft = QModelIndex(), const QModelIndex & bottomRight = QModelIndex());

  private:
    QAbstractItemModel *m_model;
    int m_positionColumn;
};

}

#endif // SGWIREFRAMEWIDGET_H
