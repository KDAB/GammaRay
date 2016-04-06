#ifndef GAMMARAY_QMLCONTEXTPROPERTYADAPTOR_H
#define GAMMARAY_QMLCONTEXTPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

#include <QVector>

namespace GammaRay {

class QmlContextPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QmlContextPropertyAdaptor(QObject *parent = Q_NULLPTR);
    ~QmlContextPropertyAdaptor();

    int count() const Q_DECL_OVERRIDE;
    PropertyData propertyData(int index) const Q_DECL_OVERRIDE;

protected:
    void doSetObject(const ObjectInstance &oi) Q_DECL_OVERRIDE;

private:
    QVector<QString> m_contextPropertyNames;
};

class QmlContextPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor* create(const ObjectInstance& oi, QObject* parent = 0) const Q_DECL_OVERRIDE;
    static QmlContextPropertyAdaptorFactory* instance();
private:
    static QmlContextPropertyAdaptorFactory *s_instance;
};

}

#endif
