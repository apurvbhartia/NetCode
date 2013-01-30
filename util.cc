/* -*-  Mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*- */
// $Header: /u/lili/cvsroot/OPRCoding/NetCode/util.cc,v 1.2 2008/09/10 04:40:00 yzhang Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

#include "util.h"

void message(const char* msg)
{
  message("%s", msg);
}

void message(const char* fmt, const char* arg)
{
  fprintf(stderr, fmt, arg);
  fprintf(stderr, "\n");
}

void internal_error(const char* msg)
{
  internal_error("%s", msg);
}

void internal_error(const char* fmt, const char* arg)
{
  fprintf(stderr, "internal error: ");
  fprintf(stderr, fmt, arg);
  fprintf(stderr, "\n");
  abort();
  exit(1);
}

double current_time()
{
  struct timeval tv;
  if ( gettimeofday(&tv, 0) < 0 )
    internal_error("gettimeofday failed in current_time()");
  
  return double(tv.tv_sec) + double(tv.tv_usec) / 1e6;
}


