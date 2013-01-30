/* -*-  Mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*- */
// $Header: /u/lili/cvsroot/OPRCoding/NetCode/util.h,v 1.2 2008/09/10 04:40:00 yzhang Exp $

#ifndef util_h
#define util_h

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

const double E = 2.71828182845904523536028747;
const double PI = 3.14159265358979323846264338;

typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

extern void message(const char* msg);
extern void message(const char* fmt, const char* msg);
extern void internal_error(const char* msg);
extern void internal_error(const char* fmt, const char* arg);

extern double current_time();

#endif
