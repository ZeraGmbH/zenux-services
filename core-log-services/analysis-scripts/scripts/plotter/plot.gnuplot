set datafile separator ","

set xdata time
set timefmt "%b %d %H:%M:%S" # specify our time string format
set format x "%H:%M:%S"

set key autotitle columnhead # use the first line as title
set xlabel 'Time' # label for the X axis
set ylabel "Measurement" # label for the Y axis
plot filename using 1:2  smooth acsplines lt rgb "#00725c" lw 2
