#ifndef QUICKITEMDELEGATEWIDGET_H
#define QUICKITEMDELEGATEWIDGET_H

#include <QWidget>
#include <QModelIndex>

namespace GammaRay {

class QuickItemDelegateWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)

public:
    QuickItemDelegateWidget(QModelIndex index, QWidget* parent = 0, Qt::WindowFlags f = 0);

    QColor textColor() const;

public Q_SLOTS:
    void setTextColor(const QColor& textColor);

protected:
    virtual void paintEvent(QPaintEvent* );

private:
    QColor m_textColor;
    QModelIndex m_index;
};

}

#endif // QUICKITEMDELEGATEWIDGET_H
