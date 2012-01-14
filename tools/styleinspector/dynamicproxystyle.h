#ifndef GAMMARAY_DYNAMICPROXYSTYLE_H
#define GAMMARAY_DYNAMICPROXYSTYLE_H

#include <QHash>
#include <QProxyStyle>

namespace GammaRay {

/**
 * A proxy style that allows runtime-editing of various parameters.
 */
class DynamicProxyStyle : public QProxyStyle
{
  Q_OBJECT
  public:
    explicit DynamicProxyStyle(QStyle *baseStyle);

    static DynamicProxyStyle* instance();
    static void insertProxyStyle();

    void setPixelMetric(PixelMetric metric, int value);

    virtual int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;

  private:
    QHash<QStyle::PixelMetric, int> m_pixelMetrics;
    static QWeakPointer<DynamicProxyStyle> s_instance;
};

}

#endif // GAMMARAY_DYNAMICPROXYSTYLE_H
