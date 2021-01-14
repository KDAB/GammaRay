/*
  modelpickerdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelpickerdialog.h"
#include "deferredtreeview.h"
#include "searchlinecontroller.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>

using namespace GammaRay;

static QPair<int, QVariant> qNullSelection()
{
    return qMakePair(-1, QVariant());
}

ModelPickerDialog::ModelPickerDialog(QWidget *parent)
    : QDialog(parent)
    , m_view(new DeferredTreeView(this))
    , m_buttons(new QDialogButtonBox(this))
    , m_searchBox (new QLineEdit(this))
    , m_showInvisibleItems(new QCheckBox(tr("Hide invisible items"),this))
    , m_pendingSelection(qNullSelection())
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_view->setUniformRowHeights(true);
    m_view->setExpandNewContent(true);
    m_buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_showInvisibleItems->setChecked(true);

    auto *vl = new QVBoxLayout(this);
    auto *hl = new QHBoxLayout;
    hl->addWidget(m_searchBox);
    hl->addWidget(m_showInvisibleItems);
    vl->addLayout(hl);
    vl->addWidget(m_view);
    vl->addWidget(m_buttons);

    selectionChanged();
    resize(640, 480);

    connect(m_view, &DeferredTreeView::newContentExpanded, this, &ModelPickerDialog::updatePendingSelection);
    connect(m_view, &DeferredTreeView::activated, this, &ModelPickerDialog::accept);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &ModelPickerDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_showInvisibleItems, &QAbstractButton::toggled, this, &ModelPickerDialog::checkBoxStateChanged);
}

QAbstractItemModel *ModelPickerDialog::model() const
{
    return m_view->model();
}

void ModelPickerDialog::setModel(QAbstractItemModel *model)
{
    m_view->setModel(model);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ModelPickerDialog::selectionChanged);
    new SearchLineController(m_searchBox, model);

    for (int i = 0; i < m_view->model()->columnCount(); ++i) {
        m_view->setDeferredResizeMode(i, QHeaderView::ResizeToContents);
    }
}

void ModelPickerDialog::setRootIndex(const QModelIndex &index)
{
    m_view->setRootIndex(index);
}

void ModelPickerDialog::setCurrentIndex(const QModelIndex &index)
{
    m_pendingSelection = qNullSelection();
    m_view->setCurrentIndex(index);
    m_view->scrollTo(index);
}

void ModelPickerDialog::setCurrentIndex(int role, const QVariant &value)
{
    QAbstractItemModel *model = m_view->model();
    const QModelIndex index = model->match(model->index(0, 0), role, value, 1, Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap).value(0);

    if (index.isValid()) {
        setCurrentIndex(index);
    }
    else {
        m_pendingSelection = qMakePair(role, value);
    }
}

void ModelPickerDialog::selectionChanged()
{
    const QModelIndex index = m_view->selectionModel() ? m_view->selectionModel()->selectedRows().value(0) : QModelIndex();
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(index.isValid());
}

void ModelPickerDialog::accept()
{
    const QModelIndex index = m_view->selectionModel()->selectedRows().value(0);
    if (index.isValid()) {
        emit activated(index);
        QDialog::accept();
    }
}

void ModelPickerDialog::updatePendingSelection()
{
    if (m_pendingSelection != qNullSelection())
        setCurrentIndex(m_pendingSelection.first, m_pendingSelection.second);
}

