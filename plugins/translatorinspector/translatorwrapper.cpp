/*
  translatorwrapper.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#include "translatorwrapper.h"

#include <QItemSelection>

using namespace GammaRay;

TranslationsModel::TranslationsModel(TranslatorWrapper *translator)
    : QAbstractTableModel(translator)
    , m_translator(translator)
{
    connect(this, &QAbstractItemModel::rowsInserted,
            this, &TranslationsModel::rowCountChanged);
    connect(this, &QAbstractItemModel::rowsRemoved,
            this, &TranslationsModel::rowCountChanged);
}

int TranslationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_nodes.size();
}

int TranslationsModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant TranslationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    Row node = m_nodes.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return node.context;
        case 1:
            return node.sourceText;
        case 2:
            return node.disambiguation;
        case 3:
            return node.translation;
        }
    }
    if (role == IsOverriddenRole && index.column() == 3) {
        return node.isOverridden;
    }
    return QVariant();
}

bool TranslationsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole && index.column() == 3) {
        Row &node = m_nodes[index.row()];
        if (node.translation == value.toString())
            return true;
        node.translation = value.toString();
        node.isOverridden = true;
        emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole
                                                      << Qt::EditRole);
        return true;
    }
    return false;
}

Qt::ItemFlags TranslationsModel::flags(const QModelIndex &index) const
{
    const auto f = QAbstractTableModel::flags(index);
    if (index.column() == 3)
        return f | Qt::ItemIsEditable;
    return f;
}

QMap<int, QVariant> TranslationsModel::itemData(const QModelIndex &index) const
{
    auto data = QAbstractTableModel::itemData(index);
    if (hasIndex(index.row(), index.column(), index.parent())) {
        if (index.column() == 3)
            data[IsOverriddenRole] = m_nodes.at(index.row()).isOverridden;
    }
    return data;
}

void TranslationsModel::resetTranslations(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    // The mapping to source make the linear selection ... non linear
    // Let rebuild linear ranges to avoid overflood...

    const QModelIndexList indexes = selection.indexes();
    QMap<int, int> rows;
    for (const QModelIndex &index : indexes)
        rows[index.row()] = -1;

    QVector<QPair<int, int>> ranges; // pair of first/last
    for (auto it = rows.constBegin(), end = rows.constEnd(); it != end; ++it) {
        const int &row = it.key();
        Q_ASSERT(row >= 0);

        if (ranges.isEmpty() || ranges.last().second != row - 1) {
            ranges << qMakePair(row, row);
        } else {
            ranges.last().second = row;
        }
    }

    for (int i = ranges.count() -1; i >= 0; --i) {
        const auto &range = ranges[i];
        beginRemoveRows(QModelIndex(), range.first, range.second);
        m_nodes.remove(range.first, range.second - range.first + 1);
        endRemoveRows();
    }
}

QString TranslationsModel::translation(const char *context, const char *sourceText,
                                       const char *disambiguation, const int n,
                                       const QString &default_)
{
    QModelIndex existingIndex
        = findNode(context, sourceText, disambiguation, n, true);
    Row &row = m_nodes[existingIndex.row()];
    if (!row.isOverridden)
        setTranslation(existingIndex, default_);
    return row.translation;
}

void TranslationsModel::resetAllUnchanged()
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (!m_nodes[i].isOverridden)
            resetTranslations(QItemSelection(index(i, 0), index(i, 0)));
    }
}

void TranslationsModel::setTranslation(const QModelIndex &index, const QString &translation)
{
    if (!index.isValid())
        return;

    auto &row = m_nodes[index.row()];
    if (row.isOverridden || row.translation == translation)
        return;
    row.translation = translation;
    emit dataChanged(index, index);
}

QModelIndex TranslationsModel::findNode(const char *context, const char *sourceText,
                                        const char *disambiguation, const int n, const bool create)
{
    Q_UNUSED(n);
    // QUESTION make use of n?
    for (int i = 0; i < m_nodes.size(); ++i) {
        const Row &node = m_nodes.at(i);
        if (node.context == context && node.sourceText == sourceText
            && node.disambiguation == disambiguation)
            return index(i, 0);
    }
    if (create) {
        Row node;
        node.context = context;
        node.sourceText = sourceText;
        node.disambiguation = disambiguation;
        const int newRow = m_nodes.size();
        beginInsertRows(QModelIndex(), newRow, newRow);
        m_nodes.append(node);
        endInsertRows();
        return index(newRow, 0);
    }
    return {};
}

TranslatorWrapper::TranslatorWrapper(QTranslator *wrapped, QObject *parent)
    : QTranslator(parent)
    , m_wrapped(wrapped)
    , m_model(new TranslationsModel(this))
{
    Q_ASSERT(wrapped);

    // not deleteLater(), otherwise we end up with a dangling pointer in here!
    connect(wrapped, &QObject::destroyed, this, [this]() {
        delete this;
    });
}

bool TranslatorWrapper::isEmpty() const
{
    return translator()->isEmpty();
}

QString TranslatorWrapper::translate(const char *context, const char *sourceText,
                                     const char *disambiguation, int n) const
{
    const QString translation = translateInternal(context, sourceText, disambiguation, n);

    if (context && strncmp(context, "GammaRay::", 10) == 0)
        return translation;
    // it's not for this translator
    if (translation.isNull())
        return translation;
    return m_model->translation(context, sourceText, disambiguation, n, translation);
}

QString TranslatorWrapper::translateInternal(const char *context, const char *sourceText,
                                             const char *disambiguation, int n)
const
{
    return translator()->translate(context, sourceText, disambiguation, n);
}

QTranslator *TranslatorWrapper::translator() const
{
    Q_ASSERT(m_wrapped);
    return m_wrapped;
}

FallbackTranslator::FallbackTranslator(QObject *parent)
    : QTranslator(parent)
{
    setObjectName(QStringLiteral("Fallback Translator"));
}

QString FallbackTranslator::translate(const char *context, const char *sourceText,
                                      const char *disambiguation, int n) const
{
    Q_UNUSED(context);
    Q_UNUSED(disambiguation);
    Q_UNUSED(n);
    return QString::fromUtf8(sourceText);
}
