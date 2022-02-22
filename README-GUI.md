AFL-GUI
=======

afl-gui is a partial GUI wrapper written by Alex Epstein, for the "American Fuzzy Lop" instrumented fuzzer written by Michal Zalewski and Jann Horn. afl-gui is EXPRESSLY for trivial educational purposes only.


Usage Guidelines (building from source):
-----------------------------------------------
1. First, install gtk3 dependencies on your machine via
	sudo apt-get install libgtk-3-0 libgtk-3-dev

2. You should then be able to compile afl-gui using pkg-config with
	``gcc afl-gui.c -o afl-gui `pkg-config --cflags --libs gtk+-3.0```
	
3. The wrapper is run by entering
	./afl-gui
	
The Application
---------------
afl-gui provides a simple interface to run afl-fuzz with a number of parameters, plus gives support for custom arguments (for unimplemented options such as parallel mode, or AFL extensions such as aflnet).
The wrapper simply accepts the user's choice of options, passes this to afl-fuzz, and sends its output directly to the initiating terminal. As such it does not extend or modify the behaviour of afl-fuzz in any way, although execution is probably going to be a bit less efficient. 

Desired Improvements
--------------------
+ Functional "open profile" and "save profile" buttons
+ Better implementation of parallelism
+ Output sent to a GTK text buffer in new window
+ Custom GUI output display, rather than Michal's retro text display (which, admittedly, is cool)