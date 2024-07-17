#!/usr/bin/env python3
from structs import *
from csvHandler import saveAsCSV
from pathlib import Path
import shutil
import subprocess
from optparse import OptionParser

available_log_outputs = ["cpu_temp","cpu_load","cpu_freq","fpga_interrupts","ram_usage"]
search_string_lookup = {
    "cpu_temp": searchLookUp("cpu_temp","CPU Temperature (°C)"),
    "cpu_load": searchLookUp("cpu_load","CPU Load (%)"),
    "cpu_freq": searchLookUp("cpu_freq","CPU Frequency (MHz)"),
    "ram_usage": searchLookUp("ram_usage","RAM usage (%)"),
    "fpga_interrupts": searchLookUp("fpga_interrupts","Fpga Interrupts (interrupt/s)")
}

selected_log_types = []

def parse_log_output(log_input, log_type):
    outputStr = []
    if not Path(log_input).is_file():
        print("Invalid log file - File does not exist. Exiting...", log_input)
        return outputStr
        exit(1)
    else:    
        with open(log_input, 'r') as file:
            for line in file:
                if search_string_lookup[log_type].searchString in line:
                    outputStr.append(search_string_lookup[log_type].extractFunction(line))
    return outputStr

def main():
    global selected_log_types
    parser = OptionParser()
    parser.add_option("-i", "--input-log", dest="log_path",
                    help="path to log file", metavar="<path_to_log>", default="./journal.log-0")
    parser.add_option("-t", "--type-of-log", dest="log_type",
                    help="types of logs comma separated. Available log types: all,cpu_temp,cpu_load,cpu_freq,fpga_interrupts,ram_usage", metavar="<log_type>", default="all")

    (options, args) = parser.parse_args()

    print("chosen log file:", options.log_path)

    if not shutil.which("gnuplot"):
        print("gnuplot could not be found. Please install. Exiting...")
        exit(0)

    log_type_string = options.log_type.split(',')

    if options.log_type == "all":
        selected_log_types = available_log_outputs
    else:
        for log_type in log_type_string:
            if log_type in available_log_outputs:
                selected_log_types.append(log_type)
    
    if not selected_log_types:
        print("no valid log types found in:", log_type_string)
        print("available log types are:", available_log_outputs)
        exit(1)
    
    print("selected log types:", selected_log_types)
    for log_type in selected_log_types:
        parsedStringList = parse_log_output(options.log_path, log_type)
        if not parsedStringList:
            print("No log output found. Old style log format? Exiting...")
            exit(0)
        if saveAsCSV(parsedStringList, log_type):
            gnuplotPath = str(Path(__file__).parent.resolve()) + "/plot.gnuplot"
            subprocess.run(["gnuplot", "-e", "filename='/tmp/plotter_out/" + log_type + ".csv'", "-p", gnuplotPath])
    

if __name__ == "__main__":
    main()
