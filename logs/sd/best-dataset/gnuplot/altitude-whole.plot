load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Высота (м)" #font ",18"
set ytics 2000 nomirror #font ",18"
set yrange [0:32000]
set mytics

#set key default
set key width -22 left top

#set border 1+2+8
set grid xtics ytics

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 3
set style line 3 lt rgb "#14acba" lw 1

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
	'../PLD_MS5611_DATA-13-2.csv' \
		using 'mins_from_launch':'altitude' \
		with lines ls 3 \
		axis x1y1 \
		title "Барометрическая высота" \
	,\
