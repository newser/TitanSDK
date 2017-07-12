#!/bin/bash

# ./gen_hc name path
GHC_NAME=$1
GHC_PATH=$2

# generate path
if [ ! -d $GHC_PATH ]
then
  echo invalid path: $GHC_PATH
  return -1
fi

H_PATH=$(cd $GHC_PATH;pwd)
if [ ! -d $H_PATH ]
then
  echo invalid c header path: $H_PATH
  return -1
fi
  
C_PATH=$(echo $H_PATH | sed 's/export/source/')
if [ ! -d $C_PATH ]
then
  echo invalid c source path: $C_PATH
  return -1
fi
  
# generate file
LNAME=$(echo $1 | tr "[:upper:]" "[:lower:]")
UNAME=$(echo $1 | tr "[:lower:]" "[:upper:]")

cp tt_sample.h $H_PATH/tt_$LNAME.h.org
sed -e "s/tt_sample.h/tt_${LNAME}.h/" -e "s/__TT_SAMPLE__/__TT_${UNAME}__/" < $H_PATH/tt_$LNAME.h.org > $H_PATH/tt_$LNAME.h
rm $H_PATH/tt_$LNAME.h.org

cp tt_sample.c $C_PATH/tt_$LNAME.c

echo generated: $H_PATH/tt_$LNAME.h
echo generated: $C_PATH/tt_$LNAME.c
