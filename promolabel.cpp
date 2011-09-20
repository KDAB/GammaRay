/*
  mainwindow.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "promolabel.h"

#include <QDesktopServices>
#include <QUrl>

using namespace Endoscope;

PromoLabel::PromoLabel(QWidget *parent, Qt::WindowFlags f)
: QLabel(parent, f)
{
  setText("<html>"
            "<a href='http://www.kdab.com'>"
              "<img src=':endoscope/kdabproducts.png' />"
            "</a>"
          "</html>");
  setToolTip(tr("Visit KDAB Website"));
  connect(this, SIGNAL(linkActivated(QString)),
          this, SLOT(openWebsite(QString)));
}

void PromoLabel::openWebsite(const QString &link)
{
  QDesktopServices::openUrl(QUrl(link));
}

#include "promolabel.moc"
