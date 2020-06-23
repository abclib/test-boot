#!/usr/bin/env bash
# set -e

readonly PRO=abckey
readonly TAG=boot
readonly DIR=$PRO-$TAG
readonly IMG=$PRO:$TAG

os() {
  if [ "$(uname)" == "Darwin" ]; then
    return 0
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    return 1
    elif [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
    return 2
  fi
}

fileInfo() {
  sha256d=$(echo -n $(sha256sum -b $1 | head -c 64) | xxd -r -ps | sha256sum | head -c 64)
  echo ""
  echo "FileName: $1"
  echo "FileSize: $(wc -c <$1) bytes"
  echo "FileHash: $sha256d"
  echo ""
}

run() {
  docker build -t $IMG .
  os
  local _os=$?
  local cmd=$1
  if [ $_os == 0 ]; then
    return "TODO: 201908211038"
    elif [ $_os == 1 ]; then
    docker run -itv $(pwd):/$DIR:z $IMG $cmd
    elif [ $_os == 2 ]; then
    winpty docker stop $(docker ps -q)
    winpty docker rm $(docker ps -aq)
    winpty docker run -itv /$(pwd):/$DIR:z $IMG sh -c "$cmd"
  fi
}

main() {
  if [ ! $1 ]; then
    $1='build'
  fi
  if [ $1 == "debug" ]; then
    run "
      cd $DIR
      make DEBUG=1
    "
    fileInfo "$TAG.bin"
    elif [ $1 == "build" ]; then
    run "
      cd $DIR
      make
    "
    fileInfo "$TAG.bin"
    elif [ $1 == "rebuild" ]; then
    run "
      cd $DIR
      make clean
      make
    "
    fileInfo "$TAG.bin"
    elif [ $1 == "clean" ]; then
    run "
      cd $DIR
      make clean
    "
    elif [ $1 == "info" ]; then
    fileInfo "$TAG.bin"
    elif [ $1 == "test" ]; then
    run "
      cd $DIR
    "
  fi
}

main "$@"
