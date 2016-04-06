#include "qmlcontextpropertyadaptor.h"

#include <core/propertydata.h>

#include <QQmlContext>
#include <private/qqmlcontext_p.h>
#include <private/qv4identifier_p.h>

#include <QDebug>

using namespace GammaRay;

QmlContextPropertyAdaptor::QmlContextPropertyAdaptor(QObject *parent) :
    PropertyAdaptor(parent)
{
}

QmlContextPropertyAdaptor::~QmlContextPropertyAdaptor()
{
}

int QmlContextPropertyAdaptor::count() const
{
    return m_contextPropertyNames.size();
}

PropertyData QmlContextPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;
    if (!object().isValid())
        return pd;

    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_contextPropertyNames.size());

    auto context = qobject_cast<QQmlContext*>(object().qtObject());
    if (!context)
        return pd;

    pd.setName(m_contextPropertyNames.at(index));
    pd.setValue(context->contextProperty(m_contextPropertyNames.at(index)));
    pd.setClassName(tr("QML Context Property"));
    return pd;
}

void QmlContextPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto context = qobject_cast<QQmlContext*>(oi.qtObject());
    Q_ASSERT(context);

    auto contextData = QQmlContextData::get(context);
    Q_ASSERT(contextData);

    const auto &propNames = contextData->propertyNames();
    m_contextPropertyNames.clear();
    m_contextPropertyNames.reserve(propNames.count());

    QV4::IdentifierHashEntry *e = propNames.d->entries;
    QV4::IdentifierHashEntry *end = e + propNames.d->alloc;
    while (e < end) {
        if (e->identifier)
            m_contextPropertyNames.push_back(e->identifier->string);
        ++e;
    }
#endif
}


QmlContextPropertyAdaptorFactory* QmlContextPropertyAdaptorFactory::s_instance = Q_NULLPTR;

PropertyAdaptor* QmlContextPropertyAdaptorFactory::create(const ObjectInstance& oi, QObject* parent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    if (oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return Q_NULLPTR;

    if (qobject_cast<QQmlContext*>(oi.qtObject()))
        return new QmlContextPropertyAdaptor(parent);
#endif

    return Q_NULLPTR;
}

QmlContextPropertyAdaptorFactory* QmlContextPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QmlContextPropertyAdaptorFactory;
    return s_instance;
}
