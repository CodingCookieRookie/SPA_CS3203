@ECHO OFF
SET NUM_TESTS=4
SET TEAM_NUMBER=13
FOR /l %%x IN (1, 1, %NUM_TESTS%) DO (
    ..\Code%TEAM_NUMBER%\Release\AutoTester.exe ..\Tests%TEAM_NUMBER%\basic%%x_source.txt ..\Tests%TEAM_NUMBER%\basic%%x_queries.txt ..\Tests%TEAM_NUMBER%\basic%%x_out.xml
)
