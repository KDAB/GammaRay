/*
  networksupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "networkreplymodel.h"
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
#include <QHstsPolicy>
#endif
#include <QLocalSocket>
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkCookieJar>
#include <QNetworkProxy>
#include <QNetworkSession>
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
Q_DECLARE_METATYPE(QHstsPolicy)
#endif
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketError)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketState)
Q_DECLARE_METATYPE(QNetworkAccessManager::NetworkAccessibility)
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
Q_DECLARE_METATYPE(QNetworkAddressEntry)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
Q_DECLARE_METATYPE(QNetworkAddressEntry::DnsEligibilityStatus)
#endif
Q_DECLARE_METATYPE(QNetworkConfiguration::BearerType)
Q_DECLARE_METATYPE(QNetworkConfigurationManager::Capabilities)
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
Q_DECLARE_METATYPE(QNetworkInterface)
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
Q_DECLARE_METATYPE(QNetworkInterface::InterfaceFlags)
#endif
Q_DECLARE_METATYPE(QNetworkProxy::Capabilities)
Q_DECLARE_METATYPE(QNetworkProxy::ProxyType)
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

    auto replyModel = new NetworkReplyModel(this);
    connect(probe, &Probe::objectCreated, replyModel, &NetworkReplyModel::objectCreated);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.NetworkReplyModel"), replyModel);

    PropertyController::registerExtension<CookieExtension>();
}

NetworkSupport::~NetworkSupport() = default;

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_METAOBJECT0(QHstsPolicy);
    MO_ADD_PROPERTY   (QHstsPolicy, expiry, setExpiry);
    MO_ADD_PROPERTY_LD(QHstsPolicy, host, [](QHstsPolicy *policy) { return policy->host(); });
    MO_ADD_PROPERTY   (QHstsPolicy, includesSubDomains, setIncludesSubDomains);
#endif

    MO_ADD_METAOBJECT1(QLocalSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QLocalSocket, error);
    MO_ADD_PROPERTY_RO(QLocalSocket, fullServerName);
    MO_ADD_PROPERTY_RO(QLocalSocket, isValid);
    MO_ADD_PROPERTY_RO(QLocalSocket, serverName);
    MO_ADD_PROPERTY_RO(QLocalSocket, socketDescriptor);
    MO_ADD_PROPERTY_RO(QLocalSocket, state);

    MO_ADD_METAOBJECT1(QNetworkAccessManager, QObject);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, activeConfiguration);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, cache);
    MO_ADD_PROPERTY   (QNetworkAccessManager, configuration, setConfiguration);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, cookieJar);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY   (QNetworkAccessManager, isStrictTransportSecurityEnabled, setStrictTransportSecurityEnabled);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, isStrictTransportSecurityStoreEnabled);
#endif
    MO_ADD_PROPERTY   (QNetworkAccessManager, proxy, setProxy);
    MO_ADD_PROPERTY   (QNetworkAccessManager, redirectPolicy, setRedirectPolicy);
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, strictTransportSecurityHosts);
#endif
    MO_ADD_PROPERTY_RO(QNetworkAccessManager, supportedSchemes);

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    MO_ADD_METAOBJECT0(QNetworkAddressEntry);
    MO_ADD_PROPERTY   (QNetworkAddressEntry, broadcast, setBroadcast);
    MO_ADD_PROPERTY   (QNetworkAddressEntry, dnsEligibility, setDnsEligibility);
    MO_ADD_PROPERTY   (QNetworkAddressEntry, ip, setIp);
    MO_ADD_PROPERTY_RO(QNetworkAddressEntry, isLifetimeKnown);
    MO_ADD_PROPERTY_RO(QNetworkAddressEntry, isPermanent);
    MO_ADD_PROPERTY_RO(QNetworkAddressEntry, isTemporary);
    MO_ADD_PROPERTY   (QNetworkAddressEntry, netmask, setNetmask);
    //MO_ADD_PROPERTY_RO(QNetworkAddressEntry, preferredLifetime);
    MO_ADD_PROPERTY   (QNetworkAddressEntry, prefixLength, setPrefixLength);
    //MO_ADD_PROPERTY_RO(QNetworkAddressEntry, validityLifetime);
#endif

    MO_ADD_METAOBJECT0(QNetworkConfiguration);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, bearerType);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, bearerTypeFamily);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, bearerTypeName);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, children);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, connectTimeout);
#endif
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, identifier);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, isRoamingAvailable);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, isValid);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, name);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, purpose);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, state);
    MO_ADD_PROPERTY_RO(QNetworkConfiguration, type);

    MO_ADD_METAOBJECT1(QNetworkConfigurationManager, QObject);
    MO_ADD_PROPERTY_RO(QNetworkConfigurationManager, capabilities);
    MO_ADD_PROPERTY_RO(QNetworkConfigurationManager, isOnline);

    MO_ADD_METAOBJECT0(QNetworkInterface);
    MO_ADD_PROPERTY_RO(QNetworkInterface, addressEntries);
    MO_ADD_PROPERTY_ST(QNetworkInterface, allAddresses);
    MO_ADD_PROPERTY_ST(QNetworkInterface, allInterfaces);
    MO_ADD_PROPERTY_RO(QNetworkInterface, flags);
    MO_ADD_PROPERTY_RO(QNetworkInterface, hardwareAddress);
    MO_ADD_PROPERTY_RO(QNetworkInterface, index);
    MO_ADD_PROPERTY_RO(QNetworkInterface, isValid);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    MO_ADD_PROPERTY_RO(QNetworkInterface, maximumTransmissionUnit);
#endif
    MO_ADD_PROPERTY_RO(QNetworkInterface, name);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    MO_ADD_PROPERTY_RO(QNetworkInterface, type);
#endif

    MO_ADD_METAOBJECT0(QNetworkProxy);
    MO_ADD_PROPERTY_ST(QNetworkProxy, applicationProxy);
    MO_ADD_PROPERTY   (QNetworkProxy, capabilities, setCapabilities);
    MO_ADD_PROPERTY   (QNetworkProxy, hostName, setHostName);
    MO_ADD_PROPERTY_RO(QNetworkProxy, isCachingProxy);
    MO_ADD_PROPERTY_RO(QNetworkProxy, isTransparentProxy);
    MO_ADD_PROPERTY   (QNetworkProxy, password, setPassword);
    MO_ADD_PROPERTY   (QNetworkProxy, port, setPort);
    //MO_ADD_PROPERTY_RO(QNetworkProxy, rawHeaderList);
    MO_ADD_PROPERTY   (QNetworkProxy, type, setType);
    MO_ADD_PROPERTY   (QNetworkProxy, user, setUser);

    MO_ADD_METAOBJECT1(QNetworkSession, QObject);
    MO_ADD_PROPERTY_RO(QNetworkSession, activeTime);
    MO_ADD_PROPERTY_RO(QNetworkSession, bytesReceived);
    MO_ADD_PROPERTY_RO(QNetworkSession, bytesWritten);
    MO_ADD_PROPERTY_RO(QNetworkSession, configuration);
    MO_ADD_PROPERTY_RO(QNetworkSession, error);
    MO_ADD_PROPERTY_RO(QNetworkSession, errorString);
    MO_ADD_PROPERTY_RO(QNetworkSession, interface);
    MO_ADD_PROPERTY_RO(QNetworkSession, isOpen);
    MO_ADD_PROPERTY_RO(QNetworkSession, state);
    MO_ADD_PROPERTY_RO(QNetworkSession, usagePolicies);

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
static const MetaEnum::Value<QNetworkConfiguration::BearerType> network_config_bearer_type_table[] = {
    E(BearerUnknown),
    E(BearerEthernet),
    E(BearerWLAN),
    E(Bearer2G),
    E(Bearer3G),
    E(Bearer4G),
    E(BearerCDMA2000),
    E(BearerWCDMA),
    E(BearerHSPA),
    E(BearerBluetooth),
    E(BearerWiMAX),
    E(BearerEVDO),
    E(BearerLTE)
};

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#define E(x) { QNetworkAddressEntry:: x, #x }
static const MetaEnum::Value<QNetworkAddressEntry::DnsEligibilityStatus> network_address_entry_dns_eligibility_table[] = {
    E(DnsEligibilityUnknown),
    E(DnsEligible),
    E(DnsIneligible)
};
#undef E
#endif

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
#define E(x) { QNetworkRequest:: x, #x }
static const MetaEnum::Value<QNetworkRequest::RedirectPolicy> network_redirect_policy_table[] = {
    E(ManualRedirectPolicy),
    E(NoLessSafeRedirectPolicy),
    E(SameOriginRedirectPolicy),
    E(UserVerifiedRedirectPolicy)
};
#undef E
#endif

#define E(x) { QNetworkProxy:: x, #x }
static const MetaEnum::Value<QNetworkProxy::Capability> network_proxy_capabilitiy_table[] = {
    E(TunnelingCapability),
    E(ListeningCapability),
    E(UdpTunnelingCapability),
    E(CachingCapability),
    E(HostNameLookupCapability),
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    E(SctpTunnelingCapability),
    E(SctpListeningCapability)
#endif
};

static const MetaEnum::Value<QNetworkProxy::ProxyType> network_proxy_type_table[] = {
    E(NoProxy),
    E(DefaultProxy),
    E(Socks5Proxy),
    E(HttpProxy),
    E(HttpCachingProxy),
    E(FtpCachingProxy)
};
#undef E

#define E(x) { QNetworkSession:: x, #x }
static const MetaEnum::Value<QNetworkSession::SessionError> network_session_error_table[] = {
    E(UnknownSessionError),
    E(SessionAbortedError),
    E(RoamingError),
    E(OperationNotSupportedError),
    E(InvalidConfigurationError)
};

static const MetaEnum::Value<QNetworkSession::State> network_session_state_table[] = {
    E(Invalid),
    E(NotAvailable),
    E(Connecting),
    E(Connected),
    E(Closing),
    E(Disconnected),
    E(Roaming)
};

static const MetaEnum::Value<QNetworkSession::UsagePolicies> network_session_usage_policy_table[] = {
    E(NoPolicy),
    E(NoBackgroundTrafficPolicy)
};
#undef E

static QString proxyToString(const QNetworkProxy &proxy)
{
    return VariantHandler::displayString(proxy.type());
}

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    ER_REGISTER_ENUM(QNetworkAddressEntry, DnsEligibilityStatus, network_address_entry_dns_eligibility_table);
#endif

    ER_REGISTER_ENUM(QNetworkConfiguration, BearerType, network_config_bearer_type_table);
    ER_REGISTER_ENUM(QNetworkConfiguration, Purpose, network_config_purpose_table);
    ER_REGISTER_FLAGS(QNetworkConfiguration, StateFlags, network_config_state_table);
    ER_REGISTER_ENUM(QNetworkConfiguration, Type, network_config_type_table);
    VariantHandler::registerStringConverter<QNetworkConfiguration>(std::mem_fn(&QNetworkConfiguration::name));
    ER_REGISTER_FLAGS(QNetworkConfigurationManager, Capabilities, network_config_manager_capabilities_table);

    VariantHandler::registerStringConverter<QNetworkInterface>(std::mem_fn(&QNetworkInterface::name));

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    ER_REGISTER_ENUM(QNetworkRequest, RedirectPolicy, network_redirect_policy_table);
#endif

    ER_REGISTER_FLAGS(QNetworkProxy, Capabilities, network_proxy_capabilitiy_table);
    ER_REGISTER_ENUM(QNetworkProxy, ProxyType, network_proxy_type_table);
    VariantHandler::registerStringConverter<QNetworkProxy>(proxyToString);

    ER_REGISTER_ENUM(QNetworkSession, SessionError, network_session_error_table);
    ER_REGISTER_ENUM(QNetworkSession, State, network_session_state_table);
    ER_REGISTER_FLAGS(QNetworkSession, UsagePolicies, network_session_usage_policy_table);
}

NetworkSupportFactory::NetworkSupportFactory(QObject *parent)
    : QObject(parent)
{
}
