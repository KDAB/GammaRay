/*
  complexcontrolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "complexcontrolmodel.h"
#include "styleoption.h"
#include "styleinspectorinterface.h"
#include <core/util.h>

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

using namespace GammaRay;

struct complex_control_element_t
{
    const char *name;
    QStyleOption *(*styleOptionFactory)();
    QStyle::ComplexControl control;
    QStyle::SubControls subControls;
};

#define MAKE_CC2(control, factory)                                              \
    {                                                                           \
        #control, &StyleOption::factory, QStyle::control, QStyle::SubControls() \
    }
#define MAKE_CC3(control, factory, subControls)                       \
    {                                                                 \
        #control, &StyleOption::factory, QStyle::control, subControls \
    }

static const complex_control_element_t complexControlElements[] = {
    MAKE_CC3(CC_SpinBox, makeSpinBoxStyleOption,
             QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxFrame
                 | QStyle::SC_SpinBoxEditField),
    MAKE_CC3(CC_ComboBox, makeComboBoxStyleOption,
             QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxArrow | QStyle::SC_ComboBoxEditField
                 | QStyle::SC_ComboBoxListBoxPopup),
    MAKE_CC3(CC_ScrollBar, makeSliderStyleOption,
             QStyle::SC_ScrollBarAddLine | QStyle::SC_ScrollBarSubLine | QStyle::SC_ScrollBarAddPage | QStyle::SC_ScrollBarSubPage | QStyle::SC_ScrollBarFirst | QStyle::SC_ScrollBarLast | QStyle::SC_ScrollBarSlider
                 | QStyle::SC_ScrollBarGroove),
    MAKE_CC3(CC_Slider, makeSliderStyleOption,
             QStyle::SC_SliderGroove | QStyle::SC_SliderHandle | QStyle::SC_SliderTickmarks),
    MAKE_CC3(CC_ToolButton, makeToolButtonStyleOption,
             QStyle::SC_ToolButton | QStyle::SC_ToolButtonMenu),
    MAKE_CC3(CC_TitleBar, makeTitleBarStyleOption,
             QStyle::SC_TitleBarSysMenu | QStyle::SC_TitleBarMinButton | QStyle::SC_TitleBarMaxButton | QStyle::SC_TitleBarCloseButton | QStyle::SC_TitleBarLabel | QStyle::SC_TitleBarNormalButton | QStyle::SC_TitleBarShadeButton | QStyle::SC_TitleBarUnshadeButton
                 | QStyle::SC_TitleBarContextHelpButton),
    MAKE_CC3(CC_Dial, makeSliderStyleOption,
             QStyle::SC_DialHandle | QStyle::SC_DialGroove | QStyle::SC_DialTickmarks),
    MAKE_CC2(CC_GroupBox, makeStyleOptionComplex),
    // MAKE_CC2(CC_GroupBox, makeGroupBoxStyleOption), // TODO: oxygen crashes with that due to widget access
    MAKE_CC3(CC_MdiControls, makeStyleOptionComplex,
             QStyle::SC_MdiNormalButton | QStyle::SC_MdiMinButton | QStyle::SC_MdiCloseButton)
};

ComplexControlModel::ComplexControlModel(QObject *parent)
    : AbstractStyleElementStateTable(parent)
{
}

QVariant ComplexControlModel::doData(int row, int column, int role) const
{
    if (role == Qt::DecorationRole) {
        QPixmap pixmap(m_interface->cellSizeHint());
        QPainter painter(&pixmap);
        Util::drawTransparencyPattern(&painter, pixmap.rect());
        painter.scale(m_interface->cellZoom(), m_interface->cellZoom());

        QScopedPointer<QStyleOptionComplex> opt(
            qstyleoption_cast<QStyleOptionComplex *>(
                complexControlElements[row].styleOptionFactory()));
        Q_ASSERT(opt);
        fillStyleOption(opt.data(), column);
        m_style->drawComplexControl(complexControlElements[row].control, opt.data(), &painter);

        int colorIndex = 7;
        unsigned int nshifts = sizeof(unsigned int) * 8;
        for (unsigned int i = 0; i < nshifts; ++i) {
            QStyle::SubControl sc = static_cast<QStyle::SubControl>(1U << i);
            if (sc & complexControlElements[row].subControls) {
                QRectF scRect = m_style->subControlRect(complexControlElements[row].control, opt.data(), sc);
                scRect.adjust(0, 0, -1.0 / m_interface->cellZoom(), -1.0 / m_interface->cellZoom());
                if (scRect.isValid() && !scRect.isEmpty()) {
                    // HACK: add some real color mapping
                    painter.setPen(static_cast<Qt::GlobalColor>(colorIndex++));
                    painter.drawRect(scRect);
                }
            }
        }

        return pixmap;
    }

    return AbstractStyleElementStateTable::doData(row, column, role);
}

int ComplexControlModel::doRowCount() const
{
    return sizeof(complexControlElements) / sizeof(complexControlElements[0]);
}

QVariant ComplexControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return complexControlElements[section].name;
    return AbstractStyleElementStateTable::headerData(section, orientation, role);
}
