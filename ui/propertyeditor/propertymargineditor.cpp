/*
  propertymargineditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#include "propertymargineditor.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

using namespace GammaRay;

template<typename SpinBox, typename MarginsType>
class MarginEditorDialog : public QDialog
{
public:
    explicit MarginEditorDialog(MarginsType m, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        auto makeSpinBox = [](decltype(MarginsType().left()) v) {
            auto b = new SpinBox();
            b->setValue(v);
            return b;
        };
        left = makeSpinBox(m.left());
        right = makeSpinBox(m.right());
        bottom = makeSpinBox(m.bottom());
        top = makeSpinBox(m.top());

        setupLayout();
    }

    MarginsType margins() const
    {
        return MarginsType(left->value(), top->value(), right->value(), bottom->value());
    }

private:
    void setupLayout()
    {
        auto main = new QVBoxLayout(this);

        auto row1 = new QHBoxLayout();
        main->addLayout(row1);
        row1->addWidget(new QLabel(tr("left")));
        row1->addWidget(left);
        row1->addWidget(new QLabel(tr("right")));
        row1->addWidget(right);

        auto row2 = new QHBoxLayout();
        main->addLayout(row2);
        row2->addWidget(new QLabel(tr("top")));
        row2->addWidget(top);
        row2->addWidget(new QLabel(tr("bottom")));
        row2->addWidget(bottom);

        auto btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        main->addWidget(btns);
    }

    SpinBox *left;
    SpinBox *right;
    SpinBox *top;
    SpinBox *bottom;
};

PropertyMarginsEditor::PropertyMarginsEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyMarginsEditor::showEditor(QWidget *parent)
{
    MarginEditorDialog<QSpinBox, QMargins> dlg(value().value<QMargins>(), parent);
    if (dlg.exec() == QDialog::Accepted)
        save(QVariant::fromValue(dlg.margins()));
    emit editorClosed();
}

PropertyMarginsFEditor::PropertyMarginsFEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyMarginsFEditor::showEditor(QWidget *parent)
{
    MarginEditorDialog<QDoubleSpinBox, QMarginsF> dlg(value().value<QMarginsF>(), parent);
    if (dlg.exec() == QDialog::Accepted)
        save(QVariant::fromValue(dlg.margins()));
    emit editorClosed();
}
