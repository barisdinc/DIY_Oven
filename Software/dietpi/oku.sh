#!/bin/bash

#declare -A positions
#positions[31]= "0"
#positions[16]= "A"
#positions[4] = "B"
#positions[6] = "B"
#positions[2] = "C"
#positions[18]= "C"
#positions[23]= "D"
#positions[3] = "E"
#positions[5] = "F"
#positions[17]= "G"
#positions[25]= "H"
#positions[29]= "H"
#positions[13]= "I"
#positions[11]= "J"



while [ 1 ]
 do 
  a=$(gpio read 1)
  b=$(gpio read 4)
  c=$(gpio read 5) 
  d=$(gpio read 2) 
  e=$(gpio read 3)

  num=$(($a+$b*2+$c*4+$d*8+$e*16))
  if [ $num == 31 ];then
     echo "0"
  fi
  if [ $num == 16 ];then
     echo "A"
  fi
  if [ $num == 4 ];then
     echo "B"
  fi
  if [ $num == 6 ];then
     echo "B"
  fi
  if [ $num == 18 ];then
     echo "C"
  fi
  if [ $num == 2 ];then
     echo "C"
  fi
  if [ $num == 23 ];then
     echo "D"
  fi
  if [ $num == 3 ];then
     echo "E"
  fi
  if [ $num == 5 ];then
     echo "F"
  fi
  if [ $num == 17 ];then
     echo "G"
  fi
  if [ $num == 25 ];then
     echo "H"
  fi
  if [ $num == 29 ];then
     echo "H"
  fi
  if [ $num == 13 ];then
     echo "I"
  fi
  if [ $num == 11 ];then
     echo "J"
  fi

  #echo $a $b $c $d $e "sum="$num 
  #echo $num
  #echo ${positions[$num]}
  sleep 0.05
 done


# 0 31
# A 16
# B 4 6
# C 2 18
# D 23
# E 3
# F 5
# G 17
# H 25 29
# I 13
# J 11







