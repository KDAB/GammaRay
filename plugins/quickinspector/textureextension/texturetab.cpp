/*
  texturetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "texturetab.h"
#include "ui_texturetab.h"
#include <ui/propertywidget.h>

#include <QActionGroup>
#include <QComboBox>
#include <QToolBar>
#include <cmath>

using namespace GammaRay;


QString formatBytes(qint64 bytes)
{
    QVector<QString> sizes;
    sizes.push_back(TextureTab::tr(" GiB"));
    sizes.push_back(TextureTab::tr(" MiB"));
    sizes.push_back(TextureTab::tr(" KiB"));
    sizes.push_back(TextureTab::tr(" B"));
    static const qint64 startMultiplier = std::pow(1024.0f, sizes.size() - 1);

    qint64 multiplier = startMultiplier;
    for (int i = 0; i < sizes.size(); ++i, multiplier /= 1024) {
        if (bytes < multiplier)
            continue;
        else if (bytes % multiplier == 0)
            return QString::number(bytes / multiplier) + sizes.at(i);
        else
            return QString::number(bytes / static_cast<qreal>(multiplier), 'f', 2) + sizes.at(i);
    }
    return TextureTab::tr("0 B");
}

TextureTab::TextureTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextureTab)
{
    ui->setupUi(this);
    ui->textureView->setName(parent->objectBaseName() + QStringLiteral(".texture.remoteView"));

    auto toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar->layout()->setContentsMargins(9, 9, 9, 9);
    toolbar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    ui->layout->setMenuBar(toolbar);

    foreach (auto action, ui->textureView->interactionModeActions()->actions())
        toolbar->addAction(action);
    toolbar->addSeparator();

    toolbar->addAction(ui->textureView->zoomOutAction());
    auto zoom = new QComboBox;
    zoom->setModel(ui->textureView->zoomLevelModel());
    toolbar->addWidget(zoom);
    toolbar->addAction(ui->textureView->zoomInAction());
    toolbar->addSeparator();

    const auto warningImage = QIcon(":/resources/warning.png");
    auto toggleTextureWasteAction = new QAction(warningImage, tr("Visualize Texture Problems"), nullptr);
    toggleTextureWasteAction->setObjectName("aVisualizeTextureProblems");
    toggleTextureWasteAction->setCheckable(true);
    toggleTextureWasteAction->setChecked(true);
    toolbar->addAction(toggleTextureWasteAction);

    ui->textureView->setSupportedInteractionModes(RemoteViewWidget::ViewInteraction | RemoteViewWidget::Measuring | RemoteViewWidget::ColorPicking);

    connect(zoom, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            ui->textureView, &RemoteViewWidget::setZoomLevel);
    connect(ui->textureView, &RemoteViewWidget::zoomLevelChanged, zoom, &QComboBox::setCurrentIndex);
    connect(toggleTextureWasteAction, &QAction::toggled, ui->textureView, &TextureViewWidget::setTextureWasteVisualizationEnabled);

    // Texture issues infobar
    connect(ui->textureView, &TextureViewWidget::textureInfoNecessary, this,
            [&](bool infoNecessary) {
                ui->textureInfo->setVisible(infoNecessary);
                if (!infoNecessary)
                    ui->infoLabel->setText(QString());
            });
    connect(ui->textureView, &TextureViewWidget::textureWasteFound, this,
            [&](bool isProblem, int percent, int bytes) {
                addInfoLine(isProblem, tr("Transparency waste: %1% / %2.").arg(QString::number(percent), formatBytes(bytes)));
            });
    connect(ui->textureView, &TextureViewWidget::textureIsUnicolor, this,
            [&](bool isProblem) {
                addInfoLine(isProblem, tr("Texture has only one color, consider using a widget or a rectangle."));
            });
    connect(ui->textureView, &TextureViewWidget::textureIsFullyTransparent, this,
            [&](bool isProblem) {
                addInfoLine(isProblem, tr("Texture is fully transparent, consider using margins or anchoring."));
            });
    connect(ui->textureView, &TextureViewWidget::textureHasBorderImageSavings, this,
            [&](bool isProblem, int percent, int bytes) {
                addInfoLine(isProblem, tr("Using a BorderImage for this texture would save %1% / %2.").arg(QString::number(percent), formatBytes(bytes)));
            });
    zoom->setCurrentIndex(ui->textureView->zoomLevelIndex());
}

void TextureTab::addInfoLine(bool isProblem, const QString &newLine)
{
    if (!isProblem)
        return;
    auto text = ui->infoLabel->text();
    if (!text.isEmpty())
        text = text + QStringLiteral("<br>");
    ui->infoLabel->setText(text + newLine);
}

TextureTab::~TextureTab() = default;
