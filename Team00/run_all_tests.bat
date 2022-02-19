@ECHO OFF
SET NUM_TESTS=2
REM Set TEAM_NUMBER to 13 when we rename the folders
SET TEAM_NUMBER=00
FOR /l %%x IN (1, 1, %NUM_TESTS%) DO (
    Code%TEAM_NUMBER%\Debug\AutoTester.exe Tests%TEAM_NUMBER%\basic%%x_source.txt Tests%TEAM_NUMBER%\basic%%x_queries.txt Tests%TEAM_NUMBER%\basic%%x_out.xml
)
