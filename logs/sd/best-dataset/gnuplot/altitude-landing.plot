load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 0.5 nomirror #font ",18"
set xrange[110:112]
set mxtics

set ylabel "Высота (м)" #font ",18"
set ytics 5 nomirror #font ",18"
set yrange [150:300]
unset mytics

set grid xtics ytics mxtics mytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 3
set style line 3 lt rgb "#14acba" lw 1

set arrow from 110.27, graph 0 to 110.27, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from graph 0,first 175 to graph 1,first 175 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

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
	'../GPS_UBX_NAV_SOL-11-0.csv' using 'mins_from_launch':'h' \
		with points ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../PLD_MS5611_DATA-13-2.csv' \
		using 'mins_from_launch':'altitude' \
		with lines ls 3 \
		axis x1y1 \
		title "Барометрическая высота" \
	,\
	'../PLD_MS5611_DATA-13-2.csv' \
		using 'mins_from_launch':'altitude' \
		with points ls 3 \
		axis x1y1 \
		notitle \

