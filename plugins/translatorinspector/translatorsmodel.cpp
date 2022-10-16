/*
  translatorsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "translatorsmodel.h"

#include <core/util.h>

#include <common/objectid.h>

#include "translatorwrapper.h"

using namespace GammaRay;

TranslatorsModel::TranslatorsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TranslatorsModel::columnCount(const QModelIndex &) const
{
    return 3;
}

int TranslatorsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_translators.size();
}

QVariant TranslatorsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == ObjectIdRole)
        return QVariant::fromValue(ObjectId(m_translators.at(index.row())->translator()));
    TranslatorWrapper *trans = m_translators.at(index.row());
    Q_ASSERT(trans);
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return Util::shortDisplayString(trans->translator());
        else if (index.column() == 1)
            return QString(trans->translator()->metaObject()->className());
        else if (index.column() == 2)
            return trans->model()->rowCount(QModelIndex());
    } else if (role == Qt::ToolTipRole) {
        return Util::tooltipForObject(trans->translator());
    }
    return QVariant();
}

QVariant TranslatorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return tr("Object");
        else if (section == 1)
            return tr("Type");
        else if (section == 2)
            return tr("Translations");
    }
    return QVariant();
}

QMap<int, QVariant> TranslatorsModel::itemData(const QModelIndex &index) const
{
    auto d = QAbstractTableModel::itemData(index);
    d.insert(ObjectIdRole, data(index, ObjectIdRole));
    return d;
}

TranslatorWrapper *TranslatorsModel::translator(const QModelIndex &index)
    const
{
    return m_translators.at(index.row());
}

void TranslatorsModel::sourceDataChanged()
{
    const int row = m_translators.indexOf(qobject_cast<TranslationsModel *>(sender())->translator());
    if (row == -1)
        return;
    const QModelIndex index = this->index(row, 2, QModelIndex());
    if (!index.isValid())
        return;
    // needed to make sure these things also update
    emit dataChanged(index, index,
                     QVector<int>() << Qt::DisplayRole << Qt::EditRole);
}

void TranslatorsModel::registerTranslator(TranslatorWrapper *translator)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_translators.prepend(translator);
    endInsertRows();
    connect(translator->model(), &TranslationsModel::rowCountChanged,
            this, &TranslatorsModel::sourceDataChanged);
}

void TranslatorsModel::unregisterTranslator(TranslatorWrapper *translator)
{
    const int index = m_translators.indexOf(translator);
    if (index == -1) {
        qWarning("TranslatorsModel::unregisterTranslator: translator %s is not registered",
                 qPrintable(Util::addressToString(translator)));
        return;
    }
    disconnect(translator->model(), nullptr, this, nullptr);
    beginRemoveRows(QModelIndex(), index, index);
    m_translators.removeAt(index);
    endRemoveRows();
}
