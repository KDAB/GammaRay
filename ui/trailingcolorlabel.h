#ifndef TRAILINGCOLORLABEL_H
#define TRAILINGCOLORLABEL_H

#include <QWidget>
#include <QLabel>

namespace GammaRay {

class TrailingColorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TrailingColorLabel(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    QColor pickedColor() const;

signals:

public slots:
    void setPickedColor(QRgb color);
private:
    QRgb m_pickedColor;
};

}
#endif // TRAILINGCOLORLABEL_H
