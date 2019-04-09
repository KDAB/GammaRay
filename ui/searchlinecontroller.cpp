/*
  searchlinecontroller.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "searchlinecontroller.h"

#include <QLineEdit>
#include <QRegExp>
#include <QTimer>
#include <QAbstractProxyModel>

using namespace GammaRay;

namespace {
static QAbstractItemModel *findEffectiveFilterModel(QAbstractItemModel *model) {
    Q_ASSERT(model);

    if (model->metaObject()->indexOfProperty("filterKeyColumn") != -1) {
        return model;
    }

    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(model);

    if (proxy) {
        return findEffectiveFilterModel(proxy->sourceModel());
    }

    return nullptr;
}
}

SearchLineController::SearchLineController(QLineEdit *lineEdit, QAbstractItemModel *proxyModel)
    : QObject(lineEdit)
    , m_lineEdit(lineEdit)
    , m_filterModel(findEffectiveFilterModel(proxyModel))
{
    Q_ASSERT(lineEdit);
    Q_ASSERT(m_filterModel);

    if (!m_filterModel) {
        QMetaObject::invokeMethod(this, "deleteLater", Qt::QueuedConnection);
        return;
    }

    m_filterModel->setProperty("filterKeyColumn", -1);
    m_filterModel->setProperty("filterCaseSensitivity", Qt::CaseInsensitive);
    activateSearch();

#if QT_VERSION >= 0x050200
    m_lineEdit->setClearButtonEnabled(true);
#endif
    if (m_lineEdit->placeholderText().isEmpty())
        m_lineEdit->setPlaceholderText(tr("Search"));

    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(300);
    connect(lineEdit, &QLineEdit::textChanged, timer, [timer]{ timer->start(); });
    connect(timer, &QTimer::timeout, this, &SearchLineController::activateSearch);
}

SearchLineController::~SearchLineController() = default;

void SearchLineController::activateSearch()
{
    if (m_filterModel) {
        m_filterModel->setProperty("filterRegExp",
                                   QRegExp(m_lineEdit->text(), Qt::CaseInsensitive, QRegExp::FixedString));
    }
}
