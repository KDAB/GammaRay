/*
  fontdatabasemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fontdatabasemodel.h"
#include "fontbrowserinterface.h"

#include <QDebug>
#include <QFontDatabase>
#include <QStringList>

#include <limits>

using namespace GammaRay;

static const int TopLevelId = std::numeric_limits<int>::max();

FontDatabaseModel::FontDatabaseModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

FontDatabaseModel::~FontDatabaseModel() = default;

int FontDatabaseModel::rowCount(const QModelIndex &parent) const
{
    ensureModelPopulated();
    if (!parent.isValid())
        return m_families.size();

    if (parent.internalId() == TopLevelId && parent.column() == 0)
        return m_styles.at(parent.row()).size();

    return 0;
}

int FontDatabaseModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant FontDatabaseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int family = -1;
    if (index.internalId() == TopLevelId)
        family = index.row();
    else
        family = index.internalId();
    Q_ASSERT(family >= 0 && family < m_families.size() && family < m_styles.size());

    if (role == Qt::DisplayRole) {
        if (index.internalId() == TopLevelId) {
            if (index.column() == 0)
                return m_families.at(family);
        } else {
            switch (index.column()) {
            case 0:
                return m_styles.at(family).at(index.row());
            case 1:
                return smoothSizeString(m_families.at(family), m_styles.at(family).at(index.row()));
            }
        }
    } else if (role == Qt::ToolTipRole) {
        if (index.internalId() != TopLevelId && index.column() == 1)
            return smoothSizeString(m_families.at(family), m_styles.at(family).at(index.row()));
    } else if (role == FontBrowserInterface::FontRole) {
        if (index.internalId() == TopLevelId) {
            return QFont(m_families.at(family));
        } else {
            QFontDatabase database;
            return database.font(m_families.at(family), m_styles.at(family).at(index.row()), 10);
        }
    } else if (role == FontBrowserInterface::FontSearchRole) {
        if (index.internalId() == TopLevelId) {
            return m_families.at(family);
        } else {
            return tr("%1 %2").arg(m_families.at(family), m_styles.at(family).at(index.row()));
        }
    }

    return QVariant();
}

QVariant FontDatabaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Fonts");
        case 1:
            return tr("Smooth Sizes");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex FontDatabaseModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= columnCount())
        return {};

    if (parent.isValid()) {
        if (row >= m_styles.at(parent.row()).size())
            return QModelIndex();
        return createIndex(row, column, parent.row());
    }
    return createIndex(row, column, TopLevelId);
}

QModelIndex FontDatabaseModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalId() == TopLevelId)
        return {};
    return createIndex(child.internalId(), 0, TopLevelId);
}

QHash<int, QByteArray> FontDatabaseModel::roleNames() const
{
    auto ret = QAbstractItemModel::roleNames();
    ret[FontBrowserInterface::FontRole] = QByteArrayLiteral("FontRole");
    ret[FontBrowserInterface::FontSearchRole] = QByteArrayLiteral("FontSearchRole");
    return ret;
}

QMap<int, QVariant> FontDatabaseModel::itemData(const QModelIndex &index) const
{
    auto ret = QAbstractItemModel::itemData(index);
    for (auto role : {FontBrowserInterface::FontRole, FontBrowserInterface::FontSearchRole})
        ret[role] = data(index, role);
    return ret;
}

QString FontDatabaseModel::smoothSizeString(const QString &family, const QString &style) const
{
    QFontDatabase database;
    const auto smoothSizes = database.smoothSizes(family, style);
    QStringList sizes;
    sizes.reserve(smoothSizes.size());
    for (auto points : smoothSizes)
        sizes.push_back(QString::number(points));
    return sizes.join(QStringLiteral(" "));
}

void FontDatabaseModel::ensureModelPopulated() const
{
    if (!m_families.isEmpty())
        return;

    const_cast<FontDatabaseModel *>(this)->populateModel();
}

void FontDatabaseModel::populateModel()
{
    QFontDatabase database;
    const auto families = database.families();
    m_families.reserve(families.size());
    m_styles.resize(families.size());
    for (int i = 0; i < families.size(); ++i) {
        const auto &family = families.at(i);
        m_families.push_back(family);

        const auto styles = database.styles(family);
        m_styles[i].reserve(styles.size());
        foreach (const auto &style, database.styles(family))
            m_styles[i].push_back(style);
    }
}
