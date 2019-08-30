/*
  fontbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "fontbrowserwidget.h"
#include "ui_fontbrowserwidget.h"

#include "fontbrowserclient.h"

#include <common/objectbroker.h>
#include <ui/searchlinecontroller.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDebug>

using namespace GammaRay;

static QObject *fontBrowserClientFactory(const QString & /*name*/, QObject *parent)
{
    return new FontBrowserClient(parent);
}

FontBrowserWidget::FontBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FontBrowserWidget)
    , m_stateManager(this)
    , m_selectedFontModel(nullptr)
    , m_fontBrowser(nullptr)
{
    ObjectBroker::registerClientObjectFactoryCallback<FontBrowserInterface *>(
        fontBrowserClientFactory);
    m_fontBrowser = ObjectBroker::object<FontBrowserInterface *>();

    ui->setupUi(this);

    m_selectedFontModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SelectedFontModel"));

    ui->selectedFontsView->header()->setObjectName("selectedFontsViewHeader");
    ui->selectedFontsView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->selectedFontsView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->selectedFontsView->setModel(m_selectedFontModel);

    connect(ui->fontText, &QLineEdit::textChanged,
            m_fontBrowser, &FontBrowserInterface::updateText);
    connect(ui->boldBox, &QAbstractButton::toggled,
            m_fontBrowser, &FontBrowserInterface::toggleBoldFont);
    connect(ui->italicBox, &QAbstractButton::toggled,
            m_fontBrowser, &FontBrowserInterface::toggleItalicFont);
    connect(ui->underlineBox, &QAbstractButton::toggled,
            m_fontBrowser, &FontBrowserInterface::toggleUnderlineFont);
    connect(ui->pointSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_fontBrowser, &FontBrowserInterface::setPointSize);

    QAbstractItemModel *fontModel = ObjectBroker::model(QStringLiteral(
                                                            "com.kdab.GammaRay.FontModel"));
    auto proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(fontModel);
    proxy->setRecursiveFilteringEnabled(true);
    proxy->setFilterRole(FontBrowserInterface::FontSearchRole);
    new SearchLineController(ui->fontSearchLine, proxy);
    ui->fontTree->header()->setObjectName("fontTreeHeader");
    ui->fontTree->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->fontTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->fontTree->setModel(proxy);
    ui->fontTree->setSelectionModel(ObjectBroker::selectionModel(proxy));

    ui->pointSize->setValue(font().pointSize());

    // init
    m_fontBrowser->updateText(ui->fontText->text());
    m_fontBrowser->toggleBoldFont(ui->boldBox->isChecked());
    m_fontBrowser->toggleItalicFont(ui->italicBox->isChecked());
    m_fontBrowser->toggleUnderlineFont(ui->underlineBox->isChecked());
    m_fontBrowser->setPointSize(ui->pointSize->value());

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%" << "50%");
    QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

FontBrowserWidget::~FontBrowserWidget() = default;

void FontBrowserWidget::delayedInit()
{
    m_fontBrowser->setColors(palette().color(QPalette::Foreground),
                             palette().color(QPalette::Base));
}
