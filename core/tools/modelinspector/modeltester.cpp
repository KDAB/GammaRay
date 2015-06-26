/*
  modeltester.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "modeltester.h"
#include <3rdparty/qt/modeltest.h>

#include "util.h"

#include <QAbstractListModel>
#include <QDebug>

#include <iostream>

using namespace GammaRay;

namespace GammaRay {
  struct ModelTester::ModelTestResult {
    ModelTestResult() : modelTest(0)
    {
    }

    ~ModelTestResult()
    {
      delete modelTest;
    }

    ModelTest *modelTest;
    QHash<int, QString> failures;
  };
}

ModelTester::ModelTester(QObject *parent) : QObject(parent)
{
}

void ModelTester::objectAdded(QObject *obj)
{
  QAbstractItemModel *model = qobject_cast<QAbstractItemModel*>(obj);
  if (model) {
    // TODO filter out our own models, way too slow otherwise
    // or even better allow to specify somehow to which models we want to attach
    connect(model, SIGNAL(destroyed(QObject*)), SLOT(modelDestroyed(QObject*)));
    ModelTestResult *result = new ModelTestResult;
    m_modelTestMap.insert(model, result); // needs to be available for the
                                          // initial calls to failure() already
    // FIXME too slow!
//     result->modelTest = new ModelTest(model, this);
  }
}

void ModelTester::modelDestroyed(QObject *model)
{
  if (m_modelTestMap.contains(static_cast<QAbstractItemModel*>(model))) {
    delete m_modelTestMap.take(static_cast<QAbstractItemModel*>(model));
  }
}

void ModelTester::failure(QAbstractItemModel *model, const char *file, int line,
                          const char *message)
{
  ModelTestResult *result = m_modelTestMap.value(model);
  Q_ASSERT(result || qgetenv("GAMMARAY_UNITTEST") == "1");
  if (!result) {
    // one of our own models
    qt_assert(message, file, line);
  }

  ///TODO: track file
  if (!result->failures.contains(line)) {
    std::cout << qPrintable(Util::displayString(model)) << " "
              << line << " " << message << std::endl;
    result->failures.insert(line, QString::fromLatin1(message));
  }
}

// inplace build of modeltest, with some slight modificatins:
// - change QVERIFY to non-fatal reporting
// - suppress qDebug etc, since those trigger qobject creating and thus
// infinite loops when model-testing the object model
//krazy:cond=includes
#include <QtCore> // avoid interference with any include used by modeltest
#define QT_QTTEST_MODULE_H
#undef QVERIFY
#undef QCOMPARE
#define QVERIFY(x) (!(x) ? static_cast<GammaRay::ModelTester*>(static_cast<QObject*>(this)->parent())->failure(this->model, __FILE__, __LINE__, #x) : qt_noop())
#define QCOMPARE(x, y) ((x != y) ? static_cast<GammaRay::ModelTester*>(static_cast<QObject*>(this)->parent())->failure(this->model, __FILE__, __LINE__, #x) : qt_noop())
#undef qDebug
#define qDebug() QNoDebug()
#include <3rdparty/qt/modeltest.cpp>
//krazy:endcond=includes
