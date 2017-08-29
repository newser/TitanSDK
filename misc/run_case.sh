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

if [ "${OS}" = "ios" ]
then
    :
elif [ "${OS}" = "ios-simulator" ]
then
    export SIMCTL_CHILD_TT_CASE=${TT_CASE}
    xcrun simctl launch --console ${DEV} com.titansdk.unit-test > ${TT_CASE}.log 2>&1
    grep "|   result:  OK" ${TT_CASE}.log > /dev/null 2>&1
    if [ $? -ne 0 ]
    then
        cat ${TT_CASE}.log

        # running ios simulator in travis ci docker image seems not stable enough,
        # so we'll search the fail log to determine if the case really failed,
        # note this may miss some exceptional failures such as crash.
        grep "|   result:  Fail" ${TT_CASE}.log > /dev/null 2>&1
        if [ $? -eq 0 ]
        then
            exit -1
        fi

        # just give a warning and show log if success log is not found
        grep "|   result:  OK" ${TT_CASE}.log > /dev/null 2>&1
        if [ $? -ne 0 ]
        then
            echo WARNING!!! ${TT_CASE} may failed
            cat ${TT_CASE}.log
        fi
    fi
elif [ "${OS}" = "android-simulator" ]
then
    adb shell am force-stop com.titansdk.titansdkunittest
    adb shell logcat -c
    adb shell am start -a android.intent.action.MAIN -n com.titansdk.titansdkunittest/com.titansdk.titansdkunittest.MainActivity -e TT_CASE ${TT_CASE}
    
    i=0
    while [ ${i} -lt 10 ]
    do
        echo [${i}] waiting ${TT_CASE}
        sleep 1
        adb logcat -d -s platform > ${TT_CASE}.log
        diff ${TT_CASE}.log ${TT_CASE}.log.prev > /dev/null 2>&1 && i=$((i + 1))
        rm ${TT_CASE}.log.prev > /dev/null 2>&1
        mv ${TT_CASE}.log ${TT_CASE}.log.prev
    done
    mv ${TT_CASE}.log.prev ${TT_CASE}.log

    grep "|   result:  Fail" ${TT_CASE}.log > /dev/null 2>&1
    if [ $? -eq 0 ]
    then
        exit -1
    fi

    grep "|   result:  OK" ${TT_CASE}.log > /dev/null 2>&1
    if [ $? -ne 0 ]
    then
        echo WARNING!!! ${TT_CASE} may failed
        cat ${TT_CASE}.log
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
