load "__common.plot-include"

set datafile separator comma

set multiplot layout 3, 1 scale 1.0, 1.1
set lmargin at screen 0.12


set xzeroaxis
#set xlabel "Время от старта (минуты)" # Под последним графиком напишем
set xtics 10 nomirror
set xrange[-10:120]
set mxtics

set yzeroaxis

set grid xtics ytics

set style line 1 lt rgb "#ff7f0e" lw 1 # orange
set style line 2 lt rgb "#1f77b4" lw 1 # blue
set style line 3 lt rgb "#008000" lw 1 # green
set style line 4 lt rgb "#ff0000" lw 1 # red
set style line 5 lt rgb "black" lw 3

set arrow from  78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'red' lw 2
set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'red' lw 2

SPACING = 5


# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
set ylabel "Угол"
set ytics 90 nomirror
set yrange [-200:200]
set mytics 5
set key on outside right bottom horizontal enhanced box
plot \
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_measured[2]')*180/pi) \
		every SPACING \
		with lines ls 2 \
		axis x1y1 \
		title "phi" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_measured[1]')*180/pi) \
		every SPACING \
		with lines ls 1 \
		axis x1y1 \
		title "theta" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_real[1]')*180/pi) \
		every SPACING \
		with lines ls 5 \
		axis x1y1 \
		title "Расчётное theta" \
	,\

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
set ylabel "Яркость"
set ytics 0.2 nomirror
set yrange [0:0.8]
set mytics 5
unset key
plot \
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_measured[0]')) \
		every SPACING \
		with lines ls 3 \
		axis x1y1 \
		notitle \
	,\

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
set ylabel "Ошибка (%)"
set ytics 0.2 nomirror
set yrange [0:1]
set mytics 5
set xlabel "Время от старта (минуты)" # Только под последним
unset key

unset arrow
set arrow from  78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_error')) \
		every SPACING \
		with lines ls 4 \
		axis x1y1 \
		notitle \

unset multiplot
