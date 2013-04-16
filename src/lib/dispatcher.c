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

#define _XOPEN_SOURCE   /* Support strtok_r in POSIX and BSD environments */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "dispatcher.h"
#include "debug.h"
#include "codes.h"
#include "utils.h"
#include "irc.h"
#include "binding.h"


/* ***************** */
/* Dispatcher thread */
/* ***************** */

struct dsp_consumer {
    pthread_t* worker;          /* The thread that consumes events */
    pthread_mutex_t* lock;      /* The mutex for the consumer thread */
    pthread_cond_t* ready;      /* Control if there is work to do */
    int terminate;              /* Flag to terminate the dispatcher thread */
};

/* The event consumer */
static struct dsp_consumer* consumer = NULL;

static void consumer_notify();                  /* Notify the consumer that there are events to process */
static void _fire_event(struct raw_event*);     /* Build the appropriate event and invoke user callbacks */


/* ************ */
/* Event queue  */
/* ************ */

/* A node in the dispatcher queue. Double linked to improve insertions. */
struct q_node {
    struct raw_event* event;    /* The target event */
    struct q_node* prev;        /* Previous node in the queue */
    struct q_node* next;        /* Next node in the queue */
};

/* The dispacher queue type */
struct q_dispatch {
    struct q_node* top;         /* The top of the queue */
    struct q_node* bottom;      /* The bottom of the queue */
    pthread_mutex_t* lock;      /* The write mutex for the event queue */
    int size;                   /* The number of elements in the queue */
};

/* The event dispatcher queue */
static struct q_dispatch* events = NULL;


/* ********************* */
/* Event queue functions */
/* ********************* */

/* Creates the event dispatcher queue */
static void q_create() {
    debug(("dispatcher: Creating event dispatcher queue\n"));

    if ((events = malloc(sizeof(struct q_dispatch))) == 0) {
        perror("Out of memory (q_create)");
        exit(EXIT_FAILURE);
    }

    if ((events->lock = malloc(sizeof(pthread_mutex_t))) == 0) {
        perror("Out of memory (q_create: lock)");
        exit(EXIT_FAILURE);
    }

    events->top = NULL;
    events->bottom = NULL;
    events->size = 0;

    pthread_mutex_init(events->lock, NULL);
}

/* Add the givent event to the dispatcher queue */
static void q_push(struct raw_event* event) {
    struct q_node* node;

    /* Prevent concurrent modifications */
    pthread_mutex_lock(events->lock);

    if ((node = malloc(sizeof(struct q_node))) == 0) {
        perror("Out of memory (q_node)");
        exit(EXIT_FAILURE);
    }

    node->event = event;
    node->prev = NULL;
    node->next = NULL;

    /* First element in the event queue*/
    if (events->top == NULL) {
        debug(("dispatcher: Adding the first element: %s\n", event->type));
        events->top = node;
        events->bottom = node;
    } else {
        debug(("dispatcher: Adding at the bottom: %s\n", event->type));
        events->bottom->next = node;
        node->prev = events->bottom;
        events->bottom = node;
    }

    events->size++;
    debug(("dispatcher: New queue size: %d\n", events->size));

    pthread_mutex_unlock(events->lock);
    consumer_notify();
}

/* Removes the top element from the queue */
struct raw_event* q_pop() {
    struct raw_event* event = NULL;

    /* Prevent concurrent modifications */
    pthread_mutex_lock(events->lock);

     /* Empty queue */
    if (events->top != NULL) {
        struct q_node* node;

        node = events->top;
        events->top = node->next;

        /* If the last element of the queue was removed, make sure the bottom pointer
         * also is set to NULL */
        if (events->top == NULL) {
            events->bottom = NULL;
        } else {
            /* Otherwise, remove the previous element for the new top */
            events->top->prev = NULL;
        }

        events->size--;
        event = node->event;
        free(node);

        debug(("dispatcher: Removed top element: %s\n", event->type));
        debug(("dispatcher: New queue size: %d\n", events->size));
    }

    pthread_mutex_unlock(events->lock);

    return event;
}

