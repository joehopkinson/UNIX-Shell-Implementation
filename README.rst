UNIX Shell in C
================

- Simple shell commands
- exit command
- cd command
- Command sequences
- Pipes
- Command redirection
- Environment Variables

Created as part of the Operating Systems course at VU Amsterdam.

shell.c & shell.h is my code, the additional framework was provided.

The code received a 9.1/10

Compiled with ``make`` and tested against the test library with ``make check``

Example commands
================

.. code:: sh

   ## simple commands:
   ls
   sleep 5

.. code:: sh

   ## simple commands, with built-ins:
   mkdir t
   cd t
   /bin/pwd
   exit 42

.. code:: sh

   ## sequences:
   echo hello; echo world
   exit 0; echo fail

.. code:: sh

   ## pipes:
   ls | grep t
   ls | more
   ls | sleep 5
   sleep 5 | ls 
   ls /usr/lib | grep net | cut -d. -f1 | sort -u

.. code:: sh

   ## redirects:
   >dl1 ls /bin; <dl1 wc -l
   >dl2 ls /usr/bin; >>dl1 cat dl2 
   <dl2 wc -l; <dl1 wc -l
   >dl3 2>&1 find /var/. 

.. code:: sh

   ## environment variables
   set hello=world; env | grep hello # prints "hello=world"
   (set top=down); env | grep top # does not print "top=down"


.. raw:: pdf

   PageBreak
