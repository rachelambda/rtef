#ifndef __GLOBAL_H
#define __GLOBAL_H
#include <stdarg.h>
#include <stdlib.h>

/*
   stolen from dylan araps k
   prints a message (printf formatting), file,
   function, and the line of code that the
   macro was triggered. Then exits with
   a status of 1.
 */
#define die(...) _m(":(", __FILE__, __func__, __LINE__, __VA_ARGS__),exit(1)

/*
   borrowed from dylan araps k
   identical to die, except it doesn't exit.
   usefull for debugging
 */
#define msg(...) _m("OK", __FILE__, __func__, __LINE__, __VA_ARGS__)

/*
   borrowed from dylan araps k
   used to print debug messages
   t is type of message
   f is the file the message arose from
   fu is the function the message arose from
   l is the line the message arose from
   fmt and .. are passed as args to printf
 */
void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...);

/*
   a printf prefixed by program name
 */ 
void plog(const char *fmt, ...);

#endif
