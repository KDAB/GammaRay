
#ifndef VARIANTINSPECTOR_H
#define VARIANTINSPECTOR_H

#include <QPointer>
#include <QWidget>

class VariantInspector : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QSharedPointer<QWidget> sharedWidget READ sharedWidget CONSTANT)
  Q_PROPERTY(QPointer<QWidget> trackingWidget READ trackingWidget CONSTANT)
  Q_PROPERTY(QVector<int> widgetVector READ widgetVector CONSTANT)
public:
  explicit VariantInspector(QObject *parent = 0);

  QSharedPointer<QWidget> sharedWidget() const;
  QPointer<QWidget> trackingWidget() const;
  QVector<int> widgetVector() const;

private:
  QSharedPointer<QWidget> m_widget;
};

#endif
