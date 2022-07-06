load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)"
set xtics 10 nomirror
set xrange[-10:120]
set mxtics 10

set ylabel "Перегрузка (м/с**2)"
#set ytics 5 nomirror
#set yrange [150:300]
unset mytics

set y2zeroaxis
set y2label "Давление (Па)" #font ",18"
set y2tics 10000 nomirror #font ",18"
set y2range [0:120000]
set my2tics

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1
set style line 3 lt rgb "orange" lw 1
set style line 4 lt rgb "gray" lw 2

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -1

plot \
	'../PLD_MS5611_DATA-13-2.csv' \
		using 'mins_from_launch':'pressure' \
		with lines ls 4 \
		axis x1y2 \
		title "Давление" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[0]' every 10 \
		with lines ls 1 \
		axis x1y1 \
		title "По оси X" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[1]' every 10 \
		with lines ls 2 \
		axis x1y1 \
		title "По оси Y" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':'accel[2]' every 10 \
		with lines ls 3 \
		axis x1y1 \
		title "По оси Z" \
	,\
