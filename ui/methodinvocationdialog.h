/*
  methodinvocationdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METHODINVOCATIONDIALOG_H
#define GAMMARAY_METHODINVOCATIONDIALOG_H

#include <QDialog>

#include <ui/uistatemanager.h>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class MethodInvocationDialog;
}

class MethodInvocationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MethodInvocationDialog(QWidget *parent = nullptr);
    ~MethodInvocationDialog() override;

    Qt::ConnectionType connectionType() const;
    void setArgumentModel(QAbstractItemModel *model);

private:
    QScopedPointer<Ui::MethodInvocationDialog> ui;
    UIStateManager m_stateManager;
};
}

#endif // GAMMARAY_METHODINVOCATIONDIALOG_H
