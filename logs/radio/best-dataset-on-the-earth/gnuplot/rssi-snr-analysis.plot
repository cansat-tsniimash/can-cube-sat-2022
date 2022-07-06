load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)"
set xtics 10 nomirror
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Среднее RSSI на получаемый пакет (dBm)"
set ytics 10 nomirror
set yrange [-100:0]
set mytics

unset key

set grid xtics ytics

set multiplot
set size 1.0, 0.5
set origin 0.0, 0.5

set style line 1 lt rgb "gray" lw 1
set style line 2 lt rgb "red" lw 2

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using ((column('time_s')-1656909365)/60):(column('rssi_pkt')) \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using ((column('time_s')-1656909365)/60):(column('rssi_pkt_smooth')) \
		with lines ls 2 \
		axis x1y1 \
		notitle

set size 1.0, 0.5
set origin 0.0,0.0

set yzeroaxis
set ylabel "SNR - Соотношение сигнал/шум (dB?)"
set ytics 5 nomirror
set yrange [-15:15]
set mytic

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2
set arrow from 110.2515, graph 0 to 110.2515, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using ((column('time_s')-1656909365)/60):(column('snr_pkt')) \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/a+radio.rssi_packet.csv' \
		using ((column('time_s')-1656909365)/60):(column('snr_pkt_smooth')) \
		with lines ls 2 \
		axis x1y1 \
		notitle

unset multiplot
