/*
 * Copyright (c) 2011 Ignasi Barrera
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * This is an example bot showing how to call Python functions
 * to handle Circus events.
 * This requires the Python 2.7 development packages to be installed
 * in your system.
 */

#include <python2.7/Python.h>
#include <stdio.h>
#include "irc.h"                    /* IRC protocol functions */

#define CONF_NICK "circus-bot"      /* The nick to be used by the bot */
#define CONF_CHAN "#circus-bot"     /* The channel to join */
#define PY_FILE "pycircus.py"       /* The python file to load */

/* Python environment */
static PyObject* py_callback;       /* The python callback */
static void py_init(void);          /* Initialize python environment */
static void py_destroy(void);       /* Clenaup python environment */

/* Invoke the callback in the python file */
void on_python(MessageEvent* event) {
    PyObject* py_ret = PyObject_CallFunction(py_callback, "s", event->message);
    char* result = PyString_AsString(py_ret);
    irc_message(event->is_channel? event->to : event->user.nick, result);
}

int main(int argc, char **argv) {
    char* server, *port;

    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server = argv[1];   /* The IRC server */
    port = argv[2];     /* The IRC server port */

    py_init();  /* Initialize the python environment */

    /* Bind IRC command to custom function */
    irc_bind_command("py", (Callback) on_python);

    /* Connect, login and join the configured channel */
    irc_connect(server, port);
    irc_login(CONF_NICK, "Circus", "Circus IRC bot");
    irc_join(CONF_CHAN);

    /* Start listening to events.
     * This method blocks until a quit signal is received */
    irc_listen();

    /* Send quit message and close connection */
    irc_quit("Bye");
    irc_disconnect();

    py_destroy();

    return 0;
}

/* Initialize the python module */
void py_init() {
    FILE* fmodule;
    PyObject* py_module, *py_env;

    Py_Initialize();

    /* Load the python module */
    fmodule = fopen(PY_FILE, "r");
    PyRun_SimpleFile(fmodule, PY_FILE);
    fclose(fmodule);

    /* Get teh module and its contents */
    py_module = PyImport_AddModule("__main__");
    py_env = PyModule_GetDict(py_module);

    /* Lookup the callback */
    py_callback = PyDict_GetItemString(py_env, "circus_callback");
}

/* Cleanup python environment */
void py_destroy() {
    Py_Finalize();
}

