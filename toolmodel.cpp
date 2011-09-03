#include "toolmodel.h"
#include "toolinterface.h"

#include "tools/scriptenginedebugger/scriptenginedebugger.h"
#include "tools/webinspector/webinspector.h"

using namespace Endoscope;

ToolModel::ToolModel(QObject* parent): QAbstractListModel(parent)
{
  // built-in tools
  m_tools.push_back( new ScriptEngineDebuggerInterface );
  m_tools.push_back( new WebInspectorInterface );

  // tool plugins

  // TODO
}

QVariant ToolModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ToolInterface *toolIface = m_tools.at( index.row() );
  if ( role == Qt::DisplayRole )
    return toolIface->name();
  else if ( role == ToolInterfaceRole )
    return QVariant::fromValue( toolIface );
  else if ( role == ToolWidgetRole )
    return QVariant::fromValue( m_toolWidgets.value( toolIface ) );

  return QVariant();
}

bool ToolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if ( index.isValid() && role == Qt::EditRole ) {
    ToolInterface *toolIface = m_tools.at( index.row() );
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
