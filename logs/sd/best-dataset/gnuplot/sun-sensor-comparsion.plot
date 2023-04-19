load "__common.plot-include"

set datafile separator comma

# Подъём
START = 10
STOP = 12

# Парение
# START = 60
# STOP = 62

# Падение
# START = 80
# STOP = 82

SPACING = 1

set multiplot layout 2, 1 scale 1.0, 1.0
set lmargin at screen 0.12

set grid xtics ytics

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1

set key right bottom outside horizontal width -1 box


#set x2label "Время от старта (секунды)"
set x2tics 10 nomirror
set x2range[START*60:STOP*60]
set mx2tics 5

#set xlabel "Время от старта (минуты)"
set xtics 0.2 nomirror
set xrange[START:STOP]
set mxtics 10


set ylabel "Theta (вертикаль)"
set ytics nomirror
#set yrange [-40:60]
unset mytics

unset xlabel
set x2label "Время от старта (секунды)"

plot \
	'../analysis-scripts/SINS_ISC-11-0+rotated-sun.csv' \
		using 'mins_from_launch':(column('sp_real_rot[theta]')*180/pi) every SPACING \
		with lines ls 1 \
		axis x1y1 \
		title "theta по AHRS" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_measured[1]')*180/pi) every SPACING \
		with lines ls 2 \
		axis x1y1 \
		title "theta по СД" \
	,\



set ylabel "Phi (горизонталь)"
set ytics nomirror
#set yrange [-40:60]
unset mytics

unset x2label
set xlabel "Время от старта (минуты)"

plot \
	'../analysis-scripts/SINS_ISC-11-0+rotated-sun.csv' \
		using 'mins_from_launch':(column('sp_real_rot[phi]')*180/pi) every SPACING \
		with lines ls 1 \
		axis x1y1 \
		title "phi по AHRS" \
	,\
	'../SINS_ISC-11-0.csv' \
		using 'mins_from_launch':(column('light_dir_measured[2]')*180/pi) every SPACING \
		with lines ls 2 \
		axis x1y1 \
		title "phi по СД" \
	,\

unset multiplot
