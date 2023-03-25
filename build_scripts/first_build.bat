@echo off
cd ..
mkdir build
cd build
cmake ..
cmake --build .
cmake ..
cmake --build .
cd ../mujocotasks
copy tosser.xml ..\build\debug
cd ../extern/mujoco/bin
copy mujoco.dll ..\..\..\build\debug
pause