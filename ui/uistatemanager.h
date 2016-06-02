/*
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 * Author: Filipe Azevedo <filipe.azevedo@kdab.com>
 *
 * Licensees holding valid commercial KDAB GammaRay licenses may use this file in
 * accordance with GammaRay Commercial License Agreement provided with the Software.
 *
 * Contact info@kdab.com if any conditions of this licensing are not clear to you.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef UISTATEMANAGER_H
#define UISTATEMANAGER_H

#include "gammaray_ui_export.h"

#include <QObject>
#include <QPointer>
#include <QList>
#include <QVector>
#include <QHash>

QT_BEGIN_NAMESPACE
class QWidget;
class QSplitter;
class QSettings;
class QHeaderView;
QT_END_NAMESPACE

namespace GammaRay {

/** @brief Interface for UI plugins to manage their states.
 *
 * The manager is able to track if changes appear and only persists them in this case.
 * That mean any not moved splitter handle will not be persisted, and % based sizes
 * will works correctly even on resize.
 * Any QMainwindow is always restored/saved.
 */

typedef QVector<QVariant> UISizeVector;

class GAMMARAY_UI_EXPORT UIStateManager : public QObject
{
  Q_OBJECT

public:
  explicit UIStateManager(QWidget *widget);
  virtual ~UIStateManager();

  virtual QList<QSplitter *> splitters() const;
  virtual QList<QHeaderView *> headers() const;
  virtual void setup();

  UISizeVector defaultSizes(QSplitter *splitter) const;
  void setDefaultSizes(QSplitter *splitter, const UISizeVector &defaultSizes);

  UISizeVector defaultSizes(QHeaderView *header) const;
  void setDefaultSizes(QHeaderView *header, const UISizeVector &defaultSizes);

public slots:
  void reset();
  virtual void restoreState();
  virtual void saveState();

protected:
  bool eventFilter(QObject *object, QEvent *event);

  QString widgetName(QWidget *widget) const;
  QString widgetPath(QWidget *widget) const;
  QString widgetGeometryKey(QWidget *widget) const;
  QString widgetStateKey(QWidget *widget) const;
  QString widgetStateSectionsKey(QWidget *widget) const;
  bool checkWidget(QWidget *widget) const;
  int percentToInt(const QString &size) const;

protected slots:
  void restoreWindowState();
  void saveWindowState();
  void restoreSplitterState(QSplitter *splitter = 0);
  void saveSplitterState(QSplitter *splitter = 0);
  void restoreHeaderState(QHeaderView *header = 0);
  void saveHeaderState(QHeaderView *header = 0);

private:
  QPointer<QWidget> m_widget;
  QSettings *m_stateSettings;
  bool m_initialized;
  bool m_resizing;
  // Int -> pixels, String -> suffixe dependant (% -> percent else pixels)
  // The UISizeVector is logical index based.
  QHash<QString, UISizeVector> m_defaultSplitterSizes;
  QHash<QString, UISizeVector> m_defaultHeaderSizes;

private slots:
  void headerSectionCountChanged();
  void widgetResized(QWidget *widget);
  void widgetCustomized();
};

} // namespace GammaRay

#endif // UISTATEMANAGER_H
