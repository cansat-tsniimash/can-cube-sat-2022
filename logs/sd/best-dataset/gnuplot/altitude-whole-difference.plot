load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Разница высот (м)" #font ",18"
set ytics 100 nomirror #font ",18"
set yrange [-300:700]
set mytics

set y2zeroaxis
set y2label "Высота (м)" #font ",18"
set y2tics 10000 nomirror #font ",18"
set y2range [0:32000]
unset my2tics

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "gray" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../GPS_UBX_NAV_SOL-11-0.csv' \
		using 'mins_from_launch':'h' \
		with lines ls 2 \
		axis x1y2 \
		title "Высота ГНСС" \
	,\
	'../GPS_UBX_NAV_SOL-11-0.csv' \
		using 'mins_from_launch':(column('gpsFix') == 3 ? (column('h')-column('baro_altitude')) : 1/0) \
		with lines ls 1 \
		axis x1y1 \
		title "Разница высот ГНСС и барометрической" \
	,\

