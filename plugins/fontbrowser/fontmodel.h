/*
  fontmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
