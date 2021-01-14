/*
  fontmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_FONTBROWSER_FONTMODEL_H
#define GAMMARAY_FONTBROWSER_FONTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QColor>
#include <QVector>

namespace GammaRay {
class FontModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FontModel(QObject *parent);

    void updateFonts(const QVector<QFont> &fonts);
    QVector<QFont> currentFonts() const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void updateText(const QString &text);
    void toggleBoldFont(bool bold);
    void toggleItalicFont(bool italic);
    void toggleUnderlineFont(bool underline);
    void setPointSize(int size);
    void setColors(const QColor &foreground, const QColor &background);

private:
    void fontDataChanged();

    QVector<QFont> m_fonts;
    QString m_text;
    int m_size;
    bool m_bold;
    bool m_italic;
    bool m_underline;
    QColor m_foreground;
    QColor m_background;
};
}

#endif
