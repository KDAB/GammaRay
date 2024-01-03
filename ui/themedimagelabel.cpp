/*
  themedimagelabel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "themedimagelabel.h"
#include "uiresources.h"

#include <QEvent>

using namespace GammaRay;

ThemedImageLabel::ThemedImageLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
}

QString ThemedImageLabel::themeFileName() const
{
    return m_themeFileName;
}

void ThemedImageLabel::setThemeFileName(const QString &themeFileName)
{
    if (m_themeFileName == themeFileName)
        return;

    m_themeFileName = themeFileName;
    updatePixmap();
}

bool ThemedImageLabel::event(QEvent *e)
{
    if (e->type() == QEvent::ScreenChangeInternal)
        updatePixmap();
    return QLabel::event(e);
}

void ThemedImageLabel::updatePixmap()
{
    if (m_themeFileName.isEmpty())
        setPixmap(QPixmap());
    else
        setPixmap(UIResources::themedPixmap(m_themeFileName, this));
}
