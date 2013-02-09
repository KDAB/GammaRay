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

#include <QDebug>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTime>

using namespace GammaRay;

PropertyController::PropertyController(const QString& baseName, QObject* parent) :
  ObjectServer(baseName + ".controller", parent),
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

  ObjectBroker::selectionModel(m_methodModel); // trigger creation

  m_inboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());
  m_outboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());

  subscribeToSignal("activateMethod", this, "methodActivated");
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

void PropertyController::signalEmitted(QObject* sender, int signalIndex)
{
  Q_ASSERT(m_object == sender);
  m_methodLogModel->appendRow(
  new QStandardItem(tr("%1: Signal %2 emitted").
  arg(QTime::currentTime().toString("HH:mm:ss.zzz")).
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  arg(sender->metaObject()->method(signalIndex).signature())));
#else
  arg(QString(sender->metaObject()->method(signalIndex).methodSignature()))));
#endif
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

  delete m_signalMapper;
  m_signalMapper = new MultiSignalMapper(this);
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int)), SLOT(signalEmitted(QObject*,int)));

  m_methodLogModel->clear();

  m_metaPropertyModel->setObject(object);
}

void PropertyController::setObject(void* object, const QString& className)
{
  setObject(0);
  m_metaPropertyModel->setObject(object, className);
}

void PropertyController::setMetaObject(const QMetaObject* metaObject)
{
  setObject(0);
  m_enumModel->setMetaObject(metaObject);
  m_classInfoModel->setMetaObject(metaObject);
  m_methodModel->setMetaObject(metaObject);
}

void PropertyController::methodActivated()
{
  QItemSelectionModel* selectionModel = ObjectBroker::selectionModel(m_methodModel);
  if (selectionModel->selectedRows().size() != 1)
    return;
  const QModelIndex index = selectionModel->selectedRows().first();

  const QMetaMethod method = index.data(ObjectMethodModelRole::MetaMethod).value<QMetaMethod>();
  if (method.methodType() == QMetaMethod::Slot) {
#if 0 // TODO this needs to be split up, argument model goes here, dialog stays in the widget
    MethodInvocationDialog *dlg = new MethodInvocationDialog(this);
    dlg->setMethod(m_object.data(), method);
    dlg->show();
    // TODO: return value should go into ui->methodLog
#endif
  } else if (method.methodType() == QMetaMethod::Signal) {
    m_signalMapper->connectToSignal(m_object, method);
  }
}

#include "propertycontroller.moc"
