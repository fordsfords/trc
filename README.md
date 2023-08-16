# trc
Trace module in C.
This tries to be portable between Mac, Linux, and Windows.

<!-- mdtoc-start -->
&DoubleRightArrow; [trc](#trc)  
&nbsp;&nbsp;&DoubleRightArrow; [Introduction](#introduction)  
&nbsp;&nbsp;&DoubleRightArrow; [License](#license)  
<!-- TOC created by '../mdtoc/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->


## Introduction

* Debugging complex code is hard.
* Debugging complex real-time code is very hard.
* Debugging complex real-time multi-threaded code is very very hard.
* Debugging complex real-time multi-threaded code where the problem is
difficult to reproduce is ... well ... you get the idea.

A common method for debugging any problem is inserting print
statements at interesting spots in the code.
However, with real-time multi-threaded software,
it is not unusual for those print statements to change the timing
of your program, causing the problem to no longer happen.

(Reminds me of the story of a programmer who was assigned the
task of fixing a bug.
They inserted some print statements and the problem went away.
So they checked the code back in, with the print statements,
and marked the bug "fixed".)

This module is sort of like a print statement with very low
overhead that it is unlikely to change the timing of your program.
Instead of writing to a log file, it writes to a circular memory
queue, further reducing the overhead.
During normal operation,
events are written (and overwritten) to the circular queue.
If the program detects an error,
it can dump the contents of the circular queue to a file.

This is NOT a line-by-line trace infrastructure.
You, the programmer, are responsible for including calls to "trc_trace()"
at interesting spots in your code.

Ideally, this will be kept live in your program even during production use.
The overhead involved is pretty small;
only the most latency-sensitive applications would notice the overhead.
The reason is that once a problem happens, it is too late to enable tracing.


## License

I want there to be NO barriers to using this code,
so I am releasing it to the public domain.
But "public domain" does not have an internationally agreed upon definition,
so I use CC0:

Copyright 2020 Steven Ford http://geeky-boy.com and licensed
"public domain" style under
[CC0](http://creativecommons.org/publicdomain/zero/1.0/):
![CC0](https://licensebuttons.net/p/zero/1.0/88x31.png "CC0")

To the extent possible under law, the contributors to this project have
waived all copyright and related or neighboring rights to this work.
In other words, you can use this code for any purpose without any
restrictions.  This work is published from: United States.  The project home
is https://github.com/fordsfords/trc

To contact me, Steve Ford, project owner, you can find my email address
at http://geeky-boy.com.  Can't see it?  Keep looking.
