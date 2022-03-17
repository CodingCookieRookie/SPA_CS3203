#!/usr/bin/env python3

import os
import subprocess

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

def main():
    # Figure out if we're using VS2019 or 2022
    VS_PATH = VS_2022_PATH if os.path.exists(VS_2022_PATH) else VS_2019_PATH
    
    # Rebuild SPA in release mode
    # Clean
    subprocess.run([VS_PATH + VS_DEVENV, SLN_PATH, PROJECT_FLAG, AUTOTESTER_PROJ,
                     CLEAN_FLAG, RELEASE_BUILD])
    # Build
    subprocess.run([VS_PATH + VS_DEVENV, SLN_PATH, PROJECT_FLAG, AUTOTESTER_PROJ,
                     BUILD_FLAG, RELEASE_BUILD])

    # Run tests
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
