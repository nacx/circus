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
 * Circus benchmark tool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <poll.h>
#include "../lib/binding.h"
#include "../lib/events.h"
#include "../lib/codes.h"
#include "../lib/dispatcher.h"
#include "../lib/listener.h"

long int evt_total = 0;
double avg_sec = 0.0;

void on_event(GenericEvent* event) {
    struct timeval current_time;
    long int avg;
    gettimeofday(&current_time, NULL);
    avg = (current_time.tv_sec - event->timestamp->tv_sec) * 1000000 + ((int) current_time.tv_usec - (int) event->timestamp->tv_usec);
    avg_sec = (avg_sec > 0 ? (avg_sec + avg) / (double) 2 : avg) / 1000000;
    evt_total++;
}

int main(int argc, char **argv) {
    long int i, evt_max, el_usec;
    struct timeval start, end;

    if (argc != 2) {
        printf("Usage: %s <num_events>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    evt_max = strtol(argv[1], NULL, 0);

    bnd_bind(ALL, (CallbackPtr) on_event);
    dsp_start();

    printf("Starting benchmark with %ld events\n", evt_max);
    gettimeofday(&start, NULL);

    for (i = 0; i < evt_max; i++) {
        char event[100] = ":nick!~user@server MODE #test +inm\r\n";
        lst_handle(event);
    }

    while (evt_total < evt_max) {
        poll(0, 0, 10);
    }

    gettimeofday(&end, NULL);
    el_usec = (end.tv_sec - start.tv_sec) * 1000000 + ((int) end.tv_usec - (int) start.tv_usec);
    
    dsp_shutdown();
    bnd_destroy();

    printf("  Run time (secs): %f\n",  el_usec / (double) 1000000);
    printf("  Processed events: %ld\n", evt_total);
    printf("  Process time per event (secs): %f\n", avg_sec);

    return 0;
}

