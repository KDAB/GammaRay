/*
  networkselectionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networkselectionmodel.h"
#include "common/modelutils.h"
#include "endpoint.h"
#include "message.h"
#include "settempvalue.h"

#include <QAbstractProxyModel>
#include <QDebug>

using namespace GammaRay;

static QDataStream &operator<<(QDataStream &out, QItemSelectionModel::SelectionFlags command)
{
    out << ( quint32 )command; // Qt4 and Qt5 use the same enum layout, so this is fine for now
    return out;
}

QDataStream &operator>>(QDataStream &in, QItemSelectionModel::SelectionFlags &command)
{
    quint32 v;
    in >> v;
    command = static_cast<QItemSelectionModel::SelectionFlags>(v);
    return in;
}

static void writeSelection(Message *msg, const QItemSelection &selection)
{
    *msg << qint32(selection.size());
    for (const QItemSelectionRange &range : selection)
        *msg << Protocol::fromQModelIndex(range.topLeft()) << Protocol::fromQModelIndex(range.bottomRight());
}

// find a model having a defaultSelectedItem method
static QAbstractItemModel *findSourceModel(QAbstractItemModel *model)
{
    if (model) {
        if (model->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(
                "defaultSelectedItem()"))
            != -1)
            return model;
        if (auto proxy = qobject_cast<QAbstractProxyModel *>(model))
            return findSourceModel(proxy->sourceModel());
    }

    return nullptr;
}

NetworkSelectionModel::NetworkSelectionModel(const QString &objectName, QAbstractItemModel *model,
                                             QObject *parent)
    : QItemSelectionModel(model, parent)
    , m_objectName(objectName)
    , m_myAddress(Protocol::InvalidObjectAddress)
    , m_pendingCommand(NoUpdate)
    , m_handlingRemoteMessage(false)
{
    setObjectName(m_objectName + QLatin1String("Network"));
    connect(this, &QItemSelectionModel::currentChanged, this,
            &NetworkSelectionModel::slotCurrentChanged);
}

NetworkSelectionModel::~NetworkSelectionModel() = default;

void NetworkSelectionModel::requestSelection()
{
    if (m_handlingRemoteMessage || !isConnected())
        return;
    Message msg(m_myAddress, Protocol::SelectionModelStateRequest);
    Endpoint::send(msg);
}

void NetworkSelectionModel::sendSelection()
{
    if (!isConnected())
        return;

    clearPendingSelection();

    if (!hasSelection()) {
        if (model()->rowCount() > 0) {
            const QItemSelectionModel::SelectionFlags selectionFlags = QItemSelectionModel::ClearAndSelect
                | QItemSelectionModel::Rows
                | QItemSelectionModel::Current;
            const Qt::MatchFlags matchFlags = Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap;
            QAbstractItemModel *sourceModel = findSourceModel(model());
            QModelIndex index = model()->index(0, 0);

            // Query the model to get its default selected index
            if (sourceModel) {
                QPair<int, QVariant> result;
                QModelIndex defaultIndex;

                QMetaObject::invokeMethod(sourceModel, "defaultSelectedItem", Qt::DirectConnection,
                                          QReturnArgument<QPair<int, QVariant>>("QPair<int,QVariant>",
                                                                                result));

                if (result.second.userType() == qMetaTypeId<ModelUtils::MatchAcceptor>()) {
                    defaultIndex = ModelUtils::match(index, result.first,
                                                     result.second.value<ModelUtils::MatchAcceptor>(),
                                                     1, matchFlags)
                                       .value(0);
                } else {
                    defaultIndex = model()->match(index, result.first, result.second, 1,
                                                  matchFlags)
                                       .value(0);
                }

                if (defaultIndex.isValid())
                    index = defaultIndex;
            }

            select(QItemSelection(index, index), selectionFlags);
        }
    } else {
        Message msg(m_myAddress, Protocol::SelectionModelSelect);
        writeSelection(&msg, selection());
        msg << ClearAndSelect;
        Endpoint::send(msg);
    }
}

Protocol::ItemSelection GammaRay::NetworkSelectionModel::readSelection(const GammaRay::Message &msg)
{
    Protocol::ItemSelection selection;
    qint32 size = 0;
    msg >> size;
    selection.reserve(size);

    for (int i = 0; i < size; ++i) {
        Protocol::ItemSelectionRange range;
        msg >> range.topLeft >> range.bottomRight;
        selection.push_back(range);
    }

    return selection;
}

bool GammaRay::NetworkSelectionModel::translateSelection(const Protocol::ItemSelection &selection,
                                                         QItemSelection &qselection) const
{
    qselection.clear();
    for (const auto &range : selection) {
        const QModelIndex qmiTopLeft = Protocol::toQModelIndex(model(), range.topLeft);
        const QModelIndex qmiBottomRight = Protocol::toQModelIndex(model(), range.bottomRight);
        if (!qmiTopLeft.isValid() && !qmiBottomRight.isValid())
            return false;
        qselection.push_back(QItemSelectionRange(qmiTopLeft, qmiBottomRight));
    }
    return true;
}

void NetworkSelectionModel::newMessage(const Message &msg)
{
    Q_ASSERT(msg.address() == m_myAddress);
    switch (msg.type()) {
    case Protocol::SelectionModelSelect: {
        Util::SetTempValue<bool> guard(m_handlingRemoteMessage, true);
        m_pendingSelection = readSelection(msg);
        msg >> m_pendingCommand;
        applyPendingSelection();
        break;
    }
    case Protocol::SelectionModelCurrent: {
        SelectionFlags flags;
        Protocol::ModelIndex index;
        msg >> flags >> index;
        const QModelIndex qmi = Protocol::toQModelIndex(model(), index);
        if (!qmi.isValid())
            break;
        Util::SetTempValue<bool> guard(m_handlingRemoteMessage, true);
        setCurrentIndex(qmi, flags);
        break;
    }
    case Protocol::SelectionModelStateRequest:
        sendSelection();
        break;
    default:
        Q_ASSERT(false);
    }
}

void NetworkSelectionModel::slotCurrentChanged(const QModelIndex &current,
                                               const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (m_handlingRemoteMessage || !isConnected())
        return;
    clearPendingSelection();

    Message msg(m_myAddress, Protocol::SelectionModelCurrent);
    msg << QItemSelectionModel::Current << Protocol::fromQModelIndex(current);
    Endpoint::send(msg);
}

void NetworkSelectionModel::select(const QItemSelection &selection,
                                   QItemSelectionModel::SelectionFlags command)
{
    QItemSelectionModel::select(selection, command);

    if (m_handlingRemoteMessage || !isConnected())
        return;
    clearPendingSelection();

    Message msg(m_myAddress, Protocol::SelectionModelSelect);
    writeSelection(&msg, selection);
    msg << command;
    Endpoint::send(msg);
}

void GammaRay::NetworkSelectionModel::applyPendingSelection()
{
    if (m_pendingSelection.isEmpty() && m_pendingCommand == NoUpdate)
        return;

    QItemSelection qmiSelection;
    if (translateSelection(m_pendingSelection, qmiSelection)) {
        if (!qmiSelection.isEmpty())
            select(qmiSelection, m_pendingCommand);
        clearPendingSelection();
    }
}

void GammaRay::NetworkSelectionModel::clearPendingSelection()
{
    m_pendingSelection.clear();
    m_pendingCommand = NoUpdate;
}

bool NetworkSelectionModel::isConnected() const
{
    return Endpoint::isConnected() && m_myAddress != Protocol::InvalidObjectAddress;
}
