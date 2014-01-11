/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

/**
 * This class checks if it's possible to overwrite symbols
 * by setting the LD_PRELOAD environment variable
 */
class PreloadCheck
{
  public:
    PreloadCheck();

    /**
     * Test whether it is possible to overwrite @p symbol
     * via LD_PRELOAD
     *
     * On Linux the 'readelf' binary is called to find out whether
     * @p symbol is marked as relocatable
     *
     * @return True in case it's possible to overwrite @p symbol, otherwise false
     * @sa errorString()
     */
    bool test(const QString &symbol);

    QString errorString() const;

  protected:
    void setErrorString(const QString &err);

  private:
#ifdef __mips__
    /**
     * Additional method for testing whether the call to the function will go
     * through .got and lazy binding stub (MIPS specific)
     *
     * @see https://github.com/KDAB/GammaRay/issues/63
     */
    bool testMips(const QString &symbol, const QString &fileName);
#endif

    QString m_errorString;
};

#endif

#endif // GAMMARAY_PRELOADCHECK
