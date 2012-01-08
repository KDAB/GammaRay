/*
  qmljsdebugger_global.h

  This file is part of GammaRay, the Qt application inspection and
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

/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef GAMMARAY_QMLJSDEBUGGER_GLOBAL_H
#define GAMMARAY_QMLJSDEBUGGER_GLOBAL_H

#include <QtCore/qglobal.h>

# if defined(BUILD_QMLJSDEBUGGER_LIB)
#    define QMLJSDEBUGGER_EXPORT Q_DECL_EXPORT
#    define QMLJSDEBUGGER_EXTERN Q_DECL_IMPORT
# elif defined(BUILD_QMLJSDEBUGGER_STATIC_LIB)
#    define QMLJSDEBUGGER_EXPORT
#    define QMLJSDEBUGGER_EXTERN Q_DECL_IMPORT
# else
#    define QMLJSDEBUGGER_EXPORT
#    define QMLJSDEBUGGER_EXTERN Q_DECL_IMPORT
#endif

#endif // QMLJSDEBUGGER_GLOBAL_H
