# Gnuplot script file for plotting data in "Newfile.dat"
# This file is called Newfile.p
set	autoscale
unset	log
unset	label
set terminal wxt size 1200,600
set grid
set xtic auto
set ytic auto
set title "Acceleration Data"
set xlabel "Time\n(seconds)"
set ylabel "Acceleration\n(G Force)"
plot	"Newfile.dat" using 1:2 title 'Z' with lines,\
"" using 1:3 title 'X' with lines, "" using 1:4 title 'Y' with lines, \
"" using 1:5 title 'Total Gs' with lines