load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Соотношение сигнал шум (dbHz)" #font ",18"
set ytics 5 nomirror #font ",18"
set yrange [-10:50]

set key on inside left width -25 Left

set grid xtics ytics

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../SINS_NAVSVINFO-11-0.csv' \
		using 'mins_from_launch':'cno_0' \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/gps-a+SINS_NAVSVINFO-11-0.csv' \
		using 'mins_from_launch':'cno_0_smooth' \
		with lines ls 2 \
		axis x1y1 \
		title "Сигнал/шум для самого \"громкого\" спутника" \
	,\
