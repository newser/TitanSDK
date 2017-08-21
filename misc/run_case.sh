#!/usr/bin/env sh

if [ -z ${OS} ]
then
    echo missing OS
    exit -1
fi

if [ -z ${TT_CASE} ]
then
    echo missing TT_CASE
    exit -1
fi

if [ -z ${UT} ]
then
    echo missing UT
    exit -1
fi

if [ ${OS} != "ios" ]
then
    echo testing ${TT_CASE}
    ${UT} > ${TT_CASE}.log 2>&1
    if [ $? -ne 0 ]
    then
        cat ${TT_CASE}.log
        exit -1
    fi
else
    echo testing ${TT_CASE}
fi

exit 0