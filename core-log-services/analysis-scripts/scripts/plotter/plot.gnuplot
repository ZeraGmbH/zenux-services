set datafile separator ","

set xdata time
set timefmt "%b %d %H:%M:%S" # specify our time string format
set format x "%H:%M"

set key autotitle columnhead # use the first line as title
set xlabel 'Time' # label for the X axis
set ylabel "Measurement" # label for the Y axis

set style line 1 \
    linecolor rgb '#00725c' \
    linetype 1 linewidth 0.25 \
    pointtype 7 pointsize 0.5


if (multi == "1") {
    plot filename using 1:2 with linespoints linestyle 1, filename using 1:3 with linespoints linestyle 1 linecolor rgb "#0066CC"
}else{
    plot filename using 1:2 with linespoints linestyle 1
}
