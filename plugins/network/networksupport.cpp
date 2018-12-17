/*
  networksupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "networkconfigurationmodel.h"
#include "cookies/cookieextension.h"

#include <core/enumrepositoryserver.h>
#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/propertycontroller.h>
#include <core/remote/serverproxymodel.h>
#include <core/varianthandler.h>

#include <QAbstractNetworkCache>
#include <QHostAddress>
#include <QLocalSocket>
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkCookieJar>
#include <QNetworkProxy>
#include <QSocketNotifier>
#include <QSslCertificateExtension>
#include <QSslCipher>
#include <QSslKey>
#include <QSslSocket>
#include <QTcpServer>
#include <QTcpSocket>

#include <functional>

using namespace GammaRay;

Q_DECLARE_METATYPE(QAbstractSocket::PauseModes)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketError)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketState)
Q_DECLARE_METATYPE(QNetworkAccessManager::NetworkAccessibility)
Q_DECLARE_METATYPE(QNetworkConfigurationManager::Capabilities)
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

NetworkSupport::NetworkSupport(Probe *probe, QObject *parent)
    : QObject(parent)
{
    registerMetaTypes();
    registerVariantHandler();

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.NetworkInterfaceModel"), new NetworkInterfaceModel(this));

    auto configProxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    configProxy->setSourceModel(new NetworkConfigurationModel(this));
    configProxy->addRole(NetworkConfigurationModelRoles::DefaultConfigRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.NetworkConfigurationModel"), configProxy);

    PropertyController::registerExtension<CookieExtension>();
}

NetworkSupport::~NetworkSupport()
{
}

void NetworkSupport::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QAbstractSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QAbstractSocket, isValid);
    MO_ADD_PROPERTY_RO(QAbstractSocket, localPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, localAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, peerPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, peerAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, peerName);
    MO_ADD_PROPERTY(QAbstractSocket, readBufferSize, setReadBufferSize);
    MO_ADD_PROPERTY(QAbstractSocket, pauseMode, setPauseMode);
    MO_ADD_PROPERTY_RO(QAbstractSocket, socketDescriptor);
    MO_ADD_PROPERTY_RO(QAbstractSocket, socketType);
    MO_ADD_PROPERTY_RO(QAbstractSocket, state);
    MO_ADD_PROPERTY_RO(QAbstractSocket, error);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QAbstractSocket, proxy);
#endif
    // FIXME: QAbstractSocket::setSocketOption() would be nice to have
    // FIXME: QQAbstractSocket::socketOption() would be nice to have

    MO_ADD_METAOBJECT0(QHostAddress);
    MO_ADD_PROPERTY_RO(QHostAddress, isLoopback);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY_RO(QHostAddress, isMulticast);
#endif
    MO_ADD_PROPERTY_RO(QHostAddress, isNull);
    MO_ADD_PROPERTY_RO(QHostAddress, protocol);
    MO_ADD_PROPERTY(QHostAddress, scopeId, setScopeId);

    MO_ADD_METAOBJECT1(QLocalSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QLocalSocket, error);
    MO_ADD_PROPERTY_RO(QLocalSocket, fullServerName);
    MO_ADD_PROPERTY_RO(QLocalSocket, isValid);
    MO_ADD_PROPERTY_RO(QLocalSocket, serverName);
    MO_ADD_PROPERTY_RO(QLocalSocket, socketDescriptor);
    MO_ADD_PROPERTY_RO(QLocalSocket, state);

    MO_ADD_METAOBJECT1(QNetworkAccessManager, QObject);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, cache);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, cookieJar);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, supportedSchemes);

    MO_ADD_METAOBJECT1(QNetworkConfigurationManager, QObject);
    MO_ADD_PROPERTY_RO(QNetworkConfigurationManager, capabilities);
    MO_ADD_PROPERTY_RO(QNetworkConfigurationManager, isOnline);

    MO_ADD_METAOBJECT1(QTcpServer, QObject);
    MO_ADD_PROPERTY_RO(QTcpServer, isListening);
    MO_ADD_PROPERTY(QTcpServer, maxPendingConnections, setMaxPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, serverPort);
    MO_ADD_PROPERTY_RO(QTcpServer, serverAddress);
    MO_ADD_PROPERTY_RO(QTcpServer, socketDescriptor);
    MO_ADD_PROPERTY_RO(QTcpServer, hasPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, serverError);
    MO_ADD_PROPERTY_RO(QTcpServer, errorString);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QTcpServer, proxy);
#endif

    MO_ADD_METAOBJECT1(QTcpSocket, QAbstractSocket);

#ifndef QT_NO_SSL
    MO_ADD_METAOBJECT0(QSslCertificate);
    MO_ADD_PROPERTY_RO(QSslCertificate, effectiveDate);
    MO_ADD_PROPERTY_RO(QSslCertificate, expiryDate);
    MO_ADD_PROPERTY_RO(QSslCertificate, extensions);
    MO_ADD_PROPERTY_RO(QSslCertificate, isBlacklisted);
    MO_ADD_PROPERTY_RO(QSslCertificate, isNull);
    MO_ADD_PROPERTY_RO(QSslCertificate, isSelfSigned);
    MO_ADD_PROPERTY_RO(QSslCertificate, issuerInfoAttributes);
    MO_ADD_PROPERTY_RO(QSslCertificate, publicKey);
    MO_ADD_PROPERTY_RO(QSslCertificate, serialNumber);
    MO_ADD_PROPERTY_RO(QSslCertificate, subjectInfoAttributes);
    MO_ADD_PROPERTY_RO(QSslCertificate, version);

    MO_ADD_METAOBJECT0(QSslCertificateExtension);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, isCritical);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, isSupported);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, name);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, oid);
    MO_ADD_PROPERTY_RO(QSslCertificateExtension, value);

    MO_ADD_METAOBJECT0(QSslCipher);
    MO_ADD_PROPERTY_RO(QSslCipher, authenticationMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, encryptionMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, isNull);
    MO_ADD_PROPERTY_RO(QSslCipher, keyExchangeMethod);
    MO_ADD_PROPERTY_RO(QSslCipher, name);
    MO_ADD_PROPERTY_RO(QSslCipher, protocol);
    MO_ADD_PROPERTY_RO(QSslCipher, protocolString);
    MO_ADD_PROPERTY_RO(QSslCipher, usedBits);

    MO_ADD_METAOBJECT0(QSslConfiguration);
    MO_ADD_PROPERTY(QSslConfiguration, allowedNextProtocols, setAllowedNextProtocols);
    MO_ADD_PROPERTY(QSslConfiguration, caCertificates, setCaCertificates);
    MO_ADD_PROPERTY(QSslConfiguration, ciphers, setCiphers);
    // TODO 5.5 ellipticCurves
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(QSslConfiguration, ephemeralServerKey);
#endif
    MO_ADD_PROPERTY_RO(QSslConfiguration, isNull);
    MO_ADD_PROPERTY(QSslConfiguration, localCertificate, setLocalCertificate);
    MO_ADD_PROPERTY(QSslConfiguration, localCertificateChain, setLocalCertificateChain);
    MO_ADD_PROPERTY_RO(QSslConfiguration, nextNegotiatedProtocol);
    MO_ADD_PROPERTY_RO(QSslConfiguration, nextProtocolNegotiationStatus); // TODO enum lookup table
    MO_ADD_PROPERTY_RO(QSslConfiguration, peerCertificate);
    MO_ADD_PROPERTY_RO(QSslConfiguration, peerCertificateChain);
    MO_ADD_PROPERTY(QSslConfiguration, peerVerifyDepth, setPeerVerifyDepth);
    MO_ADD_PROPERTY(QSslConfiguration, peerVerifyMode, setPeerVerifyMode);
    MO_ADD_PROPERTY(QSslConfiguration, privateKey, setPrivateKey);
    MO_ADD_PROPERTY(QSslConfiguration, protocol, setProtocol);
    MO_ADD_PROPERTY_RO(QSslConfiguration, sessionCipher);
    MO_ADD_PROPERTY_RO(QSslConfiguration, sessionProtocol);
    MO_ADD_PROPERTY(QSslConfiguration, sessionTicket, setSessionTicket);
    MO_ADD_PROPERTY_RO(QSslConfiguration, sessionTicketLifeTimeHint);
    MO_ADD_PROPERTY_ST(QSslConfiguration, systemCaCertificates);

    MO_ADD_METAOBJECT0(QSslKey);
    MO_ADD_PROPERTY_RO(QSslKey, algorithm);
    MO_ADD_PROPERTY_RO(QSslKey, isNull);
    MO_ADD_PROPERTY_RO(QSslKey, length);
    MO_ADD_PROPERTY_RO(QSslKey, type);

    MO_ADD_METAOBJECT1(QSslSocket, QTcpSocket);
    MO_ADD_PROPERTY_RO(QSslSocket, isEncrypted);
    MO_ADD_PROPERTY_RO(QSslSocket, localCertificate);
    MO_ADD_PROPERTY_RO(QSslSocket, localCertificateChain);
    MO_ADD_PROPERTY_RO(QSslSocket, mode);
    MO_ADD_PROPERTY_RO(QSslSocket, peerCertificate);
    MO_ADD_PROPERTY_RO(QSslSocket, peerCertificateChain);
    MO_ADD_PROPERTY(QSslSocket, peerVerifyDepth, setPeerVerifyDepth);
    MO_ADD_PROPERTY(QSslSocket, peerVerifyMode, setPeerVerifyMode);
    MO_ADD_PROPERTY(QSslSocket, peerVerifyName, setPeerVerifyName);
    MO_ADD_PROPERTY(QSslSocket, privateKey, setPrivateKey);
    MO_ADD_PROPERTY_RO(QSslSocket, protocol);
    MO_ADD_PROPERTY_RO(QSslSocket, sessionProtocol);
    MO_ADD_PROPERTY_RO(QSslSocket, sessionCipher);
    MO_ADD_PROPERTY(QSslSocket, sslConfiguration, setSslConfiguration);
    MO_ADD_PROPERTY_RO(QSslSocket, sslErrors);
#endif // QT_NO_SSL

    MO_ADD_METAOBJECT1(QSocketNotifier, QObject);
    MO_ADD_PROPERTY_RO(QSocketNotifier, socket);
    MO_ADD_PROPERTY_RO(QSocketNotifier, type);
    MO_ADD_PROPERTY(QSocketNotifier, isEnabled, setEnabled);
}

#define E(x) { QAbstractSocket:: x, #x }
static const MetaEnum::Value<QAbstractSocket::PauseMode> socket_pause_mode_table[] = {
    E(PauseNever),
    E(PauseOnSslErrors),
};
#undef E

#define E(x) { QNetworkAccessManager:: x, #x }
static const MetaEnum::Value<QNetworkAccessManager::NetworkAccessibility>
network_accessibility_table[] = {
    E(UnknownAccessibility),
    E(NotAccessible),
    E(Accessible)
};
#undef E

#ifndef QT_NO_SSL
#define E(x) { QSslSocket:: x, #x }
static const MetaEnum::Value<QSslSocket::SslMode> ssl_mode_table[] = {
    E(UnencryptedMode),
    E(SslClientMode),
    E(SslServerMode)
};
#undef E

#define E(x) { QSslSocket:: x, #x }
static const MetaEnum::Value<QSslSocket::PeerVerifyMode> ssl_peer_verify_mode_table[] = {
    E(VerifyNone),
    E(QueryPeer),
    E(VerifyPeer),
    E(AutoVerifyPeer)
};
#undef E

#define E(x) { QSsl:: x, #x }
static const MetaEnum::Value<QSsl::KeyAlgorithm> ssl_key_algorithm_table[] = {
    E(Opaque),
    E(Rsa),
    E(Dsa),
    E(Ec)
};
#undef E

#define E(x) { QSsl:: x, #x }
static const MetaEnum::Value<QSsl::KeyType> ssl_key_type_table[] = {
    E(PrivateKey),
    E(PublicKey)
};
#undef E

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
    E(TlsV1_0OrLater),
    E(TlsV1_1OrLater),
    E(TlsV1_2OrLater),
    E(UnknownProtocol)
};
#undef E

static QString sslCertificateToString(const QSslCertificate &cert)
{
    if (cert.isNull())
        return QStringLiteral("<null>");
    return cert.digest().toHex();
}

#endif // QT_NO_SSL

#define E(x) { QNetworkConfiguration:: x, #x }
static const MetaEnum::Value<QNetworkConfiguration::Purpose> network_config_purpose_table[] = {
    E(UnknownPurpose),
    E(PublicPurpose),
    E(PrivatePurpose),
    E(ServiceSpecificPurpose)
};

static const MetaEnum::Value<QNetworkConfiguration::StateFlag> network_config_state_table[] = {
    E(Undefined),
    E(Defined),
    E(Discovered),
    E(Active)
};

static const MetaEnum::Value<QNetworkConfiguration::Type> network_config_type_table[] = {
    E(InternetAccessPoint),
    E(ServiceNetwork),
    E(UserChoice),
    E(Invalid)
};
#undef E

#define E(x) { QNetworkConfigurationManager:: x, #x }
static const MetaEnum::Value<QNetworkConfigurationManager::Capabilities> network_config_manager_capabilities_table[] = {
    E(CanStartAndStopInterfaces),
    E(DirectConnectionRouting),
    E(SystemSessionSupport),
    E(ApplicationLevelRoaming),
    E(ForcedRoaming),
    E(DataStatistics),
    E(NetworkSessionRequired)
};
#undef E

void NetworkSupport::registerVariantHandler()
{
    ER_REGISTER_FLAGS(QAbstractSocket, PauseModes, socket_pause_mode_table);
    VariantHandler::registerStringConverter<QHostAddress>(std::mem_fn(&QHostAddress::toString));
    ER_REGISTER_ENUM(QNetworkAccessManager, NetworkAccessibility, network_accessibility_table);
#ifndef QT_NO_SSL
    ER_REGISTER_ENUM(QSslSocket, PeerVerifyMode, ssl_peer_verify_mode_table);
    ER_REGISTER_ENUM(QSslSocket, SslMode, ssl_mode_table);
    ER_REGISTER_ENUM(QSsl, KeyAlgorithm, ssl_key_algorithm_table);
    ER_REGISTER_ENUM(QSsl, KeyType, ssl_key_type_table);
    ER_REGISTER_ENUM(QSsl, SslProtocol, ssl_protocol_table);
    VariantHandler::registerStringConverter<QSslCertificate>(sslCertificateToString);
    VariantHandler::registerStringConverter<QSslCertificateExtension>(std::mem_fn(&QSslCertificateExtension::name));
    VariantHandler::registerStringConverter<QSslCipher>(std::mem_fn(&QSslCipher::name));
    VariantHandler::registerStringConverter<QSslError>(std::mem_fn(&QSslError::errorString));
#endif

    ER_REGISTER_ENUM(QNetworkConfiguration, Purpose, network_config_purpose_table);
    ER_REGISTER_FLAGS(QNetworkConfiguration, StateFlags, network_config_state_table);
    ER_REGISTER_ENUM(QNetworkConfiguration, Type, network_config_type_table);
    ER_REGISTER_FLAGS(QNetworkConfigurationManager, Capabilities, network_config_manager_capabilities_table);
}

NetworkSupportFactory::NetworkSupportFactory(QObject *parent)
    : QObject(parent)
{
}
