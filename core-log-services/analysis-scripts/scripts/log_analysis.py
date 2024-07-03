#!/usr/bin/env python3

from optparse import OptionParser
import glob, os

key_strings_default = ["Failed with result 'core-dump'", "DSP stuck", "SEGV"]
limit_strings_default = [(" Memory used: ", 80)]

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def check_file_exists(filename):
    if not os.path.isfile(filename):
        print("Provided file ", filename, " can not be accessed")
        return False
    return True

def print_lines_before_matching_line(line_number, content):
    ret = []
    if line_number > 5:
            before_block = content[line_number-4:line_number-1]
            for b_num, b_line in enumerate(before_block, 1):
                print((line_number - 3) + b_num, b_line.strip())
                ret.append(str((line_number - 3) + b_num) + " " + b_line.strip())
    return ret

def print_lines_after_matching_line(line_number, content):
    ret = []
    if (line_number + 5) < len(content):
            after_block = content[line_number+1:line_number+4]
            for b_num, b_line in enumerate(after_block, 1):
                print((line_number+1) + b_num, b_line.strip())
                ret.append(str((line_number+1) + b_num) + " " + b_line.strip())
    return ret


def process_key_strings(filename, key_strings=key_strings_default):
    if not check_file_exists(filename):
        return []
    
    offending_lines = check_strings(filename, key_strings)
    file = open(filename)
    content = file.readlines()
    output = []
    print("======================================KEY_STRINGS======================================")
    # num just counter for number of block in output
    for num, line in enumerate(offending_lines,1):
        print("offending key string " + str(num) + ":")
        output.extend(print_lines_before_matching_line(line, content))

        print(bcolors.WARNING + str(line+1) + " " +  str(content[line].strip()) + bcolors.ENDC)

        output.append(content[line].strip())
        output.extend(print_lines_after_matching_line(line, content))
        print("\n")
        output.append("\n")
    return output

def process_limit_strings(filename, limit_strings=limit_strings_default):
    if not check_file_exists(filename):
        return []
    
    offending_lines = check_strings(filename, limit_strings)
    file = open(filename)
    content = file.readlines()
    blocks = 1
    output = []
    print("======================================LIMITS======================================")
    for line in offending_lines:
        for limit_string in limit_strings:
            index_of_search_string = content[line].find(limit_string[0])
            cutoff_before_search_string = content[line][index_of_search_string + len(limit_string[0]):]
            percent = int(float(cutoff_before_search_string.split("%")[0]))
            if percent >= limit_string[1]:
                print("offending limit string " + str(blocks) + ":")
                blocks += 1
                output.extend(print_lines_before_matching_line(line, content))
                    
                print(bcolors.WARNING + str(line+1) + " " +  str(content[line].strip()) + bcolors.ENDC)
                output.append(content[line].strip())
                output.extend(print_lines_after_matching_line(line, content))
                print("\n")
                output.append("\n")
    return output

def check_strings(input_file, strings):
    matches = []
    if not os.path.isfile(input_file):
        print("Provided file ", input_file, " can not be accessed")
        return []
    
    with open(input_file) as myFile:
        for num, line in enumerate(myFile,0):
            for string in strings:
                if type(string) is tuple and string[0].lower() in line.lower():
                    matches.append(num)
                elif type(string) is not tuple and string.lower() in line.lower():
                    matches.append(num)
    myFile.close()
    return matches


def main():
    parser = OptionParser()
    parser.add_option("-i", "--input-folder", dest="log_folder_path",
                    help="folder path in which to look for *.log files", metavar="<path_to_log_folder>", default="./")

    (options, args) = parser.parse_args()

    options.log_folder_path = options.log_folder_path.rstrip('/')

    print("chosen folder path:", options.log_folder_path)

    for file in glob.glob("*.log*", root_dir=options.log_folder_path):
        absolute_path = options.log_folder_path + '/' + file
        print("processing:", absolute_path)
        process_key_strings(absolute_path)
        process_limit_strings(absolute_path)

if __name__ == "__main__":
    main()
