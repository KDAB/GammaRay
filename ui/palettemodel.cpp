/*
  palettemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "palettemodel.h"

#include <QIcon>
#include <QPainter>

using namespace GammaRay;

struct role_t
{
    const char *name;
    QPalette::ColorRole role;
};

static const role_t paletteRoles[] = {
    { "Window", QPalette::Window },
    { "WindowText", QPalette::WindowText },
    { "Base", QPalette::Base },
    { "AlternateBase", QPalette::AlternateBase },
    { "Text", QPalette::Text },
    { "ToolTipBase", QPalette::ToolTipBase },
    { "ToolTipText", QPalette::ToolTipText },
    { "Button", QPalette::Button },
    { "ButtonText", QPalette::ButtonText },
    { "BrightText", QPalette::BrightText },
    { "Light", QPalette::Light },
    { "Midlight", QPalette::Midlight },
    { "Dark", QPalette::Dark },
    { "Mid", QPalette::Mid },
    { "Shadow", QPalette::Shadow },
    { "Highlight", QPalette::Highlight },
    { "HighlightedText", QPalette::HighlightedText },
    { "Link", QPalette::Link },
    { "LinkVisited", QPalette::LinkVisited }
};

struct group_t
{
    const char *name;
    QPalette::ColorGroup group;
};

static const group_t paletteGroups[] = {
    { "Active", QPalette::Active },
    { "Inactive", QPalette::Inactive },
    { "Disabled", QPalette::Disabled },
};

PaletteModel::PaletteModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_editable(false)
{
}

QPalette PaletteModel::palette() const
{
    return m_palette;
}

void PaletteModel::setPalette(const QPalette &palette)
{
    beginResetModel();
    m_palette = palette;
    endResetModel();
}

void PaletteModel::setEditable(bool editable)
{
    m_editable = editable;
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return paletteRoles[index.row()].name;

        return m_palette.color(paletteGroups[index.column() - 1].group,
                               paletteRoles[index.row()].role)
            .name();
    } else if (role == Qt::EditRole && index.column() > 0) {
        // TODO return QBrush once we have an editor for that
        return m_palette.color(paletteGroups[index.column() - 1].group,
                               paletteRoles[index.row()].role);
    } else if (role == Qt::DecorationRole && index.column() != 0) {
        const QBrush brush = m_palette.brush(paletteGroups[index.column() - 1].group,
                                             paletteRoles[index.row()].role);
        QPixmap pixmap(32, 32);
        QPainter painter(&pixmap);
        painter.fillRect(pixmap.rect(), Qt::black);
        painter.fillRect(pixmap.rect().adjusted(1, 1, -1, -1), brush);
        return QIcon(pixmap);
    }

    return QVariant();
}

bool PaletteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_editable)
        return false;

    if (index.isValid() && role == Qt::EditRole) {
        if (value.type() == QVariant::Color) {
            m_palette.setColor(paletteGroups[index.column() - 1].group,
                               paletteRoles[index.row()].role, value.value<QColor>());
        } else if (value.type() == QVariant::Brush) {
            m_palette.setBrush(paletteGroups[index.column() - 1].group,
                               paletteRoles[index.row()].role, value.value<QBrush>());
        }
    }
    return QAbstractItemModel::setData(index, value, role);
}

int PaletteModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1 + sizeof(paletteGroups) / sizeof(paletteGroups[0]);
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return sizeof(paletteRoles) / sizeof(paletteRoles[0]);
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return tr("Role");
        return paletteGroups[section - 1].name;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags baseFlags = QAbstractTableModel::flags(index);
    if (m_editable && index.column() > 0)
        return baseFlags | Qt::ItemIsEditable;
    return baseFlags;
}
