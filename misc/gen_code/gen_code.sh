#!/bin/bash

function usage
{
  printf "\nthis script is used for generating source code files\n"
  printf "\nusage: gen_code.sh mode name [path]\n"
  printf "\nmode:\n"
  printf "  hc    generating both .h file and .c file\n"
  printf "\nname: file name, generated file name would be tt_[name].h and tt_[name].c\n"
  printf "\npath: if specified, the path is considered as the directory for storing header file\n"
  printf "\n"
}

if [ $# -eq 2 ]
then
  GC_PATH=../../platform/include
elif [ $# -eq 3 ]
then
  GC_PATH=$3
else
  usage
  exit -1
fi

case $1 in
  hc)
    bash ./mode_hc.sh $2 $GC_PATH
	;;

  *)
    usage
    ;;
esac
