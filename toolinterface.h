#ifndef ENDOSCOPE_TOOLINTERFACE_H
#define ENDOSCOPE_TOOLINTERFACE_H

#include <QtPlugin>
#include <QtCore/QStringList>
#include <QMetaType>

namespace Endoscope {

class ProbeInterface;

/**
 * Abstract interface for probe tools.
 */
class ToolInterface
{
  public:
    virtual inline ~ToolInterface() {}

    /** Human readable name of this tool. */
    virtual QString name() const = 0;

    /**
     * Class names of types this tool can handle.
     * The tool will only be activated if an object of one of these types is seen in the probed application.
     */
    virtual QStringList supportedTypes() const = 0;

    /**
     * Create an instance of this tool.
     * @param probeIface The probe interface allowing access to the object models.
     * @param parentWidget The parent widget for the visual elements of this tool.
     */
    virtual QWidget* createInstance( ProbeInterface *probeIface, QWidget *parentWidget ) = 0;
};

}

Q_DECLARE_INTERFACE(Endoscope::ToolInterface, "com.kdab.Endoscope.ToolInterface/1.0")
Q_DECLARE_METATYPE(Endoscope::ToolInterface*)

#endif
