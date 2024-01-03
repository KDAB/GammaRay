/*
  selftestpage.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
