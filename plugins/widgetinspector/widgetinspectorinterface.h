/*
  widgetinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    enum Feature
    {
        NoFeature = 0,
        InputRedirection = 1,
        AnalyzePainting = 2,
        SvgExport = 4,
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
