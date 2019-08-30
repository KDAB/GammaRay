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
    return NUM_COLUMNS;
}

QVariant FontDatabaseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int styleIndex = -1;
    int familyIndex = -1;
    if (index.internalId() == TopLevelId) {
        familyIndex = index.row();
    } else {
        familyIndex = index.internalId();
        styleIndex = index.row();
    }
    Q_ASSERT(familyIndex >= 0 && familyIndex < m_families.size() && familyIndex < m_styles.size());
    Q_ASSERT(styleIndex == -1 || (styleIndex >= 0 && styleIndex < m_styles.at(familyIndex).size()));

    const auto &style = styleIndex == -1 ? QString() : m_styles.at(familyIndex).at(styleIndex);
    const auto &family = m_families.at(familyIndex);
    const auto isSortRole = role == FontBrowserInterface::SortRole;

    if (role == Qt::DisplayRole || isSortRole) {
        auto toSortVariant = [isSortRole] (bool state) {
            return isSortRole ? QVariant(state) : QVariant();
        };
        switch (static_cast<Columns>(index.column())) {
        case Label:
            return styleIndex == -1 ? family : style;
        case Weight:
            return QFontDatabase().weight(family, style);
        case SmoothSizes:
            return smoothSizeString(family, style);
        case Bold:
            return toSortVariant(QFontDatabase().bold(family, style));
        case Italic:
            return toSortVariant(QFontDatabase().italic(family, style));
        case Scalable:
            return toSortVariant(QFontDatabase().isScalable(family, style));
        case BitmapScalable:
            return toSortVariant(QFontDatabase().isBitmapScalable(family, style));
        case SmoothlyScalable:
            return toSortVariant(QFontDatabase().isSmoothlyScalable(family, style));
        case NUM_COLUMNS:
            return {};
        }
    } else if (role == Qt::CheckStateRole) {
        auto checkState = [](bool state) {
            return state ? Qt::Checked : Qt::Unchecked;
        };
        switch (static_cast<Columns>(index.column())) {
        case Bold:
            return checkState(QFontDatabase().bold(family, style));
        case Italic:
            return checkState(QFontDatabase().italic(family, style));
        case Scalable:
            return checkState(QFontDatabase().isScalable(family, style));
        case BitmapScalable:
            return checkState(QFontDatabase().isBitmapScalable(family, style));
        case SmoothlyScalable:
            return checkState(QFontDatabase().isSmoothlyScalable(family, style));
        case Weight:
        case Label:
        case SmoothSizes:
        case NUM_COLUMNS:
            return {};
        }
    } else if (role == Qt::ToolTipRole) {
        if (index.column() == SmoothSizes)
            return smoothSizeString(family, style);
    } else if (role == FontBrowserInterface::FontRole) {
        if (style == -1) {
            return QFont(family);
        } else {
            return QFontDatabase().font(family, style, 10);
        }
    } else if (role == FontBrowserInterface::FontSearchRole) {
        if (index.internalId() == TopLevelId) {
            return family;
        } else {
            return tr("%1 %2").arg(family, style);
        }
    }

    return QVariant();
}

QVariant FontDatabaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Columns>(section)) {
        case Label:
            return tr("Fonts");
        case Weight:
            return tr("Weight");
        case Bold:
            return tr("Bold");
        case Italic:
            return tr("Italic");
        case Scalable:
            return tr("Scalable");
        case BitmapScalable:
            return tr("Bitmap Scalable");
        case SmoothlyScalable:
            return tr("Smoothly Scalable");
        case SmoothSizes:
            return tr("Smooth Sizes");
        case NUM_COLUMNS:
            return {};
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
    ret[FontBrowserInterface::SortRole] = QByteArrayLiteral("SortRole");
    return ret;
}

QMap<int, QVariant> FontDatabaseModel::itemData(const QModelIndex &index) const
{
    auto ret = QAbstractItemModel::itemData(index);
    for (auto role : {FontBrowserInterface::FontRole, FontBrowserInterface::FontSearchRole,
                      FontBrowserInterface::SortRole})
    {
        ret[role] = data(index, role);
    }
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
