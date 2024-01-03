/*
  sidepane.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "sidepane.h"
#include "uiresources.h"

#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QStyledItemDelegate>

using namespace GammaRay;

class Delegate : public QStyledItemDelegate
{
public:
    explicit Delegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        static const int heightMargin = 10;

        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(size.height() + heightMargin);
        return size;
    }
};

SidePane::SidePane(QWidget *parent)
    : QListView(parent)
{
    viewport()->installEventFilter(this);
    viewport()->setAutoFillBackground(false);
    setAttribute(Qt::WA_MacShowFocusRect, false);

    setItemDelegate(new Delegate(this));
}

SidePane::~SidePane() = default;

QSize SidePane::sizeHint() const
{
    static const int widthMargin = 10;

    if (!model())
        return QSize(0, 0);

    const int width = sizeHintForColumn(0) + widthMargin;
    const int height = QListView::sizeHint().height();

    return { width, height };
}

void SidePane::setModel(QAbstractItemModel *model)
{
    if (model) {
        connect(model, &QAbstractItemModel::rowsInserted, this, &SidePane::updateSizeHint);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &SidePane::updateSizeHint);
        connect(model, &QAbstractItemModel::dataChanged, this, &SidePane::updateSizeHint);
        connect(model, &QAbstractItemModel::modelReset, this, &SidePane::updateSizeHint);
    }
    QAbstractItemView::setModel(model);
}

bool SidePane::eventFilter(QObject *object, QEvent *event)
{
    if (object == viewport()) {
        if (event->type() == QEvent::Resize)
            updateSizeHint();
        else if (event->type() == QEvent::ScreenChangeInternal)
            m_background = QPixmap();
        else if (event->type() == QEvent::Paint) {
            if (m_background.isNull())
                m_background = UIResources::themedPixmap(QStringLiteral("kdab-gammaray-logo.png"), this);

            qreal dpr = 1.0;
            dpr = m_background.devicePixelRatio();
            QPainter p(viewport());
            p.drawPixmap(viewport()->width() - (m_background.width() / dpr),
                         viewport()->height() - (m_background.height() / dpr), m_background);
        }
    }

    return QListView::eventFilter(object, event);
}

void SidePane::updateSizeHint()
{
    setMinimumWidth(sizeHint().width());
}
