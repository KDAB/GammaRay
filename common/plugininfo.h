#ifndef GAMMARAY_PLUGININFO_H
#define GAMMARAY_PLUGININFO_H

#include <QString>
#include <QStringList>

namespace GammaRay {

/** Meta-data about a specifc plugin.
 *  This abstracts Qt5-style embedded JSON data and Qt4-style desktop files.
 */
class PluginInfo
{
public:
    PluginInfo();
    explicit PluginInfo(const QString &path);

    QString path() const;
    QString id() const;
    QString interface() const;
    QStringList supportedTypes() const;
    QString name() const;
    bool remoteSupport() const;
    bool isHidden() const;

    bool isValid() const;

private:
    void initFromJSON(const QString &path);
    void initFromDesktopFile(const QString &path);

    QString m_path;
    QString m_id;
    QString m_interface;
    QStringList m_supportedTypes;
    QString m_name;
    bool m_remoteSupport;
    bool m_hidden;
};
}

#endif // GAMMARAY_PLUGININFO_H
