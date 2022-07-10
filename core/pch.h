/*
  pch.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

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

#pragma once

#ifdef __cplusplus

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QAbstractProxyModel>
#include <QAbstractTableModel>
#include <QCoreApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QHash>
#include <QIcon>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QLoggingCategory>
#include <QMap>
#include <QMetaEnum>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaType>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QPair>
#include <QPersistentModelIndex>
#include <QPointer>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QModelIndexList>
#include <QMouseEvent>
#include <QPixmapCache>
#include <QPluginLoader>
#include <qplugin.h>
#include <QProcess>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QReadWriteLock>
#include <QScopeGuard>
#include <QScopedPointer>
#include <QSettings>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QUrl>
#include <QVector>
#include <QString>
#include <QSet>
#include <QTimer>
#include <QTranslator>
#include <QtCore/QtGlobal>
#include <QtMath>
#include <QVariant>
#include <QThread>
#include <qglobal.h>
#include <qnamespace.h>
#include <QList>

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

#endif
