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
	'../THERMAL_STATE-10-0.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 1 \
		axis x1y1 \
		title "БСК1" \
	,\
	'../THERMAL_STATE-10-1.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 2 \
		axis x1y1 \
		title "БСК2" \
	,\
	'../THERMAL_STATE-10-2.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 3 \
		axis x1y1 \
		title "БСК3" \
	,\
	'../THERMAL_STATE-10-3.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 4 \
		axis x1y1 \
		title "БСК4" \
	,\
	'../THERMAL_STATE-10-4.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 5 \
		axis x1y1 \
		title "БСК5" \
	,\
	'../THERMAL_STATE-10-5.csv' \
		using 'baro_altitude':'temperature' every ::4980 \
		with lines ls 6 \
		axis x1y1 \
		title "БСК6" \
	,\
