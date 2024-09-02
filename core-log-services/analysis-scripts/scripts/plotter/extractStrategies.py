import structs

def get_timestamp(input_text, search_label):
    partsSplit = input_text.split(search_label)
    partsSplit = partsSplit[0].rstrip().rsplit(" ", 1)
    timeStamp = partsSplit[0].strip().split('.')[0]
    return timeStamp

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
            returnOutput.time = get_timestamp(parts, "system-metrics-logger")
    return returnOutput

def extract_min_max_mean_dsp(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            partsSplit = parts.split(' ')
            returnOutput.value = partsSplit[1]
        if "zdsp1d" in parts:
            returnOutput.time = get_timestamp(parts, "zdsp1d")
    return returnOutput

def extract_dsp(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('/',1)
    returnOutput = structs.measValue()

    for parts in splitString:
        if "max load:" in parts:
            value = parts.rsplit(" ", 1)
            returnOutput.value = value[1].strip()
        if "zera-modulemanager" in parts:
            returnOutput.time = get_timestamp(parts, "zera-modulemanager")
    return returnOutput

def extract_dsp_read_write(input_line):
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
            returnOutput.time = get_timestamp(parts, "zdsp1d")
    return returnOutput

def extract_meas_freq(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('~',1)
    returnOutput = structs.measValue()
    for parts in splitString:
        if "Hz" in parts:
            returnOutput.value = parts.replace("Hz", "")
        if "zera-modulemanager" in parts:
            returnOutput.time = get_timestamp(parts, "zera-modulemanager")
    return returnOutput