/* Destroys the event queue */
static void q_destroy() {
    if (events != NULL) {
        struct q_node* node, *next;

        debug(("dispatcher: Destroying event dispatcher queue\n"));

        node = events->top;
        while (node != NULL) {
            next = node->next;
            free(node);
            node = next;
        }

        pthread_mutex_destroy(events->lock);

        free(events->lock);
        free(events);
        events = NULL;
    }
}


/* ************************ */
/* Event consumer functions */
/* ************************ */

/* Consumer thread to process the events in the queue */
static void* event_consumer(void* arg) {
    pthread_mutex_lock(consumer->lock);

    while (consumer->terminate == 0) {
        /* Wait until there are events to process */
        if (events->size == 0) {
            debug(("dispatcher: Waiting for events\n"));
            pthread_cond_wait(consumer->ready, consumer->lock);
            debug(("dispatcher: Notification received\n"));
        }

        /* Need to recheck, because a shutdown request may
         * have changed it */
        if (consumer->terminate == 0) {
            struct raw_event* event = q_pop();  /* Get the next event */
            _fire_event(event);                 /* Invoke user callbacks */
            evt_raw_destroy(event);   /* Free memory once the event has been handled */
        }
    }

    debug(("dispatcher: Terminating consumer\n"));

    pthread_mutex_unlock(consumer->lock);
    pthread_exit(NULL);
}

/* Creates the event consumer thread */
static void consumer_create() {
    if ((consumer = malloc(sizeof(struct dsp_consumer))) == 0) {
        perror("Out of memory (consumer_create)");
        exit(EXIT_FAILURE);
    }

    if ((consumer->worker = malloc(sizeof(pthread_t))) == 0) {
        perror("Out of memory (consumer_create: worker)");
        exit(EXIT_FAILURE);
    }

    if ((consumer->lock = malloc(sizeof(pthread_mutex_t))) == 0) {
        perror("Out of memory (consumer_create: lock)");
        exit(EXIT_FAILURE);
    }

    if ((consumer->ready = malloc(sizeof(pthread_cond_t))) == 0) {
        perror("Out of memory (consumer_create: ready)");
        exit(EXIT_FAILURE);
    }

    consumer->terminate = 0;
    pthread_mutex_init(consumer->lock, NULL);
    pthread_cond_init(consumer->ready, NULL);

    /* Create and start the dispatcher thread */
    debug(("dispatcher: Creating the event dispatcher thread\n"));
    if (pthread_create(consumer->worker, NULL, event_consumer, NULL) != 0) {
        printf("dispatcher: Error creating event consumer thread\n");
        exit(EXIT_FAILURE);
    }
}

