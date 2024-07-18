set datafile separator ","

set xdata time
set timefmt "%b %d %H:%M:%S" # specify our time string format
set format x "%H:%M"

set key autotitle columnhead # use the first line as title
set xlabel 'Time' # label for the X axis
set ylabel "Measurement" # label for the Y axis
plot filename using 1:2  smooth acsplines lt rgb "#00725c" lw 2

if (multi == "1") {
    plot filename using 1:2 smooth acsplines lt rgb "#00725c" lw 2, filename using 1:3 smooth acsplines lt rgb "#0066CC" lw 2  
}else{
    plot filename using 1:2 smooth acsplines lt rgb "#00725c" lw 2
}
