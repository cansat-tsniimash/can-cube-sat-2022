load "__common.plot-include"

set datafile separator comma

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "К" #font ",18"
set ytics 0.1 nomirror #font ",18"
set yrange [-0.1:1.4]
set mytics

set grid xtics ytics

#set style line 1 lt rgb "red" lw 1

set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -7

plot \
	'../analysis-scripts/com-a+COMMISSAR_REPORT-13-0.csv' \
		using 'mins_from_launch':'rela_delta_bads' \
		with lines ls 1 \
		axis x1y1 \
		title "BME280 внутренний" \
	,\
	'../analysis-scripts/com-a+COMMISSAR_REPORT-13-1.csv' \
		using 'mins_from_launch':'rela_delta_bads' \
		with lines ls 2 \
		axis x1y1 \
		title "BME280 внешний" \
	,\
	'../analysis-scripts/com-a+COMMISSAR_REPORT-13-2.csv' \
		using 'mins_from_launch':'rela_delta_bads' \
		with lines ls 3 \
		axis x1y1 \
		title "MS5611 внутренний" \
	,\
	'../analysis-scripts/com-a+COMMISSAR_REPORT-13-3.csv' \
		using 'mins_from_launch':'rela_delta_bads' \
		with lines ls 4 \
		axis x1y1 \
		title "MS5611 внешний" \
	,\
	'../analysis-scripts/com-a+COMMISSAR_REPORT-13-4.csv' \
		using 'mins_from_launch':'rela_delta_bads' \
		with lines ls 5 \
		axis x1y1 \
		title "Системная шина" \
	,\
