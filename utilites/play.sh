#!/bin/bash

while :
  do
    files=($(ls ~/.config/PianoBG/midi/))
    for ((i=0; i<${#files[@]}; i++))
      do
        echo "$i) ${files[$i]%.*}"
      done
    read  -n 1 -p "Pick one: " i
    ~/binary/PianoBG ${files[$i]}
  done