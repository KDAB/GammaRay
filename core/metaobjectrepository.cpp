/*
  metaobjectrepository.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metaobjectrepository.h"
#include "metaobject.h"

#include <common/metatypedeclarations.h>

#include <QAbstractSocket>
#include <QFile>
#include <QNetworkProxy>
#include <QObject>
#include <QPalette>
#include <QPen>
#include <QSocketNotifier>
#include <QTcpServer>
#include <QThread>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QOpenGLContext>
#include <QOpenGLShader>
#include <QScreen>
#include <QSurface>
#include <QWindow>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
#include <QSaveFile>
#endif

using namespace GammaRay;

namespace GammaRay {

class StaticMetaObjectRepository : public MetaObjectRepository
{
  public:
    StaticMetaObjectRepository() : MetaObjectRepository()
    {
    }
};

}

Q_GLOBAL_STATIC(StaticMetaObjectRepository, s_instance)

MetaObjectRepository::MetaObjectRepository() : m_initialized(false)
{
}

MetaObjectRepository::~MetaObjectRepository()
{
  qDeleteAll(m_metaObjects);
}

void MetaObjectRepository::initBuiltInTypes()
{
  m_initialized = true;
  initQObjectTypes();
  initIOTypes();
  initNetworkTypes();
  initGuiTypes();
  initOpenGLTypes();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QThread*)
#endif
Q_DECLARE_METATYPE(QThread::Priority)

void MetaObjectRepository::initQObjectTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT0(QObject);
  MO_ADD_PROPERTY_RO(QObject, QObject*, parent);
  MO_ADD_PROPERTY_RO(QObject, bool, signalsBlocked); // TODO setter has non-void return type
  MO_ADD_PROPERTY_RO(QObject, QThread*, thread);

  MO_ADD_METAOBJECT1(QThread, QObject)
  MO_ADD_PROPERTY_RO(QThread, bool, isFinished);
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  MO_ADD_PROPERTY_RO(QThread, bool, isInterruptionRequested);
#endif
  MO_ADD_PROPERTY_RO(QThread, bool, isRunning);
  MO_ADD_PROPERTY   (QThread, QThread::Priority, priority, setPriority);
  MO_ADD_PROPERTY   (QThread, uint, stackSize, setStackSize);

  MO_ADD_METAOBJECT0(QPaintDevice);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, colorCount);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, heightMM);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiX);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiY);
  MO_ADD_PROPERTY_RO(QPaintDevice, bool, paintingActive);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiX);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiY);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, widthMM);
}


Q_DECLARE_METATYPE(QAbstractSocket::SocketType)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QIODevice::OpenMode)
Q_DECLARE_METATYPE(QSocketNotifier::Type)

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QAbstractSocket::PauseModes)
Q_DECLARE_METATYPE(QFileDevice::FileError)
Q_DECLARE_METATYPE(QFileDevice::Permissions)
#else // !Qt5
Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
Q_DECLARE_METATYPE(QAbstractSocket::SocketState)
#ifndef QT_NO_NETWORKPROXY
Q_DECLARE_METATYPE(QNetworkProxy)
#endif
#endif

void MetaObjectRepository::initIOTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QIODevice, QObject);
  MO_ADD_PROPERTY_RO(QIODevice, QIODevice::OpenMode, openMode);
  MO_ADD_PROPERTY   (QIODevice, bool, isTextModeEnabled, setTextModeEnabled);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isOpen);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isReadable);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isWritable);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isSequential);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, pos);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, size);
  MO_ADD_PROPERTY_RO(QIODevice, bool, atEnd);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, bytesAvailable);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, bytesToWrite);
  MO_ADD_PROPERTY_RO(QIODevice, bool, canReadLine);
  MO_ADD_PROPERTY_RO(QIODevice, QString, errorString);

  // FIXME: QIODevice::readAll() would be nice to have

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_METAOBJECT1(QFileDevice, QIODevice);
  MO_ADD_PROPERTY_RO(QFileDevice, QFileDevice::FileError, error);
  MO_ADD_PROPERTY_RO(QFileDevice, QString, fileName);
  MO_ADD_PROPERTY_RO(QFileDevice, int, handle);
  MO_ADD_PROPERTY_RO(QFileDevice, QFileDevice::Permissions, permissions);

  MO_ADD_METAOBJECT1(QFile, QFileDevice);
  MO_ADD_PROPERTY_RO(QFile, bool, exists);
  MO_ADD_PROPERTY_RO(QFile, QString, symLinkTarget);

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
  MO_ADD_METAOBJECT1(QSaveFile, QFileDevice);
#endif
#endif
}


void MetaObjectRepository::initNetworkTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QAbstractSocket, QIODevice);
  MO_ADD_PROPERTY_RO(QAbstractSocket, bool, isValid);
  MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, localPort);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, localAddress);
  MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, peerPort);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, peerAddress);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QString, peerName);
  MO_ADD_PROPERTY   (QAbstractSocket, qint64, readBufferSize, setReadBufferSize);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_PROPERTY   (QAbstractSocket, QAbstractSocket::PauseModes, pauseMode, setPauseMode);
  MO_ADD_PROPERTY_RO(QAbstractSocket, qintptr, socketDescriptor);
#else // !Qt5
  MO_ADD_PROPERTY_RO(QAbstractSocket, int, socketDescriptor);
#endif
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketType, socketType);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketState, state);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketError, error);
#ifndef QT_NO_NETWORKPROXY
  MO_ADD_PROPERTY_RO(QAbstractSocket, QNetworkProxy, proxy);
#endif

  // FIXME: QAbstractSocket::setSocketOption() would be nice to have
  // FIXME: QQAbstractSocket::socketOption() would be nice to have

  MO_ADD_METAOBJECT1(QTcpServer, QObject);
  MO_ADD_PROPERTY_RO(QTcpServer, bool, isListening);
  MO_ADD_PROPERTY   (QTcpServer, int, maxPendingConnections, setMaxPendingConnections);
  MO_ADD_PROPERTY_RO(QTcpServer, quint16, serverPort);
  MO_ADD_PROPERTY_RO(QTcpServer, QHostAddress, serverAddress);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_PROPERTY_RO(QTcpServer, qintptr, socketDescriptor);
#else // !QT5
  MO_ADD_PROPERTY_RO(QTcpServer, int, socketDescriptor);
#endif
  MO_ADD_PROPERTY_RO(QTcpServer, bool, hasPendingConnections);
  MO_ADD_PROPERTY_RO(QTcpServer, QAbstractSocket::SocketError, serverError);
  MO_ADD_PROPERTY_RO(QTcpServer, QString, errorString);
#ifndef QT_NO_NETWORKPROXY
  MO_ADD_PROPERTY_RO(QTcpServer, QNetworkProxy, proxy);
#endif

  MO_ADD_METAOBJECT1(QSocketNotifier, QObject);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_PROPERTY_RO(QSocketNotifier, qintptr, socket);
#else // !Qt5
  MO_ADD_PROPERTY_RO(QSocketNotifier, int, socket);
#endif
  MO_ADD_PROPERTY_RO(QSocketNotifier, QSocketNotifier::Type, type);
  MO_ADD_PROPERTY   (QSocketNotifier, bool, isEnabled, setEnabled);
}

void MetaObjectRepository::initGuiTypes()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT0(QSurface);
  MO_ADD_PROPERTY_RO(QSurface, QSurfaceFormat, format);
  MO_ADD_PROPERTY_RO(QSurface, QSize, size);
  MO_ADD_PROPERTY_RO(QSurface, QSurface::SurfaceClass, surfaceClass);
  MO_ADD_PROPERTY_RO(QSurface, QSurface::SurfaceType, surfaceType);

  MO_ADD_METAOBJECT2(QWindow, QObject, QSurface);
  MO_ADD_PROPERTY_CR(QWindow, QSize, baseSize, setBaseSize);
#ifndef QT_NO_CURSOR
  MO_ADD_PROPERTY_CR(QWindow, QCursor, cursor, setCursor);
#endif
  MO_ADD_PROPERTY_RO(QWindow, qreal, devicePixelRatio);
  MO_ADD_PROPERTY_CR(QWindow, QString, filePath, setFilePath);
  MO_ADD_PROPERTY_RO(QWindow, QObject*, focusObject);
  MO_ADD_PROPERTY_RO(QWindow, QRect, frameGeometry);
  MO_ADD_PROPERTY_RO(QWindow, QMargins, frameMargins);
  MO_ADD_PROPERTY_CR(QWindow, QPoint, framePosition, setFramePosition);
  MO_ADD_PROPERTY_CR(QWindow, QRect, geometry, setGeometry);
  MO_ADD_PROPERTY_CR(QWindow, QIcon, icon, setIcon);
  MO_ADD_PROPERTY_RO(QWindow, bool, isExposed);
  MO_ADD_PROPERTY_RO(QWindow, bool, isTopLevel);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
  MO_ADD_PROPERTY_CR(QWindow, QRegion, mask, setMask);
#endif
  MO_ADD_PROPERTY_CR(QWindow, QPoint, position, setPosition);
  MO_ADD_PROPERTY_RO(QWindow, QSurfaceFormat, requestedFormat);
  MO_ADD_PROPERTY_RO(QWindow, QScreen*, screen);
  MO_ADD_PROPERTY_CR(QWindow, QSize, sizeIncrement, setSizeIncrement);
  MO_ADD_PROPERTY   (QWindow, Qt::WindowState, windowState, setWindowState);
  MO_ADD_PROPERTY_RO(QWindow, QWindow*, transientParent);
  MO_ADD_PROPERTY_RO(QWindow, Qt::WindowType, type);
#endif
}

void MetaObjectRepository::initOpenGLTypes()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QOpenGLShader, QObject);
  MO_ADD_PROPERTY_RO(QOpenGLShader, bool, isCompiled);
  MO_ADD_PROPERTY_RO(QOpenGLShader, QString, log);
  MO_ADD_PROPERTY_RO(QOpenGLShader, uint, shaderId);
  MO_ADD_PROPERTY_RO(QOpenGLShader, QOpenGLShader::ShaderType, shaderType);
  MO_ADD_PROPERTY_RO(QOpenGLShader, QByteArray, sourceCode);

  MO_ADD_METAOBJECT1(QOpenGLShaderProgram, QObject);
  MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, bool, isLinked);
  MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, QString, log);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
// FIXME calling this asserts in debug builds of some newer Qt versions
//   MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, int, maxGeometryOutputVertices);
  MO_ADD_PROPERTY   (QOpenGLShaderProgram, int, patchVertexCount, setPatchVertexCount);
#endif
  MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, uint, programId);

  MO_ADD_METAOBJECT1(QOpenGLContext, QObject);
  MO_ADD_PROPERTY_RO(QOpenGLContext, uint, defaultFramebufferObject);
  MO_ADD_PROPERTY_RO(QOpenGLContext, QSet<QByteArray>, extensions);
  MO_ADD_PROPERTY_RO(QOpenGLContext, QSurfaceFormat, format);
  MO_ADD_PROPERTY_RO(QOpenGLContext, bool, isValid);
  MO_ADD_PROPERTY_RO(QOpenGLContext, QScreen*, screen);
  MO_ADD_PROPERTY_RO(QOpenGLContext, QOpenGLContext*, shareContext);
  MO_ADD_PROPERTY_RO(QOpenGLContext, QOpenGLContextGroup*, shareGroup);
//   MO_ADD_PROPERTY_RO(QOpenGLContext, QSurface*, surface);
#endif
}

MetaObjectRepository *MetaObjectRepository::instance()
{
  if (!s_instance()->m_initialized)
    s_instance()->initBuiltInTypes();
  return s_instance();
}

void MetaObjectRepository::addMetaObject(MetaObject *mo)
{
  Q_ASSERT(!mo->className().isEmpty());
  m_metaObjects.insert(mo->className(), mo);
}

MetaObject *MetaObjectRepository::metaObject(const QString &typeName) const
{
  QString typeName_ = typeName;
  typeName_.remove('*');
  typeName_.remove('&');
  typeName_.remove("const ");
  typeName_.remove(" const");
  typeName_.remove(' ');
  return m_metaObjects.value(typeName_);
}

bool MetaObjectRepository::hasMetaObject(const QString &typeName) const
{
  return m_metaObjects.contains(typeName);
}
