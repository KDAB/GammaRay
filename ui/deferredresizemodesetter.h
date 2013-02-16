#ifndef GAMMARAY_DEFERREDRESIZEMODESETTER_H
#define GAMMARAY_DEFERREDRESIZEMODESETTER_H

#include <QObject>
#include <QHeaderView>

namespace GammaRay {

/** Sets the resize mode on a QHeaderView section once that section is actually available.
 *
 * This is a workaround for QHeaderView asserting when manipulating if the corresponding section
 * hasn't been loaded yet by the corresponding model, as well as forgetting the setting if the
 * model reports a columnCount of 0 again inbetween.
 */
class DeferredResizeModeSetter : public QObject
{
  Q_OBJECT
public:
  DeferredResizeModeSetter(QHeaderView *headerView, int section, QHeaderView::ResizeMode resizeMode);
  ~DeferredResizeModeSetter();

private slots:
  void setSectionResizeMode();

private:
  QHeaderView *m_view;
  int m_section;
  QHeaderView::ResizeMode m_resizeMode;
};

}

#endif // GAMMARAY_DEFERREDRESIZEMODESETTER_H
