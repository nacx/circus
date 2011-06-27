#ifndef __MINUNIT_H__
#define __MINUNIT_H__

/*
 * MinUnit testing framework
 * http://www.jera.com/techinfo/jtns/jtn002.html
 */

#define mu_assert(test, msg) do { if (!(test)) return msg; } while (0)
#define mu_run(test) do { char *msg = test(); tests_run++; if (msg) return msg; } while (0)
#define mu_suite(suite) do { char *msg = suite(); if (msg) return msg; } while (0)

extern int tests_run;

#endif
