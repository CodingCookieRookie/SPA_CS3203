@ECHO OFF
SET NUM_TESTS=1
SET TEAM_NUMBER=13
FOR /l %%x IN (1, 1, %NUM_TESTS%) DO (
    Code00\Debug\AutoTester.exe Tests00\basic%%x_source.txt Tests00\basic%%x_queries.txt Tests00\basic%%x_out.xml
)