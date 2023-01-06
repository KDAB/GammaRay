/*
  styleinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORINTERFACE_H
#define GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORINTERFACE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QSize;
QT_END_NAMESPACE

namespace GammaRay {
class StyleInspectorInterface : public QObject
{
    Q_OBJECT
public:
    explicit StyleInspectorInterface(QObject *parent = nullptr);
    ~StyleInspectorInterface() override;

    int cellHeight() const;
    int cellWidth() const;
    int cellZoom() const;
    QSize cellSizeHint() const;

signals:
    void cellSizeChanged();

public slots:
    virtual void setCellHeight(int height);
    virtual void setCellWidth(int width);
    virtual void setCellZoom(int zoom);

private:
    int m_cellHeight;
    int m_cellWidth;
    int m_cellZoom;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::StyleInspectorInterface, "com.kdab.GammaRay.StyleInspectorInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_STYLEINSPECTORINTERFACE_H
