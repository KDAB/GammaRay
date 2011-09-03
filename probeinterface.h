#ifndef ENDOSCOPE_PROBEINTERFACE_H
#define ENDOSCOPE_PROBEINTERFACE_H

class QAbstractItemModel;

namespace Endoscope
{

/**
 * Abstract interface for accessing the core probe without linking to it.
 */
class ProbeInterface
{
  public:
    virtual inline ~ProbeInterface() {}

    /** Returns the object list model. */
    virtual QAbstractItemModel* objectListModel() const = 0;
    /** Returns the object tree model. */
    virtual QAbstractItemModel* objectTreeModel() const = 0;
};

}

#endif
