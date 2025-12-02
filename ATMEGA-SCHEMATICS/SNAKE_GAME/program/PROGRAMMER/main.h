// mainprog.h
#ifndef MAINPROG_H
#define MAINPROG_H

// ---- Standard C / POSIX (portable) ----
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// If you use POSIX file ops (open/read/write/close), keep these:
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// ---- FTDI D2XX (vendor API) ----
// Put the FTDI SDK's header on your include path (e.g., ./include/ftd2xx.h)
#include "ftd2xx.h"

#endif // MAINPROG_H
