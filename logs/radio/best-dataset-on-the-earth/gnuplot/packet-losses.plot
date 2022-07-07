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
set yrange [0:200]
set mytics

set key left width -10

set grid xtics ytics

set style line 1 lt rgb "black" lw 1

set multiplot
set size 1.0, 0.5
set origin 0.0, 0.5

#set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
#set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set arrow from 58.583, graph 0 to 58.583, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from 93.867, graph 0 to 93.867, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2


plot \
	'../analysis-scripts/a+lost_frames.csv' \
		using (column('mins_from_start')):'total_frames' \
		with boxes fs solid 1.0 border rgb "black" lt 1 lc rgb "green"\
		axis x1y1 \
		title "Полученных пакетов" \
	,\
	'../analysis-scripts/a+lost_frames.csv' \
		using (column('mins_from_start')):'lost_frames' \
		with boxes fs solid 1.0 border rgb "black" lt 1 lc rgb "red" \
		axis x1y1 \
		title "Потерянных пакетов"


set size 1.0, 0.5
set origin 0.0,0.0

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2

set yzeroaxis
set ylabel "Соотношение сигнал/шум (dB?)"
set ytics 5 nomirror
set yrange [-15:15]
set mytics


plot \
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using (column('mins_from_start')):(column('snr_pkt_smooth')) \
		with lines ls 2 \
		axis x1y1 \
		notitle \
	,\

unset multiplot
