#!/bin/bash

# $1 = executable avec nos threads
# $2 = executable avec pthread
# $3 = fichier .dat pour nos threads
# $4 = fichier .dat pour pthread
# $5 = fichier .png de sortie
# $6 = titre du graph
# $7 = nbr max de threads lancés ou arg
# $8 = fichier .dat avec préemption

rm -f $3 $4 $8

fibo=$(echo $6 | grep -c "fibonacci")

for i in `seq 1 $7`
do
  if [ $fibo = 0 ]
  then
    for j in `seq 1 5`
    do
    x[$j]=$(LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $1 $i $i | grep -oP '[0-9]+' | tr "\n" " " | cut -d" " -f2)
    y[$j]=$(LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $2 $i $i| grep -oP '[0-9]+' | tr "\n" " "| cut -d" " -f2)
    z[$j]=$(LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $1 $i $i -DPREEMPTION | grep -oP '[0-9]+' | tr "\n" " "| cut -d" " -f2)
    done
    xm=$(echo "(${x[1]}+${x[2]}+${x[3]}+${x[4]}+${x[5]})/5" | bc)
    ym=$(echo "(${y[1]}+${y[2]}+${y[3]}+${y[4]}+${y[5]})/5" | bc)
    zm=$(echo "(${z[1]}+${z[2]}+${z[3]}+${z[4]}+${z[5]})/5" | bc)
    echo "$i $xm" >> $3; echo "$i $ym" >> $4; echo "$i $zm" >> $8
  else
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $1 $i | cut -d" " -f'7 3' >> $3
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $1 $i -DPREEMPTION | cut -d" " -f'7 3' >> $8
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib taskset -c 0 $2 $i | cut -d" " -f'7 3' >> $4
  fi
done

echo "set terminal png size 800,500 enhanced background rgb 'white'" > gnuplot_script
echo "set output '$5'" >> gnuplot_script
echo "set title '$6'" >> gnuplot_script
if [ $fibo = 0 ]
then
  echo "set xlabel 'valeur en paramètre'" >> gnuplot_script
  echo "set ylabel 'temps en micro secondes'" >> gnuplot_script
  echo "set noautoscale" >> gnuplot_script
  echo "set xr [0:$7]" >> gnuplot_script
  b=($(sort -k2 -g $3 | sed -n '1p;$p' | cut -d' ' -f2))
  vmax=$((b[1]))
  echo "set yr [0:$vmax]" >> gnuplot_script
  echo "plot '$4' using 1:2:(0.1) smooth acsplines t 'with pthread', '$3' using 1:2:(0.1) smooth acsplines t 'our implementation', '$8' using 1:2:(0.1) smooth acsplines t 'with premption'" >> gnuplot_script
else
  echo "set xlabel 'entrée de fibonacci'" >> gnuplot_script
  echo "set ylabel 'temps en secondes'" >> gnuplot_script
  echo "set autoscale" >> gnuplot_script
  echo "set logscale y 10" >> gnuplot_script
  echo "plot '$4' with lines ls 1 t 'with pthread', '$3' with lines ls 2 t 'our implementation', '$8' with lines ls 3 t 'with premption' " >> gnuplot_script
fi

gnuplot gnuplot_script
rm gnuplot_script
