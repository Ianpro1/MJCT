If the user intent to use this environment repository to develop tasks and create his own python bindings,
he can move the build.bat into his build repository and run it to automatically build the project through cmake.

The test.bat file is to: quickly open a terminal from its current directory, and run python.
Similarly, test_mjct.bat quickly opens a terminal from its current directory, run python, and import the mjct module.
Hence, it is a good idea to move those files into the debug location where the .pyd module will be created (default is ${CMAKE_CURRENT_DIR}/build/debug)