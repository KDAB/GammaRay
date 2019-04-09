/*
  widgetinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORINTERFACE_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORINTERFACE_H

#include <QDataStream>
#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVector>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

namespace GammaRay {
class WidgetInspectorInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        GammaRay::WidgetInspectorInterface::Features features READ features WRITE setFeatures NOTIFY featuresChanged)
public:
    enum Feature {
        NoFeature = 0,
        InputRedirection = 1,
        AnalyzePainting = 2,
        SvgExport = 4,
        PdfExport = 8,
        UiExport = 16
    };
    Q_DECLARE_FLAGS(Features, Feature)

    explicit WidgetInspectorInterface(QObject *parent = nullptr);
    ~WidgetInspectorInterface() override;

    Features features() const;
    void setFeatures(Features features);

public slots:
    virtual void saveAsImage(const QString &fileName) = 0;
    virtual void saveAsSvg(const QString &fileName) = 0;
    virtual void saveAsPdf(const QString &fileName) = 0;
    virtual void saveAsUiFile(const QString &fileName) = 0;

    virtual void analyzePainting() = 0;

signals:
    void featuresChanged();

private:
    Features m_features;
};

class WidgetFrameData
{
public:
    QVector<QRect> tabFocusRects;
};

QDataStream &operator<<(QDataStream &out, const WidgetFrameData &data);
QDataStream &operator>>(QDataStream &in, WidgetFrameData &data);

}

Q_DECLARE_METATYPE(GammaRay::WidgetInspectorInterface::Features)
Q_DECLARE_METATYPE(GammaRay::WidgetFrameData)
Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::WidgetInspectorInterface::Features)
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::WidgetInspectorInterface, "com.kdab.GammaRay.WidgetInspector")
QT_END_NAMESPACE

#endif // GAMMARAY_WIDGETINSPECTORINTERFACE_H
