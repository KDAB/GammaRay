/*
  propertytest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QCoreApplication>
#include <QTimer>
#include <QVariant>

class MyTestObject : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int staticChangingProperty READ staticChangingProperty RESET staticChangingPropertyReset NOTIFY staticChangingPropertyChanged)
public:
  explicit MyTestObject(QObject *parent = 0) : QObject(parent), m_count(0)
  {
    setObjectName("propertyTestObject");
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(5000);
  }

  int staticChangingProperty() { return m_count; }
  void staticChangingPropertyReset()
  {
    m_count = 0;
    emit staticChangingPropertyChanged();
  }

signals:
  void staticChangingPropertyChanged();

private slots:
  void timeout()
  {
    ++m_count;
    setProperty("dynamicChangingProperty", m_count);
    emit staticChangingPropertyChanged();
  }

private:
  int m_count;

};

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  MyTestObject obj;
  return app.exec();
}

#include "propertytest.moc"
