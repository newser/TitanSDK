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

if [ ${OS} == "ios" ]
then
    :
elif [ ${OS} == "ios-simulator" ]
then
    export SIMCTL_CHILD_TT_CASE=${TT_CASE}
    xcrun simctl launch --console ${DEV} com.titansdk.unit-test > ${TT_CASE}.log 2>&1
    grep "|   result:  OK" ${TT_CASE}.log
    if [ $? -ne 0 ]
    then
        cat ${TT_CASE}.log
        exit -1
    fi
else
    echo testing ${TT_CASE}
    ${UT} > ${TT_CASE}.log 2>&1
    if [ $? -ne 0 ]
    then
        cat ${TT_CASE}.log
        exit -1
    fi
fi

exit 0
