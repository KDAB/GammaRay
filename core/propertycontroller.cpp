#include "propertycontroller.h"

#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"
#include "metapropertymodel.h"
#include "methodinvocationdialog.h"
#include "multisignalmapper.h"
#include "objectclassinfomodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectenummodel.h"
#include "objectmethodmodel.h"
#include "objectstaticpropertymodel.h"
#include "probe.h"

#include "remote/remotemodelserver.h"

#include <network/objectbroker.h>

#include <QStandardItemModel>

using namespace GammaRay;

PropertyController::PropertyController(const QString& baseName, QObject* parent) :
  QObject(parent),
  m_objectBaseName(baseName),
  m_staticPropertyModel(new ObjectStaticPropertyModel(this)),
  m_dynamicPropertyModel(new ObjectDynamicPropertyModel(this)),
  m_classInfoModel(new ObjectClassInfoModel(this)),
  m_methodModel(new ObjectMethodModel(this)),
  m_inboundConnectionModel(new ConnectionFilterProxyModel(this)),
  m_outboundConnectionModel(new ConnectionFilterProxyModel(this)),
  m_enumModel(new ObjectEnumModel(this)),
  m_signalMapper(0),
  m_methodLogModel(new QStandardItemModel(this)),
  m_metaPropertyModel(new MetaPropertyModel(this))
{
  registerModel(m_staticPropertyModel, "staticProperties");
  registerModel(m_dynamicPropertyModel, "dynamicProperties");
  registerModel(m_methodModel, "methods");
  registerModel(m_methodLogModel, "methodLog");
  registerModel(m_classInfoModel, "classInfo");
  registerModel(m_inboundConnectionModel, "inboundConnections");
  registerModel(m_outboundConnectionModel, "outboundConnections");
  registerModel(m_enumModel, "enums");
  registerModel(m_metaPropertyModel, "nonQProperties");
}

PropertyController::~PropertyController()
{
}

void PropertyController::registerModel(QAbstractItemModel* model, const QString& nameSuffix)
{
  RemoteModelServer *server = new RemoteModelServer(m_objectBaseName + "." + nameSuffix, this);
  server->setModel(model);
  ObjectBroker::registerModel(m_objectBaseName + "." + nameSuffix, model);
}

void PropertyController::setObject(QObject* object)
{
  m_object = object;
  m_staticPropertyModel->setObject(object);
  m_dynamicPropertyModel->setObject(object);
  m_inboundConnectionModel->filterReceiver(object);
  m_outboundConnectionModel->filterSender(object);

  const QMetaObject *metaObject = 0;
  if (object) {
    metaObject = object->metaObject();
  }
  m_enumModel->setMetaObject(metaObject);
  m_classInfoModel->setMetaObject(metaObject);
  m_methodModel->setMetaObject(metaObject);

#if 0 // TODO
  delete m_signalMapper;
  m_signalMapper = new MultiSignalMapper(this);
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int)), SLOT(signalEmitted(QObject*,int)));
#endif

  m_methodLogModel->clear();

  m_metaPropertyModel->setObject(object);
}

#include "propertycontroller.moc"
