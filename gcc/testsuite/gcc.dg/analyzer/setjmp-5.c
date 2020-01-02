/* { dg-additional-options "-fdiagnostics-show-line-numbers -fdiagnostics-nn-line-numbers -fdiagnostics-path-format=inline-events -fdiagnostics-show-caret" } */

#include <setjmp.h>
#include <stddef.h>
#include "analyzer-decls.h"

static jmp_buf env;

static void inner (void)
{
  setjmp (env);
}

void outer (void)
{
  int i;

  inner ();

  longjmp (env, 42); /* { dg-warning "'longjmp' called after enclosing function of 'setjmp' has returned" } */
}

/* { dg-begin-multiline-output "" }
  'outer': events 1-2
    |
    |   NN | void outer (void)
    |      |      ^~~~~
    |      |      |
    |      |      (1) entry to 'outer'
    |......
    |   NN |   inner ();
    |      |   ~~~~~~~~
    |      |   |
    |      |   (2) calling 'inner' from 'outer'
    |
    +--> 'inner': event 3
           |
           |   NN | static void inner (void)
           |      |             ^~~~~
           |      |             |
           |      |             (3) entry to 'inner'
           |
         'inner': event 4
           |
           |   NN |   setjmp (env);
           |      |   ^~~~~~
           |      |   |
           |      |   (4) 'setjmp' called here
           |
    <------+
    |
  'outer': events 5-6
    |
    |   NN |   inner ();
    |      |   ^~~~~~~~
    |      |   |
    |      |   (5) returning to 'outer' from 'inner'
    |   NN | 
    |   NN |   longjmp (env, 42);
    |      |   ~~~~~~~~~~~~~~~~~
    |      |   |
    |      |   (6) here
    |
    { dg-end-multiline-output "" } */
