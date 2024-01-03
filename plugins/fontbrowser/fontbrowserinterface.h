/*
  fontbrowserinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERINTERFACE_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERINTERFACE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QColor;
QT_END_NAMESPACE

namespace GammaRay {
class FontBrowserInterface : public QObject
{
    Q_OBJECT
public:
    explicit FontBrowserInterface(QObject *parent);
    ~FontBrowserInterface() override;

    enum
    {
        FontRole = Qt::UserRole + 1,
        FontSearchRole = Qt::UserRole + 2,
        SortRole = Qt::UserRole + 3,
    };

public slots:
    virtual void updateText(const QString &text) = 0;
    virtual void toggleBoldFont(bool bold) = 0;
    virtual void toggleItalicFont(bool italic) = 0;
    virtual void toggleUnderlineFont(bool underline) = 0;
    virtual void setPointSize(int size) = 0;
    virtual void setColors(const QColor &foreground, const QColor &background) = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::FontBrowserInterface, "com.kdab.GammaRay.FontBrowser")
QT_END_NAMESPACE

#endif // FONTBROWSERINTERFACE_H
