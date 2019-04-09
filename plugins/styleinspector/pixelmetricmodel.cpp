/*
  pixelmetricmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "pixelmetricmodel.h"
#include "dynamicproxystyle.h"

#include <QStyle>

using namespace GammaRay;

struct pixel_metric_t {
    const char *name;
    QStyle::PixelMetric pixelMetric;
};

#define MAKE_PM(metric) { #metric, QStyle:: metric }

static const pixel_metric_t pixelMetrics[] = {
    MAKE_PM(PM_ButtonMargin),
    MAKE_PM(PM_ButtonDefaultIndicator),
    MAKE_PM(PM_MenuButtonIndicator),
    MAKE_PM(PM_ButtonShiftHorizontal),
    MAKE_PM(PM_ButtonShiftVertical),
    MAKE_PM(PM_DefaultFrameWidth),
    MAKE_PM(PM_SpinBoxFrameWidth),
    MAKE_PM(PM_ComboBoxFrameWidth),
    MAKE_PM(PM_MaximumDragDistance),
    MAKE_PM(PM_ScrollBarExtent),
    MAKE_PM(PM_ScrollBarSliderMin),
    MAKE_PM(PM_SliderThickness),
    MAKE_PM(PM_SliderControlThickness),
    MAKE_PM(PM_SliderLength),
    MAKE_PM(PM_SliderTickmarkOffset),
    MAKE_PM(PM_SliderSpaceAvailable),
    MAKE_PM(PM_DockWidgetSeparatorExtent),
    MAKE_PM(PM_DockWidgetHandleExtent),
    MAKE_PM(PM_DockWidgetFrameWidth),
    MAKE_PM(PM_TabBarTabOverlap),
    MAKE_PM(PM_TabBarTabHSpace),
    MAKE_PM(PM_TabBarTabVSpace),
    MAKE_PM(PM_TabBarBaseHeight),
    MAKE_PM(PM_TabBarBaseOverlap),
    MAKE_PM(PM_ProgressBarChunkWidth),
    MAKE_PM(PM_SplitterWidth),
    MAKE_PM(PM_TitleBarHeight),
    MAKE_PM(PM_MenuScrollerHeight),
    MAKE_PM(PM_MenuHMargin),
    MAKE_PM(PM_MenuVMargin),
    MAKE_PM(PM_MenuPanelWidth),
    MAKE_PM(PM_MenuTearoffHeight),
    MAKE_PM(PM_MenuDesktopFrameWidth),
    MAKE_PM(PM_MenuBarPanelWidth),
    MAKE_PM(PM_MenuBarItemSpacing),
    MAKE_PM(PM_MenuBarVMargin),
    MAKE_PM(PM_MenuBarHMargin),
    MAKE_PM(PM_IndicatorWidth),
    MAKE_PM(PM_IndicatorHeight),
    MAKE_PM(PM_ExclusiveIndicatorWidth),
    MAKE_PM(PM_ExclusiveIndicatorHeight),
    MAKE_PM(PM_DialogButtonsSeparator),
    MAKE_PM(PM_DialogButtonsButtonWidth),
    MAKE_PM(PM_DialogButtonsButtonHeight),
    MAKE_PM(PM_MdiSubWindowFrameWidth),
    MAKE_PM(PM_MdiSubWindowMinimizedWidth),
    MAKE_PM(PM_HeaderMargin),
    MAKE_PM(PM_HeaderMarkSize),
    MAKE_PM(PM_HeaderGripMargin),
    MAKE_PM(PM_TabBarTabShiftHorizontal),
    MAKE_PM(PM_TabBarTabShiftVertical),
    MAKE_PM(PM_TabBarScrollButtonWidth),
    MAKE_PM(PM_ToolBarFrameWidth),
    MAKE_PM(PM_ToolBarHandleExtent),
    MAKE_PM(PM_ToolBarItemSpacing),
    MAKE_PM(PM_ToolBarItemMargin),
    MAKE_PM(PM_ToolBarSeparatorExtent),
    MAKE_PM(PM_ToolBarExtensionExtent),
    MAKE_PM(PM_SpinBoxSliderHeight),
    MAKE_PM(PM_DefaultTopLevelMargin),
    MAKE_PM(PM_DefaultChildMargin),
    MAKE_PM(PM_DefaultLayoutSpacing),
    MAKE_PM(PM_ToolBarIconSize),
    MAKE_PM(PM_ListViewIconSize),
    MAKE_PM(PM_IconViewIconSize),
    MAKE_PM(PM_SmallIconSize),
    MAKE_PM(PM_LargeIconSize),
    MAKE_PM(PM_FocusFrameVMargin),
    MAKE_PM(PM_FocusFrameHMargin),
    MAKE_PM(PM_ToolTipLabelFrameWidth),
    MAKE_PM(PM_CheckBoxLabelSpacing),
    MAKE_PM(PM_TabBarIconSize),
    MAKE_PM(PM_SizeGripSize),
    MAKE_PM(PM_DockWidgetTitleMargin),
    MAKE_PM(PM_MessageBoxIconSize),
    MAKE_PM(PM_ButtonIconSize),
    MAKE_PM(PM_DockWidgetTitleBarButtonMargin),
    MAKE_PM(PM_RadioButtonLabelSpacing),
    MAKE_PM(PM_LayoutLeftMargin),
    MAKE_PM(PM_LayoutTopMargin),
    MAKE_PM(PM_LayoutRightMargin),
    MAKE_PM(PM_LayoutBottomMargin),
    MAKE_PM(PM_LayoutHorizontalSpacing),
    MAKE_PM(PM_LayoutVerticalSpacing),
    MAKE_PM(PM_TabBar_ScrollButtonOverlap),
    MAKE_PM(PM_TextCursorWidth),
    MAKE_PM(PM_TabCloseIndicatorWidth),
    MAKE_PM(PM_TabCloseIndicatorHeight),
    MAKE_PM(PM_ScrollView_ScrollBarSpacing),
    MAKE_PM(PM_SubMenuOverlap)
};

PixelMetricModel::PixelMetricModel(QObject *parent)
    : AbstractStyleElementModel(parent)
{
}

QVariant PixelMetricModel::doData(int row, int column, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case 0:
            return pixelMetrics[row].name;
        case 1:
            return effectiveStyle()->pixelMetric(pixelMetrics[row].pixelMetric);
        }
    }

    return QVariant();
}

int PixelMetricModel::doColumnCount() const
{
    return 2;
}

int PixelMetricModel::doRowCount() const
{
    return sizeof(pixelMetrics) / sizeof(pixel_metric_t);
}

QVariant PixelMetricModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Metric");
        case 1:
            return tr("Default Value");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags PixelMetricModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags baseFlags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.column() == 1 && isMainStyle())
        return baseFlags | Qt::ItemIsEditable;
    return baseFlags;
}

bool PixelMetricModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()
        || index.column() != 1
        || !value.isValid()
        || !value.canConvert(QVariant::Int)
        || role != Qt::EditRole)
        return false;

    DynamicProxyStyle::instance()->setPixelMetric(
        pixelMetrics[index.row()].pixelMetric, value.toInt());
    emit dataChanged(index, index);
    return true;
}
