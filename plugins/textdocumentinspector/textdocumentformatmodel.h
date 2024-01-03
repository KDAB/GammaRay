/*
  textdocumentformatmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTFORMATMODEL_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTFORMATMODEL_H

#include <QAbstractTableModel>
#include <QTextFormat>

namespace GammaRay {
class TextDocumentFormatModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TextDocumentFormatModel(QObject *parent = nullptr);

    void setFormat(const QTextFormat &format);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QTextFormat m_format;
};
}

#endif
