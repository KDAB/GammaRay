/*
  wk2application.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0
import QtWebKit 3.0
import QtWebKit.experimental 1.0

WebView {
  width: 800
  height: 600
  url: "https://www.kdab.com/gammaray"
}
