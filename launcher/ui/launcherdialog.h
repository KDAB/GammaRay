/*
  launcherdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#ifndef GAMMARAY_LAUNCHERDIALOG_H
#define GAMMARAY_LAUNCHERDIALOG_H

#include <QObject>
#include <QUrl>

#include "gammaray_launcher_dialog_export.h"

namespace GammaRay {

class GAMMARAY_LAUNCHER_DIALOG_EXPORT LauncherDialog : public QObject
{
public:
    enum class Mode {
        Connect,
        Attach,
    };

    class Result
    {
    public:
        operator bool() const { return m_valid; }

        Mode mode() const { return m_mode; }

        QUrl url() const { return m_url; }

        QString processExe() const { return m_procExe; }
        qint64 processPid() const { return m_procPid; }

    private:
        bool m_valid;
        Mode m_mode;
        QUrl m_url;
        QString m_procExe;
        qint64 m_procPid;
        friend class LauncherDialog;
    };

    Result exec(Mode mode);
};

}

#endif
