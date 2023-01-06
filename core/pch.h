/*
  pch.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2022-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
