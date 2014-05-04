:: you can add this location to PATH and use RUN to run playa commands

@echo off

:: change dir to the location of the file
cd /d %~dp0

:: start via the venv python
start /B env\Scripts\pythonw.exe src\playa.py %*

