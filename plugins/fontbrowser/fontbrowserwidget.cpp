/*
  fontbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    m_selectedFontModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SelectedFontModel"));

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
    connect(ui->pointSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_fontBrowser, &FontBrowserInterface::setPointSize);

    QAbstractItemModel *fontModel = ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.FontModel"));
    auto proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(fontModel);
    proxy->setRecursiveFilteringEnabled(true);
    proxy->setFilterRole(FontBrowserInterface::FontSearchRole);
    proxy->setSortRole(FontBrowserInterface::SortRole);
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

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");
    QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

FontBrowserWidget::~FontBrowserWidget() = default;

void FontBrowserWidget::delayedInit()
{
    m_fontBrowser->setColors(palette().color(QPalette::WindowText),
                             palette().color(QPalette::Base));
}
