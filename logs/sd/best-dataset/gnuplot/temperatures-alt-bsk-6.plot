load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Высота (м)" #font ",18"
set xtics 5000 nomirror #font ",18"
set xrange [0:32000]
set mxtics

set yzeroaxis
set ylabel "Температура °C"
set ytics 10 nomirror
set yrange[-50:60]
set mytics 10

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1
set style line 3 lt rgb "blue" lw 1
set style line 4 lt rgb "orange" lw 1
set style line 5 lt rgb "cyan" lw 1
set style line 6 lt rgb "violet" lw 1

set key right

plot \
	'../THERMAL_STATE-10-5.csv' \
		using 'baro_altitude':'temperature' \
		with lines ls 1 \
		axis x1y1 \
		title "БСК1" \
