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

def extract_dsp(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('/',1)
    returnOutput = structs.measValue()

    for parts in splitString:
        if "Max load:" in parts:
            resetCutOff = parts.rsplit(",",1)[0]
            value = resetCutOff.rsplit(" ", 1)
            returnOutput.value = value[1].replace("%","").strip()
        if "zdsp1d" in parts:
            partsSplit = parts.split("zdsp1d")
            partsSplit = partsSplit[0].rstrip().rsplit(" ", 1)
            timeStamp = partsSplit[0].strip().split('.')[0]
            returnOutput.time = timeStamp
    return returnOutput

def extract_dsp_read(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('/',1)
    returnOutput = structs.measValue()
    
    for parts in splitString:
        if "Read:" in parts:
            value = parts.rstrip().rsplit(" ",1)[1].strip()
            returnOutput.value = value
        if "Write" in parts:
            value = parts.rstrip().rsplit(" ",1)[1].strip()
            returnOutput.value2 = value
        if "zdsp1d" in parts:
            partsSplit = parts.split("zdsp1d")
            partsSplit = partsSplit[0].rstrip().rsplit(" ", 1)
            timeStamp = partsSplit[0].strip().split('.')[0]
            returnOutput.time = timeStamp
    return returnOutput

def extract_dsp_write(input_line):
    return structs.measValue()