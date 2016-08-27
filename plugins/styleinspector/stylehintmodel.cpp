/*
  stylehintmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "stylehintmodel.h"
#include "styleoption.h"

#include <core/enumutil.h>
#include <core/varianthandler.h>

#include <QAbstractItemView>
#include <QDebug>
#include <QEvent>
#include <QFormLayout>
#include <QStyleHintReturnMask>
#include <QStyleHintReturnVariant>
#include <QTabBar>
#include <QWizard>

using namespace GammaRay;

StyleHintModel::StyleHintModel(QObject *parent) :
    AbstractStyleElementModel(parent)
{
}

StyleHintModel::~StyleHintModel()
{
}

QVariant StyleHintModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Style Hint");
            case 1: return tr("Value");
            case 2: return tr("Return Data");
        }
    }
    return AbstractStyleElementModel::headerData(section, orientation, role);
}

#define MAKE_SH(hint) case QStyle:: hint: return QStringLiteral(#hint);

QVariant StyleHintModel::doData(int row, int column, int role) const
{
    if (role == Qt::DisplayRole && column == 0) {
        switch (row) {
            MAKE_SH(SH_EtchDisabledText)
            MAKE_SH(SH_DitherDisabledText)
            MAKE_SH(SH_ScrollBar_MiddleClickAbsolutePosition)
            MAKE_SH(SH_ScrollBar_ScrollWhenPointerLeavesControl)
            MAKE_SH(SH_TabBar_SelectMouseType)
            MAKE_SH(SH_TabBar_Alignment)
            MAKE_SH(SH_Header_ArrowAlignment)
            MAKE_SH(SH_Slider_SnapToValue)
            MAKE_SH(SH_Slider_SloppyKeyEvents)
            MAKE_SH(SH_ProgressDialog_CenterCancelButton)
            MAKE_SH(SH_ProgressDialog_TextLabelAlignment)
            MAKE_SH(SH_PrintDialog_RightAlignButtons)
            MAKE_SH(SH_MainWindow_SpaceBelowMenuBar)
            MAKE_SH(SH_FontDialog_SelectAssociatedText)
            MAKE_SH(SH_Menu_AllowActiveAndDisabled)
            MAKE_SH(SH_Menu_SpaceActivatesItem)
            MAKE_SH(SH_Menu_SubMenuPopupDelay)
            MAKE_SH(SH_ScrollView_FrameOnlyAroundContents)
            MAKE_SH(SH_MenuBar_AltKeyNavigation)
            MAKE_SH(SH_ComboBox_ListMouseTracking)
            MAKE_SH(SH_Menu_MouseTracking)
            MAKE_SH(SH_MenuBar_MouseTracking)
            MAKE_SH(SH_ItemView_ChangeHighlightOnFocus)
            MAKE_SH(SH_Widget_ShareActivation)
            MAKE_SH(SH_Workspace_FillSpaceOnMaximize)
            MAKE_SH(SH_ComboBox_Popup)
            MAKE_SH(SH_TitleBar_NoBorder)
            MAKE_SH(SH_Slider_StopMouseOverSlider)
            MAKE_SH(SH_BlinkCursorWhenTextSelected)
            MAKE_SH(SH_RichText_FullWidthSelection)
            MAKE_SH(SH_Menu_Scrollable)
            MAKE_SH(SH_GroupBox_TextLabelVerticalAlignment)
            MAKE_SH(SH_GroupBox_TextLabelColor)
            MAKE_SH(SH_Menu_SloppySubMenus)
            MAKE_SH(SH_Table_GridLineColor)
            MAKE_SH(SH_LineEdit_PasswordCharacter)
            MAKE_SH(SH_DialogButtons_DefaultButton)
            MAKE_SH(SH_ToolBox_SelectedPageTitleBold)
            MAKE_SH(SH_TabBar_PreferNoArrows)
            MAKE_SH(SH_ScrollBar_LeftClickAbsolutePosition)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            MAKE_SH(SH_ListViewExpand_SelectMouseType)
#else
            MAKE_SH(SH_Q3ListViewExpand_SelectMouseType)
#endif
            MAKE_SH(SH_UnderlineShortcut)
            MAKE_SH(SH_SpinBox_AnimateButton)
            MAKE_SH(SH_SpinBox_KeyPressAutoRepeatRate)
            MAKE_SH(SH_SpinBox_ClickAutoRepeatRate)
            MAKE_SH(SH_Menu_FillScreenWithScroll)
            MAKE_SH(SH_ToolTipLabel_Opacity)
            MAKE_SH(SH_DrawMenuBarSeparator)
            MAKE_SH(SH_TitleBar_ModifyNotification)
            MAKE_SH(SH_Button_FocusPolicy)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            MAKE_SH(SH_MenuBar_DismissOnSecondClick)
#endif
            MAKE_SH(SH_MessageBox_UseBorderForButtonSpacing)
            MAKE_SH(SH_TitleBar_AutoRaise)
            MAKE_SH(SH_ToolButton_PopupDelay)
            MAKE_SH(SH_FocusFrame_Mask)
            MAKE_SH(SH_RubberBand_Mask)
            MAKE_SH(SH_WindowFrame_Mask)
            MAKE_SH(SH_SpinControls_DisableOnBounds)
            MAKE_SH(SH_Dial_BackgroundRole)
            MAKE_SH(SH_ComboBox_LayoutDirection)
            MAKE_SH(SH_ItemView_EllipsisLocation)
            MAKE_SH(SH_ItemView_ShowDecorationSelected)
            MAKE_SH(SH_ItemView_ActivateItemOnSingleClick)
            MAKE_SH(SH_ScrollBar_ContextMenu)
            MAKE_SH(SH_ScrollBar_RollBetweenButtons)
            MAKE_SH(SH_Slider_AbsoluteSetButtons)
            MAKE_SH(SH_Slider_PageSetButtons)
            MAKE_SH(SH_Menu_KeyboardSearch)
            MAKE_SH(SH_TabBar_ElideMode)
            MAKE_SH(SH_DialogButtonLayout)
            MAKE_SH(SH_ComboBox_PopupFrameStyle)
            MAKE_SH(SH_MessageBox_TextInteractionFlags)
            MAKE_SH(SH_DialogButtonBox_ButtonsHaveIcons)
            MAKE_SH(SH_SpellCheckUnderlineStyle)
            MAKE_SH(SH_MessageBox_CenterButtons)
            MAKE_SH(SH_Menu_SelectionWrap)
            MAKE_SH(SH_ItemView_MovementWithoutUpdatingSelection)
            MAKE_SH(SH_ToolTip_Mask)
            MAKE_SH(SH_FocusFrame_AboveWidget)
            MAKE_SH(SH_TextControl_FocusIndicatorTextCharFormat)
            MAKE_SH(SH_WizardStyle)
            MAKE_SH(SH_ItemView_ArrowKeysNavigateIntoChildren)
            MAKE_SH(SH_Menu_Mask)
            MAKE_SH(SH_Menu_FlashTriggeredItem)
            MAKE_SH(SH_Menu_FadeOutOnHide)
            MAKE_SH(SH_SpinBox_ClickAutoRepeatThreshold)
            MAKE_SH(SH_ItemView_PaintAlternatingRowColorsForEmptyArea)
            MAKE_SH(SH_FormLayoutWrapPolicy)
            MAKE_SH(SH_TabWidget_DefaultTabPosition)
            MAKE_SH(SH_ToolBar_Movable)
            MAKE_SH(SH_FormLayoutFieldGrowthPolicy)
            MAKE_SH(SH_FormLayoutFormAlignment)
            MAKE_SH(SH_FormLayoutLabelAlignment)
            MAKE_SH(SH_ItemView_DrawDelegateFrame)
            MAKE_SH(SH_TabBar_CloseButtonPosition)
            MAKE_SH(SH_DockWidget_ButtonsHaveFrame)
            MAKE_SH(SH_ToolButtonStyle)
            MAKE_SH(SH_RequestSoftwareInputPanel)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            MAKE_SH(SH_ScrollBar_Transient)
            MAKE_SH(SH_Menu_SupportsSections)
            MAKE_SH(SH_ToolTip_WakeUpDelay)
            MAKE_SH(SH_ToolTip_FallAsleepDelay)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
            MAKE_SH(SH_Widget_Animate)
            MAKE_SH(SH_Splitter_OpaqueResize)
            MAKE_SH(SH_ComboBox_UseNativePopup)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
            MAKE_SH(SH_LineEdit_PasswordMaskDelay)
            MAKE_SH(SH_TabBar_ChangeCurrentDelay)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            MAKE_SH(SH_Menu_SubMenuUniDirection)
            MAKE_SH(SH_Menu_SubMenuUniDirectionFailCount)
            MAKE_SH(SH_Menu_SubMenuSloppySelectOtherActions)
            MAKE_SH(SH_Menu_SubMenuSloppyCloseTimeout)
            MAKE_SH(SH_Menu_SubMenuResetWhenReenteringParent)
            MAKE_SH(SH_Menu_SubMenuDontStartSloppyOnLeave)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
            MAKE_SH(SH_ItemView_ScrollMode)
#endif
        }
    }
    if (role == Qt::DisplayRole && column == 1) {
        const auto h = m_style->styleHint(static_cast<QStyle::StyleHint>(row), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        return VariantHandler::displayString(styleHintToVariant(static_cast<QStyle::StyleHint>(row), h));
    }
    if (role == Qt::DecorationRole && column == 1) {
        const auto h = m_style->styleHint(static_cast<QStyle::StyleHint>(row), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        return VariantHandler::decoration(styleHintToVariant(static_cast<QStyle::StyleHint>(row), h));

    }
    if (role == Qt::DisplayRole && column == 2) {
        return VariantHandler::displayString(styleHintData(static_cast<QStyle::StyleHint>(row)));
    }
    return QVariant();
}

int StyleHintModel::doColumnCount() const
{
    return 3;
}

int StyleHintModel::doRowCount() const
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QStyle::SH_RequestSoftwareInputPanel + 1;
#elif QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    return QStyle::SH_ToolTip_FallAsleepDelay + 1;
#elif QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
    return QStyle::SH_ComboBox_UseNativePopup + 1;
#elif QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    return QStyle::SH_TabBar_ChangeCurrentDelay + 1;
#elif QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    return QStyle::SH_MenuDontStartSloppyOnLeave + 1;
#else
    return QStyle::SH_ItemView_ScrollMode + 1;
#endif
}

QVariant StyleHintModel::styleHintToVariant(QStyle::StyleHint hint, int value) const
{
    switch (hint) {
        case QStyle::SH_EtchDisabledText:
        case QStyle::SH_DitherDisabledText:
        case QStyle::SH_Slider_SnapToValue:
        case QStyle::SH_Slider_SloppyKeyEvents:
        case QStyle::SH_ProgressDialog_CenterCancelButton:
        case QStyle::SH_PrintDialog_RightAlignButtons:
        case QStyle::SH_ScrollBar_MiddleClickAbsolutePosition:
        case QStyle::SH_ScrollBar_ScrollWhenPointerLeavesControl:
        case QStyle::SH_Menu_SloppySubMenus:
        case QStyle::SH_TitleBar_AutoRaise:
        case QStyle::SH_FontDialog_SelectAssociatedText:
        case QStyle::SH_Menu_AllowActiveAndDisabled:
        case QStyle::SH_Menu_SpaceActivatesItem:
        case QStyle::SH_ScrollView_FrameOnlyAroundContents:
        case QStyle::SH_MenuBar_AltKeyNavigation:
        case QStyle::SH_ComboBox_ListMouseTracking:
        case QStyle::SH_Menu_MouseTracking:
        case QStyle::SH_MenuBar_MouseTracking:
        case QStyle::SH_ItemView_ChangeHighlightOnFocus:
        case QStyle::SH_Widget_ShareActivation:
        case QStyle::SH_Workspace_FillSpaceOnMaximize:
        case QStyle::SH_ComboBox_Popup:
        case QStyle::SH_TitleBar_NoBorder:
        case QStyle::SH_Slider_StopMouseOverSlider:
        case QStyle::SH_BlinkCursorWhenTextSelected:
        case QStyle::SH_RichText_FullWidthSelection:
        case QStyle::SH_Menu_Scrollable:
        case QStyle::SH_ToolBox_SelectedPageTitleBold:
        case QStyle::SH_TabBar_PreferNoArrows:
        case QStyle::SH_ScrollBar_LeftClickAbsolutePosition:
        case QStyle::SH_UnderlineShortcut:
        case QStyle::SH_SpinBox_AnimateButton:
        case QStyle::SH_DrawMenuBarSeparator:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        case QStyle::SH_MenuBar_DismissOnSecondClick:
#endif
        case QStyle::SH_MessageBox_UseBorderForButtonSpacing:
        case QStyle::SH_ScrollBar_ContextMenu:
        case QStyle::SH_SpinControls_DisableOnBounds:
        case QStyle::SH_Menu_KeyboardSearch:
        case QStyle::SH_Menu_SelectionWrap:
        case QStyle::SH_Menu_FillScreenWithScroll:
        case QStyle::SH_Menu_FlashTriggeredItem:
        case QStyle::SH_Menu_FadeOutOnHide:
        case QStyle::SH_TitleBar_ModifyNotification:
        case QStyle::SH_ItemView_ShowDecorationSelected:
        case QStyle::SH_ItemView_ActivateItemOnSingleClick:
        case QStyle::SH_ScrollBar_RollBetweenButtons:
        case QStyle::SH_DialogButtonBox_ButtonsHaveIcons:
        case QStyle::SH_MessageBox_CenterButtons:
        case QStyle::SH_ItemView_MovementWithoutUpdatingSelection:
        case QStyle::SH_FocusFrame_AboveWidget:
        case QStyle::SH_FocusFrame_Mask:
        case QStyle::SH_RubberBand_Mask:
        case QStyle::SH_WindowFrame_Mask:
        case QStyle::SH_ToolTip_Mask:
        case QStyle::SH_Menu_Mask:
        case QStyle::SH_ItemView_ArrowKeysNavigateIntoChildren:
        case QStyle::SH_ItemView_PaintAlternatingRowColorsForEmptyArea:
        case QStyle::SH_ToolBar_Movable:
        case QStyle::SH_ItemView_DrawDelegateFrame:
        case QStyle::SH_DockWidget_ButtonsHaveFrame:
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        case QStyle::SH_ScrollBar_Transient:
        case QStyle::SH_Menu_SupportsSections:
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        case QStyle::SH_Widget_Animate:
        case QStyle::SH_Splitter_OpaqueResize:
        case QStyle::SH_ComboBox_UseNativePopup:
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case QStyle::SH_Menu_SubMenuUniDirection:
        case QStyle::SH_Menu_SubMenuSloppySelectOtherActions:
        case QStyle::SH_Menu_SubMenuResetWhenReenteringParent:
        case QStyle::SH_Menu_SubMenuDontStartSloppyOnLeave:
#endif
            return QVariant::fromValue<bool>(value);
        case QStyle::SH_TabBar_Alignment:
        case QStyle::SH_Header_ArrowAlignment:
        case QStyle::SH_ProgressDialog_TextLabelAlignment:
        case QStyle::SH_GroupBox_TextLabelVerticalAlignment:
        case QStyle::SH_FormLayoutFormAlignment:
        case QStyle::SH_FormLayoutLabelAlignment:
        case QStyle::SH_ItemView_EllipsisLocation:
            return EnumUtil::enumToString(value, "Qt::Alignment");
        case QStyle::SH_TabBar_SelectMouseType:
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        case QStyle::SH_ListViewExpand_SelectMouseType:
#else
        case QStyle::SH_Q3ListViewExpand_SelectMouseType:
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            return EnumUtil::enumToString(value, "QEvent::Type", &QEvent::staticMetaObject);
#else
            return value;
#endif
        case QStyle::SH_GroupBox_TextLabelColor:
        case QStyle::SH_Table_GridLineColor:
            return QColor(value);
        case QStyle::SH_LineEdit_PasswordCharacter:
            return QChar(value);
        case QStyle::SH_Button_FocusPolicy:
            return EnumUtil::enumToString(value, "Qt::FocusPolicy");
        case QStyle::SH_Dial_BackgroundRole:
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            return EnumUtil::enumToString(value, "QPalette::ColorRole", &QPalette::staticMetaObject);
#else
            return value;
#endif
        case QStyle::SH_TabBar_ElideMode:
            return EnumUtil::enumToString(value, "Qt::TextElideMode");
        case QStyle::SH_Slider_AbsoluteSetButtons:
        case QStyle::SH_Slider_PageSetButtons:
            return EnumUtil::enumToString(value, "Qt::MouseButtons");
        case QStyle::SH_DialogButtons_DefaultButton:
            return value; // EnumUtil::enumToString(value, "QDialogButtonBox::ButtonRole", &QDialogButtonBox::staticMetaObject);
        case QStyle::SH_DialogButtonLayout:
            return value; // TODO meta enum for QDialogButtonBox::ButtonLayout
        case QStyle::SH_ComboBox_LayoutDirection:
            return EnumUtil::enumToString(value, "Qt::LayoutDirection");
        case QStyle::SH_ComboBox_PopupFrameStyle:
            return QString(EnumUtil::enumToString(value & QFrame::Shadow_Mask, "QFrame::Shadow", &QFrame::staticMetaObject)
                 + " / " + EnumUtil::enumToString(value & QFrame::Shape_Mask, "QFrame::Shape", &QFrame::staticMetaObject));
        case QStyle::SH_MessageBox_TextInteractionFlags:
            return EnumUtil::enumToString(value, "Qt::TextInteractionFlags");
        case QStyle::SH_SpellCheckUnderlineStyle:
            return value; // TODO QTextCharFormat::UnderlineStyle
        case QStyle::SH_TabWidget_DefaultTabPosition:
            return EnumUtil::enumToString(value, "QTabWidget::TabPosition", &QTabWidget::staticMetaObject);
        case QStyle::SH_WizardStyle:
            return EnumUtil::enumToString(value, "QWizard::WizardStyle", &QWizard::staticMetaObject);
        case QStyle::SH_FormLayoutWrapPolicy:
            return EnumUtil::enumToString(value, "QFormLayout::RowWrapPolicy", &QFormLayout::staticMetaObject);
        case QStyle::SH_FormLayoutFieldGrowthPolicy:
            return EnumUtil::enumToString(value, "QFormLayout::FieldGrowthPolicy", &QFormLayout::staticMetaObject);
        case QStyle::SH_ToolButtonStyle:
            return EnumUtil::enumToString(value, "Qt::ToolButtonStyle");
        case QStyle::SH_TabBar_CloseButtonPosition:
            return value; // EnumUtil::enumToString(value, "QTabBar::ButtonPosition", &QTabBar::staticMetaObject);
        case QStyle::SH_RequestSoftwareInputPanel:
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
            return EnumUtil::enumToString(value, "QStyle::RequestSoftwareInputPanel", &QStyle::staticMetaObject);
#else
            return value;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        case QStyle::SH_ItemView_ScrollMode:
            return EnumUtil::enumToString(value, "QAbstractItemView::ScrollMode", &QAbstractItemView::staticMetaObject);
#endif
        default: break;
    }
    return value;
}

QVariant StyleHintModel::styleHintData(QStyle::StyleHint hint) const
{
    switch (hint) {
        case QStyle::SH_TextControl_FocusIndicatorTextCharFormat:
        {
            const auto opt = StyleOption::makeFrameStyleOption();
            QStyleHintReturnVariant data;
            m_style->styleHint(hint, opt, Q_NULLPTR, &data);
            delete opt;
            return data.variant;
        }
        case QStyle::SH_RubberBand_Mask:
        {
            QStyleOptionRubberBand opt;
            opt.shape = QRubberBand::Rectangle;
            opt.rect = QRect(0, 0, 100, 100);
            QStyleHintReturnMask data;
            m_style->styleHint(hint, &opt, Q_NULLPTR, &data);
            return data.region;
        }
        case QStyle::SH_FocusFrame_Mask:
        case QStyle::SH_WindowFrame_Mask:
        case QStyle::SH_ToolTip_Mask:
        case QStyle::SH_Menu_Mask:
        {
            QStyleOption opt;
            opt.rect = QRect(0, 0, 100, 100);
            QStyleHintReturnMask data;
            m_style->styleHint(hint, &opt, Q_NULLPTR, &data);
            return data.region;
        }
        default: break;
    }
    return QVariant();
}
