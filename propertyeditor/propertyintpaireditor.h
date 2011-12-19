#ifndef GAMMARAY_PROPERTYINTPAIREDITOR_H
#define GAMMARAY_PROPERTYINTPAIREDITOR_H

#include <QWidget>

class QPoint;

namespace GammaRay {

namespace Ui {
class PropertyIntPairEditor;
}

/** Property editor for QPoint, QSize and anything else consisting of two integer values. */
class PropertyIntPairEditor : public QWidget
{
  Q_OBJECT
public:
  explicit PropertyIntPairEditor(QWidget* parent = 0);

protected:
  QScopedPointer<Ui::PropertyIntPairEditor> ui;
};


/** Property editor for points. Since QStyledItemDelegate ignore valuePropertyName and insists on
 * USER properties we need one class per type here...
 */
class PropertyPointEditor : public PropertyIntPairEditor
{
  Q_OBJECT
  Q_PROPERTY(QPoint point READ point WRITE setPoint USER true)
public:
  explicit PropertyPointEditor(QWidget* parent = 0);

  QPoint point() const;
  void setPoint( const QPoint &point );
};


/** Same again for size. */
class PropertySizeEditor : public PropertyIntPairEditor
{
  Q_OBJECT
  Q_PROPERTY(QSize sizeValue READ sizeValue WRITE setSizeValue USER true)
public:
  explicit PropertySizeEditor(QWidget* parent = 0);

  QSize sizeValue() const;
  void setSizeValue( const QSize &size );
};

}

#endif // GAMMARAY_PROPERTYINTPAIREDITOR_H
