#ifndef GAMMARAY_UIEXTRACTOR_H
#define GAMMARAY_UIEXTRACTOR_H

#include <QFormBuilder>

namespace GammaRay {

class UiExtractor : public QFormBuilder
{
protected:
  virtual bool checkProperty(QObject* obj, const QString& prop) const;
};

}

#endif // GAMMARAY_UIEXTRACTOR_H
