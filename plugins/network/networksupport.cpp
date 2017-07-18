/*
  networksupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

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

#include "networksupport.h"
#include "networkinterfacemodel.h"
#include "cookies/cookieextension.h"

#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/propertycontroller.h>
#include <core/varianthandler.h>

#include <QAbstractNetworkCache>
#include <QHostAddress>
#include <QLocalSocket>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkProxy>
#include <QSocketNotifier>
#include <QSslCertificateExtension>
#include <QSslCipher>
#include <QSslKey>
#include <QSslSocket>
#include <QTcpServer>
#include <QTcpSocket>

using namespace GammaRay;

Q_DECLARE_METATYPE(QAbstractSocket::PauseModes)
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(QAbstractSocket::NetworkLayerProtocol)
Q_DECLARE_METATYPE(QAbstractSocket::SocketType)
#endif
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketError)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketState)
Q_DECLARE_METATYPE(QNetworkAccessManager::NetworkAccessibility)
Q_DECLARE_METATYPE(QSocketNotifier::Type)
#ifndef QT_NO_SSL
Q_DECLARE_METATYPE(QSsl::KeyAlgorithm)
Q_DECLARE_METATYPE(QSsl::KeyType)
Q_DECLARE_METATYPE(QSsl::SslProtocol)
Q_DECLARE_METATYPE(QSslCertificateExtension)
Q_DECLARE_METATYPE(QSslCipher)
Q_DECLARE_METATYPE(QSslConfiguration::NextProtocolNegotiationStatus)
Q_DECLARE_METATYPE(QSslError)
Q_DECLARE_METATYPE(QSslKey)
Q_DECLARE_METATYPE(QSslSocket::PeerVerifyMode)
Q_DECLARE_METATYPE(QSslSocket::SslMode)
#endif // QT_NO_SSL

NetworkSupport::NetworkSupport(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    registerMetaTypes();
    registerVariantHandler();

    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.NetworkInterfaceModel"),
                         new NetworkInterfaceModel(this));

    PropertyController::registerExtension<CookieExtension>();
}

NetworkSupport::~NetworkSupport()
{
}

void NetworkSupport::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QAbstractSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QAbstractSocket, bool, isValid);
    MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, localPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, localAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, peerPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, peerAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QString, peerName);
    MO_ADD_PROPERTY(QAbstractSocket, qint64, readBufferSize, setReadBufferSize);
    MO_ADD_PROPERTY(QAbstractSocket, QAbstractSocket::PauseModes, pauseMode, setPauseMode);
    MO_ADD_PROPERTY_RO(QAbstractSocket, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketType, socketType);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketState, state);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketError, error);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QAbstractSocket, QNetworkProxy, proxy);
#endif

    MO_ADD_METAOBJECT0(QHostAddress);
    MO_ADD_PROPERTY_RO(QHostAddress, bool, isLoopback);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY_RO(QHostAddress, bool, isMulticast);
#endif
    MO_ADD_PROPERTY_RO(QHostAddress, bool, isNull);
    MO_ADD_PROPERTY_RO(QHostAddress, QAbstractSocket::NetworkLayerProtocol, protocol);
    MO_ADD_PROPERTY_CR(QHostAddress, QString, scopeId, setScopeId);

    MO_ADD_METAOBJECT1(QLocalSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QLocalSocket, QLocalSocket::LocalSocketError, error);
    MO_ADD_PROPERTY_RO(QLocalSocket, QString, fullServerName);
    MO_ADD_PROPERTY_RO(QLocalSocket, bool, isValid);
    MO_ADD_PROPERTY_RO(QLocalSocket, QString, serverName);
    MO_ADD_PROPERTY_RO(QLocalSocket, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QLocalSocket, QLocalSocket::LocalSocketState, state);

    MO_ADD_METAOBJECT1(QNetworkAccessManager, QObject);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, QAbstractNetworkCache *, cache);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, QNetworkCookieJar *, cookieJar);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, QStringList, supportedSchemes);

    // FIXME: QAbstractSocket::setSocketOption() would be nice to have
    // FIXME: QQAbstractSocket::socketOption() would be nice to have

    MO_ADD_METAOBJECT1(QTcpServer, QObject);
    MO_ADD_PROPERTY_RO(QTcpServer, bool, isListening);
    MO_ADD_PROPERTY(QTcpServer, int, maxPendingConnections, setMaxPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, quint16, serverPort);
    MO_ADD_PROPERTY_RO(QTcpServer, QHostAddress, serverAddress);
    MO_ADD_PROPERTY_RO(QTcpServer, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QTcpServer, bool, hasPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, QAbstractSocket::SocketError, serverError);
    MO_ADD_PROPERTY_RO(QTcpServer, QString, errorString);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QTcpServer, QNetworkProxy, proxy);
#endif

    MO_ADD_METAOBJECT1(QTcpSocket, QAbstractSocket);

#ifndef QT_NO_SSL
    MO_ADD_METAOBJECT0(QSslCertificate);
    MO_ADD_PROPERTY_RO(QSslCertificate, QDateTime, effectiveDate);
    MO_ADD_PROPERTY_RO(QSslCertificate, QDateTime, expiryDate);
    MO_ADD_PROPERTY_RO(QSslCertificate, QList<QSslCertificateExtension>, extensions);
    MO_ADD_PROPERTY_RO(QSslCertificate, bool, isBlacklisted);
    MO_ADD_PROPERTY_RO(QSslCertificate, bool, isNull);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QSslCertificate, bool, isSelfSigned);
#endif
    MO_ADD_PROPERTY_RO(QSslCertificate, QList<QByteArray>, issuerInfoAttributes);
    MO_ADD_PROPERTY_RO(QSslCertificate, QSslKey, publicKey);
    MO_ADD_PROPERTY_RO(QSslCertificate, QByteArray, serialNumber);
    MO_ADD_PROPERTY_RO(QSslCertificate, QList<QByteArray>, subjectInfoAttributes);
    MO_ADD_PROPERTY_RO(QSslCertificate, QByteArray, version);

    MO_ADD_METAOBJECT0(QSslCertificateExtension);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, bool, isCritical);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, bool, isSupported);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, QString, name);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, QString, oid);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, QVariant, value);

    MO_ADD_METAOBJECT0(QSslCipher);
    MO_ADD_PROPERTY_RO(QSslCipher, QString, authenticationMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, QString, encryptionMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, bool, isNull);
    MO_ADD_PROPERTY_RO(QSslCipher, QString, keyExchangeMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, QString, name);
    MO_ADD_PROPERTY_RO(QSslCipher, QSsl::SslProtocol, protocol);
    MO_ADD_PROPERTY_RO(QSslCipher, QString, protocolString);
    MO_ADD_PROPERTY_RO(QSslCipher, int, usedBits);

    MO_ADD_METAOBJECT0(QSslConfiguration);
    MO_ADD_PROPERTY(QSslConfiguration, QList<QByteArray>, allowedNextProtocols,
                    setAllowedNextProtocols);
    MO_ADD_PROPERTY_CR(QSslConfiguration, QList<QSslCertificate>, caCertificates,
                       setCaCertificates);
    MO_ADD_PROPERTY_CR(QSslConfiguration, QList<QSslCipher>, ciphers, setCiphers);
    // TODO 5.5 ellipticCurves
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(QSslConfiguration, QSslKey, ephemeralServerKey);
#endif
    MO_ADD_PROPERTY_RO(QSslConfiguration, bool, isNull);
    MO_ADD_PROPERTY_CR(QSslConfiguration, QSslCertificate, localCertificate, setLocalCertificate);
    MO_ADD_PROPERTY_CR(QSslConfiguration, QList<QSslCertificate>, localCertificateChain,
                       setLocalCertificateChain);
    MO_ADD_PROPERTY_RO(QSslConfiguration, QByteArray, nextNegotiatedProtocol);
    MO_ADD_PROPERTY_RO(QSslConfiguration, QSslConfiguration::NextProtocolNegotiationStatus,
                       nextProtocolNegotiationStatus);                                                                      // TODO enum lookup table
    MO_ADD_PROPERTY_RO(QSslConfiguration, QSslCertificate, peerCertificate);
    MO_ADD_PROPERTY_RO(QSslConfiguration, QList<QSslCertificate>, peerCertificateChain);
    MO_ADD_PROPERTY(QSslConfiguration, int, peerVerifyDepth, setPeerVerifyDepth);
    MO_ADD_PROPERTY(QSslConfiguration, QSslSocket::PeerVerifyMode, peerVerifyMode,
                    setPeerVerifyMode);
    MO_ADD_PROPERTY_CR(QSslConfiguration, QSslKey, privateKey, setPrivateKey);
    MO_ADD_PROPERTY(QSslConfiguration, QSsl::SslProtocol, protocol, setProtocol);
    MO_ADD_PROPERTY_RO(QSslConfiguration, QSslCipher, sessionCipher);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QSslConfiguration, QSsl::SslProtocol, sessionProtocol);
#endif
    MO_ADD_PROPERTY_CR(QSslConfiguration, QByteArray, sessionTicket, setSessionTicket);
    MO_ADD_PROPERTY_RO(QSslConfiguration, int, sessionTicketLifeTimeHint);
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    MO_ADD_PROPERTY_ST(QSslConfiguration, QList<QSslCertificate>, systemCaCertificates);
#endif

    MO_ADD_METAOBJECT0(QSslKey);
    MO_ADD_PROPERTY_RO(QSslKey, QSsl::KeyAlgorithm, algorithm);
    MO_ADD_PROPERTY_RO(QSslKey, bool, isNull);
    MO_ADD_PROPERTY_RO(QSslKey, int, length);
    MO_ADD_PROPERTY_RO(QSslKey, QSsl::KeyType, type);

    MO_ADD_METAOBJECT1(QSslSocket, QTcpSocket);
    MO_ADD_PROPERTY_RO(QSslSocket, bool, isEncrypted);
    MO_ADD_PROPERTY_RO(QSslSocket, QSslCertificate, localCertificate);
    MO_ADD_PROPERTY_RO(QSslSocket, QList<QSslCertificate>, localCertificateChain);
    MO_ADD_PROPERTY_RO(QSslSocket, QSslSocket::SslMode, mode);
    MO_ADD_PROPERTY_RO(QSslSocket, QSslCertificate, peerCertificate);
    MO_ADD_PROPERTY_RO(QSslSocket, QList<QSslCertificate>, peerCertificateChain);
    MO_ADD_PROPERTY(QSslSocket, int, peerVerifyDepth, setPeerVerifyDepth);
    MO_ADD_PROPERTY(QSslSocket, QSslSocket::PeerVerifyMode, peerVerifyMode, setPeerVerifyMode);
    MO_ADD_PROPERTY_CR(QSslSocket, QString, peerVerifyName, setPeerVerifyName);
    MO_ADD_PROPERTY_CR(QSslSocket, QSslKey, privateKey, setPrivateKey);
    MO_ADD_PROPERTY_RO(QSslSocket, QSsl::SslProtocol, protocol);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QSslSocket, QSsl::SslProtocol, sessionProtocol);
#endif
    MO_ADD_PROPERTY_RO(QSslSocket, QSslCipher, sessionCipher);
    MO_ADD_PROPERTY_CR(QSslSocket, QSslConfiguration, sslConfiguration, setSslConfiguration);
    MO_ADD_PROPERTY_RO(QSslSocket, QList<QSslError>, sslErrors);
#endif // QT_NO_SSL

    MO_ADD_METAOBJECT1(QSocketNotifier, QObject);
    MO_ADD_PROPERTY_RO(QSocketNotifier, qintptr, socket);
    MO_ADD_PROPERTY_RO(QSocketNotifier, QSocketNotifier::Type, type);
    MO_ADD_PROPERTY(QSocketNotifier, bool, isEnabled, setEnabled);
}

#define E(x) { QAbstractSocket:: x, #x }
static const MetaEnum::Value<QAbstractSocket::PauseMode> socket_pause_mode_table[] = {
    E(PauseNever),
    E(PauseOnSslErrors),
};
#undef E

static QString socketPauseModeToString(QAbstractSocket::PauseModes flags)
{
    return MetaEnum::flagsToString(flags, socket_pause_mode_table);
}

#define E(x) { QNetworkAccessManager:: x, #x }
static const MetaEnum::Value<QNetworkAccessManager::NetworkAccessibility>
network_accessibility_table[] = {
    E(UnknownAccessibility),
    E(NotAccessible),
    E(Accessible)
};
#undef E

static QString networkAccessibilityToString(QNetworkAccessManager::NetworkAccessibility value)
{
    return MetaEnum::enumToString(value, network_accessibility_table);
}

#ifndef QT_NO_SSL
#define E(x) { QSslSocket:: x, #x }
static const MetaEnum::Value<QSslSocket::SslMode> ssl_mode_table[] = {
    E(UnencryptedMode),
    E(SslClientMode),
    E(SslServerMode)
};
#undef E

static QString sslModeToString(QSslSocket::SslMode value)
{
    return MetaEnum::enumToString(value, ssl_mode_table);
}

#define E(x) { QSslSocket:: x, #x }
static const MetaEnum::Value<QSslSocket::PeerVerifyMode> ssl_peer_verify_mode_table[] = {
    E(VerifyNone),
    E(QueryPeer),
    E(VerifyPeer),
    E(AutoVerifyPeer)
};
#undef E

static QString sslPeerVerifyModeToString(QSslSocket::PeerVerifyMode value)
{
    return MetaEnum::enumToString(value, ssl_peer_verify_mode_table);
}

#define E(x) { QSsl:: x, #x }
static const MetaEnum::Value<QSsl::KeyAlgorithm> ssl_key_algorithm_table[] = {
    E(Opaque),
    E(Rsa),
    E(Dsa),
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    E(Ec)
#endif
};
#undef E

static QString sslKeyAlgorithmToString(QSsl::KeyAlgorithm value)
{
    return MetaEnum::enumToString(value, ssl_key_algorithm_table);
}

#define E(x) { QSsl:: x, #x }
static const MetaEnum::Value<QSsl::KeyType> ssl_key_type_table[] = {
    E(PrivateKey),
    E(PublicKey)
};
#undef E

static QString sslKeyTypeToString(QSsl::KeyType value)
{
    return MetaEnum::enumToString(value, ssl_key_type_table);
}

#define E(x) { QSsl:: x, #x }
static const MetaEnum::Value<QSsl::SslProtocol> ssl_protocol_table[] = {
    E(SslV3),
    E(SslV2),
    E(TlsV1_0),
    E(TlsV1_1),
    E(TlsV1_2),
    E(AnyProtocol),
    E(TlsV1SslV3),
    E(SecureProtocols),
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    E(TlsV1_0OrLater),
    E(TlsV1_1OrLater),
    E(TlsV1_2OrLater),
#endif
    E(UnknownProtocol)
};
#undef E

static QString sslProtocolToString(QSsl::SslProtocol value)
{
    return MetaEnum::enumToString(value, ssl_protocol_table);
}

static QString sslCertificateToString(const QSslCertificate &cert)
{
    if (cert.isNull())
        return QStringLiteral("<null>");
    return cert.digest().toHex();
}

static QString sslCertificateExtensionToString(const QSslCertificateExtension &ext)
{
    return ext.name();
}

static QString sslCipherToString(const QSslCipher &cipher)
{
    return cipher.name();
}

static QString sslErrorToString(const QSslError &error)
{
    return error.errorString();
}

#endif // QT_NO_SSL

void NetworkSupport::registerVariantHandler()
{
    VariantHandler::registerStringConverter<QAbstractSocket::PauseModes>(socketPauseModeToString);
    VariantHandler::registerStringConverter<QHostAddress>([](const QHostAddress &addr) {
        return addr.toString();
    });
    VariantHandler::registerStringConverter<QNetworkAccessManager::NetworkAccessibility>(
        networkAccessibilityToString);
#ifndef QT_NO_SSL
    VariantHandler::registerStringConverter<QSslSocket::PeerVerifyMode>(sslPeerVerifyModeToString);
    VariantHandler::registerStringConverter<QSslSocket::SslMode>(sslModeToString);
    VariantHandler::registerStringConverter<QSsl::KeyAlgorithm>(sslKeyAlgorithmToString);
    VariantHandler::registerStringConverter<QSsl::KeyType>(sslKeyTypeToString);
    VariantHandler::registerStringConverter<QSsl::SslProtocol>(sslProtocolToString);
    VariantHandler::registerStringConverter<QSslCertificate>(sslCertificateToString);
    VariantHandler::registerStringConverter<QSslCertificateExtension>(
        sslCertificateExtensionToString);
    VariantHandler::registerStringConverter<QSslCipher>(sslCipherToString);
    VariantHandler::registerStringConverter<QSslError>(sslErrorToString);
#endif
}

NetworkSupportFactory::NetworkSupportFactory(QObject *parent)
    : QObject(parent)
{
}
