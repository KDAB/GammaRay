/*
  themedimagelabel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_THEMEDIMAGELABEL_H
#define GAMMARAY_THEMEDIMAGELABEL_H

#include "gammaray_ui_export.h"

#include <QLabel>

namespace GammaRay {
class GAMMARAY_UI_EXPORT ThemedImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ThemedImageLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    QString themeFileName() const;
    void setThemeFileName(const QString &themeFileName);

protected:
    bool event(QEvent *e) override;
    virtual void updatePixmap();

private:
    QString m_themeFileName;
};
}

#endif // GAMMARAY_THEMEDIMAGELABEL_H
