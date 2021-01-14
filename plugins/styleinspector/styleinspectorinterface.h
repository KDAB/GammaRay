/*
  styleinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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
