/*
  quickoverlaylegend.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quickoverlaylegend.h"
#include "quickdecorationsdrawer.h"

#include <ui/uiresources.h>

#include <QAction>
#include <QVBoxLayout>
#include <QListView>
#include <QAbstractListModel>
#include <QPainter>
#include <QLayout>

namespace GammaRay {

class LegendModel : public QAbstractListModel
{
    Q_OBJECT

    struct Item
    {
        enum Mode {
            Rect,
            Line,
            Last = Line + 1
        };

        explicit Item(const QBrush &brush = QBrush(), const QPen &pen = QPen(),
             const QByteArray &label = QByteArray())
            : brush(brush)
            , pen(pen)
            , label(label)
        { }

        QBrush brush;
        QPen pen;
        QByteArray label;
        QPixmap pixmap;

        static void createPixmap(Item &item) {
            static qreal dpr = 2.0;
            static QSize iconSize(44, 44);
            QPixmap pixmap(iconSize.width() * Last * dpr, iconSize.height() * dpr);
            pixmap.setDevicePixelRatio(dpr);
            pixmap.fill(QColor(Qt::white));

            {
                const int margin(2);

                QPainter painter(&pixmap);
                QPen pen(item.pen);

                pen.setWidth(pen.width() * 2);

                painter.setPen(pen);
                painter.setBrush(item.brush);

                for (int i = Rect; i < Last; i++) {
                    const QRect rect(QPoint(iconSize.width() * i, 0), iconSize);

                    switch (i) {
                    case Rect: {
                        painter.drawRect(rect.adjusted(margin, margin, -margin, -margin));
                        break;
                    }
                    case Line:{
                        painter.drawLine(QLine(QPoint(rect.left() + margin, rect.center().y() - 2),
                                               QPoint(rect.right() - margin, rect.center().y() - 2)));
                        pen.setStyle(Qt::DotLine);
                        painter.setPen(pen);
                        painter.drawLine(QLine(QPoint(rect.left() + margin, rect.center().y() + 2),
                                               QPoint(rect.right() - margin, rect.center().y() + 2)));
                        break;
                    }
                    }
                }
            }

            item.pixmap = pixmap;
        }
    };

public:
    explicit LegendModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        return QAbstractListModel::flags(index) & ~Qt::ItemIsSelectable;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : m_items.count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (hasIndex(index.row(), index.column(), index.parent())) {
            const Item &item(m_items[index.row()]);

            switch (role) {
            // Return a pixmap so the view will draw it at the given size instead of the view icon size
            case Qt::DecorationRole:
                return item.pixmap;
            case Qt::ForegroundRole:
                return item.pen.brush();
            case Qt::DisplayRole:
                return LegendModel::tr(item.label.constData());
            case Qt::SizeHintRole:
                return QSize(-1, (item.pixmap.height() / item.pixmap.devicePixelRatio()) + (2 * 2));
            }
        }

        return QVariant();
    }

    void setSettings(const QuickDecorationsSettings &settings)
    {
        beginResetModel();
        m_items.clear();

        m_items << Item(
                       settings.boundingRectBrush,
                       settings.boundingRectColor,
                       QT_TR_NOOP("Bounding rect")
                       );

        m_items << Item(
                       settings.geometryRectBrush,
                       settings.geometryRectColor,
                       QT_TR_NOOP("Geometry rect")
                       );

        m_items << Item(
                       settings.childrenRectBrush,
                       settings.childrenRectColor,
                       QT_TR_NOOP("Children rect")
                       );

        m_items << Item(
                       QBrush(),
                       settings.transformOriginColor,
                       QT_TR_NOOP("Transform origin")
                       );

        m_items << Item(
                       QBrush(),
                       settings.coordinatesColor,
                       QT_TR_NOOP("Coordinates (x, y...)")
                       );

        m_items << Item(
                       QBrush(),
                       settings.marginsColor,
                       QT_TR_NOOP("Margins/Anchors")
                       );

        m_items << Item(
                       QBrush(),
                       settings.paddingColor,
                       QT_TR_NOOP("Padding")
                       );

        m_items << Item(
                       QBrush(),
                       settings.gridColor,
                       QT_TR_NOOP("Grid")
                       );

        for (int i = 0; i < m_items.count(); ++i) {
            Item::createPixmap(m_items[i]);
        }
        endResetModel();
    }

private:
    QVector<Item> m_items;
};

QuickOverlayLegend::QuickOverlayLegend(QWidget *parent)
    : QWidget(parent, Qt::Tool)
    , m_model(new LegendModel(this))
{
    setWindowTitle(tr("Legend"));

    auto *view = new QListView(this);
    view->setUniformItemSizes(true);
    view->setModel(m_model);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(view);

    m_visibilityAction = new QAction(UIResources::themedIcon(QLatin1String("legend.png")),
                                     tr("Show Legend"), this);
    m_visibilityAction->setObjectName("aShowLegend");
    m_visibilityAction->setCheckable(true);
    m_visibilityAction->setToolTip(tr("<b>Show Legend</b><br>"
                                      "This shows a legend explaining the various diagnostic decorations."));

    connect(m_visibilityAction, &QAction::triggered, this, [this](bool toggled) {
        setVisible(toggled);
    });
}

QAction *QuickOverlayLegend::visibilityAction() const
{
    return m_visibilityAction;
}

void QuickOverlayLegend::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    m_model->setSettings(settings);

    const int titleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight);
    const QMargins margins(layout()->contentsMargins());
    const int viewHeight = m_model->index(0, 0).data(Qt::SizeHintRole).toSize().height() * m_model->rowCount();
    resize(280, titleBarHeight + margins.top() + margins.bottom() + viewHeight);
}

void QuickOverlayLegend::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_visibilityAction->setChecked(true);
}

void QuickOverlayLegend::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    m_visibilityAction->setChecked(false);
}

}

#include "quickoverlaylegend.moc"
