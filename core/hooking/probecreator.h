/*
  probecreator.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_PROBECREATOR_H
#define GAMMARAY_PROBECREATOR_H

#include <QObject>

namespace GammaRay {

/**
 * Creates Probe instance in main thread and deletes self afterwards.
 */
class ProbeCreator : public QObject
{
  Q_OBJECT
  public:
    enum Type {
      CreateOnly,
      CreateAndFindExisting
    };
    explicit ProbeCreator(Type t);

    static void trackObject(QObject* object);
    static void untrackObject(QObject* object);

  private slots:
    void createProbe();

  private:
    Type m_type;
};

}

#endif // GAMMARAY_PROBECREATOR_H
