#!/bin/bash

FPATH=
ONLY_MODIFIED=0

function usage
{
  printf "\nthis script is used for formatting source code in desired coding style\n"
  printf "\nusage: code_format.sh [options]\n"
  printf "\noptions:\n"
  printf "  -d <path>     format all supported source code under <path> recursively\n"
  printf "  -h            help\n"
  printf "  -m            only format modified files\n"
  printf "\n"
}

# parse options
while getopts "d:hm" arg
do
  case $arg in 
    d)
      FPATH=$OPTARG
      ;;
    m)
      ONLY_MODIFIED=1
      ;;
    h)
      usage
      exit 0
      ;;
    *)
      usage
      exit -1
      ;;
  esac
done

# check options
FPATH=${FPATH%/}

if [ -z $FPATH ]
then
  # if no fpath specified, use root path of the project
  FPATH=../..
fi

if [ ! -d $FPATH ]
then
  echo no such directory: $FPATH
  exit -1
fi

# format c code
printf "\n========================================\n" 
printf "formatting c\n\n"
bash ./c_format.sh $FPATH $ONLY_MODIFIED
printf "\nformatting c done\n"
printf "========================================\n\n"
