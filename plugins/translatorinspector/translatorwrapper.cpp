/*
  translatorwrapper.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#include <QFont>

using namespace GammaRay;

TranslationsModel::TranslationsModel(TranslatorWrapper *translator)
    : QAbstractListModel(translator), m_translator(translator)
{
  connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
          SIGNAL(rowCountChanged()));
  connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
          SIGNAL(rowCountChanged()));
}
int TranslationsModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_nodes.size();
}
int TranslationsModel::columnCount(const QModelIndex &) const
{
  return 4;
}
QVariant TranslationsModel::data(const QModelIndex &index, int role) const
{
  if (index.parent().isValid()) {
    return QVariant();
  }
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
  if (role == Qt::FontRole && index.column() == 3 && node.isOverriden) {
    QFont font;
    font.setItalic(true);
    return font;
  }
  return QVariant();
}
bool TranslationsModel::setData(const QModelIndex &index, const QVariant &value,
                                int role)
{
  if (role == Qt::EditRole && index.column() == 3) {
    Row &node = m_nodes[index.row()];
    if (node.translation == value.toString()) {
      return true;
    }
    node.translation = value.toString();
    node.isOverriden = true;
    emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole
                                                  << Qt::EditRole);
    return true;
  }
  return false;
}
QVariant TranslationsModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0:
        return tr("Context");
      case 1:
        return tr("Source Text");
      case 2:
        return tr("Disambiguation");
      case 3:
        return tr("Translation");
    }
  }
  return QVariant();
}
Qt::ItemFlags TranslationsModel::flags(const QModelIndex &index) const
{
  if (index.column() == 3) {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  } else {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
}
void TranslationsModel::resetTranslations(const QModelIndex &first,
                                          const QModelIndex &last)
{
  if (!first.isValid() || !last.isValid()) {
    return;
  }
  int top = first.row();
  int bottom = last.row();
  beginRemoveRows(QModelIndex(), top, bottom);
  for (int i = 0; i < (bottom - top + 1); ++i) {
    m_nodes.remove(top);
  }
  endRemoveRows();
}
QString TranslationsModel::translation(const QByteArray &context,
                                       const QByteArray &sourceText,
                                       const QByteArray &disambiguation,
                                       const int n, const QString &default_)
{
  QModelIndex existingIndex =
      findNode(context, sourceText, disambiguation, n, true);
  Row &row = m_nodes[existingIndex.row()];
  if (!row.isOverriden) {
    setTranslation(existingIndex, default_);
  }
  return row.translation;
}
void TranslationsModel::resetAllUnchanged()
{
  for (int i = 0; i < m_nodes.size(); ++i) {
    if (!m_nodes[i].isOverriden) {
      resetTranslations(index(i), index(i));
    }
  }
}
void TranslationsModel::setTranslation(const QModelIndex &index,
                                       const QString &translation)
{
  if (!index.isValid()) {
    return;
  }
  if (m_nodes[index.row()].isOverriden) {
    return;
  }
  m_nodes[index.row()].translation = translation;
  emit dataChanged(index, index);
}
QModelIndex TranslationsModel::findNode(const QByteArray &context,
                                        const QByteArray &sourceText,
                                        const QByteArray &disambiguation,
                                        const int n, const bool create)
{
  Q_UNUSED(n);
  // QUESTION make use of n?
  for (int i = 0; i < m_nodes.size(); ++i) {
    const Row node = m_nodes.at(i);
    if (node.context == context && node.sourceText == sourceText &&
        node.disambiguation == disambiguation) {
      return index(i, 0);
    }
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
  return QModelIndex();
}

TranslatorWrapper::TranslatorWrapper(QObject *parent)
    : QTranslator(parent), m_wrapped(0), m_model(new TranslationsModel(this))
{
}
TranslatorWrapper::TranslatorWrapper(QTranslator *wrapped, QObject *parent)
    : QTranslator(parent), m_wrapped(wrapped),
      m_model(new TranslationsModel(this))
{
	connect(wrapped, SIGNAL(destroyed()), SLOT(deleteLater()));
}
bool TranslatorWrapper::isEmpty() const
{
  return translator()->isEmpty();
}
QString TranslatorWrapper::translate(const char *context,
                                     const char *sourceText,
                                     const char *disambiguation, int n) const
{
  const QString translation =
      translateInternal(context, sourceText, disambiguation, n);

  if (QByteArray(context).startsWith("GammaRay::")) { //krazy:exclude=strings
    return translation;
  }
  // it's not for this translator
  if (translation.isNull()) {
    return translation;
  }
  return m_model->translation(context, sourceText, disambiguation, n,
                              translation);
}
QString TranslatorWrapper::translateInternal(const char *context,
                                             const char *sourceText,
                                             const char *disambiguation, int n)
    const
{
  return translator()->translate(context, sourceText, disambiguation, n);
}
const QTranslator *TranslatorWrapper::translator() const
{
  return m_wrapped == 0 ? this : m_wrapped;
}

FallbackTranslator::FallbackTranslator(QObject *parent)
  : QTranslator(parent)
{
  setObjectName("Fallback Translator");
}
QString FallbackTranslator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
  Q_UNUSED(context);
  Q_UNUSED(disambiguation);
  Q_UNUSED(n);
  return QString::fromUtf8(sourceText);
}
