/*
  aboutwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ABOUTWIDGET_H
#define GAMMARAY_ABOUTWIDGET_H

#include "gammaray_ui_export.h"

#include <QWidget>
#include <QPointer>

namespace GammaRay {
namespace Ui {
class AboutWidget;
}

class GAMMARAY_UI_EXPORT AboutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AboutWidget(QWidget *parent = nullptr);
    ~AboutWidget() override;

    void setLogo(const QString &iconFileName);
    void setThemeLogo(const QString &fileName);
    void setTitle(const QString &title);
    void setHeader(const QString &header);
    void setAuthors(const QString &authors);
    void setFooter(const QString &footer);

    void setText(const QString &text);

    void setBackgroundWindow(QWidget *window);

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QScopedPointer<Ui::AboutWidget> ui;
    QPointer<QWidget> m_backgroundWindow;
    QPixmap m_watermark;
};
}

#endif // GAMMARAY_ABOUTWIDGET_H
