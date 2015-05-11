Introduction
============
GammaRay is a software introspection tool for Qt applications developed by KDAB.
Leveraging the QObject introspection mechanism it allows you to observe and
manipulate your application at runtime. This works both locally on your
workstation and remotely on an embedded target.

Augmenting your instruction-level debugger, GammaRay allows you to work on a
much higher level, with the same concepts as the frameworks you use. This is
especially useful for the more complex Qt frameworks such as model/view, state
machines or scene graphs.

Among other things GammaRay can:

 * Browse the QObject tree with live updates.

 * View, and to some extent, edit QObject static and dynamic properties.

 * View and call slots of a QObject.

 * View other QObject elements such as signals, enums and class infos.

 * List all QObject inbound and outbound signal/slot connections.

 * Provide a layout information overlay for QWidget applications.

 * Inspect all QPainter operations used to draw a specific widget.

 * Browse the QtQuick2 item tree and scenegraph.

 * Plot object lifetime and emitted signals.

 * View the content of any QAbstractItemModel. Very useful when
   debugging a proxy model chain for example.

 * Browse the QAbstractProxyModel hierarchy.

 * Browse the item tree of any QGraphicsView scene.

 * Show a live preview of QGraphicsView items, including showing their coordinate system,
   transformation origin, rotate/zoom/pan, etc.

 * Intercept translations and change them at runtime.

 * Inspect all building blocks of a QStyle.

 * Act as a complete java script debugger, attachable to any QScriptEngine
  (including the usually not accessible one used by QtQuick1 internally).

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

Contact
=======
* Feel free to visit us on IRC: Channel is #gammaray on Freenode (irc://irc.freenode.net/gammaray)
* Or send mail to our mailing list: https://mail.kdab.com/mailman/listinfo/gammaray-interest

Get Involved
=================
If you want to contribute, please check out: https://github.com/KDAB/GammaRay/wiki/Get-Involved

Contributing
============
KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement.
Contact support@kdab.com for more information.

License
=======
The GammaRay Software is (C) 2010-2015 Klarälvdalens Datakonsult AB (KDAB),
and is available under the terms of the GPL version 2 (or any later version,
at your option).  See LICENSE.GPL.txt for license details.

Commercial use is also permitted as described in ReadMe-commercial.txt.

About KDAB
==========
GammaRay is supported and maintained by Klarälvdalens Datakonsult AB (KDAB).

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. We continue to help develop parts of Qt and are one
of the major contributors to the Qt Project. We can give advanced or
standard trainings anywhere around the globe.

Please visit http://www.kdab.com to meet the people who write code like this.
