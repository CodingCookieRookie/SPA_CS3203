#!/usr/bin/env python3

import os
import subprocess
import xml.etree.ElementTree as ET

AUTOTESTER_PATH = '..\Code13\Release\AutoTester.exe'
SLN_PATH = '..\Code13\StartupSPASolution.sln'
PROJECT_FLAG = '/Project'
AUTOTESTER_PROJ = 'AutoTester'
CLEAN_FLAG = '/Clean'
BUILD_FLAG = '/Build'
RELEASE_BUILD = 'Release|Win32'
VS_DEVENV = 'devenv.com'

VS_2022_PATH = 'C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\'
VS_2019_PATH = 'C:\\Program Files\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\'

def clean_and_build():
    # Figure out if we're using VS2019 or 2022
    VS_PATH = VS_2022_PATH if os.path.exists(VS_2022_PATH) else VS_2019_PATH
    
    # Rebuild SPA in release mode
    # Clean
    subprocess.run([VS_PATH + VS_DEVENV, SLN_PATH, PROJECT_FLAG, AUTOTESTER_PROJ,
                     CLEAN_FLAG, RELEASE_BUILD])
    # Build
    subprocess.run([VS_PATH + VS_DEVENV, SLN_PATH, PROJECT_FLAG, AUTOTESTER_PROJ,
                     BUILD_FLAG, RELEASE_BUILD])

def run_tests(subfolder):
    # Make sure that the subfolder exists
    cwd = os.getcwd()
    subdirectory = os.path.join(cwd, subfolder)
    if not os.path.exists(subdirectory) or not os.path.isdir(subdirectory):
        return
    
    # Run tests
    print('=' * 40)
    print(f'Running tests in: {subfolder}')
    print('=' * 40)
    
    txt_files = set()
    for file in os.listdir(subdirectory):
        if file.endswith('.txt'):
            txt_files.add(file)

    for file in txt_files:
        if not file.endswith('_queries.txt'):
            continue
        root_name = file[:-12]
        query_file = os.path.join(subdirectory, root_name + '_queries.txt')
        source_file = os.path.join(subdirectory, root_name + '_source.txt')
        xml_file = os.path.join(subdirectory, root_name + '_out.xml')
        if os.path.basename(source_file) not in txt_files:
            continue
        print(f'Running test set: {root_name}')
        subprocess.call([AUTOTESTER_PATH, source_file, query_file, xml_file])
        if os.path.exists(xml_file) and os.path.isfile(xml_file):
            tree = ET.parse(xml_file)
            num_errors = len(tree.findall('.//failed')) \
                + len(tree.findall('.//timeout')) \
                + len(tree.findall('.//exception')) \
                + len(tree.findall('.//crash'))
            num_queries = len(tree.findall('.//query'))
            print(f'Number of errors: {num_errors} / {num_queries}')

def main():
    clean_and_build()
    run_tests('./')
    curr_dir = os.getcwd()
    for sub in os.listdir():
        if os.path.isdir(os.path.join(curr_dir, sub)):
            run_tests(sub)

    
if __name__ == '__main__':
    main()
