/*
  fontbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include <network/objectbroker.h>
#include <network/networkobject.h>

#include <QAbstractItemModel>

using namespace GammaRay;

FontBrowserWidget::FontBrowserWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::FontBrowserWidget)
  , m_selectedFontModel(0)
  , m_fontBrowser(ObjectBroker::object("com.kdab.GammaRay.FontBrowser"))
{
  ui->setupUi(this);

  m_selectedFontModel = ObjectBroker::model("com.kdab.GammaRay.SelectedFontModel");

  ui->selectedFontsView->setModel(m_selectedFontModel);
  ui->selectedFontsView->setRootIsDecorated(false);
  ui->selectedFontsView->header()->setResizeMode(0, QHeaderView::ResizeToContents);

  connect(ui->fontText, SIGNAL(textChanged(QString)),
          this, SLOT(updateText(QString)));
  connect(ui->boldBox, SIGNAL(toggled(bool)),
          this, SLOT(toggleBoldFont(bool)));
  connect(ui->italicBox, SIGNAL(toggled(bool)),
          this, SLOT(toggleItalicFont(bool)));
  connect(ui->underlineBox, SIGNAL(toggled(bool)),
          this, SLOT(toggleUnderlineFont(bool)));
  connect(ui->pointSize, SIGNAL(valueChanged(int)),
          this, SLOT(setPointSize(int)));

  QAbstractItemModel *fontModel = ObjectBroker::model("com.kdab.GammaRay.FontModel");
  ui->fontTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->fontTree->setModel(fontModel);
  ui->fontTree->setSelectionModel(ObjectBroker::selectionModel(fontModel));
  ui->fontTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);

  ui->pointSize->setValue(font().pointSize());

  // init
  updateText(ui->fontText->text());
  toggleBoldFont(ui->boldBox->isChecked());
  toggleItalicFont(ui->italicBox->isChecked());
  toggleUnderlineFont(ui->underlineBox->isChecked());
  setPointSize(ui->pointSize->value());
}

void FontBrowserWidget::updateText(const QString &text)
{
  m_fontBrowser->emitSignal("updateText", QVariantList() << text);
}

void FontBrowserWidget::toggleBoldFont(bool bold)
{
  m_fontBrowser->emitSignal("toggleBoldFont", QVariantList() << bold);
}

void FontBrowserWidget::toggleItalicFont(bool italic)
{
  m_fontBrowser->emitSignal("toggleItalicFont", QVariantList() << italic);
}

void FontBrowserWidget::toggleUnderlineFont(bool underline)
{
  m_fontBrowser->emitSignal("toggleUnderlineFont", QVariantList() << underline);
}

void FontBrowserWidget::setPointSize(int pointSize)
{
  m_fontBrowser->emitSignal("setPointSize", QVariantList() << pointSize);
}

#include "fontbrowserwidget.moc"
