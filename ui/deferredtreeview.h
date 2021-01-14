/*
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef DEFERREDTREEVIEW_H
#define DEFERREDTREEVIEW_H

#include "gammaray_ui_export.h"

#include <QTreeView>
#include <QHeaderView>
#include <QMap>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
/** @brief Base tree view which allow deferred configurations.
 *
 * If you starts using some deferred members - stick to them to avoid
 * synchronization states issues.
 */

class GAMMARAY_UI_EXPORT DeferredTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit DeferredTreeView(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model) override;

    // TODO: Move sections members into HeaderView so it can be reused by QTableView.
    QHeaderView::ResizeMode deferredResizeMode(int logicalIndex) const;
    void setDeferredResizeMode(int logicalIndex, QHeaderView::ResizeMode mode);

    bool deferredHidden(int logicalIndex) const;
    void setDeferredHidden(int logicalIndex, bool hidden);

    bool expandNewContent() const;
    void setExpandNewContent(bool expand);

    bool stretchLastSection() const;
    void setStretchLastSection(bool stretch);

signals:
    void newContentExpanded();

protected:
    void resetDeferredInitialized();

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;

private:
    struct DeferredHeaderProperties
    {
        DeferredHeaderProperties() = default;

        bool initialized = false;
        // When trying to only play with hidden, we guess the resizeMode using header->resizeMode().
        // Though hidden/unexisting columns always return resize mode Fixed, which is bad.
        // Also when trying to only play with resizeMode, we guess the hidden property, can be wrong too.
        // Let use int with -1 meaning don't apply the property.
        int resizeMode = -1;
        int hidden = -1;
    };

    typedef QMap<int, DeferredHeaderProperties> SectionsProperties;

    // This use logical indexes
    SectionsProperties m_sectionsProperties;
    bool m_expandNewContent;
    bool m_allExpanded;
    QVector<QPersistentModelIndex> m_insertedRows;
    QTimer *m_timer;

private slots:
    void sectionCountChanged();
    void triggerExpansion(const QModelIndex &parent);
    void timeout();
};
} // namespace GammaRay

#endif // DEFERREDTREEVIEW_H
