Introduction
============
GammaRay is a tool to poke around in a Qt-application and also to manipulate the
application to some extent. GammaRay uses various DLL injection techniques to
hook into an application at runtime and provide access to a lot of interesting
information.

GammaRay can:

 * Browse the QObject tree with live updates.

 * View, and to some extent, edit QObject static and dynamic properties.

 * View and call slots of a QObject (similar to qdbusviewer).

 * View other QObject elements such as signals, enums and class infos
   introspectively.

 * List all QObject inbound and outbound signal/slot connections.

 * Provide live widget preview. Useful for finding layout issues).

 * View the content of any QAbstractItemModel (QAIM). Very useful when
   debugging a proxy model chain for example.

 * Browse the QAbstractProxyModel (QAPM) hierarchy.

 * Browse the QGraphicsView (QGV) item tree of any QGV scene.

 * Show a live preview of QGV items, including showing their coordinate system,
   transformation origin, rotate/zoom/pan, etc.

 * Act as a complete java script debugger, attachable to any QScriptEngine
  (including the usually not accessible one used by QML internally).

 * Perform HTML/CSS/DOM/JS introspection/editing/profiling on any QWebPage,
   thanks to QWebInspector.

 * Browse the QResource tree and its content.

 * Browse QStateMachines, along with their states and transitions.

 * Show all registered meta types.

 * Show all installed fonts.

 * Show all available codecs.

 * Browse all QTextDocuments, along with the ability to edit them and view
   their internal structures.

 * Show all QTimers and their statistics (number of wakeups, wakeup time, ...)

Head Engineer for GammaRay is Volker Krause <volker.krause@kdab.com>

Contact
=======

* Feel free to visit us on IRC: Channel is #gammaray on Freenode (irc://irc.freenode.net/gammaray)
* Or send a mail to our mailing list: https://mail.kdab.com/mailman/listinfo/gammaray-interest

Get Involved
=================

If you want to contribute, please check out: https://github.com/KDAB/GammaRay/wiki/Get-Involved

License
=======
Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, <info@kdab.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

About KDAB
==========
GammaRay is supported and maintained by Klarälvdalens Datakonsult AB (KDAB)

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. Our software products increase Qt productivity and our
Qt trainers have trained 50% of commercial Qt developers globally.

Please visit http://www.kdab.com to meet the people who write code like this.
We also offer Qt training courses.
