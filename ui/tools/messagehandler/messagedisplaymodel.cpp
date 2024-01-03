/*
  messagedisplaymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        return MessageDisplayModel::tr("Unknown"); // never reached in theory
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
    case Qt::ToolTipRole: {
        const auto srcIdx = mapToSource(proxyIndex);
        Q_ASSERT(srcIdx.isValid());

        const auto msgType = typeToString(srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Type).toInt());
        const auto msgTime = srcIdx.sibling(srcIdx.row(), MessageModelColumn::Time).data().toString();
        const auto msgText = srcIdx.sibling(srcIdx.row(), MessageModelColumn::Message).data().toString();
        const auto backtrace = srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Backtrace).toStringList();
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
                      "</dl></qt>")
                .arg(msgType, msgTime, msgText, bt);
        } else {
            return tr("<qt><dl>"
                      "<dt><b>Type:</b></dt><dd>%1</dd>"
                      "<dt><b>Time:</b></dt><dd>%2</dd>"
                      "<dt><b>Message:</b></dt><dd>%3</dd>"
                      "</dl></qt>")
                .arg(msgType, msgTime, msgText);
        }
    }
    case Qt::DecorationRole:
        if (proxyIndex.column() == 0) {
            const auto srcIdx = mapToSource(proxyIndex);
            Q_ASSERT(srcIdx.isValid());

            const auto msgType = srcIdx.sibling(srcIdx.row(), 0).data(MessageModelRole::Type).toInt();
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
    case MessageModelRole::File: {
        const auto srcIdx = mapToSource(proxyIndex);
        Q_ASSERT(srcIdx.isValid());
        return srcIdx.sibling(srcIdx.row(), MessageModelColumn::File).data();
    }
    }

    return QAbstractProxyModel::data(proxyIndex, role);
}
