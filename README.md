Circus: An event-driven IRC framework written in C
==================================================

[![Build Status](https://secure.travis-ci.org/nacx/circus.png)](http://travis-ci.org/nacx/circus)

Circus is an event-driven IRC framework written in C that provides a simple
way to write IRC clients in pure C. Its main purpose is to provide a base
to write IRC bots, but it may be extended to implement fully-featured IRC
clients.


Compiling Circus
----------------

To compile Circus, you can simply run:

    make

This will compile the Circus library, run all unit tests, and build all the
examples in the *examples* directory, so you can run them.

If you want to install the library, you can do it as usual to install it in
the default location, or you can provide the *prefix* parameter to customize
it, as follows:

    make install                       # will install Circus in /usr/local/lib
    make install prefix=<install dir>  # will install Circus in <install dir>


Building Circus based applications
----------------------------------

To build your own Circus application, you can take a look at the *examples*
folder. There you will find an example Makefile that you can use to build it.

If you installed Circus using the *make install* command, you will need to change
the *LIBDIR* variable to point to the location where you installed the library.


Note on patches/pull requests
-----------------------------
 
 * Fork the project.
 * Make your feature addition or bug fix.
 * Add tests for it. This is important so I don't break it in a future version unintentionally.
 * Commit.
 * Send me a pull request. Bonus points for topic branches.


Issue Tracking
--------------

If you find any issue, please submit it to the [Bug tracking system](https://github.com/nacx/circus/issues) and we
will do our best to fix it.


License
-------

This sowftare is licensed under the MIT license. See LICENSE file for details.

