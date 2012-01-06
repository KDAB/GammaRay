#include "pixelmetricmodel.h"

#include <QStyle>

using namespace GammaRay;

PixelMetricModel::PixelMetricModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void PixelMetricModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

#define MAKE_PM(metric) \
if (index.row() == QStyle:: metric && index.column() == 0) return QLatin1String(#metric); \
if (index.row() == QStyle:: metric && index.column() == 1) return m_style->pixelMetric(QStyle :: metric)

QVariant PixelMetricModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    MAKE_PM(PM_ButtonMargin);
    MAKE_PM(PM_ButtonDefaultIndicator);
    MAKE_PM(PM_MenuButtonIndicator);
    MAKE_PM(PM_ButtonShiftHorizontal);
    MAKE_PM(PM_ButtonShiftVertical);
    MAKE_PM(PM_DefaultFrameWidth);
    MAKE_PM(PM_SpinBoxFrameWidth);
    MAKE_PM(PM_ComboBoxFrameWidth);
    MAKE_PM(PM_MaximumDragDistance);
    MAKE_PM(PM_ScrollBarExtent);
    MAKE_PM(PM_ScrollBarSliderMin);
    MAKE_PM(PM_SliderThickness);
    MAKE_PM(PM_SliderControlThickness);
    MAKE_PM(PM_SliderLength);
    MAKE_PM(PM_SliderTickmarkOffset);
    MAKE_PM(PM_SliderSpaceAvailable);
    MAKE_PM(PM_DockWidgetSeparatorExtent);
    MAKE_PM(PM_DockWidgetHandleExtent);
    MAKE_PM(PM_DockWidgetFrameWidth);
    MAKE_PM(PM_TabBarTabOverlap);
    MAKE_PM(PM_TabBarTabHSpace);
    MAKE_PM(PM_TabBarTabVSpace);
    MAKE_PM(PM_TabBarBaseHeight);
    MAKE_PM(PM_TabBarBaseOverlap);
    MAKE_PM(PM_ProgressBarChunkWidth);
    MAKE_PM(PM_SplitterWidth);
    MAKE_PM(PM_TitleBarHeight);
    MAKE_PM(PM_MenuScrollerHeight);
    MAKE_PM(PM_MenuHMargin);
    MAKE_PM(PM_MenuVMargin);
    MAKE_PM(PM_MenuPanelWidth);
    MAKE_PM(PM_MenuTearoffHeight);
    MAKE_PM(PM_MenuDesktopFrameWidth);
    MAKE_PM(PM_MenuBarPanelWidth);
    MAKE_PM(PM_MenuBarItemSpacing);
    MAKE_PM(PM_MenuBarVMargin);
    MAKE_PM(PM_MenuBarHMargin);
    MAKE_PM(PM_IndicatorWidth);
    MAKE_PM(PM_IndicatorHeight);
    MAKE_PM(PM_ExclusiveIndicatorWidth);
    MAKE_PM(PM_ExclusiveIndicatorHeight);
    MAKE_PM(PM_CheckListButtonSize);
    MAKE_PM(PM_CheckListControllerSize);
    MAKE_PM(PM_DialogButtonsSeparator);
    MAKE_PM(PM_DialogButtonsButtonWidth);
    MAKE_PM(PM_DialogButtonsButtonHeight);
    MAKE_PM(PM_MdiSubWindowFrameWidth);
    MAKE_PM(PM_MdiSubWindowMinimizedWidth);
    MAKE_PM(PM_HeaderMargin);
    MAKE_PM(PM_HeaderMarkSize);
    MAKE_PM(PM_HeaderGripMargin);
    MAKE_PM(PM_TabBarTabShiftHorizontal);
    MAKE_PM(PM_TabBarTabShiftVertical);
    MAKE_PM(PM_TabBarScrollButtonWidth);
    MAKE_PM(PM_ToolBarFrameWidth);
    MAKE_PM(PM_ToolBarHandleExtent);
    MAKE_PM(PM_ToolBarItemSpacing);
    MAKE_PM(PM_ToolBarItemMargin);
    MAKE_PM(PM_ToolBarSeparatorExtent);
    MAKE_PM(PM_ToolBarExtensionExtent);
    MAKE_PM(PM_SpinBoxSliderHeight);
    MAKE_PM(PM_DefaultTopLevelMargin);
    MAKE_PM(PM_DefaultChildMargin);
    MAKE_PM(PM_DefaultLayoutSpacing);
    MAKE_PM(PM_ToolBarIconSize);
    MAKE_PM(PM_ListViewIconSize);
    MAKE_PM(PM_IconViewIconSize);
    MAKE_PM(PM_SmallIconSize);
    MAKE_PM(PM_LargeIconSize);
    MAKE_PM(PM_FocusFrameVMargin);
    MAKE_PM(PM_FocusFrameHMargin);
    MAKE_PM(PM_ToolTipLabelFrameWidth);
    MAKE_PM(PM_CheckBoxLabelSpacing);
    MAKE_PM(PM_TabBarIconSize);
    MAKE_PM(PM_SizeGripSize);
    MAKE_PM(PM_DockWidgetTitleMargin);
    MAKE_PM(PM_MessageBoxIconSize);
    MAKE_PM(PM_ButtonIconSize);
    MAKE_PM(PM_DockWidgetTitleBarButtonMargin);
    MAKE_PM(PM_RadioButtonLabelSpacing);
    MAKE_PM(PM_LayoutLeftMargin);
    MAKE_PM(PM_LayoutTopMargin);
    MAKE_PM(PM_LayoutRightMargin);
    MAKE_PM(PM_LayoutBottomMargin);
    MAKE_PM(PM_LayoutHorizontalSpacing);
    MAKE_PM(PM_LayoutVerticalSpacing);
    MAKE_PM(PM_TabBar_ScrollButtonOverlap);
    MAKE_PM(PM_TextCursorWidth);
    MAKE_PM(PM_TabCloseIndicatorWidth);
    MAKE_PM(PM_TabCloseIndicatorHeight);
    MAKE_PM(PM_ScrollView_ScrollBarSpacing);
    MAKE_PM(PM_SubMenuOverlap);
  }

  return QVariant();
}

int PixelMetricModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int PixelMetricModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return QStyle::PM_SubMenuOverlap + 1;
}

QVariant PixelMetricModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Metric");
      case 1: return tr("Default Value");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "pixelmetricmodel.moc"
