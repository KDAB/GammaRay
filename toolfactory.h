#ifndef ENDOSCOPE_TOOLFACTORY_H
#define ENDOSCOPE_TOOLFACTORY_H

#include <QtPlugin>
#include <QtCore/QStringList>
#include <QMetaType>

namespace Endoscope {

class ProbeInterface;

/**
 * Abstract interface for probe tools.
 */
class ToolFactory
{
  public:
    virtual inline ~ToolFactory() {}

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

template <typename Type, typename Tool>
class StandardToolFactory : public ToolFactory
{
  public:
    virtual inline QStringList supportedTypes() const { return QStringList( Type::staticMetaObject.className() ); }
    virtual inline QWidget* createInstance( ProbeInterface *probe, QWidget *parentWidget ) { return new Tool( probe, parentWidget ); }
};

}

Q_DECLARE_INTERFACE(Endoscope::ToolFactory, "com.kdab.Endoscope.ToolFactory/1.0")
Q_DECLARE_METATYPE(Endoscope::ToolFactory*)

#endif
