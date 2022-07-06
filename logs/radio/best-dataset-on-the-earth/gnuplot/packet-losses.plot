load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)"
set xtics 10 nomirror
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Потери пакетов"
set ytics 50 nomirror
#set yrange [-15:15]
set mytics

unset key

set grid xtics ytics

set style line 1 lt rgb "black" lw 1

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/a+lost_frames.csv' \
		using (column('mins_from_start')):'total_frames' \
		with boxes fs solid 1.0 border rgb "black" lt 1 lc rgb "green"\
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/a+lost_frames.csv' \
		using (column('mins_from_start')):'lost_frames' \
		with boxes fs solid 1.0 border rgb "black" lt 1 lc rgb "red" \
		axis x1y1 \
		notitle \
	,\
