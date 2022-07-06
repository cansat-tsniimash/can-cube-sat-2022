set datafile separator comma

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Дозиметр в целом
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

set xzeroaxis
set xlabel "Время от старта (минуты)" #font ",18"
set xtics 10 nomirror #font ",18"
set xrange[-10:120]
set mxtics

set yzeroaxis
set ylabel "Обмен между бортовыми ВМ (пакетов в секунду)" #font ",18"
set ytics 10 nomirror #font ",18"
set yrange [0:50]
set mytics

set grid xtics ytics

set style line 1 lt rgb "#f2cbcb" lw 1
set style line 2 lt rgb "#d1f7cb" lw 1
set style line 3 lt rgb "#b5c3f5" lw 1

set style line 4 lt rgb "#f51414" lw 2
set style line 5 lt rgb "#03ad09" lw 2
set style line 6 lt rgb "#0903ad" lw 2


set arrow from 78.335, graph 0 to 78.335, graph 1 nohead front dt (5, 5, 10, 10) lc rgb 'blue' lw 2

set key left width -20

plot \
	'its-link-a+I2C_LINK_STATS-11-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done' \
		with lines ls 1 \
		axis x1y1 \
		notitle \
	,\
	'its-link-a+I2C_LINK_STATS-12-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done' \
		with lines ls 2 \
		axis x1y1 \
		notitle \
	,\
	'its-link-a+I2C_LINK_STATS-13-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done' \
		with lines ls 3 \
		axis x1y1 \
		notitle \
	,\
	'its-link-a+I2C_LINK_STATS-11-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done_smooth' \
		with lines ls 4 \
		axis x1y1 \
		title "Обмен с БФНП" \
	,\
	'its-link-a+I2C_LINK_STATS-12-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done_smooth' \
		with lines ls 5 \
		axis x1y1 \
		title "Обмен с АРК" \
	,\
	'its-link-a+I2C_LINK_STATS-13-0.csv' \
		using 'mins_from_launch':'rela_delta_tx_done_smooth' \
		with lines ls 6 \
		axis x1y1 \
		title "Обмен с полезной нагрузкой" \
	,\
