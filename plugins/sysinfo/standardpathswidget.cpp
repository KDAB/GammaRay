/*
  standardpathswidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "standardpathswidget.h"
#include "ui_standardpathswidget.h"

#include <common/objectbroker.h>

#include <QApplication>
#include <QIdentityProxyModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>

using namespace GammaRay;

namespace GammaRay {
class StandardPathsProxy : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit StandardPathsProxy(QObject *parent = nullptr)
        : QIdentityProxyModel(parent)
    {
    }

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override
    {
        if (section == 2 && role == Qt::DisplayRole)
            return tr("Locations Standard / Writable");

        return QIdentityProxyModel::headerData(section, orientation, role);
    }
};

class StandardPathsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StandardPathsDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        if (index.column() == 2) {
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);

            const QWidget *widget = opt.widget;
            QStyle *style = widget ? widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

            const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, widget) + 1;
            const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget)
                                       .adjusted(textMargin, 1, -textMargin, -1);
            painter->setPen(((opt.state
                              & QStyle::State_Selected)
                                 ? opt.palette.highlightedText()
                                 : opt.palette.text())
                                .color());
            painter->drawText(textRect, Qt::AlignBottom | Qt::AlignLeft,
                              index.sibling(index.row(), 3).data().toString());
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        if (index.column() == 2) {
            QSize s1 = QStyledItemDelegate::sizeHint(option, index.sibling(index.row(), 2));
            QSize s2 = QStyledItemDelegate::sizeHint(option, index.sibling(index.row(), 3));
            return { qMax(s1.width(), s2.width()),
                     s1.height() + s2.height() + option.fontMetrics.height() };
        } else {
            return QStyledItemDelegate::sizeHint(option, index);
        }
    }
};
}

StandardPathsWidget::StandardPathsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StandardPathsWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    auto *proxy = new StandardPathsProxy(this);
    proxy->setSourceModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.StandardPathsModel")));

    ui->pathView->header()->setObjectName("pathViewHeader");
    ui->pathView->setUniformRowHeights(false);
    ui->pathView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->pathView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->pathView->setDeferredHidden(3, true);
    ui->pathView->setItemDelegateForColumn(2, new StandardPathsDelegate(this));
    ui->pathView->setModel(proxy);
}

StandardPathsWidget::~StandardPathsWidget() = default;

#include "standardpathswidget.moc"
