#!/bin/sh
#
# Update the subset of KUserFeedback relevant for us from upstream
#

upstream=$1
if ! [ -d "$upstream" ]; then
    echo "Usage $0 <kuserfeedback checkout>"
    exit 1
fi

for i in `ls common`; do
    if [ -f $upstream/src/common/$i ]; then
        cp $upstream/src/common/$i common/$i
    fi
done

for i in `ls core`; do
    cp $upstream/src/provider/core/$i core/$i
done

for i in `ls widgets`; do
    cp $upstream/src/provider/widgets/$i widgets/$i
done

pushd common
flex --header-file=surveytargetexpressionscanner.h -o surveytargetexpressionlexer.cpp surveytargetexpressionlexer.l
bison --defines=surveytargetexpressionparser_p.h -d -o surveytargetexpressionparser_p.cpp surveytargetexpressionparser.y
popd
