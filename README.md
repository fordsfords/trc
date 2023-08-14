# trc
Trace module in C.
This tries to be portable between Mac, Linux, and Windows.

<!-- mdtoc-start -->
&DoubleRightArrow; [trc](#trc)  
&nbsp;&nbsp;&DoubleRightArrow; [License](#license)  
&nbsp;&nbsp;&DoubleRightArrow; [Introduction](#introduction)  
<!-- TOC created by '../mdtoc/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->

## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

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


## Introduction

Debugging complex code is hard.
Debugging complex real-time code is very hard.
Debugging complex real-time multi-threaded code is very vary hard.

This module is a tool to help.

Ideally, this will be kept live in your program even during production use.
The overhead involved is pretty small;
only the most latency-sensitive applications would notice the overhead.

