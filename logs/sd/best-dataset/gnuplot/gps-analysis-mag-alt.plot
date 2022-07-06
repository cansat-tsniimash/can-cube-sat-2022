load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Высота (м)" #font ",18"
#set xtics 10 nomirror #font ",18"
#set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Параметры сигнала (нормированные)" #font ",18"
set ytics 0.1 nomirror #font ",18"
set yrange [-1:1]
set mytics

set key

set grid xtics ytics mxtics

plot \
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'baro_altitude':(column('ofs_i_smooth')/255) \
		with lines ls 1 \
		axis x1y1 \
		title "Offset I" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'baro_altitude':(column('mag_i_smooth')/255) \
		with lines ls 2 \
		axis x1y1 \
		title "Magnitude I" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'baro_altitude':(column('ofs_q_smooth')/128) \
		with lines ls 3 \
		axis x1y1 \
		title "Offset Q" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'baro_altitude':(column('mag_q_smooth')/255) \
		with lines ls 4 \
		axis x1y1 \
		title "Magnitude Q" \
	,\
