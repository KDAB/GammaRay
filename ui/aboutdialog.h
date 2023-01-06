/*
  aboutdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ABOUTDIALOG_H
#define GAMMARAY_ABOUTDIALOG_H

#include <QDialog>

namespace GammaRay {
class AboutWidget;

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog() override;

    void setLogo(const QString &iconFileName);
    void setThemeLogo(const QString &fileName);
    void setTitle(const QString &title);
    void setHeader(const QString &header);
    void setAuthors(const QString &authors);
    void setFooter(const QString &footer);

    void setText(const QString &text);

    QSize sizeHint() const override;

private:
    AboutWidget *ui;
};
}

#endif // GAMMARAY_ABOUTDIALOG_H
