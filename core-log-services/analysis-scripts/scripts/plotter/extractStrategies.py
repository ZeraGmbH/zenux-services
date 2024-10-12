import structs

def get_timestamp(line):
    # journalctl log line has the following layout:
    # Jun 24 13:15:24.856115 imx6qdl-variscite-som alsactl[353]: Found hardware: "tlv320aic3106-a" "" "" "" ""
    linesplit = line.split(': ', 1)
    if(len(linesplit) != 2):
        return ''
    lead = linesplit[0]
    # lead is: Jun 24 13:15:24.856115 imx6qdl-variscite-som alsactl[353]
    timestamp = lead.rsplit(' ', 2)[0]
    # timestamp is 'Jun 24 13:15:24.856115' or 'Jun 24 13:15:24'
    timestamp = timestamp.split('.')[0]
    # timestamp is now 'Jun 24 13:15:24'
    return timestamp

def extract_min_max_mean(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            partsSplit = parts.split(' ')
            returnOutput.value = partsSplit[1]
    return returnOutput

def extract_min_max_mean_dsp(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            partsSplit = parts.split(' ')
            returnOutput.value = partsSplit[1].replace(';','')
        if "max" in parts:
            parts = parts.lstrip()
            partsSplit = parts.rsplit(' ',1)
            returnOutput.value2 = partsSplit[1].replace(';','')
    return returnOutput

def extract_min_max_mean_total_int(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            returnOutput.value = parts.rsplit(' ',1)[1]
    return returnOutput

def extract_dsp(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('/',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "max load:" in parts:
            value = parts.rsplit(" ", 1)
            returnOutput.value = value[1].strip()
    return returnOutput

def extract_dsp_read_write(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit('/',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "Read:" in parts:
            value = parts.rstrip().rsplit(" ",1)[1].strip()
            returnOutput.value = value
        if "Write" in parts:
            value = parts.rstrip().rsplit(" ",1)[1].strip()
            returnOutput.value2 = value
    return returnOutput

def extract_meas_freq(input_line):
    input_line = input_line.rstrip()
    splitString = input_line.rsplit(',',1)
    returnOutput = structs.measValue()
    returnOutput.time = get_timestamp(input_line)
    for parts in splitString:
        if "mean" in parts:
            parts = parts.lstrip()
            partsSplit = parts.split(' ')
            returnOutput.value = partsSplit[1].replace(';','')
        if "max" in parts:
            parts = parts.lstrip()
            partsSplit = parts.rsplit(' ',1)
            returnOutput.value2 = partsSplit[1].replace(';','')
    return returnOutput
