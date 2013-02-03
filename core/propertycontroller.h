#ifndef GAMMARAY_PROPERTYCONTROLLER_H
#define GAMMARAY_PROPERTYCONTROLLER_H

#include "include/gammaray_export.h"

#include <QObject>
#include <QPointer>

class QAbstractItemModel;
class QStandardItemModel;

namespace GammaRay {

class ConnectionFilterProxyModel;
class MultiSignalMapper;
class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class ObjectClassInfoModel;
class ObjectMethodModel;
class ObjectEnumModel;
class MetaPropertyModel;

/** Non-UI part of the property widget. */
class GAMMARAY_EXPORT PropertyController : public QObject
{
  Q_OBJECT
public:
  explicit PropertyController(const QString &baseName, QObject *parent);
  ~PropertyController();

  void setObject(QObject *object);
  void setObject(void *object, const QString &className);
  void setMetaObject(const QMetaObject *metaObject);

private:
  void registerModel(QAbstractItemModel *model, const QString &nameSuffix);

private slots:
  void signalEmitted(QObject *sender, int signalIndex);

private:
  QString m_objectBaseName;

  QPointer<QObject> m_object;
  ObjectStaticPropertyModel *m_staticPropertyModel;
  ObjectDynamicPropertyModel *m_dynamicPropertyModel;
  ObjectClassInfoModel *m_classInfoModel;
  ObjectMethodModel *m_methodModel;
  ConnectionFilterProxyModel *m_inboundConnectionModel;
  ConnectionFilterProxyModel *m_outboundConnectionModel;
  ObjectEnumModel *m_enumModel;
  MultiSignalMapper *m_signalMapper;
  QStandardItemModel *m_methodLogModel;
  MetaPropertyModel *m_metaPropertyModel;

};

}

#endif // GAMMARAY_PROPERTYCONTROLLER_H
