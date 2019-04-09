/*
  selftestpage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SELFTESTPAGE_H
#define GAMMARAY_SELFTESTPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class SelfTestPage;
}

class SelfTestPage : public QWidget
{
    Q_OBJECT
public:
    explicit SelfTestPage(QWidget *parent = nullptr);
    ~SelfTestPage() override;

public slots:
    void run();

private slots:
    void error(const QString &msg);
    void information(const QString &msg);

private:
    Ui::SelfTestPage *ui;
    QStandardItemModel *m_resultModel;
};
}

#endif // GAMMARAY_SELFTESTPAGE_H
