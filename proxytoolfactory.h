#ifndef GAMMARAY_PROXYTOOLFACTORY_H
#define GAMMARAY_PROXYTOOLFACTORY_H

#include "toolfactory.h"

namespace GammaRay {

/**
 * A wrapper around a plugin ToolFactory that only loads the actual plugin
 * once initialized.
 * Until then, meta-data is provided based on a plugin spec file.
 */
class ProxyToolFactory : public QObject, public ToolFactory
{
  Q_OBJECT
  public:
    /**
     * @param path Path to the plugin spec file
     */
    explicit ProxyToolFactory(const QString &path, QObject *parent = 0);

    /** Returns @c true if the plugin seems valid from all the information we have so far. */
    bool isValid() const;

    virtual QString id() const;
    virtual QString name() const;
    virtual QStringList supportedTypes() const;

    virtual void init(ProbeInterface *probe);
    virtual QWidget *createWidget(ProbeInterface *probe, QWidget *parentWidget);

  private:
    QString m_id;
    QString m_pluginPath;
    QString m_name;
    QStringList m_supportedTypes;
    ToolFactory *m_factory;
};

}

#endif // GAMMARAY_PROXYTOOLFACTORY_H
