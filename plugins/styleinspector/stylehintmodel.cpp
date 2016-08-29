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
#include "dynamicproxystyle.h"

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

namespace StyleHintType {
enum Type {
    Bool,
    Int,
    Color,
    Char,
    Alignment,
    EventType,
    FocusPolicy,
    ColorRole,
    ElideMode,
    MouseButtons,
    LayoutDirection,
    FrameStyle,
    TextInteractionFlags,
    WizardStyle,
    TabPosition,
    FormWrapPolicy,
    FormGrowthPolicy,
    ToolButtonStyle,
    RequestInputPanel,
    ScrollMode,

    FirstEnumType = Alignment
};
}

namespace StyleHintExtraType {
enum Type {
    None,
    Region,
    Variant
};
}

struct StyleHintInfo {
    QStyle::StyleHint hint;
    const char *name;
    StyleHintType::Type type;
    StyleHintExtraType::Type extraType;
};

#define SH(h, t, x) { QStyle::h, #h, StyleHintType::t, StyleHintExtraType::x },
static const StyleHintInfo style_hint_table[] = {
    SH(SH_EtchDisabledText,                               Bool,                 None   )
    SH(SH_DitherDisabledText,                             Bool,                 None   )
    SH(SH_ScrollBar_MiddleClickAbsolutePosition,          Bool,                 None   )
    SH(SH_ScrollBar_ScrollWhenPointerLeavesControl,       Bool,                 None   )
    SH(SH_TabBar_SelectMouseType,                         EventType,            None   )
    SH(SH_TabBar_Alignment,                               Alignment,            None   )
    SH(SH_Header_ArrowAlignment,                          Alignment,            None   )
    SH(SH_Slider_SnapToValue,                             Bool,                 None   )
    SH(SH_Slider_SloppyKeyEvents,                         Bool,                 None   )
    SH(SH_ProgressDialog_CenterCancelButton,              Bool,                 None   )
    SH(SH_ProgressDialog_TextLabelAlignment,              Alignment,            None   )
    SH(SH_PrintDialog_RightAlignButtons,                  Bool,                 None   )
    SH(SH_MainWindow_SpaceBelowMenuBar,                   Int,                  None   )
    SH(SH_FontDialog_SelectAssociatedText,                Bool,                 None   )
    SH(SH_Menu_AllowActiveAndDisabled,                    Bool,                 None   )
    SH(SH_Menu_SpaceActivatesItem,                        Bool,                 None   )
    SH(SH_Menu_SubMenuPopupDelay,                         Int,                  None   )
    SH(SH_ScrollView_FrameOnlyAroundContents,             Bool,                 None   )
    SH(SH_MenuBar_AltKeyNavigation,                       Bool,                 None   )
    SH(SH_ComboBox_ListMouseTracking,                     Bool,                 None   )
    SH(SH_Menu_MouseTracking,                             Bool,                 None   )
    SH(SH_MenuBar_MouseTracking,                          Bool,                 None   )
    SH(SH_ItemView_ChangeHighlightOnFocus,                Bool,                 None   )
    SH(SH_Widget_ShareActivation,                         Bool,                 None   )
    SH(SH_Workspace_FillSpaceOnMaximize,                  Bool,                 None   )
    SH(SH_ComboBox_Popup,                                 Bool,                 None   )
    SH(SH_TitleBar_NoBorder,                              Bool,                 None   )
    SH(SH_Slider_StopMouseOverSlider,                     Bool,                 None   )
    SH(SH_BlinkCursorWhenTextSelected,                    Bool,                 None   )
    SH(SH_RichText_FullWidthSelection,                    Bool,                 None   )
    SH(SH_Menu_Scrollable,                                Bool,                 None   )
    SH(SH_GroupBox_TextLabelVerticalAlignment,            Alignment,            None   )
    SH(SH_GroupBox_TextLabelColor,                        Color,                None   )
    SH(SH_Menu_SloppySubMenus,                            Bool,                 None   )
    SH(SH_Table_GridLineColor,                            Color,                None   )
    SH(SH_LineEdit_PasswordCharacter,                     Char,                 None   )
    SH(SH_DialogButtons_DefaultButton,                    Int,                  None   ) // TODO QDialogButtonBox::ButtonRole
    SH(SH_ToolBox_SelectedPageTitleBold,                  Bool,                 None   )
    SH(SH_TabBar_PreferNoArrows,                          Bool,                 None   )
    SH(SH_ScrollBar_LeftClickAbsolutePosition,            Bool,                 None   )
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    SH(SH_ListViewExpand_SelectMouseType,                 EventType,            None   )
#else
    SH(SH_Q3ListViewExpand_SelectMouseType,               EventType,            None   )
#endif
    SH(SH_UnderlineShortcut,                              Bool,                 None   )
    SH(SH_SpinBox_AnimateButton,                          Bool,                 None   )
    SH(SH_SpinBox_KeyPressAutoRepeatRate,                 Int,                  None   )
    SH(SH_SpinBox_ClickAutoRepeatRate,                    Int,                  None   )
    SH(SH_Menu_FillScreenWithScroll,                      Bool,                 None   )
    SH(SH_ToolTipLabel_Opacity,                           Int,                  None   )
    SH(SH_DrawMenuBarSeparator,                           Bool,                 None   )
    SH(SH_TitleBar_ModifyNotification,                    Bool,                 None   )
    SH(SH_Button_FocusPolicy,                             FocusPolicy,          None   )
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    SH(SH_MenuBar_DismissOnSecondClick,                   Bool,                 None   )
#endif
    SH(SH_MessageBox_UseBorderForButtonSpacing,           Bool,                 None   )
    SH(SH_TitleBar_AutoRaise,                             Bool,                 None   )
    SH(SH_ToolButton_PopupDelay,                          Int,                  None   )
    SH(SH_FocusFrame_Mask,                                Bool,                 Region )
    SH(SH_RubberBand_Mask,                                Bool,                 Region )
    SH(SH_WindowFrame_Mask,                               Bool,                 Region )
    SH(SH_SpinControls_DisableOnBounds,                   Bool,                 None   )
    SH(SH_Dial_BackgroundRole,                            ColorRole,            None   )
    SH(SH_ComboBox_LayoutDirection,                       LayoutDirection,      None   )
    SH(SH_ItemView_EllipsisLocation,                      Alignment,            None   )
    SH(SH_ItemView_ShowDecorationSelected,                Bool,                 None   )
    SH(SH_ItemView_ActivateItemOnSingleClick,             Bool,                 None   )
    SH(SH_ScrollBar_ContextMenu,                          Bool,                 None   )
    SH(SH_ScrollBar_RollBetweenButtons,                   Bool,                 None   )
    SH(SH_Slider_AbsoluteSetButtons,                      MouseButtons,         None   )
    SH(SH_Slider_PageSetButtons,                          MouseButtons,         None   )
    SH(SH_Menu_KeyboardSearch,                            Bool,                 None   )
    SH(SH_TabBar_ElideMode,                               ElideMode,            None   )
    SH(SH_DialogButtonLayout,                             Int,                  None   ) // TODO QDialogButtonBox::ButtonLayout
    SH(SH_ComboBox_PopupFrameStyle,                       FrameStyle,           None   )
    SH(SH_MessageBox_TextInteractionFlags,                TextInteractionFlags, None   )
    SH(SH_DialogButtonBox_ButtonsHaveIcons,               Bool,                 None   )
    SH(SH_SpellCheckUnderlineStyle,                       Int,                  None   ) // TODO QTextCharFormat::UnderlineStyle
    SH(SH_MessageBox_CenterButtons,                       Bool,                 None   )
    SH(SH_Menu_SelectionWrap,                             Bool,                 None   )
    SH(SH_ItemView_MovementWithoutUpdatingSelection,      Bool,                 None   )
    SH(SH_ToolTip_Mask,                                   Bool,                 Region )
    SH(SH_FocusFrame_AboveWidget,                         Bool,                 None   )
    SH(SH_TextControl_FocusIndicatorTextCharFormat,       Int,                  Variant)
    SH(SH_WizardStyle,                                    WizardStyle,          None   )
    SH(SH_ItemView_ArrowKeysNavigateIntoChildren,         Bool,                 None   )
    SH(SH_Menu_Mask ,                                     Bool,                 Region )
    SH(SH_Menu_FlashTriggeredItem,                        Bool,                 None   )
    SH(SH_Menu_FadeOutOnHide,                             Bool,                 None   )
    SH(SH_SpinBox_ClickAutoRepeatThreshold,               Int,                  None   )
    SH(SH_ItemView_PaintAlternatingRowColorsForEmptyArea, Bool,                 None   )
    SH(SH_FormLayoutWrapPolicy,                           FormWrapPolicy,       None   )
    SH(SH_TabWidget_DefaultTabPosition,                   TabPosition,          None   )
    SH(SH_ToolBar_Movable,                                Bool,                 None   )
    SH(SH_FormLayoutFieldGrowthPolicy,                    FormGrowthPolicy,     None   )
    SH(SH_FormLayoutFormAlignment,                        Alignment,            None   )
    SH(SH_FormLayoutLabelAlignment,                       Alignment,            None   )
    SH(SH_ItemView_DrawDelegateFrame,                     Bool,                 None   )
    SH(SH_TabBar_CloseButtonPosition,                     Int,                  None   ) // TODO QTabBar::ButtonPosition
    SH(SH_DockWidget_ButtonsHaveFrame,                    Bool,                 None   )
    SH(SH_ToolButtonStyle,                                ToolButtonStyle,      None   )
    SH(SH_RequestSoftwareInputPanel,                      RequestInputPanel,    None   )
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    SH(SH_ScrollBar_Transient,                            Bool,                 None   )
    SH(SH_Menu_SupportsSections,                          Bool,                 None   )
    SH(SH_ToolTip_WakeUpDelay,                            Int,                  None   )
    SH(SH_ToolTip_FallAsleepDelay,                        Int,                  None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    SH(SH_Widget_Animate,                                 Bool,                 None   )
    SH(SH_Splitter_OpaqueResize,                          Bool,                 None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    SH(SH_ComboBox_UseNativePopup,                        Bool,                 None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    SH(SH_LineEdit_PasswordMaskDelay,                     Int,                  None   )
    SH(SH_TabBar_ChangeCurrentDelay,                      Int,                  None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    SH(SH_Menu_SubMenuUniDirection,                       Bool,                 None   )
    SH(SH_Menu_SubMenuUniDirectionFailCount,              Int,                  None   )
    SH(SH_Menu_SubMenuSloppySelectOtherActions,           Bool,                 None   )
    SH(SH_Menu_SubMenuSloppyCloseTimeout,                 Int,                  None   )
    SH(SH_Menu_SubMenuResetWhenReenteringParent,          Bool,                 None   )
    SH(SH_Menu_SubMenuDontStartSloppyOnLeave,             Bool,                 None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    SH(SH_ItemView_ScrollMode,                            ScrollMode,           None   )
#endif
};
#undef SH

static const int style_hint_count = sizeof(style_hint_table) / sizeof(StyleHintInfo);


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
    Q_ASSERT(row >= 0 && row < style_hint_count);
    if (role == Qt::DisplayRole && column == 0) {
        return style_hint_table[row].name;
    }
    if (role == Qt::DisplayRole && column == 1) {
        const auto h = effectiveStyle()->styleHint(static_cast<QStyle::StyleHint>(row), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        return VariantHandler::displayString(styleHintToVariant(static_cast<QStyle::StyleHint>(row), h));
    }
    if (role == Qt::EditRole && column == 1) {
        const auto h = effectiveStyle()->styleHint(static_cast<QStyle::StyleHint>(row), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        if (style_hint_table[row].type < StyleHintType::FirstEnumType)
            return styleHintToVariant(static_cast<QStyle::StyleHint>(row), h);
    }
    if (role == Qt::DecorationRole && column == 1) {
        const auto h = effectiveStyle()->styleHint(static_cast<QStyle::StyleHint>(row), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
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
    return style_hint_count;
}

QVariant StyleHintModel::styleHintToVariant(QStyle::StyleHint hint, int value) const
{
    switch (style_hint_table[hint].type) {
        case StyleHintType::Bool:
            return QVariant::fromValue<bool>(value);
        case StyleHintType::Int:
            return value;
        case StyleHintType::Alignment:
            return EnumUtil::enumToString(value, "Qt::Alignment");
        case StyleHintType::EventType:
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            return EnumUtil::enumToString(value, "QEvent::Type", &QEvent::staticMetaObject);
#else
            return value;
#endif
        case StyleHintType::Color:
            return QVariant::fromValue(QColor(value));
        case StyleHintType::Char:
            return QChar(value);
        case StyleHintType::FocusPolicy:
            return EnumUtil::enumToString(value, "Qt::FocusPolicy");
        case StyleHintType::ColorRole:
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            return EnumUtil::enumToString(value, "QPalette::ColorRole", &QPalette::staticMetaObject);
#else
            return value;
#endif
        case StyleHintType::ElideMode:
            return EnumUtil::enumToString(value, "Qt::TextElideMode");
        case StyleHintType::MouseButtons:
            return EnumUtil::enumToString(value, "Qt::MouseButtons");
        case StyleHintType::LayoutDirection:
            return EnumUtil::enumToString(value, "Qt::LayoutDirection");
        case StyleHintType::FrameStyle:
            return QString(EnumUtil::enumToString(value & QFrame::Shadow_Mask, "QFrame::Shadow", &QFrame::staticMetaObject)
                 + " / " + EnumUtil::enumToString(value & QFrame::Shape_Mask, "QFrame::Shape", &QFrame::staticMetaObject));
        case StyleHintType::TextInteractionFlags:
            return EnumUtil::enumToString(value, "Qt::TextInteractionFlags");
        case StyleHintType::WizardStyle:
            return EnumUtil::enumToString(value, "QWizard::WizardStyle", &QWizard::staticMetaObject);
        case StyleHintType::TabPosition:
            return EnumUtil::enumToString(value, "QTabWidget::TabPosition", &QTabWidget::staticMetaObject);
        case StyleHintType::FormWrapPolicy:
            return EnumUtil::enumToString(value, "QFormLayout::RowWrapPolicy", &QFormLayout::staticMetaObject);
        case StyleHintType::FormGrowthPolicy:
            return EnumUtil::enumToString(value, "QFormLayout::FieldGrowthPolicy", &QFormLayout::staticMetaObject);
        case StyleHintType::ToolButtonStyle:
            return EnumUtil::enumToString(value, "Qt::ToolButtonStyle");
        case StyleHintType::RequestInputPanel:
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
            return EnumUtil::enumToString(value, "QStyle::RequestSoftwareInputPanel", &QStyle::staticMetaObject);
#else
            return value;
#endif
        case StyleHintType::ScrollMode:
            return EnumUtil::enumToString(value, "QAbstractItemView::ScrollMode", &QAbstractItemView::staticMetaObject);
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
            effectiveStyle()->styleHint(hint, opt, Q_NULLPTR, &data);
            delete opt;
            return data.variant;
        }
        case QStyle::SH_RubberBand_Mask:
        {
            QStyleOptionRubberBand opt;
            opt.shape = QRubberBand::Rectangle;
            opt.rect = QRect(0, 0, 100, 100);
            QStyleHintReturnMask data;
            effectiveStyle()->styleHint(hint, &opt, Q_NULLPTR, &data);
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
            effectiveStyle()->styleHint(hint, &opt, Q_NULLPTR, &data);
            return data.region;
        }
        default: break;
    }
    return QVariant();
}

Qt::ItemFlags StyleHintModel::flags(const QModelIndex &index) const
{
    const auto baseFlags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.column() == 1 && isMainStyle()
        && style_hint_table[index.row()].extraType == StyleHintExtraType::None
        && style_hint_table[index.row()].type < StyleHintType::FirstEnumType) {
        return baseFlags | Qt::ItemIsEditable;
    }
    return baseFlags;
}

bool StyleHintModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 1 || role != Qt::EditRole)
        return false;

    int i = value.toInt();
    if (value.type() == QVariant::Color)
        i = value.value<QColor>().rgba();
    DynamicProxyStyle::instance()->setStyleHint(static_cast<QStyle::StyleHint>(index.row()), i);
    emit dataChanged(index, index);
    return true;
}