/* Terminates the event consumer thread */
static void consumer_destroy() {
    if (consumer != NULL) {
        void* status;

        /* Force the dispatcher thread to terminate */
        consumer->terminate = 1;
        pthread_cond_signal(consumer->ready); /* Unlock the dispatcher thread */

        /* Wait until the dispatcher thread terminates */
        debug(("dispatcher: Terminating the event dispatcher thread\n"));
        if (pthread_join(*consumer->worker, &status) != 0) {
            printf("dispatcher: Error waiting for the event consumer thread\n");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_destroy(consumer->lock);
        pthread_cond_destroy(consumer->ready);

        free(consumer->lock);
        free(consumer->ready);
        free(consumer->worker);
        free(consumer);
        consumer = NULL;
    }
}

/* Notifies the event consumer that there are new events in the queue */
static void consumer_notify() {
    if (consumer != NULL) {
        pthread_cond_signal(consumer->ready);
    }
}


/* ******************** */
/* Dispatcher functions */
/* ******************** */

void dsp_start() {
    q_create();         /* Create the dispatcher events queue */
    consumer_create();  /* Create the events consumer */
}

void dsp_shutdown() {
    consumer_destroy();     /* Terminate the consumer thread and free resources */
    q_destroy();            /* Free the event queue resources */
}

void dsp_dispatch(struct raw_event* event) {
    q_push(event);
}

/* **************** */
/* Event triggering */
/* **************** */

static void __circus__ping_handler(PingEvent* event) {
    irc_pong(event->server);
}

static void _fire_event(struct raw_event* raw) {
    CallbackPtr callback = NULL;
    upper(raw->type);

    /* Check if there is a concrete binding for the
     * incoming message type. */
    debug(("dispatcher: Looking for a binding for %s\n", raw->type));

    /* Connection registration */
    if (s_eq(raw->type, NICK)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            NickEvent event = evt_nick(raw);
            NickCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, QUIT)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            QuitEvent event = evt_quit(raw);
            QuitCallback(callback)(&event);
        }
    } /* Channel operations */
    else if (s_eq(raw->type, JOIN)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            JoinEvent event = evt_join(raw);
            JoinCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, PART)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            PartEvent event = evt_part(raw);
            PartCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, TOPIC)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            TopicEvent event = evt_topic(raw);
            TopicCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, RPL_NAMREPLY) || s_eq(raw->type, RPL_ENDOFNAMES)) {
        callback = bnd_lookup(NAMES);
        if (callback != NULL) {
            NamesEvent event = evt_names(raw);
            NamesCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, RPL_LIST) || s_eq(raw->type, RPL_LISTEND)) {
        callback = bnd_lookup(LIST);
        if (callback != NULL) {
            ListEvent event = evt_list(raw);
            ListCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, INVITE)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            InviteEvent event = evt_invite(raw);
            InviteCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, KICK)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            KickEvent event = evt_kick(raw);
            KickCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, PRIVMSG)) {
        /* Look for a command binding */
        char key[50];
        char* buffer, *command, *command_params;
        size_t lparam = strlen(raw->params[1]);

        /* The first parameter in PRIVMSG contains the whole message.
         * We need to consider only the first word */
        if ((buffer = malloc((lparam + 1) * sizeof(char))) == 0) {
            perror("Out of memory (fire_event)");
            exit(EXIT_FAILURE);
        }

        memset(buffer, '\0', lparam + 1);
        strncpy(buffer, raw->params[1], lparam);
        command = strtok_r(buffer, " ", &command_params);

        if (command != NULL) {
            build_command_key(key, command);
            debug(("dispatcher: Looking for command: %s\n", command));
            callback = bnd_lookup(key);
            if (callback != NULL) {
                /* Remove the command name from the raw message */
                raw->params[1] = command_params;
            }
        }

        /* If no command binding is found, look for an event binding */
        if (callback == NULL) {
            debug(("dispatcher: No command found. Looking for event.\n"));
            callback = bnd_lookup(raw->type);
        }

        if (callback != NULL) {
            MessageEvent event = evt_message(raw);
            MessageCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, MODE)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            ModeEvent event = evt_mode(raw);
            ModeCallback(callback)(&event);
        }
    } /* Miscellaneous events */
    else if (s_eq(raw->type, PING)) {
        PingEvent event = evt_ping(raw);
        __circus__ping_handler(&event);    /* Call the system callback for ping before calling the bindings */
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            PingCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, NOTICE)) {
        callback = bnd_lookup(raw->type);
        if (callback != NULL) {
            NoticeEvent event = evt_notice(raw);
            NoticeCallback(callback)(&event);
        }
    }

    /* If no specific callback is found, check if there is
     * a global binding defined to handle the incoming message. */

    if (callback == NULL) {
        if (is_error(raw->type)) {
            /* Look for a concrete error binding */
            callback = bnd_lookup(raw->type);
            /* If none is found, look for a generic error binding */
            if (callback == NULL) {
                callback = bnd_lookup(ERROR);
            }

            if (callback != NULL) {
                ErrorEvent event = evt_error(raw);
                ErrorCallback(callback)(&event);
            }
        } else {
            /* Look for a concrete message binding */
            callback = bnd_lookup(raw->type);
            /* If none is found, look for a generic message binding */
            if (callback == NULL) {
                callback = bnd_lookup(ALL);
            }

            if (callback != NULL) {
                GenericEvent event = evt_generic(raw);
                GenericCallback(callback)(&event);
            }
        }
    }

    #ifdef DEBUG
    if (callback == NULL) {
        debug(("dispatcher: Binding not found\n"));
    }
    #endif
}

