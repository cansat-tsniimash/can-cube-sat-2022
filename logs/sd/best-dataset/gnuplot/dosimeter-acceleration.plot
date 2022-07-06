load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 1 nomirror #font ",18"
set xrange[75:95]
set mxtics

set yzeroaxis
set ylabel "Скорость счёта (импульсов в секунду)"
set ytics 5 nomirror
set yrange [-20:20]
set mytics

set y2zeroaxis
set y2label "Перегрузка (м/c**2)"
set y2tics 5 nomirror
set y2range [0:50]
set my2tics

set grid xtics ytics

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2
set style line 3 lt rgb "black" lw 1

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -13

plot \
	'../analysis-scripts/dosim-a+PLD_DOSIM_DATA-13-0.csv' \
		using 'mins_from_launch':'ticks_per_sec' \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/dosim-a+PLD_DOSIM_DATA-13-0.csv' \
		using 'mins_from_launch':'ticks_per_sec_smooth' \
		with lines ls 2 \
		axis x1y1 \
		title "Скорость счёта" \
	,\
	'../SINS_ISC-11-0.csv' \
		using \
			'mins_from_launch' \
			: \
			(sqrt(column('accel[0]')**2 + column('accel[1]')**2 + column('accel[2]')**2)) \
			every 1 \
		with lines ls 3 \
		axis x1y2 \
		title "Перегрузка" \
	,\
