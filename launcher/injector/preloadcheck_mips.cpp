/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Dejan Latinovic <Dejan.Latinovic@rt-rk.com>

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
//krazy:excludeall=kdabcopyright,kdabcommercial

// The way to determine whether the call to function will go
// through .got and lazy binding stub is:
// - find the value of dynamic symbol index of the function with the command
//   "readelf --dyn-syms"
// - find the value of dynamic tag MIPS_GOTSYM with the command "readelf -d"
// - if (dyn_sym_index >= MIPS_GOTSYM) then the function has entry in the global
//   part of the .got, and the call will go through lazy binding stub and be
//   resolved by dynamic linker.
bool PreloadCheck::testMips(const QString &symbol, const QString &fileName)
{
  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);
  proc.start("readelf", QStringList() << "--dyn-syms" << "-d" << fileName, QIODevice::ReadOnly);

  if (!proc.waitForFinished()) {
    setErrorString(QObject::tr("Failed to run 'readelf' (binutils) binary: %1").
                   arg(QString(proc.errorString())));
    return false;
  }

  if (proc.exitCode() != 0) {
    setErrorString(QObject::tr("Cannot read shared object: %1").arg(QString(proc.readAll())));
    return false;
  }

  //Example line of dynamic symbol table on mips:
  //3851: 001e66f4     8 FUNC    GLOBAL DEFAULT   11 qt_startup_hook
  QRegExp rxSym("^(\\d+):\\s+(?:[^ ]+\\s+){6}([^ ]+)(?:.*)$");

  //Example line of dynamic tag on mips:
  //0x70000013 (MIPS_GOTSYM)                0xec3
  QRegExp rxGot("^0x[0-9a-fA-F]+\\s+\\((.+)\\)\\s+(0x[0-9a-fA-F]+)(?:.*)$");

  int dyn_sym_index = 0;
  int mips_gotsym = 0;
  bool foundDynSymbol = false;
  bool foundGotTag = false;

  while (proc.canReadLine()) {
    const QString line = proc.readLine().trimmed();
    QString currentMatch;
    const QString tag = "MIPS_GOTSYM";

    if (rxGot.exactMatch(line)) {
      currentMatch = rxGot.cap(1);

      if (currentMatch == tag) {
        bool conversionOk = false;
        int value = rxGot.cap(2).toInt(&conversionOk, 16);
        if (conversionOk){
          mips_gotsym = value;
          foundGotTag = true;
        }
      }
    }
    else if (rxSym.exactMatch(line)) {
      currentMatch = rxSym.cap(2);
      if (currentMatch == symbol) {
        dyn_sym_index = rxSym.cap(1).toInt();
        foundDynSymbol = true;
      }
    }
    if (foundGotTag && foundDynSymbol)
      break;
  }
  if (foundGotTag && foundDynSymbol && (dyn_sym_index >= mips_gotsym)) {
    return true;
  }

  return false;
}
