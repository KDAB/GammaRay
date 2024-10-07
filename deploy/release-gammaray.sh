#!/bin/bash

#
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2024 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

PROJECT=gammaray
FORMAL_PROJECT=GammaRay
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
TOP=$(dirname "$SCRIPT_DIR")

#function HELP
# print a help message and exit
HELP() {
  echo
  echo "Usage: $(basename "$0") [-f] X.Y.Z"
  echo
  echo "Create the tars/zips and sign for project release X.Y.Z"
  echo " Options:"
  echo "  -f  Force everything to run, even if the tag for X.Y.Z has already been pushed."
  echo
  exit 0
}

#function SYNCECM: $1 is "KDAB" or "ECM"; $2 is the fullpath to the official version
SYNCECM() {
  set +e
  echo -n "Comparing $1 cmake modules to upstream: "
  savepwd=$(pwd)
  if (test ! -d "$TOP/cmake/$1/modules"); then
    echo "FAIL"
    echo " This project does not have the $1 CMake modules collected under cmake/$1/modules. Please deal with this first"
    exit 1
  fi
  cd "$TOP/cmake/$1/modules"
  whiteList="(ECMQt5ToQt6Porting.cmake)" #removed since kf6
  for m in *.cmake; do
    if [ -n "$whiteList" ]; then
      if [[ $m =~ $whiteList ]]; then
        echo "SKIPPING $m"
        continue
      fi
    fi
    if (test -f "$2/modules/$m"); then
      module="modules"
      diff "$m" "$2/modules/$m" 2>&1
      savestat=$?
    else
      if (test -f "$2/find-modules/$m"); then
        module="find-modules"
        diff "$m" "$2/find-modules/$m" 2>&1
        savestat=$?
      else
        echo "What is $m doing here?"
        exit 1
      fi
    fi
    if (test $savestat -ne 0); then
      echo "FAIL. Differences encountered in upstream $m"
      echo "  Upstream: $2/$module/$m"
      echo "  $PROJECT: cmake/$1/modules/$m"
      echo "Please sync the $PROJECT version before continuing (review changes first!)"
      exit 1
    fi
  done
  echo "OK"
  cd "$savepwd"
  set -e
}

options=$(getopt -o "hf" --long "help,force" -- "$@")
eval set -- "$options"
force=0
while true; do
  case "$1" in
  -h | --help)
    HELP
    ;;
  -f | --force)
    force=1
    shift
    ;;
  --)
    shift
    break
    ;;
  *)
    echo "Internal error!"
    exit 1
    ;;
  esac
done

