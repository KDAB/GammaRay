/*
  paintbufferviewer.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "paintbufferviewer.h"

#include "ui_paintbufferviewer.h"

#include <common/paintanalyzerinterface.h>
#include <common/objectbroker.h>

#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QSettings>

using namespace GammaRay;

PaintBufferViewer::PaintBufferViewer(const QString &name, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PaintBufferViewer)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    ui->paintAnalyzerWidget->setBaseName(name);

    QSettings settings;
    settings.beginGroup(QStringLiteral("PaintBufferViewer"));
    restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
}

PaintBufferViewer::~PaintBufferViewer()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("PaintBufferViewer"));
    settings.setValue(QStringLiteral("Geometry"), saveGeometry());
}
