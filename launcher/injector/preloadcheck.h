/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PRELOADCHECK
#define GAMMARAY_PRELOADCHECK

#if !defined Q_OS_WIN && !defined Q_OS_MAC

#include <QString>

class PreloadCheck
{
  public:
    PreloadCheck();

    bool test(const QString &symbol);

    QString errorString() const
    {
      return m_errorString;
    }

  protected:
    void setErrorString(const QString &err);

  private:
    static QString findSharedObjectFile(const QString &symbol);

    QString m_errorString;
};

#endif // Q_OS_UNIX

#endif // GAMMARAY_PRELOADCHECK
