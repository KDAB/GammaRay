/*
  modelutils.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELUTILS_H
#define GAMMARAY_MODELUTILS_H

#include "gammaray_common_export.h"

#include <QModelIndex>

namespace GammaRay {
namespace ModelUtils {

typedef bool (*MatchAcceptor)(const QVariant &);

/**
 * Return a list of items that are accepted by an acceptor function
 * This class extends the functionality provided in QAbstractItemModel::match(...)
 *
 * If (accept(v)) for an item v in the model returns true, it will be appended
 * to the result list
 *
 * @param accept Function in the form 'bool f(const QVariant&)'
 *
 * @see QAbstractItemModel::match(...)
 */
GAMMARAY_COMMON_EXPORT QModelIndexList match(const QModelIndex &start, int role,
                                             MatchAcceptor accept, int hits = 1,
                                             Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchWrap));
}
}

Q_DECLARE_METATYPE(GammaRay::ModelUtils::MatchAcceptor)

#endif // GAMMARAY_MODELUTILS_H
