load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Индикатор шума GPS" #font ",18"
#set ytics 50 nomirror #font ",18"
#set yrange [-100:0]
set mytics 10

unset key

set grid xtics ytics mxtics

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using ((column('time_s')-1656909365)/60):(column('rssi_pkt')) \
		with lines ls 2 \
		axis x1y1 \
		notitle \
	,\
