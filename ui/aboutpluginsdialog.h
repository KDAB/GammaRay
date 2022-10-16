/*
  aboutpluginsdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ABOUTPLUGINSDIALOG_H
#define GAMMARAY_ABOUTPLUGINSDIALOG_H

#include <QDialog>

namespace GammaRay {
class AboutPluginsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutPluginsDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
};
}

#endif // GAMMARAY_ABOUTPLUGINSDIALOG_H
