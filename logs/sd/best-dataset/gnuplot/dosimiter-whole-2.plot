load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Скорость счёта (импульсов в секунду)" #font ",18"
set ytics 200 nomirror #font ",18"
set yrange [0:3000]
set mytics


set y2zeroaxis
set y2label "Высота (м)" #font ",18"
set y2tics 2000 nomirror #font ",18"
set y2range [0:32000]
set my2tics

set grid xtics ytics

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2
set style line 3 lt rgb "green" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

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
	'../PLD_MS5611_DATA-13-2.csv' using 'mins_from_launch':'altitude' \
		with lines ls 3 \
		axis x1y2 \
		title "Высота полёта" \
	,\
