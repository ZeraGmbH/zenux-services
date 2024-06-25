#!/usr/bin/env python3

from optparse import OptionParser
import glob, os

key_strings_default = ["Failed with result 'core-dump'", "DSP stuck"]
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


def process_key_strings(filename, key_strings=key_strings_default):
    offending_lines = check_strings(filename, key_strings)
    file = open(filename)
    content = file.readlines()
    ret = []
    print("======================================KEY_STRINGS======================================")
    for num, line in enumerate(offending_lines,1):
        print("offending key string " + str(num) + ":")
        if line > 5:
            before_block = content[line-4:line-1]
            for b_num, b_line in enumerate(before_block, 1):
                print((line - 3) + b_num, b_line.strip())
                ret.append(str((line - 3) + b_num) + " " + b_line.strip())
            
        print(bcolors.WARNING + str(line+1) + " " +  str(content[line].strip()) + bcolors.ENDC)
        ret.append(content[line].strip())
        if (line + 5) < len(content):
            after_block = content[line+1:line+4]
            for b_num, b_line in enumerate(after_block, 1):
                print((line+1) + b_num, b_line.strip())
                ret.append(str((line+1) + b_num) + " " + b_line.strip())
        print("\n")
        ret.append("\n")
    return ret

def process_limit_strings(filename, limit_strings=limit_strings_default):
    offending_lines = check_strings(filename, limit_strings)
    file = open(filename)
    content = file.readlines()
    blocks = 1
    ret = []
    print("======================================LIMITS======================================")
    for line in offending_lines:
        for limit_string in limit_strings:
            temp_index = content[line].find(limit_string[0])
            temp_string = content[line][temp_index + len(limit_string[0]):]
            percent = int(float(temp_string.split("%")[0]))
            if percent >= limit_string[1]:
                print("offending limit string " + str(blocks) + ":")
                blocks += 1
                if line > 5:
                    before_block = content[line-4:line-1]
                    for b_num, b_line in enumerate(before_block, 1):
                        print((line - 3) + b_num, b_line.strip())
                        ret.append(str((line - 3) + b_num) + " " + b_line.strip())
                    
                print(bcolors.WARNING + str(line+1) + " " +  str(content[line].strip()) + bcolors.ENDC)
                ret.append(content[line].strip())
                if (line + 5) < len(content):
                    after_block = content[line+1:line+4]
                    for b_num, b_line in enumerate(after_block, 1):
                        print((line+1) + b_num, b_line.strip())
                        ret.append(str((line+1) + b_num) + " " + b_line.strip())
                print("\n")
                ret.append("\n")
    return ret

def check_strings(input_file, strings):
    matches = []
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