from pathlib import Path
import csv

def saveAsCSV(inputList, logType):
    Path("/tmp/plotter_out/").mkdir(parents=True, exist_ok=True)
    with open('/tmp/plotter_out/' + logType + '.csv', 'w') as file:
        writer = csv.writer(file)
        if inputList[0].value2:
            writer.writerow(["datetime", logType.replace("_", " "), "dsp write"])
        else:
            writer.writerow(["datetime", logType.replace("_", " ")])
        for values in inputList:
            if values.value2:
                writer.writerow([values.time, values.value, values.value2])
            else:
                writer.writerow([values.time, values.value])
    return True    
