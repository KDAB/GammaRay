#!/bin/bash

authorName=`git config user.name`
authorEmail=`git config user.email`
thisYear=`date +%Y`

find "$@" -name '*.h' -o -name '*.cpp' -o -name '*.qml' | grep -v /3rdparty/ | grep -v /qmldebugcontrol/ | grep -v /StackWalker | grep -v /modeltest | grep -v /processlist | grep -v /interactiveprocess | grep -v /preloadcheck_mips | grep -v /classesiconsindex_data | grep -v /build | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} Klar.*lvdalens Datakonsult AB" "$FILE" ; then continue; fi
    thisfile=`basename $FILE`
    cat <<EOF > "$FILE".tmp
/*
  $thisfile

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) $thisYear Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: $authorName <$authorEmail>

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
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done

find "$@" -name '*.qdoc' | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} Klar.*lvdalens Datakonsult AB" "$FILE" ; then continue; fi
    thisfile=`basename $FILE`
    cat <<EOF > "$FILE".tmp
/*
    $thisfile

    This file is part of the GammaRay documentation.

    Copyright (C) $thisYear Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: $authorName <$authorEmail>

    Licensees holding valid commercial KDAB GammaRay licenses may use this file in
    accordance with GammaRay Commercial License Agreement provided with the Software.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This work is also licensed under the Creative Commons Attribution-ShareAlike 4.0
    International License. See <https://creativecommons.org/licenses/by-sa/4.0/>.
*/

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done


#remove the following exit if you want to add a header to CMakeLists.txt files
exit

find "$@" -name 'CMakeLists.txt' | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} Klar.*lvdalens Datakonsult AB" "$FILE" ; then continue; fi
    cat <<EOF > "$FILE".tmp
#  This file is part of GammaRay, the Qt application inspection and
#  manipulation tool.
#
#  Copyright (C) $thisYear Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
#  Author: $authorName <$authorEmail>
#
#  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
#  accordance with GammaRay Commercial License Agreement provided with the Software.
#
#  Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done
