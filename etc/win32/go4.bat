:: script uses go4init and starts go4.exe

@set THIS=%0
@set THIS=%THIS:~0,-7%.
@cd /d %THIS%
call go4init.bat

start go4.exe
