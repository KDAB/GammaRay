#ifndef GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H
#define GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H

#include <QtGui/QWidget>

namespace GammaRay {

namespace Ui {
class PropertyDoublePairEditor;
}

/** Property editor for pairs of doubles, such as PointF and SizeF. */
class PropertyDoublePairEditor : public QWidget
{
  Q_OBJECT
public:
  explicit PropertyDoublePairEditor(QWidget* parent = 0);

protected:
  QScopedPointer<Ui::PropertyDoublePairEditor> ui;
};

class PropertyPointFEditor : public PropertyDoublePairEditor
{
  Q_OBJECT
  Q_PROPERTY(QPointF pointF READ pointF WRITE setPointF USER true)
public:
  explicit PropertyPointFEditor(QWidget* parent = 0);
  QPointF pointF() const;
  void setPointF(const QPointF& point);
};

class PropertySizeFEditor : public PropertyDoublePairEditor
{
  Q_OBJECT
  Q_PROPERTY(QSizeF sizeF READ sizeF WRITE setSizeF USER true)
public:
  explicit PropertySizeFEditor(QWidget* parent = 0);
  QSizeF sizeF() const;
  void setSizeF(const QSizeF& size);
};

}

#endif // GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H
