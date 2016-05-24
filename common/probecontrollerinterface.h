/*
  probecontrollerinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_PROBECONTROLLERINTERFACE_H
#define GAMMARAY_PROBECONTROLLERINTERFACE_H

#include <QObject>
#include <QDataStream>
#include <QDebug>
#include <QMetaType>
#include <QVector>

namespace GammaRay {

/** @brief Type-safe and cross-process object identifier. */
class ObjectId
{
public:
  enum Type
  {
    Invalid,
    QObjectType,
    VoidStarType
  };

  explicit ObjectId(void *obj, const QByteArray &typeName)
    : m_type(VoidStarType)
    , m_id(reinterpret_cast<quint64>(obj))
    , m_typeName(typeName)
  {}
  explicit ObjectId(QObject *obj)
    : m_type(QObjectType)
    , m_id(reinterpret_cast<quint64>(obj))
  {}
  explicit ObjectId()
    : m_type(Invalid)
    , m_id(0)
  {}
  inline bool isNull() const { return m_id == 0; }
  inline quint64 id() const { return m_id; }
  inline Type type() const { return m_type; }
  inline QByteArray typeName() const { return m_typeName; }

  inline QObject *asQObject()
  {
    Q_ASSERT(m_type == QObjectType);
    return reinterpret_cast<QObject *>(m_id);
  }
  inline void *asVoidStar()
  {
    Q_ASSERT(m_type == VoidStarType);
    return reinterpret_cast<void *>(m_id);
  }

  inline operator quint64() const { return m_id; }

private:
  friend QDataStream &operator<<(QDataStream &out, ObjectId id);
  friend QDataStream &operator>>(QDataStream &out, ObjectId &id);

  Type m_type;
  quint64 m_id;
  QByteArray m_typeName;
};

/** @brief GammaRay tool identifier. */
struct ToolInfo
{
  QString id;
  QString name;
};
typedef QVector<ToolInfo> ToolInfos;

/** @brief Probe and host process remote control functions. */
class ProbeControllerInterface : public QObject
{
  Q_OBJECT

public:
  explicit ProbeControllerInterface(QObject *parent = nullptr);
  virtual ~ProbeControllerInterface();

  virtual void selectObject(ObjectId id, const QString &toolId) = 0;

  virtual void requestSupportedTools(ObjectId id) = 0;

  /** Terminate host application. */
  virtual void quitHost() = 0;

  /** Detach GammaRay but keep host application running. */
  virtual void detachProbe() = 0;

Q_SIGNALS:
  void supportedToolsResponse(GammaRay::ObjectId id, const GammaRay::ToolInfos& toolInfos);

private:
  Q_DISABLE_COPY(ProbeControllerInterface)
};

inline QDebug &operator<<(QDebug dbg, ObjectId id)
{
  dbg.nospace() << "ObjectId(" << id.type() << ", " << id.id() << ", " << id.typeName() << ")";
  return dbg.space();
}

inline QDataStream &operator<<(QDataStream &out, ObjectId id)
{
  out << static_cast<quint8>(id.m_type);
  out << id.m_id;
  out << id.m_typeName;
  return out;
}

inline QDataStream &operator>>(QDataStream &in, ObjectId &id)
{
  quint8 u;
  in >> u;
  id.m_type = static_cast<ObjectId::Type>(u);
  in >> id.m_id;
  in >> id.m_typeName;
  return in;
}

inline QDataStream &operator<<(QDataStream &out, const ToolInfo &toolInfo)
{
  out << toolInfo.id;
  out << toolInfo.name;
  return out;
}

inline QDataStream &operator>>(QDataStream &in, ToolInfo &toolInfo)
{
  in >> toolInfo.id;
  in >> toolInfo.name;
  return in;
}

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ProbeControllerInterface, "com.kdab.GammaRay.ProbeControllerInterface")
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::ObjectId)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::ObjectId, Q_MOVABLE_TYPE);
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::ToolInfo)
Q_DECLARE_METATYPE(GammaRay::ToolInfos)

#endif // GAMMARAY_PROBECONTROLLERINTERFACE_H
