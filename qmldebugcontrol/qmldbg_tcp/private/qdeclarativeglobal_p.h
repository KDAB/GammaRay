/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGLOBAL_H
#define QDECLARATIVEGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

#define DEFINE_BOOL_CONFIG_OPTION(name, var) \
    static bool name() \
    { \
        static enum { Yes, No, Unknown } status = Unknown; \
        if (status == Unknown) { \
            QByteArray v = qgetenv(#var); \
            bool value = !v.isEmpty() && v != "0" && v != "false"; \
            if (value) status = Yes; \
            else status = No; \
        } \
        return status == Yes; \
    }

#ifdef Q_OS_SYMBIAN
#define Q_DECLARATIVE_PRIVATE_EXPORT Q_AUTOTEST_EXPORT
#else
#define Q_DECLARATIVE_PRIVATE_EXPORT Q_DECLARATIVE_EXPORT
#endif

struct QDeclarativeGraphics_DerivedObject : public QObject
{
    void setParent_noEvent(QObject *parent) {
        bool sce = d_ptr->sendChildEvents;
        d_ptr->sendChildEvents = false;
        setParent(parent);
        d_ptr->sendChildEvents = sce;
    }
};

/*!
    Returns true if the case of \a fileName is equivalent to the file case of 
    \a fileName on disk, and false otherwise.

    This is used to ensure that the behavior of QML on a case-insensitive file 
    system is the same as on a case-sensitive file system.  This function 
    performs a "best effort" attempt to determine the real case of the file. 
    It may have false positives (say the case is correct when it isn't), but it
    should never have a false negative (say the case is incorrect when it is 
    correct).
*/
bool QDeclarative_isFileCaseCorrect(const QString &fileName);

/*!
    Makes the \a object a child of \a parent.  Note that when using this method,
    neither \a parent nor the object's previous parent (if it had one) will
    receive ChildRemoved or ChildAdded events.
*/
inline void QDeclarative_setParent_noEvent(QObject *object, QObject *parent)
{
    static_cast<QDeclarativeGraphics_DerivedObject *>(object)->setParent_noEvent(parent);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEGLOBAL_H
