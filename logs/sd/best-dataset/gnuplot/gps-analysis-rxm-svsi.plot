load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Параметры видимых спутников GPS" #font ",18"
set ytics 5 nomirror #font ",18"
set yrange [0: 37]

set key on inside left width -25 Left

set grid xtics ytics

#set style line 1 lt rgb "gray" lw 1
#set style line 5 lt rgb "gray" lw 2

set arrow from 78.3373, graph 0 to 78.3373, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

plot \
	'../SINS_RXMSVSI-11-0.csv' \
		using 'mins_from_launch':'num_vis' \
		with lines ls 1 lw 2\
		axis x1y1 \
		title "Количество видимых спутников" \
	,\
	'../SINS_RXMSVSI-11-0.csv' \
		using 'mins_from_launch':'satellites_with_ephs' \
		with lines ls 2 lw 2 \
		axis x1y1 \
		title "Спутников с эфемеридами" \
	,\
	'../SINS_RXMSVSI-11-0.csv' \
		using 'mins_from_launch':'satellites_with_alms' \
		with lines ls 3 lw 2\
		axis x1y1 \
		title "Спутников с альманахом" \
	,\
	'../SINS_RXMSVSI-11-0.csv' \
		using 'mins_from_launch':'	satellites_with_ephs_and_alms' \
		with lines ls 4 lw 2\
		axis x1y1 \
		title "Спутников с альманахом и эфемеридами" \
	,\

