/*
  basequicktest.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#ifndef BASEQUICKTEST_H
#define BASEQUICKTEST_H

#include "baseprobetest.h"

#include <QQuickView>
#include <QSignalSpy>

#include <memory>

using namespace GammaRay;

class BaseQuickTest : public BaseProbeTest
{
    Q_OBJECT
public:
    explicit BaseQuickTest(QObject *parent = nullptr)
        : BaseProbeTest(parent)
    {
    }

private:
    std::unique_ptr<QQuickView> m_view;
    bool m_exposed = false;

protected slots:
    virtual void init()
    {
        createProbe();

        m_exposed = false;
        m_view.reset(new QQuickView);
        m_view->setResizeMode(QQuickView::SizeViewToRootObject);
        QTest::qWait(1); // event loop re-entry
    }

    virtual void cleanup()
    {
        m_exposed = false;
        m_view.reset();
        QTest::qWait(1);
    }

protected:
    virtual bool ignoreNonExposedView() const
    {
        return false;
    }

    bool isViewExposed() const
    {
        return m_exposed;
    }

    bool showSource(const QString &sourceFile)
    {
        QSignalSpy renderSpy(m_view.get(), SIGNAL(frameSwapped()));
        Q_ASSERT(renderSpy.isValid());

        m_view->setSource(QUrl(sourceFile));
        m_view->show();
        m_exposed = QTest::qWaitForWindowExposed(m_view.get());
        if (!m_exposed) {
            qWarning() << "Unable to expose window, probably running tests on a headless system - ignoring all following render failures.";
            if (!ignoreNonExposedView())
                return false;
        }

        // wait at least two frames so we have the final window size with all render loop/driver combinations...
        QTest::qWait(20);
        renderSpy.wait();
        renderSpy.clear();
        m_view->update();
        return (ignoreNonExposedView() && !m_exposed) ||
                (renderSpy.wait() || !renderSpy.isEmpty());
    }

    QQuickView *view() const {
        return m_view.get();
    }
};

#endif // BASEQUICKTEST_H
