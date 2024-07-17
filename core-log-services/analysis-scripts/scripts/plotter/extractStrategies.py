import structs

def extract_min_max_mean(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            partsSplit = parts.split(' ')
            returnOutput.value = partsSplit[1]
        if "system-metrics-logger" in parts:
            partsSplit = parts.split("system-metrics-logger")
            partsSplit = partsSplit[0].rstrip().rsplit(" ", 1)
            timeStamp = partsSplit[0].strip().split('.')[0]
            returnOutput.time = timeStamp
    return returnOutput
