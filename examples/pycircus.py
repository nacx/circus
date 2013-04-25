#!/usr/bin/env python

def circus_callback(message):
    """ Callback is going to be invoked by Circus """
    return "Python received message: " + message

