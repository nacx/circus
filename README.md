A simple IRC framework written in C
===================================

Circus is a simple IRC framework written in C that provides a simple
way to write IRC clients in pure C. Its main purpose is to provide
a base to write IRC bots, but it may be extended to implement fully-featured
IRC clients.


Compiling Circus
----------------

To compile your own Circus application, edit the `src/Makefile`file and add all your
source files to the `SRC` variable. Once you have declared your sources, you can build
them as usual invoking make:

    make
    
This will compile the Circus source code, run all the unit tests and finally build the
entire application if all tests passed.

Issue Tracking
--------------

If you find any issue, please submit it to the [Bug tracking system](https://github.com/nacx/circus/issues) and we
will do our best to fix it.

License
-------

This sowftare is licensed under the MIT license. See LICENSE file for details.
