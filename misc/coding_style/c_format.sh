#!/bin/bash

REQUIRED_CF_VER=3.9.

# check format path
FPATH=${1%/}
if [ -z "$FPATH" ]
then
  echo no directory specified
  exit -1
fi

if [ ! -d "$FPATH" ]
then
  echo no such directory: $FPATH
  exit -1
fi

# whether only format modified files
if [ $2 == 0 ]
then
  FLIST=$(find $FPATH -not -path "*library/*" -type f \( -name "*.h" -o -name "*.c" -o -name "*.cpp" \))
  FLIST=$FLIST" "$(find $FPATH -type f \( -name "*wrapper.c" -o -name "*wrapper.h" \))
else
  FLIST=$(git status -s $FPATH | awk '/\.h$/{if ($1 != "D") print $NF}')
  FLIST=$FLIST" "$(git status -s $FPATH | awk '/\.c$/{if ($1 != "D") print $NF}')
  FLIST=$FLIST" "$(git status -s $FPATH | awk '/\.cpp$/{if ($1 != "D") print $NF}')
fi

# check clang-format
clang-format -h &> /dev/null
if [ $? != 0 ]
then
  echo clang-format is not installed
  exit -1
fi

CF_VER=$(clang-format -version | awk '{print $3}')
printf "clang format version: $CF_VER\n"
if [[ "$CF_VER" != "$REQUIRED_CF_VER"* ]]
then
  printf "WARNING! required clang-format version: $REQUIRED_CF_VER\n"
#  exit -1
fi
printf "\n"

function do_format
{
  echo Formatting [$(basename $(dirname $1))] $(basename $1)
  clang-format -assume-filename=./_clang_format -style=file -i $1
}

for f in $FLIST
do
  do_format $f
done
