/*
  connectpage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CONNECTPAGE_H
#define GAMMARAY_CONNECTPAGE_H

#include <QWidget>

namespace GammaRay {

namespace Ui {
class ConnectPage;
}

/** UI for connecting to a running GammaRay instance. */
class ConnectPage : public QWidget
{
  Q_OBJECT
public:
  explicit ConnectPage(QWidget* parent = 0);
  ~ConnectPage();

  bool isValid() const;
  void writeSettings();

public slots:
  void launchClient();

signals:
  void updateButtonState();
  void activate();

private slots:
  void instanceSelected();

private:
  QScopedPointer<Ui::ConnectPage> ui;

};
}

#endif // GAMMARAY_CONNECTPAGE_H
