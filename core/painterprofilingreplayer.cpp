/*
  painterprofilingreplayer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <config-gammaray.h>
#include "painterprofilingreplayer.h"

#include <QElapsedTimer>

#include <algorithm>
#include <memory>

using namespace GammaRay;

namespace GammaRay {

class Replayer : public QPaintEngineExReplayer
{
public:
    explicit Replayer(const PaintBuffer *buffer, QPainter *p)
    {
        d = buffer->data();
        painter = p;
    }

    void process(const QPaintBufferCommand &cmd) override
    {
        if (painter->paintEngine()->isExtended())
            QPaintEngineExReplayer::process(cmd);
        else
            QPainterReplayer::process(cmd);
    }
};

}

PainterProfilingReplayer::PainterProfilingReplayer() = default;

PainterProfilingReplayer::~PainterProfilingReplayer() = default;

void PainterProfilingReplayer::profile(const PaintBuffer& buffer)
{
    const auto sourceSize = buffer.boundingRect().size().toSize();
    if (sourceSize.width() <= 0 || sourceSize.height() <= 0)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    const auto ratio = buffer.devicePixelRatioF();
#else
    const auto ratio = buffer.devicePixelRatio();
#endif
    QImage image(sourceSize * ratio, QImage::Format_ARGB32); // TODO use the right format, this has considerable impact on performance!
    image.setDevicePixelRatio(ratio);
    image.fill(Qt::transparent);
    QPainter p(&image);

    Replayer replayer(&buffer, &p);
    auto d = buffer.data();
    const auto cmdSize = d->commands.size();
    const auto runs = 5;
    std::unique_ptr<double[]> samples(new double[cmdSize * runs]);
    for (int run = 0; run < runs; ++run) {
        for (int i = 0; i < cmdSize; ++i) {
            const auto &cmd = d->commands.at(i);
            QElapsedTimer t;
            t.start();
            replayer.process(cmd);
            samples[i * runs + run] = t.nsecsElapsed();
        }
    }

    m_costs.reserve(cmdSize);
    for (int i = 0; i < cmdSize; ++i) {
        std::nth_element(samples.get() + i * runs, samples.get() + i * runs + runs/2, samples.get() + i * runs + runs);
        m_costs.push_back(samples[i * runs + runs/2]);
    }
    const auto sum = std::accumulate(m_costs.constBegin(), m_costs.constEnd(), 0.0);
    std::for_each(m_costs.begin(), m_costs.end(), [sum](double &c) { c = 100.0 * c / sum; });
}

QVector<double> PainterProfilingReplayer::costs() const
{
    return m_costs;
}
