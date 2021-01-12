/*
  stylehintmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/enumrepositoryserver.h>
#include <core/enumutil.h>
#include <core/varianthandler.h>

#include <QAbstractItemView>
#include <QDebug>
#include <QEvent>
#include <QFormLayout>
#include <QMetaEnum>
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
    FrameStyle, // this one is merged from two enums, thus not working out of the box with our enum editor
    Alignment,
    EventType,
    FocusPolicy,
    ColorRole,
    ElideMode,
    MouseButtons,
    LayoutDirection,
    TextInteractionFlags,
    WizardStyle,
    TabPosition,
    FormWrapPolicy,
    FormGrowthPolicy,
    ToolButtonStyle,
    RequestInputPanel,
    ScrollMode,
    KeyboardModifier,

    LastBasicType = Char,
    FirstEnumType = Alignment
};
}

struct StyleHintTypeInfo {
    const char *name;
    const QMetaObject *metaObject;
};

// ### must be the same order as the above enum
static const struct StyleHintTypeInfo style_hint_type_table[] = {
    { "Qt::Alignment", nullptr },
    { "QEvent::Type", &QEvent::staticMetaObject },
    { "Qt::FocusPolicy", nullptr },
    { "QPalette::ColorRole", &QPalette::staticMetaObject },
    { "Qt::TextElideMode", nullptr },
    { "Qt::MouseButtons", nullptr },
    { "Qt::LayoutDirection", nullptr },
    { "Qt::TextInteractionFlags", nullptr },
    { "QWizard::WizardStyle", &QWizard::staticMetaObject },
    { "QTabWidget::TabPosition", &QTabWidget::staticMetaObject },
    { "QFormLayout::RowWrapPolicy", &QFormLayout::staticMetaObject },
    { "QFormLayout::FieldGrowthPolicy", &QFormLayout::staticMetaObject },
    { "Qt::ToolButtonStyle", nullptr },
    { "QStyle::RequestSoftwareInputPanel", &QStyle::staticMetaObject },
    { "QAbstractItemView::ScrollMode", &QAbstractItemView::staticMetaObject },
    { "Qt::KeyboardModifier", nullptr },
};

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
    SH(SH_ListViewExpand_SelectMouseType,                 EventType,            None   )
    SH(SH_UnderlineShortcut,                              Bool,                 None   )
    SH(SH_SpinBox_AnimateButton,                          Bool,                 None   )
    SH(SH_SpinBox_KeyPressAutoRepeatRate,                 Int,                  None   )
    SH(SH_SpinBox_ClickAutoRepeatRate,                    Int,                  None   )
    SH(SH_Menu_FillScreenWithScroll,                      Bool,                 None   )
    SH(SH_ToolTipLabel_Opacity,                           Int,                  None   )
    SH(SH_DrawMenuBarSeparator,                           Bool,                 None   )
    SH(SH_TitleBar_ModifyNotification,                    Bool,                 None   )
    SH(SH_Button_FocusPolicy,                             FocusPolicy,          None   )
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    SH(SH_SpellCheckUnderlineStyle,                       Int,                  None   ) // TODO QTextCharFormat::UnderlineStyle
#endif
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
    SH(SH_ScrollBar_Transient,                            Bool,                 None   )
    SH(SH_Menu_SupportsSections,                          Bool,                 None   )
    SH(SH_ToolTip_WakeUpDelay,                            Int,                  None   )
    SH(SH_ToolTip_FallAsleepDelay,                        Int,                  None   )
    SH(SH_Widget_Animate,                                 Bool,                 None   )
    SH(SH_Splitter_OpaqueResize,                          Bool,                 None   )
    SH(SH_ComboBox_UseNativePopup,                        Bool,                 None   )
    SH(SH_LineEdit_PasswordMaskDelay,                     Int,                  None   )
    SH(SH_TabBar_ChangeCurrentDelay,                      Int,                  None   )
    SH(SH_Menu_SubMenuUniDirection,                       Bool,                 None   )
    SH(SH_Menu_SubMenuUniDirectionFailCount,              Int,                  None   )
    SH(SH_Menu_SubMenuSloppySelectOtherActions,           Bool,                 None   )
    SH(SH_Menu_SubMenuSloppyCloseTimeout,                 Int,                  None   )
    SH(SH_Menu_SubMenuResetWhenReenteringParent,          Bool,                 None   )
    SH(SH_Menu_SubMenuDontStartSloppyOnLeave,             Bool,                 None   )
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    SH(SH_ItemView_ScrollMode,                            ScrollMode,           None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    SH(SH_TitleBar_ShowToolTipsOnButtons,                 Bool,                 None   )
    SH(SH_Widget_Animation_Duration,                      Int,                  None   )
    SH(SH_ComboBox_AllowWheelScrolling,                   Bool,                 None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    SH(SH_SpinBox_ButtonsInsideFrame,                     Bool,                 None   )
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    SH(SH_SpinBox_StepModifier,                           KeyboardModifier,     None   )
#endif
};
#undef SH

static const int style_hint_count = sizeof(style_hint_table) / sizeof(StyleHintInfo);


StyleHintModel::StyleHintModel(QObject *parent) :
    AbstractStyleElementModel(parent)
{
}

StyleHintModel::~StyleHintModel() = default;

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
    if (column == 1) {
        const auto hint = static_cast<QStyle::StyleHint>(row);
        QStyleOption option;
        const auto value = effectiveStyle()->styleHint(hint, &option, nullptr, nullptr);
        switch (role) {
            case Qt::DisplayRole:
                if (style_hint_table[row].type == StyleHintType::Bool)
                    return QVariant();
                return VariantHandler::displayString(styleHintToVariant(hint, value));
            case Qt::EditRole:
            {
                const auto type = style_hint_table[row].type;
                if (type <= StyleHintType::LastBasicType)
                    return styleHintToVariant(hint, value);
                const auto enumType = type - StyleHintType::FirstEnumType;
                if (type >= StyleHintType::FirstEnumType ) {
                    if (!style_hint_type_table[enumType].name)
                        return value; // fallback to int for older Qt without meta enums for these types
                    const auto me = EnumUtil::metaEnum(value, style_hint_type_table[enumType].name,
                                                              style_hint_type_table[enumType].metaObject);
                    return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(value, me));
                }
                return QVariant();
            }
            case Qt::DecorationRole:
                return VariantHandler::decoration(styleHintToVariant(hint, value));
            case Qt::CheckStateRole:
                if (style_hint_table[row].type == StyleHintType::Bool)
                    return value ? Qt::Checked : Qt::Unchecked;
                return QVariant();
        }
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
    const auto type = style_hint_table[hint].type;
    switch (type) {
        case StyleHintType::Bool:
            return QVariant::fromValue<bool>(value);
        case StyleHintType::Int:
            return value;
        case StyleHintType::Color:
            return QVariant::fromValue(QColor(value));
        case StyleHintType::Char:
            return QChar(value);
        case StyleHintType::FrameStyle:
            return QString(EnumUtil::enumToString(value & QFrame::Shadow_Mask, "QFrame::Shadow", &QFrame::staticMetaObject)
                 + " / " + EnumUtil::enumToString(value & QFrame::Shape_Mask, "QFrame::Shape", &QFrame::staticMetaObject));
        default: break;
    }
    const auto enumType = type - StyleHintType::FirstEnumType;
    if (type >= StyleHintType::FirstEnumType && style_hint_type_table[enumType].name) {
        return EnumUtil::enumToString(value, style_hint_type_table[enumType].name,
                                             style_hint_type_table[enumType].metaObject);
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
            effectiveStyle()->styleHint(hint, opt, nullptr, &data);
            delete opt;
            return data.variant;
        }
        case QStyle::SH_RubberBand_Mask:
        {
            QStyleOptionRubberBand opt;
            opt.shape = QRubberBand::Rectangle;
            opt.rect = QRect(0, 0, 100, 100);
            QStyleHintReturnMask data;
            effectiveStyle()->styleHint(hint, &opt, nullptr, &data);
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
            effectiveStyle()->styleHint(hint, &opt, nullptr, &data);
            return data.region;
        }
        default: break;
    }
    return QVariant();
}

Qt::ItemFlags StyleHintModel::flags(const QModelIndex &index) const
{
    const auto baseFlags = QAbstractTableModel::flags(index);
    if (!index.isValid() || index.column() != 1 || !isMainStyle() ||
        style_hint_table[index.row()].extraType != StyleHintExtraType::None) {
        return baseFlags;
    }

    if (style_hint_table[index.row()].type != StyleHintType::FrameStyle &&
        style_hint_table[index.row()].type != StyleHintType::Bool) {
        return baseFlags | Qt::ItemIsEditable;
    }

    if (style_hint_table[index.row()].type == StyleHintType::Bool) {
        return baseFlags | Qt::ItemIsUserCheckable;
    }

    return baseFlags;
}

bool StyleHintModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 1)
        return false;

    if (role != Qt::EditRole && role != Qt::CheckStateRole)
        return false;

    int i = value.toInt();
    if (value.type() == QVariant::Color)
        i = value.value<QColor>().rgba();
    else if (value.userType() == qMetaTypeId<EnumValue>())
        i = value.value<EnumValue>().value();
    else if (role == Qt::CheckStateRole)
        i = (i == Qt::Checked) ? 1 : 0;
    DynamicProxyStyle::instance()->setStyleHint(static_cast<QStyle::StyleHint>(index.row()), i);
    emit dataChanged(index, index);
    return true;
}
