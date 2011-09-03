/*
  toolmodel.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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

#include "toolmodel.h"
#include "toolfactory.h"

#include "tools/codecbrowser/codecbrowser.h"
#include "tools/fontbrowser/fontbrowser.h"
#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/scriptenginedebugger/scriptenginedebugger.h"
#include "tools/selectionmodelinspector/selectionmodelinspector.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/webinspector/webinspector.h"

using namespace Endoscope;

ToolModel::ToolModel(QObject* parent): QAbstractListModel(parent)
{
  // built-in tools
  m_tools.push_back( new ScriptEngineDebuggerFactory );
  m_tools.push_back( new WebInspectorFactory );
  m_tools.push_back( new MetaTypeBrowserFactory );
  m_tools.push_back( new SelectionModelInspectorFactory );
  m_tools.push_back( new FontBrowserFactory );
  m_tools.push_back( new CodecBrowserFactory );
  m_tools.push_back( new TextDocumentInspectorFactory );

  // tool plugins

  // TODO
}

QVariant ToolModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ToolFactory *toolIface = m_tools.at( index.row() );
  if ( role == Qt::DisplayRole )
    return toolIface->name();
  else if ( role == ToolFactoryRole )
    return QVariant::fromValue( toolIface );
  else if ( role == ToolWidgetRole )
    return QVariant::fromValue( m_toolWidgets.value( toolIface ) );

  return QVariant();
}

bool ToolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if ( index.isValid() && role == Qt::EditRole ) {
    ToolFactory *toolIface = m_tools.at( index.row() );
    m_toolWidgets.insert( toolIface, value.value<QWidget*>() );
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

int ToolModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return m_tools.size();
}

#include "toolmodel.moc"
