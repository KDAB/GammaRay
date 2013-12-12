/*
  objectinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTOR_H
#define GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTOR_H

#include "toolfactory.h"

#include <QObject>

class QItemSelection;
class QItemSelectionModel;
class QModelIndex;

namespace GammaRay {

class PropertyController;

class ObjectInspector : public QObject
{
  Q_OBJECT
  public:
    explicit ObjectInspector(ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void selectDefaultItem();
    void objectSelected(const QModelIndex &index);
    void objectSelectionChanged(const QItemSelection &selection);
    void objectSelected(QObject *object);

  private:
    PropertyController *m_propertyController;
    QItemSelectionModel *m_selectionModel;
};

class ObjectInspectorFactory : public QObject, public StandardToolFactory<QObject, ObjectInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ObjectInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Objects");
    }
};

}

#endif // GAMMARAY_OBJECTINSPECTOR_H
