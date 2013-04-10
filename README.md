Circus: An event-driven IRC framework written in C
==================================================

[![Build Status](https://secure.travis-ci.org/nacx/circus.png)](http://travis-ci.org/nacx/circus)

Circus is an event-driven IRC framework written in ANSI C that provides a simple
way to write IRC clients in pure C. Its main purpose is to provide a base
to write IRC bots, but it may be extended to implement fully-featured IRC
clients.


Compiling Circus
----------------

To compile Circus, you can simply run:

    make
    
If you want to enable debug messages, you can also compile Circus with debug support. It might
be too verbose, so you'd better use it just during development:

    make DEBUG=1

This will compile the Circus library and build all the examples in the *examples* directory, so
you can start playing with them. It is a good starting point to modify the existing examples and
make them do some cool stuff :)

Before you start coding, it is usually a good idea to run the unit tests. This way you will make
sure everything is working as expected:

    make test

If you want to install the library, you can do it as usual. It will be installed by default in
/usr/local, but you can customize the installation directory by adding the *PREFIX* parameter:

    make install                       # Installs Circus in /usr/local
    make install PREFIX=<install dir>  # Installs Circus in <install dir>


Building Circus based applications
----------------------------------

To build your own Circus application, you can take a look at the *examples* folder.
There you will find the `Makefile.example` file that you can copy and modify to build
your application. The example Makefile assumes that you installed Circus in a default location
(such as /usr or /usr/local); if you installed it in a different place, you may need to configure
the `CFLAGS` and the `LDFLAGS` variables accordingly.

When including the Circus header files in your source code files, you can include them as follows:

    #include <circus/irc.h>

The code examples include the library in a different way, because they point to the library in the
compilation directory. The common pattern is, however, to have the Circus library installed in your
system and reference the headers files as shown above.

Also take into account that circus uses POSIX threads to dispatch events to the user defined callbacks.
Make sure you link the `pthread` library when building your application.


How to contribute
-----------------

Contributions are always welcome. Feel free to share your code!
 
 * Fork the project.
 * Create a topic branch for your feature or bug fix.
 * Develop in the just created topic branch.
 * Add tests for your changes. This is important so I don't break them in a future version unintentionally.
 * Commit.
 * Rebase to the latest upstream version to make sure your topic branch is up to date.
 * Send me a pull request.


Issue Tracking
--------------

If you find any issue or want to ask for a feature, please submit your request to the
[Bug tracking system](https://github.com/nacx/circus/issues) and I'll do my best to fix/add it.


License
-------

This sowftare is licensed under the MIT license. See LICENSE file for details.

