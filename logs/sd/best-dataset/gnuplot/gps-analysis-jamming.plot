load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Индикатор подавления несущей GPS (нормированный)" #font ",18"
set ytics 0.1 nomirror #font ",18"
set yrange [0:1]
set mytics 5

unset key

set grid xtics ytics

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/gps-a+SINS_MONHW-11-0.csv' \
		using 'mins_from_launch':(column('jam_ind')/255) \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW-11-0.csv' \
		using 'mins_from_launch':(column('jam_ind_smooth')/255) \
		with lines ls 2 \
		axis x1y1 \
		title "Индикатор подавления несущей" \
	,\
