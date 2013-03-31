/*
  modelinspector.h

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

#ifndef GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H
#define GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H

#include "include/toolfactory.h"
#include "modelinspectorwidget.h"

#include <QWidget>

namespace GammaRay {

class ModelModel;
class ModelTester;

class ModelInspector : public QObject
{
  Q_OBJECT
  public:
    explicit ModelInspector(ProbeInterface *probe, QObject *parent = 0);

    ModelModel *modelModel() const;

  private:
    ModelModel *m_modelModel;
    ModelTester *m_modelTester;
};

class ModelInspectorFactory : public QObject, public StandardToolFactory2<QAbstractItemModel, ModelInspector, ModelInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ModelInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    QString name() const;
};

}

#endif // GAMMARAY_MODELINSPECTOR_H
