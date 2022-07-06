load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)"
set xtics 0.05 nomirror
set xrange[78.32:78.5]
set mxtics 10

set x2zeroaxis
set x2label "Время от старта (секунды)"
set x2tics 1 nomirror
set x2range[78.32*60 : 78.5*60]
set mx2tics 10

set ylabel "Перегрузка (м/с**2)"
#set ytics 5 nomirror
#set yrange [150:300]
unset mytics

set grid xtics ytics

set style line 1 lt rgb "red" lw 2
set style line 2 lt rgb "green" lw 2
set style line 3 lt rgb "blue" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key right width -1

plot \
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[0]' every 1 \
		with lines ls 1 \
		axis x1y1 \
		title "По оси X" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[1]' every 1 \
		with lines ls 2 \
		axis x1y1 \
		title "По оси Y" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[2]' every 1 \
		with lines ls 3 \
		axis x1y1 \
		title "По оси Z" \
	,\
