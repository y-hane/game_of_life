@echo off
rc.exe menu.rc
cl.exe /utf-8 /W4 /std:c11 /Fe"game_of_life.exe" user32.lib gdi32.lib *.c *.res
del *.obj
del *.res
game_of_life.exe