if (test $# -ne 1); then
  HELP
fi

#compute X(major), Y(minor), Z(patchlevel)
if [[ ! $1 =~ ^[0-9]*\.[0-9]*\.[0-9]*$ ]]; then
  echo "\"$1\" is not a valid version string of the form X.Y.Z"
  exit 1
fi
X=$(echo "$1" | cut -d. -f1)
Y=$(echo "$1" | cut -d. -f2)
Z=$(echo "$1" | cut -d. -f3)

#set the branch and tag
branch=$X.$Y
tag=v$branch.$Z
release=$X.$Y.$Z

cd "$TOP" || exit 1
tbranch=$(sed -e 's,.*/,,' "$TOP/.git/HEAD")
if (test "$tbranch" != "$branch"); then
  echo "please git checkout $branch first"
  exit
fi

#Sanity Checking

### Auto-update OBS files
#TODO
### Auto-update homebrew files
#TODO
### Auto-update conan files
#TODO

# Update doxyfile
#CI uses 1.12.0 at this time
export PATH=/usr/local/opt/doxygen-1.12.0/bin:$PATH
echo -n "Ensuring Doxyfile.cmake is up-to-date: "
doxygen -u docs/api/Doxyfile.cmake >/dev/null 2>&1
set +e
diff docs/api/Doxyfile.cmake docs/api/Doxyfile.cmake.bak >/dev/null 2>&1
if (test $? -ne 0); then
  echo "Doxyfile.cmake has been updated by 'doxygen -u'. Please deal with this first"
  exit 1
else
  echo "OK"
  rm -f docs/api/Doxyfile.cmake.bak
fi
set -e

### KDAB cmake modules are synced
(cd "$HOME/projects/kdecm" && git checkout master)
SYNCECM "KDAB" "$HOME/projects/kdecm"
### KDE cmake modules are synced
(cd /data/KDE/src/frameworks/extra-cmake-modules && git checkout master)
SYNCECM "ECM" "/data/KDE/src/frameworks/extra-cmake-modules"

### pre-commit checking
echo "Pre-commit checking: "
pre-commit run --all-files
if (test $? -ne 0); then
  echo "There are pre-commit issues.  Please deal with this first"
  exit 1
else
  echo "OK"
fi

if (test "$(git tag -l | grep -c "$tag$")" -ne 1); then
  echo "please create the git tag $tag first:"
  echo "git tag -m \"$FORMAL_PROJECT $release\" $tag"
  exit
fi

if (test $force -eq 0 -a "$(git ls-remote --tags origin | grep -c "refs/tags/$tag$")" -gt 0); then
  echo "The tag for $tag has already been pushed."
  echo "Change the release number you provided on the command line."
  echo 'Or, if you simply want to re-create the tar and zips use the "-f" option.'
  exit
fi

# create the API documentation
rm -rf build-docs "$PROJECT-$release-doc.zip"
mkdir build-docs
cd build-docs || exit 1
#cmake -G Ninja --warn-uninitialized -Werror=dev -DGAMMARAY_BUILD_DOCS=True ..
cmake -G Ninja -DGAMMARAY_BUILD_DOCS=True ..
cmake --build .
cd docs/api/html || exit 1
7z a "$TOP/$PROJECT-$release-doc.zip" .
cd "$TOP" || exit 1
cd build-docs
cd docs/manual/gammaray-manual
7z a "$TOP/$PROJECT-$release-manual.zip" .
cd "$TOP" || exit 1
rm -rf build-docs

# create tar.gz version of the source
rm -rf "$PROJECT-$release"
mkdir -p "$PROJECT-$release/3rdparty"
# copy in the submodules by-hand as git archive won't deal with submodules
git submodule update --recursive --init
cp -r 3rdparty/KDStateMachineEditor "$PROJECT-$release/3rdparty"
#tar up everything else
git archive --format=tar --prefix="$PROJECT-$release/" "$tag" >"$PROJECT-$release.tar"
tar -rf "$PROJECT-$release.tar" "$PROJECT-$release"
gzip -f "$PROJECT-$release.tar"

# create the zip version of the tarball
tar xvfz "$PROJECT-$release.tar.gz"
7z a "$PROJECT-$release.zip" "$PROJECT-$release"
rm -rf "$PROJECT-$release"

# create the qt5 version of the source
tar xvzf "$PROJECT-$release.tar.gz"
mv "$PROJECT-$release" "qt5-$PROJECT-$release"
tar cvfz "qt5-$PROJECT-$release.tar.gz" "qt5-$PROJECT-$release"
rm -rf "qt5-$PROJECT-$release"

# create the qt6 version of the source
tar xvzf "$PROJECT-$release.tar.gz"
mv "$PROJECT-$release" "qt6-$PROJECT-$release"
tar cvfz "qt6-$PROJECT-$release.tar.gz" "qt6-$PROJECT-$release"
rm -rf "qt6-$PROJECT-$release"

# sign the tarballs
gpg --yes --local-user "KDAB Products" --armor --detach-sign "$PROJECT-$release.tar.gz"
gpg --yes --local-user "KDAB Products" --armor --detach-sign "$PROJECT-$release.zip"
gpg --yes --local-user "KDAB Products" --armor --detach-sign "qt5-$PROJECT-$release.tar.gz"
gpg --yes --local-user "KDAB Products" --armor --detach-sign "qt6-$PROJECT-$release.tar.gz"

# final cleaning
#anything to clean?

# sanity
files="\
$PROJECT-$release.tar.gz \
$PROJECT-$release.tar.gz.asc \
$PROJECT-$release.zip \
$PROJECT-$release.zip.asc \
$PROJECT-$release-doc.zip \
$PROJECT-$release-manual.zip \
qt5-$PROJECT-$release.tar.gz \
qt5-$PROJECT-$release.tar.gz.asc \
qt6-$PROJECT-$release.tar.gz \
qt6-$PROJECT-$release.tar.gz.asc \
"
for f in $files; do
  ls -l "$f"
done
