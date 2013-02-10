#ifndef GAMMARAY_METHODARGUMENT_H
#define GAMMARAY_METHODARGUMENT_H

#include <QSharedDataPointer>
#include <QVariant>

namespace GammaRay {

class MethodArgumentPrivate;

/** Q[Generic]Argument that works on a QVariant, with some memory handling safety. */
class MethodArgument
{
  public:
    MethodArgument();
    explicit MethodArgument(const QVariant &v);
    MethodArgument(const MethodArgument &other);
    ~MethodArgument();

    MethodArgument& operator=(const MethodArgument &other);
    operator QGenericArgument() const;

  private:
    QExplicitlySharedDataPointer<MethodArgumentPrivate> d;
};

}

#endif // GAMMARAY_METHODARGUMENT_H
