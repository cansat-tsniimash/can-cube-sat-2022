load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Расстояние до аппарата (м)"
set xtics 10000 nomirror
set xrange[0:60000]
set mxtics

set yzeroaxis
set ylabel "Среднее RSSI на получаемый пакет (dBm)"
set ytics 10 nomirror
set yrange [-100:-40]
set mytics 10

set y2zeroaxis
set y2label "SNR - Соотношение сигнал/шум (dB?)"
set y2tics 10 nomirror
set y2range [-50:20]
set my2tics 10

set key right width -10

set grid xtics ytics

#set style line 1 lt rgb "red" lw 2
set style fill transparent solid 0.03 noborder
set style circle radius 500

plot \
	'../analysis-scripts/dist-a+GPS_UBX_NAV_SOL-11-0.csv' \
		using 'distance':(column('gpsFix') == 3 ? column('rssi_pkt') : 1/0) \
		with circles lc rgb "red" \
		axis x1y1 \
		notitle \
	,\
	'../analysis-scripts/dist-a+GPS_UBX_NAV_SOL-11-0.csv' \
		using 'distance':(column('gpsFix') == 3 ? column('snr_pkt') : 1/0) \
		with circles lc rgb "blue" \
		axis x1y2 \
		notitle \
	,\
	keyentry with circles lc rgb "blue" fs solid 1 title "SNR", \
	keyentry with circles lc rgb "red" fs solid 1 title "RSSI", \
