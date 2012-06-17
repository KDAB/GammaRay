/*
  launchpage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_LAUNCHPAGE_H
#define GAMMARAY_LAUNCHPAGE_H

#include <QWidget>

class QStringListModel;

namespace GammaRay {

namespace Ui {
  class LaunchPage;
}

class LaunchPage : public QWidget
{
  Q_OBJECT
  public:
    explicit LaunchPage(QWidget *parent = 0);
    ~LaunchPage();

    QStringList launchArguments() const;
    bool isValid();

    void writeSettings();

  signals:
    void updateButtonState();

  private slots:
    void showFileDialog();
    void addArgument();
    void removeArgument();
    void updateArgumentButtons();

  private:
    QStringList notEmptyString(const QStringList &list) const;
    Ui::LaunchPage *ui;
    QStringListModel *m_argsModel;
};

}

#endif // GAMMARAY_LAUNCHPAGE_H
