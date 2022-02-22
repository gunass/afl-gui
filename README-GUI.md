AFL-GUI 0.1
=======

afl-gui is a partial GUI wrapper written by Alex Epstein, for the "American Fuzzy Lop" instrumented fuzzer written by Michal Zalewski and Jann Horn (https://github.com/google/afl). It should be readily compatible with any extension of AFL which uses the same command-line syntax.

afl-gui is for trivial educational purposes only.


Usage Guidelines (building from source on Ubuntu):
-----------------------------------------------
1. First, install gtk3 dependencies on your machine via
	`sudo apt-get install libgtk-3-0 libgtk-3-dev`
	
2. Move all files in this repository directly to your AFL folder (i.e. wherever "afl-fuzz.c" is) and run
	`make clean all`
there if you haven't already

3. You should then be able to compile afl-gui using pkg-config with
	`` gcc afl-gui.c -o afl-gui `pkg-config --cflags --libs gtk+-3.0` ``
	
4. The wrapper is run by entering
	./afl-gui
	
Building on macOS might be possible as GTK3 can be installed via Homebrew but it probably requires a lot of tweaking. 
	
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
+ Cocoa version for macOS support
