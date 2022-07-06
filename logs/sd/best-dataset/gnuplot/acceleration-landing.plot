load "__common.plot-include"

set datafile separator comma

set xlabel "Время от старта (минуты)"
set xtics 0.01 nomirror
set xrange[110.24:110.27]
set mxtics 10

set x2label "Время от старта (секунды)"
set x2tics 0.5 nomirror
set x2range[110.24*60:110.27*60]
set mx2tics 5

set ylabel "Перегрузка (м/с**2)"
set ytics 5 nomirror
set yrange [-5:60]
unset mytics

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1
set style line 3 lt rgb "orange" lw 1
set style line 4 lt rgb "black" lw 1

set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -1

plot \
	'../SINS_ISC-11-0.csv' \
		using \
			'mins_from_launch' \
			: \
			(sqrt(column('accel[0]')**2 + column('accel[1]')**2 + column('accel[2]')**2)) \
			every 1 \
		with lines ls 4 \
		axis x1y1 \
		title "По модулю" \
	,\
	'../SINS_ISC-11-0.csv' \
		using \
			'mins_from_launch' \
			: \
			(sqrt(column('accel[0]')**2 + column('accel[1]')**2 + column('accel[2]')**2)) \
			every 1 \
		with points ls 1 \
		axis x1y1 \
		notitle \
	,\
	# '../SINS_ISC-11-0.csv' \
	# 	using 'mins_from_launch':'accel[0]' every 1 \
	# 	with lines ls 1 \
	# 	axis x1y1 \
	# 	title "По оси X" \
	# ,\
	# '../SINS_ISC-11-0.csv' \
	# 	using 'mins_from_launch':'accel[1]' every 1 \
	# 	with lines ls 2 \
	# 	axis x1y1 \
	# 	title "По оси Y" \
	# ,\
	# '../SINS_ISC-11-0.csv' \
	# 	using 'mins_from_launch':'accel[2]' every 1 \
	# 	with lines ls 3 \
	# 	axis x1y1 \
	# 	title "По оси Z" \
	# ,\