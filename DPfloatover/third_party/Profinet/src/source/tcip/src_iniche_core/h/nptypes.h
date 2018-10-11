/*
 * v3.1 February 9, 2012 h/nptypes.h (bool_t for SNMP bugfix)
 * v3.1 August 30, 2011 h/nptypes.h
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: nptypes.h
 *
 * Copyright  2000 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: INET
 *
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */
/* Portions Copyright 1990-1997 by NetPort Software. */
/* Portions Copyright 1986 by Carnegie Mellon */
/* Portions Copyright 1983 by the Massachusetts Institute of Technology  */


#ifndef NPTYPES_H
#define  NPTYPES_H   1


typedef  unsigned short unshort;
typedef  unsigned long ulong;
typedef  int bool_t; /* boolean */

/* now do it the way the CMU snmp code likes it: */
typedef   unsigned char u_char;
typedef   unsigned short u_short;
typedef   unsigned long u_long;

typedef  unsigned long ip_addr;

#if ((!defined _SYS_STDINT_H_) && (!defined _STDINT_H)) /* INICHE_TCIP_ITGR see AP01346673 */

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed long     int32_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;

#endif

#if ((defined _SYS_TYPES_H_) || (defined _TYPES_H))   /* INICHE_TCIP_ITGR see AP01382171: if adonis defines struct timeval prevent re-definition, see bsdsock.h */

#define BSD_TIMEVAL_T struct timeval

#endif



#ifdef NOT_USED

/* general stuff C code */
#ifndef  TRUE
#define  TRUE  -1
#endif

#ifndef  FALSE
#define  FALSE    0
#endif

#ifndef  NULL
#define  NULL  ((void*)0)
#endif

#endif   /* NOT_USED */


/* usefull macros: */
#ifdef TCIP_CFG_PNTEST
#ifndef min
#define  min(x,y)          ((x) <= (y) ? (x) : (y))
#endif
#ifndef max
#define  max(x,y)          ((x) >= (y) ? (x) : (y))
#endif
#endif

#endif   /* NPTYPES_H */



