#include "clienttoolmodel.h"

#include <network/modelroles.h>
#include <include/toolfactory.h>

#include <QWidget>

using namespace GammaRay;

ClientToolModel::ClientToolModel(QObject* parent) : RemoteModel(QLatin1String("com.kdab.GammaRay.ToolModel"), parent)
{
  // TODO add tools
}

ClientToolModel::~ClientToolModel()
{
}

QVariant ClientToolModel::data(const QModelIndex& index, int role) const
{
  const QString toolId = RemoteModel::data(index, ToolModelRole::ToolId).toString();
  if (toolId.isEmpty())
    return QVariant();

  if (role == ToolModelRole::ToolFactory)
    return QVariant::fromValue(m_factories.value(toolId));
  if (role == ToolModelRole::ToolWidget) {
    const QHash<QString, QWidget*>::const_iterator it = m_widgets.constFind(toolId);
    if (it != m_widgets.constEnd())
      return QVariant::fromValue(it.value());
    ToolFactory *factory = m_factories.value(toolId);
    if (!factory)
      return QVariant();
    QWidget *widget = factory->createWidget(0, m_parentWidget);
    m_widgets.insert(toolId, widget);
    return QVariant::fromValue(widget);
  }

  return RemoteModel::data(index, role);
}

bool ClientToolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == ToolModelRole::ToolWidget) {
    const QString toolId = RemoteModel::data(index, ToolModelRole::ToolId).toString();
    Q_ASSERT(!toolId.isEmpty());
    Q_ASSERT(!m_widgets.contains(toolId));
    m_widgets.insert(toolId, value.value<QWidget*>());
    return true;
  } else if (role == ToolModelRole::ToolWidgetParent) {
    m_parentWidget = value.value<QWidget*>();
    return true;
  }

  return RemoteModel::setData(index, value, role);
}

void ClientToolModel::insertFactory(ToolFactory* factory)
{
  m_factories.insert(factory->id(), factory);
}


#include "clienttoolmodel.moc"
