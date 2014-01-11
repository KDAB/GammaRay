/*
  modeltester.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELINSPECTOR_MODELTESTER_H
#define GAMMARAY_MODELINSPECTOR_MODELTESTER_H

#include <QHash>
#include <QObject>
#include <QPointer>

class QAbstractItemModel;
class ModelTest;

namespace GammaRay {

class ModelTester : public QObject
{
  Q_OBJECT
  public:
    explicit ModelTester(QObject *parent = 0);

    void failure(QAbstractItemModel *model, const char *file, int line, const char *message);

  public slots:
    void objectAdded(QObject *obj);

  private slots:
    void modelDestroyed(QObject *model);

  private:
    struct ModelTestResult;
    QHash<QAbstractItemModel*, ModelTestResult*> m_modelTestMap;
};

}

#endif // GAMMARAY_MODELTESTER_H
