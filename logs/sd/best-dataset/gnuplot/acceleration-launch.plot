load "__common.plot-include"

set datafile separator comma

set xlabel "Время от старта (минуты)"
set xtics 0.5 nomirror
set xrange[-1:3]
set mxtics

set ylabel "Измеренная перегрузка (м/с**2)"
set ytics 5 nomirror
set yrange [-5:20]
unset mytics

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1
set style line 3 lt rgb "orange" lw 1
set style line 4 lt rgb "black" lw 1

set arrow from 0, graph 0 to 0, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -1

plot \
	'../SINS_ISC-11-0.csv' \
		using \
			(column('time_s_from_launch') + column('time_us')/1000000) \
			: \
			(sqrt(column('accel[0]')**2 + column('accel[1]')**2 + column('accel[2]')**2)) \
			every 1 \
		with lines ls 4 \
		axis x1y1 \
		title "По модулю" \
	,\


	# '../SINS_ISC-11-0.csv' \
	# 	using (column('time_s_from_launch') + column('time_us')/1000000):'accel[0]' every 1 \
	# 	with lines ls 1 \
	# 	axis x1y1 \
	# 	title "По оси X" \
	# ,\
	# '../SINS_ISC-11-0.csv' \
	# 	using (column('time_s_from_launch') + column('time_us')/1000000):'accel[1]' every 1 \
	# 	with lines ls 2 \
	# 	axis x1y1 \
	# 	title "По оси Y" \
	# ,\
	# '../SINS_ISC-11-0.csv' \
	# 	using (column('time_s_from_launch') + column('time_us')/1000000):'accel[2]' every 1 \
	# 	with lines ls 3 \
	# 	axis x1y1 \
	# 	title "По оси Z" \
	# ,\
