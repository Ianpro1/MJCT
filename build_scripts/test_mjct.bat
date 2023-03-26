@echo off
cd ../build/debug
start cmd.exe /k python -i -c "import mjct; import matplotlib.pyplot as plt; print('module was imported successfully!')"