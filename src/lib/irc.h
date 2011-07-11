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

#ifndef __IRC_H__
#define __IRC_H__

// Include the common files, so circus apps only need to include this file
#include "codes.h"
#include "events.h"

// Event binding
void irc_bind_event(char* event, void* callback);       // Bind an IRC event to a callback function
void irc_bind_command(char* command, void* callback);   // Bind a channel or private chat command to a callback function
void irc_unbind_event(char* event);                     // Unbind an IRC event
void irc_unbind_command(char* command);                 // Unbind a channel or private message chat command

// Connection registration
void irc_connect(char* address, int port);                      // Connect to the IRC server
void irc_disconnect();                                          // Disconnect from the IRC server
void irc_listen();                                              // Listen to IRC server messages (blocks until quit signal is received)
void irc_nick(char* nick);                                      // Set or change the nick of the user
void irc_user(char* user_name, char* real_name);                // Set the user information
void irc_login(char* nick, char* user_name, char* real_name);   // Sets the nick and the user information
void irc_quit(char* message);                                   // Sends a quit message to the server to close the connection

// Channel operations
void irc_join(char* channel);                               // Join a channel
void irc_join_pass(char* channel, char* pass);              // Join a password protected channel
void irc_part(char* channel);                               // Leave a channel
void irc_topic(char* channel, char* topic);                 // Change the topic of a channel
void irc_names(char* channel);                              // List the users in a channel
void irc_list();                                            // List channels and their topics
void irc_invite(char* nick, char* channel);                 // Invite a user to a channel
void irc_channel(char* channel, char* messge);              // Send a message to a channel
void irc_private(char* nick, char* message);                // Send a private message to a user
void irc_op(char* channel, char* nick);                     // Give operator to a user
void irc_deop(char* channel, char* nick);                   // Take operator from a user
void irc_voice(char* channel, char* nick);                  // Give voice to a user
void irc_devoice(char* channel, char* nick);                // Take voice from a user
void irc_kick(char* channel, char* nick, char* message);    // Kick a user from a channel

// Miscellaneous functions
void irc_pong(char* server);                // Respond to ping message to keep connected if inactive

// System functions
void irc_raw(char* prefix, char* type, char* message);      // Send a raw message to the IRC server

#endif

