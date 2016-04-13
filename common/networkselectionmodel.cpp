/*
  networkselectionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#include "networkselectionmodel.h"
#include "common/modelutils.h"
#include "endpoint.h"
#include "message.h"
#include "settempvalue.h"

#include <QRegExp>
#include <QAbstractProxyModel>
#include <QDebug>

using namespace GammaRay;

static void writeSelection(Message *msg, const QItemSelection &selection)
{
  msg->payload() << qint32(selection.size());
  foreach(const QItemSelectionRange& range, selection) {
    msg->payload() << Protocol::fromQModelIndex(range.topLeft()) << Protocol::fromQModelIndex(range.bottomRight());
  }
}

// find a model having a defaultSelectedItem method
static QAbstractItemModel *findSourceModel(QAbstractItemModel *model)
{
  if (model) {
    if (model->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("defaultSelectedItem()")) != -1) {
      return model;
    }
    else if (auto proxy = qobject_cast<QAbstractProxyModel*>(model)) {
      return findSourceModel(proxy->sourceModel());
    }
  }

  return 0;
}

NetworkSelectionModel::NetworkSelectionModel(const QString &objectName, QAbstractItemModel* model, QObject* parent):
  QItemSelectionModel(model, parent),
  m_objectName(objectName),
  m_myAddress(Protocol::InvalidObjectAddress),
  m_handlingRemoteMessage(false)
{
  setObjectName(m_objectName + QLatin1String("Network"));
  connect(this, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentChanged(QModelIndex,QModelIndex)));
  connect(this, SIGNAL(currentColumnChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentColumnChanged(QModelIndex,QModelIndex)));
  connect(this, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentRowChanged(QModelIndex,QModelIndex)));
  connect(this, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
}

NetworkSelectionModel::~NetworkSelectionModel()
{
}

void NetworkSelectionModel::requestSelection()
{
  if (m_handlingRemoteMessage || !Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress) {
    return;
  }
  Message msg(m_myAddress, Protocol::SelectionModelStateRequest);
  Endpoint::send(msg);
}

void NetworkSelectionModel::sendSelection()
{
  if (!Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress) {
    return;
  }

  clearPendingSelection();

  if (!hasSelection()) {
    if (model()->rowCount() > 0) {
      const QItemSelectionModel::SelectionFlags selectionFlags = QItemSelectionModel::ClearAndSelect |
          QItemSelectionModel::Rows | QItemSelectionModel::Current;
      const Qt::MatchFlags matchFlags = Qt::MatchExactly | Qt::MatchRecursive;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
      QAbstractItemModel *sourceModel = findSourceModel(model());
#else
      QAbstractItemModel *sourceModel = findSourceModel(const_cast<QAbstractItemModel*>(model()));
#endif
      QModelIndex index = model()->index(0, 0);

      // Query the model to get its default selected index
      if (sourceModel) {
        QPair<int, QVariant> result;
        QModelIndex defaultIndex;

        QMetaObject::invokeMethod(sourceModel, "defaultSelectedItem", Qt::DirectConnection,
                                  QReturnArgument<QPair<int, QVariant> >("QPair<int, QVariant>", result));

        if (result.second.userType() == qMetaTypeId<ModelUtils::MatchAcceptor>()) {
          defaultIndex = ModelUtils::match(index, result.first, result.second.value<ModelUtils::MatchAcceptor>(),
                                           1, matchFlags).value(0);
        } else {
          defaultIndex = model()->match(index, result.first, result.second, 1, matchFlags).value(0);
        }

        if (defaultIndex.isValid()) {
          index = defaultIndex;
        }
      }

      select(QItemSelection(index, index), selectionFlags);
    }
  } else {
    Message msg(m_myAddress, Protocol::SelectionModelClearSelect);
    writeSelection(&msg, selection());
    Endpoint::send(msg);
  }
}

Protocol::ItemSelection GammaRay::NetworkSelectionModel::readSelection(const GammaRay::Message& msg)
{
  Protocol::ItemSelection selection;
  qint32 size = 0;
  msg.payload() >> size;
  selection.reserve(size);

  for (int i = 0; i < size; ++i) {
    Protocol::ItemSelectionRange range;
    msg.payload() >> range.topLeft >> range.bottomRight;
    selection.push_back(range);
  }

  return selection;
}

bool GammaRay::NetworkSelectionModel::translateSelection(const Protocol::ItemSelection& selection, QItemSelection &qselection) const
{
  qselection.clear();
  foreach (const auto &range, selection) {
    const QModelIndex qmiTopLeft = Protocol::toQModelIndex(model(), range.topLeft);
    const QModelIndex qmiBottomRight = Protocol::toQModelIndex(model(), range.bottomRight);
    if (!qmiTopLeft.isValid() && !qmiBottomRight.isValid())
      return false;
    qselection.push_back(QItemSelectionRange(qmiTopLeft, qmiBottomRight));
  }
  return true;
}

void NetworkSelectionModel::newMessage(const Message& msg)
{
  Q_ASSERT(msg.address() == m_myAddress);
  switch (msg.type()) {
  case Protocol::SelectionModelSelect:
  {
    Util::SetTempValue<bool> guard(m_handlingRemoteMessage, true);
    m_pendingSelection = readSelection(msg);
    const auto deselected = readSelection(msg);

    QItemSelection qmiSelection;
    if (translateSelection(deselected, qmiSelection) && !qmiSelection.isEmpty()) {
      select(qmiSelection, Deselect);
    }

    applyPendingSelection();
    break;
  }
  case Protocol::SelectionModelClearSelect:
  {
    Util::SetTempValue<bool> guard(m_handlingRemoteMessage, true);
    m_pendingSelection = readSelection(msg);
    reset();
    applyPendingSelection();
    break;
  }
  case Protocol::SelectionModelCurrent:
  {
    qint32 flags;
    Protocol::ModelIndex index;
    msg.payload() >> flags >> index;
    const QModelIndex qmi = Protocol::toQModelIndex(model(), index);
    if (!qmi.isValid())
      break;
    Util::SetTempValue<bool> guard(m_handlingRemoteMessage, true);
    setCurrentIndex(qmi, QItemSelectionModel::SelectionFlags(flags));
    break;
  }
  case Protocol::SelectionModelStateRequest:
  {
    sendSelection();
    break;
  }
  default:
    Q_ASSERT(false);
  }
}

void NetworkSelectionModel::slotCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  Q_UNUSED(previous);
  if (m_handlingRemoteMessage || !Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress)
    return;
  clearPendingSelection();

  Message msg(m_myAddress, Protocol::SelectionModelCurrent);
  msg.payload() << qint32(QItemSelectionModel::Current) << Protocol::fromQModelIndex(current);
  Endpoint::send(msg);
}

void NetworkSelectionModel::slotCurrentColumnChanged(const QModelIndex& current, const QModelIndex& previous)
{
  Q_UNUSED(previous);
  if (m_handlingRemoteMessage ||!Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress)
    return;
  clearPendingSelection();

  Message msg(m_myAddress, Protocol::SelectionModelCurrent);
  msg.payload() << qint32(QItemSelectionModel::Current|QItemSelectionModel::Columns) << Protocol::fromQModelIndex(current);
  Endpoint::send(msg);
}

void NetworkSelectionModel::slotCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
  Q_UNUSED(previous);
  if (m_handlingRemoteMessage ||!Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress)
    return;
  clearPendingSelection();

  Message msg(m_myAddress, Protocol::SelectionModelCurrent);
  msg.payload() << qint32(QItemSelectionModel::Current|QItemSelectionModel::Rows) << Protocol::fromQModelIndex(current);
  Endpoint::send(msg);
}

void NetworkSelectionModel::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  if (m_handlingRemoteMessage ||!Endpoint::isConnected() || m_myAddress == Protocol::InvalidObjectAddress)
    return;
  clearPendingSelection();

  Message msg(m_myAddress, Protocol::SelectionModelSelect);
  writeSelection(&msg, selected);
  writeSelection(&msg, deselected);
  Endpoint::send(msg);
}

void GammaRay::NetworkSelectionModel::applyPendingSelection()
{
  if (m_pendingSelection.isEmpty())
    return;

  QItemSelection qmiSelection;
  if (translateSelection(m_pendingSelection, qmiSelection)) {
    if (!qmiSelection.isEmpty())
      select(qmiSelection, Select);
    clearPendingSelection();
  }
}

void GammaRay::NetworkSelectionModel::clearPendingSelection()
{
  m_pendingSelection.clear();
}
