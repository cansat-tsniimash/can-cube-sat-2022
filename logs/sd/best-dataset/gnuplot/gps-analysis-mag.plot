load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Параметры сигнала (нормированные)" #font ",18"
set ytics 0.1 nomirror #font ",18"
set yrange [-1:1.5]

set y2zeroaxis
set y2label "Температура (°C)" #font ",18"
set y2tics 10 nomirror #font ",18"
set y2range [-40:50]

set key left width -10

set grid xtics ytics

set style line 5 lt rgb "gray" dt (5, 10, 10, 10) lw 2
set style line 6 lt rgb "black" dt (5, 10, 10, 10) lw 1

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'mins_from_launch':(column('ofs_i_smooth')/255) \
		with lines ls 1 \
		axis x1y1 \
		title "Offset I" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'mins_from_launch':(column('mag_i_smooth')/255) \
		with lines ls 2 \
		axis x1y1 \
		title "Magnitude I" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'mins_from_launch':(column('ofs_q_smooth')/128) \
		with lines ls 3 \
		axis x1y1 \
		title "Offset Q" \
	,\
	'../analysis-scripts/gps-a+SINS_MONHW2-11-0.csv' \
		using 'mins_from_launch':(column('mag_q_smooth')/255) \
		with lines ls 4 \
		axis x1y1 \
		title "Magnitude Q" \
	,\
	'../THERMAL_STATE-10-5.csv' \
		using 'mins_from_launch':'temperature' \
		with lines ls 5 \
		axis x1y2 \
		title "Температура (БСК-5)" \
	,\
	# '../OWN_TEMP-11-0.csv' \
	# 	using 'mins_from_launch':'deg' \
	# 	with lines ls 6 \
	# 	axis x1y2 \
	# 	title "Температура (БФНП)" \
	# ,\
