/*
  modelpickerdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MODELPICKERDIALOG_H
#define GAMMARAY_MODELPICKERDIALOG_H

#include "gammaray_ui_export.h"

#include <QDialog>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QAbstractItemModel;
class QDialogButtonBox;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE

namespace GammaRay {

class DeferredTreeView;

/** @brief A simple dialog that allow to pick a model index. */
class GAMMARAY_UI_EXPORT ModelPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelPickerDialog(QWidget *parent = nullptr);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    void setRootIndex(const QModelIndex &index);
    void setCurrentIndex(const QModelIndex &index);
    void setCurrentIndex(int role, const QVariant &value);

public slots:
    void accept() override;

signals:
    void activated(const QModelIndex &index);
    void checkBoxStateChanged(const bool &checked);

private:
    DeferredTreeView *m_view;
    QDialogButtonBox *m_buttons;
    QLineEdit *m_searchBox;
    QCheckBox *m_showInvisibleItems;
    QPair<int, QVariant> m_pendingSelection;

private slots:
    void selectionChanged();
    void updatePendingSelection();
};

} // namespace GammaRay

#endif // GAMMARAY_MODELPICKERDIALOG_H
