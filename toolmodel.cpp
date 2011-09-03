#include "toolmodel.h"
#include "toolfactory.h"

#include "tools/codecbrowser/codecbrowser.h"
#include "tools/fontbrowser/fontbrowser.h"
#include "tools/scriptenginedebugger/scriptenginedebugger.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/webinspector/webinspector.h"

using namespace Endoscope;

ToolModel::ToolModel(QObject* parent): QAbstractListModel(parent)
{
  // built-in tools
  m_tools.push_back( new ScriptEngineDebuggerFactory );
  m_tools.push_back( new WebInspectorFactory );
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
