#!/bin/bash

if [ $# != 3 ];
then
  echo 'replace.sh <path> <from> <to>'
  exit -1;
fi

grep $2 -rl $1 | xargs sed -i "" -e "s!$2!$3!g"
