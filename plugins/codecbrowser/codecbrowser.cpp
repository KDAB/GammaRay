/*
  codecbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "codecbrowser.h"
#include "codecmodel.h"

#include <common/objectbroker.h>

#include <QItemSelectionModel>

#include <QtPlugin>

using namespace GammaRay;

CodecBrowser::CodecBrowser(ProbeInterface* probe, QObject* parent)
  : QObject(parent)
{
  ObjectBroker::registerObject("com.kdab.GammaRay.CodecBrowser", this);

  AllCodecsModel* model = new AllCodecsModel(this);
  probe->registerModel("com.kdab.GammaRay.AllCodecsModel", model);

  m_codecSelectionModel = ObjectBroker::selectionModel(model);
  connect(m_codecSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(updateCodecs(QItemSelection,QItemSelection)));

  m_selectedCodecsModel = new SelectedCodecsModel(this);
  probe->registerModel("com.kdab.GammaRay.SelectedCodecsModel", m_selectedCodecsModel);
}

void CodecBrowser::textChanged(const QString &text)
{
  m_selectedCodecsModel->updateText(text);
}

void CodecBrowser::updateCodecs(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);

  QStringList currentCodecNames;
  foreach (const QModelIndex &index, m_codecSelectionModel->selectedRows()) {
    const QString codecName = index.data().toString();
    currentCodecNames.append(codecName);
  }

  m_selectedCodecsModel->setCodecs(currentCodecNames);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(CodecBrowserFactory)
#endif
