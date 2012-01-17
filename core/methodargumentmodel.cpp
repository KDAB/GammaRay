/*
  methodargumentmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "methodargumentmodel.h"
#include <QtCore/qsharedpointer.h>

using namespace GammaRay;

// TODO: this should be implicitly shared to avoid m_data double deletion
SafeArgument::SafeArgument() : m_data(0)
{
}

SafeArgument::SafeArgument(const QVariant &v) : m_value(v), m_name(v.typeName()), m_data(0)
{
}

SafeArgument::~SafeArgument()
{
  if (m_data) {
    QMetaType::destroy(m_value.type(), m_data);
  }
}

SafeArgument::operator QGenericArgument() const
{
  if (m_value.isValid()) {
    m_data = QMetaType::construct(m_value.type(), m_value.constData());
    return QGenericArgument(m_name.data(), m_data);
  }
  return QGenericArgument();
}

MethodArgumentModel::MethodArgumentModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void MethodArgumentModel::setMethod(const QMetaMethod &method)
{
  m_method = method;
  m_arguments.clear();
  m_arguments.resize(method.parameterTypes().size());
  for (int i = 0; i < m_arguments.size(); ++i) {
    const QByteArray typeName = method.parameterTypes().at(i);
    const QVariant::Type variantType = QVariant::nameToType(typeName);
    m_arguments[i] = QVariant(variantType);
  }
  reset();
}

QVariant MethodArgumentModel::data(const QModelIndex &index, int role) const
{
  if (!m_method.signature() || m_arguments.isEmpty() ||
      index.row() < 0 ||
      index.row() >= m_arguments.size()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    const QVariant value = m_arguments.at(index.row());
    const QByteArray parameterName = m_method.parameterNames().at(index.row());
    const QByteArray parameterType = m_method.parameterTypes().at(index.row());
    switch (index.column()) {
    case 0:
      if (parameterName.isEmpty()) {
        return tr("<unnamed> (%1)").arg(QString::fromLatin1(parameterType));
      }
      return parameterName;
    case 1:
      return value;
    case 2:
      return parameterType;
    }
  }
  return QVariant();
}

int MethodArgumentModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int MethodArgumentModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_arguments.size();
}

bool MethodArgumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.row() >= 0 && index.row() < m_arguments.size() && role == Qt::EditRole) {
    m_arguments[index.row()] = value;
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

QVariant MethodArgumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Argument");
    case 1:
      return tr("Value");
    case 2:
      return tr("Type");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags MethodArgumentModel::flags(const QModelIndex &index) const
{
  const Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if (index.column() == 1) {
    return flags | Qt::ItemIsEditable;
  }
  return flags;
}

QVector<SafeArgument> MethodArgumentModel::arguments() const
{
  QVector<SafeArgument> args(10);
  for (int i = 0; i < rowCount(); ++i) {
    args[i] = SafeArgument(m_arguments.at(i));
  }
  return args;
}

#include "methodargumentmodel.moc"
