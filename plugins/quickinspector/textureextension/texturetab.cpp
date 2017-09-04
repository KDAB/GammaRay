/*
  texturetab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "texturetab.h"
#include "ui_texturetab.h"
#include <ui/propertywidget.h>

#include <QComboBox>
#include <QToolBar>
#include <cmath>

using namespace GammaRay;


QString formatBytes(qint64 bytes)
{
    if (bytes == 0)
        return QStringLiteral("0 B");

    static QVector<QString> sizes;
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
    return QStringLiteral("0 B");
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
    toolbar->layout()->setContentsMargins(9,9,9,9);
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
    toggleTextureWasteAction->setCheckable(true);
    toggleTextureWasteAction->setChecked(true);
    toolbar->addAction(toggleTextureWasteAction);

    ui->textureView->setSupportedInteractionModes(RemoteViewWidget::ViewInteraction | RemoteViewWidget::Measuring | RemoteViewWidget::ColorPicking);

    connect(zoom, SIGNAL(currentIndexChanged(int)), ui->textureView, SLOT(setZoomLevel(int)));
    connect(ui->textureView, SIGNAL(zoomLevelChanged(int)), zoom, SLOT(setCurrentIndex(int)));
    connect(toggleTextureWasteAction, SIGNAL(toggled(bool)), ui->textureView, SLOT(setTextureWasteVisualizationEnabled(bool)));
    connect(ui->textureView, SIGNAL(textureInfoNecessary(bool)), ui->textureInfo, SLOT(setVisible(bool)));
    connect(ui->textureView, &TextureViewWidget::textureWasteFound,
            [&](bool isProblem, int percent, int bytes) {
                ui->textureWasteLabel->setText(tr("Transparency Waste:") + QString::number(percent) + tr("% or ") + formatBytes(bytes));
                ui->textureWasteLabel->setVisible(isProblem);
    });
    connect(ui->textureView, SIGNAL(textureIsUnicolor(bool)), ui->unicolorWarningLabel, SLOT(setVisible(bool)));
    connect(ui->textureView, SIGNAL(textureIsFullyTransparent(bool)), ui->fullTransparencyWarningLabel, SLOT(setVisible(bool)));
    connect(ui->textureView, &TextureViewWidget::textureHasHorizontalBorderImageSavings,
            [&](bool isProblem, int percentSaved) {
                ui->horizontalBorderImageLabel->setText(tr("Using a horizontal Border Image for this texture would save ") + QString::number(percentSaved) + tr("%."));
                ui->horizontalBorderImageLabel->setVisible(isProblem);
    });
    connect(ui->textureView, &TextureViewWidget::textureHasVerticalBorderImageSavings,
            [&](bool isProblem, int percentSaved) {
                ui->verticalBorderImageLabel->setText(tr("Using a vertical Border Image for this texture would save ") + QString::number(percentSaved) + tr("%."));
                ui->verticalBorderImageLabel->setVisible(isProblem);
    });
    zoom->setCurrentIndex(ui->textureView->zoomLevelIndex());
}

TextureTab::~TextureTab()
{
}
