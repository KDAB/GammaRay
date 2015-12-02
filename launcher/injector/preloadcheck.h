/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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
     * @p symbol in library @p fileName is marked as relocatable.
     *
     * @return True in case it's possible to overwrite @p symbol, otherwise false
     * @sa errorString()
     */
    bool test(const QString &fileName, const QString &symbol);

    QString errorString() const;

  protected:
    void setErrorString(const QString &err);

  private:
    /**
     * Additional method for testing whether the call to the function will go
     * through .got and lazy binding stub (MIPS specific)
     *
     * @see https://github.com/KDAB/GammaRay/issues/63
     */
    bool testMips(const QString &symbol, const QString &fileName);

    QString m_errorString;
};

#endif

#endif // GAMMARAY_PRELOADCHECK
