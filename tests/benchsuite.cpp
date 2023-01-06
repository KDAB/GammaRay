/*
  benchsuite.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "benchsuite.h"
#include "core/probe.h"
#include "core/util.h"

#include <QtTestGui>

#include <QLabel>
#include <QTreeView>

QTEST_MAIN(GammaRay::BenchSuite)

using namespace GammaRay;

void BenchSuite::iconForObject()
{
    QWidget widget;
    QLabel label;
    QTreeView treeView;
    QBENCHMARK
    {
        Util::iconIdForObject(this);
        Util::iconIdForObject(&widget);
        Util::iconIdForObject(&label);
        Util::iconIdForObject(&treeView);
    }
}

void BenchSuite::probe_objectAdded()
{
    Probe::createProbe(false);

    static const int NUM_OBJECTS = 10000;
    QVector<QObject *> objects;
    objects.reserve(NUM_OBJECTS + 1);
    // fill it
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        auto *obj = new QObject;
        objects << obj;
    }

    auto it = objects.constBegin();
    auto end = objects.constEnd();
    QBENCHMARK_ONCE
    {
        while (it != end) {
            Probe::objectAdded(*it);
            ++it;
        }
    }

    qDeleteAll(objects);
    delete Probe::instance();
}
