/*
  messagedisplaymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "messagedisplaymodel.h"

#include <common/tools/messagehandler/messagemodelroles.h>

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

QString typeToString(int type)
{
    switch (type) {
    case QtDebugMsg:
        return MessageDisplayModel::tr("Debug");
    case QtWarningMsg:
        return MessageDisplayModel::tr("Warning");
    case QtCriticalMsg:
        return MessageDisplayModel::tr("Critical");
    case QtFatalMsg:
        return MessageDisplayModel::tr("Fatal");
    case QtInfoMsg:
        return MessageDisplayModel::tr("Info");
    default:
        return MessageDisplayModel::tr("Unknown");          // never reached in theory
    }
}

MessageDisplayModel::MessageDisplayModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

MessageDisplayModel::~MessageDisplayModel() = default;

QVariant MessageDisplayModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (proxyIndex.column() == MessageModelColumn::File) {
            const auto srcIdx = mapToSource(proxyIndex);
            Q_ASSERT(srcIdx.isValid());

            const auto fileName = srcIdx.data().toString();
            const auto line = srcIdx.data(MessageModelRole::Line).toInt();
            if (line <= 0)
                return fileName;
            return static_cast<QString>(fileName + ':' + QString::number(line));
        }
        break;
    case Qt::ToolTipRole:
    {
        const auto srcIdx = mapToSource(proxyIndex);
        Q_ASSERT(srcIdx.isValid());

        const auto msgType
            = typeToString(srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Type).toInt());
        const auto msgTime
            = srcIdx.sibling(srcIdx.row(), MessageModelColumn::Time).data().toString();
        const auto msgText
            = srcIdx.sibling(srcIdx.row(), MessageModelColumn::Message).data().toString();
        const auto backtrace
            = srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Backtrace).toStringList();
        if (!backtrace.isEmpty()) {
            QString bt;
            int i = 0;
            for (const auto &frame : backtrace) {
                bt += QStringLiteral("#%1: %2\n").arg(i, 2).arg(frame.trimmed());
                ++i;
            }
            return tr("<qt><dl>"
                      "<dt><b>Type:</b></dt><dd>%1</dd>"
                      "<dt><b>Time:</b></dt><dd>%2</dd>"
                      "<dt><b>Message:</b></dt><dd>%3</dd>"
                      "<dt><b>Backtrace:</b></dt><dd><pre>%4</pre></dd>"
                      "</dl></qt>").arg(msgType, msgTime, msgText, bt);
        } else {
            return tr("<qt><dl>"
                      "<dt><b>Type:</b></dt><dd>%1</dd>"
                      "<dt><b>Time:</b></dt><dd>%2</dd>"
                      "<dt><b>Message:</b></dt><dd>%3</dd>"
                      "</dl></qt>").arg(msgType, msgTime, msgText);
        }
    }
    case Qt::DecorationRole:
        if (proxyIndex.column() == 0) {
            const auto srcIdx = mapToSource(proxyIndex);
            Q_ASSERT(srcIdx.isValid());

            const auto msgType
                = srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Type).toInt();
            auto style = qApp->style();
            switch (msgType) {
            case QtDebugMsg:
                return style->standardIcon(QStyle::SP_MessageBoxInformation);
            case QtWarningMsg:
                return style->standardIcon(QStyle::SP_MessageBoxWarning);
            case QtCriticalMsg:
            case QtFatalMsg:
                return style->standardIcon(QStyle::SP_MessageBoxCritical);
            }
        }
        break;
    case MessageModelRole::File:
    {
        const auto srcIdx = mapToSource(proxyIndex);
        Q_ASSERT(srcIdx.isValid());
        return srcIdx.sibling(srcIdx.row(), MessageModelColumn::File).data();
    }
    }

    return QAbstractProxyModel::data(proxyIndex, role);
}
