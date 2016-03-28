/*
  sourcelocation.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sourcelocation.h"

using namespace GammaRay;

SourceLocation::SourceLocation() :
    m_line(-1),
    m_column(-1)
{
}

SourceLocation::~SourceLocation()
{
}

bool SourceLocation::isValid() const
{
    return !m_fileName.isEmpty();
}

QString SourceLocation::fileName() const
{
    return m_fileName;
}

void SourceLocation::setFileName(const QString& fileName)
{
    m_fileName = fileName;
}

int SourceLocation::line() const
{
    return m_line;
}

void SourceLocation::setLine(int line)
{
    m_line = line;
}

int SourceLocation::column() const
{
    return m_column;
}

void SourceLocation::setColumn(int column)
{
    m_column = column;
}

QDataStream& GammaRay::operator<<(QDataStream& out, const SourceLocation& location)
{
    out << location.fileName();
    out << location.line();
    out << location.column();
    return out;
}

QDataStream& GammaRay::operator>>(QDataStream& in, SourceLocation& location)
{
    in >> location.m_fileName;
    in >> location.m_line;
    in >> location.m_column;
    return in;
}
