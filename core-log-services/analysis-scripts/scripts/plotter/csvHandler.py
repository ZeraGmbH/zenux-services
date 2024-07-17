from pathlib import Path
import csv

def saveAsCSV(inputList, logType):
    Path("/tmp/plotter_out/").mkdir(parents=True, exist_ok=True)
    with open('/tmp/plotter_out/' + logType + '.csv', 'w') as file:
        writer = csv.writer(file)
        writer.writerow(["datetime", logType.replace("_", " ")])
        for values in inputList:
            writer.writerow([values.time, values.value])
    return True    
