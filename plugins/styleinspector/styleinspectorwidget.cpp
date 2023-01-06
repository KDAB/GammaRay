/*
  styleinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "styleinspectorwidget.h"
#include "ui_styleinspectorwidget.h"

#include <ui/propertyeditor/propertyeditordelegate.h>

#include <common/objectbroker.h>

using namespace GammaRay;

StyleInspectorWidget::StyleInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StyleInspectorWidget)
{
    ui->setupUi(this);

    ui->styleSelector->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StyleList")));
    connect(ui->styleSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &StyleInspectorWidget::styleSelected);

    ui->primitivePage->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.PrimitiveModel")));
    ui->controlPage->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.ControlModel")));
    ui->complexControlPage->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.ComplexControlModel")));

    ui->pixelMetricView->header()->setObjectName("pixelMetricViewHeader");
    ui->pixelMetricView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->pixelMetricView->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->pixelMetricView->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.PixelMetricModel")));

    ui->standardIconView->header()->setObjectName("standardIconViewHeader");
    ui->standardIconView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->standardIconView->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->standardIconView->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.StandardIconModel")));

    ui->standardPaletteView->header()->setObjectName("standardPaletteViewHeader");
    ui->standardPaletteView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->standardPaletteView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->standardPaletteView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->standardPaletteView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->standardPaletteView->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StyleInspector.PaletteModel")));

    ui->styleHintView->header()->setObjectName("styleHintViewHeader");
    ui->styleHintView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->styleHintView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->styleHintView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StyleInspector.StyleHintModel")));
    ui->styleHintView->setItemDelegate(new PropertyEditorDelegate(this));

    // TODO this will fail due to lazy model population
    if (ui->styleSelector->count())
        styleSelected(0);
}

StyleInspectorWidget::~StyleInspectorWidget()
{
    delete ui;
}

void StyleInspectorWidget::styleSelected(int index)
{
    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(ui->styleSelector->model());
    selectionModel->select(ui->styleSelector->model()->index(index,
                                                             0),
                           QItemSelectionModel::ClearAndSelect);
}
