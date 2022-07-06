load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)"
set xtics 10 nomirror
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Высота (м)"
set ytics 2000 nomirror
set yrange [0:32000]
set mytics

set y2zeroaxis
set y2label "Температура (°C)"
set y2tics 10 nomirror
set y2range [-40:50]

set key width -22 left top

set grid xtics ytics

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 3
set style line 3 lt rgb "gray" lw 2

plot \
	'../GPS_UBX_NAV_SOL-11-0.csv' using 'mins_from_launch':'h' \
		with lines ls 1 \
		axis x1y1 \
		title "Высота ГНСС" \
	,\
	'../GPS_UBX_NAV_SOL-11-0.csv' \
		using 'mins_from_launch':(column('gpsFix') == 3 ? column('h') : 1/0) \
		with lines ls 2 \
		axis x1y1 \
		title "Достоверная высота ГНСС (3d-fix)" \
	,\
	'../THERMAL_STATE-10-5.csv' \
		using 'mins_from_launch':'temperature' \
		with lines ls 3 \
		axis x1y2 \
		title "Температура БСК-5" \
	,\
