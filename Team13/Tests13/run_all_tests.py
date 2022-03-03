#!/usr/bin/env python3

import os
import subprocess

AUTOTESTER_PATH = '..\Code13\Release\AutoTester.exe'

def main():
    txt_files = set()
    for file in os.listdir():
        if file.endswith('.txt'):
            txt_files.add(file)

    for file in txt_files:
        if not file.endswith('_queries.txt'):
            continue
        root_name = file[:-12]
        query_file = file
        source_file = root_name + '_source.txt'
        xml_file = root_name + '_out.xml'
        if source_file not in txt_files:
            continue
        print(f'Running test set: {root_name}')
        subprocess.call([AUTOTESTER_PATH, source_file, query_file, xml_file])
    
if __name__ == '__main__':
    main()
