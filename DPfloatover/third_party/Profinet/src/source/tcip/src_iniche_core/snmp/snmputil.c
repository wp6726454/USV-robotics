/*
 * v3.1 August 30, 2011 snmp/snmputil.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmputil.c
 *
 * Copyright 2002-2004 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES: prep_snmp(), agent(), setcommstr(), sendtrap(), 
 * ROUTINES: sendalltraps(), snmp_stat(), snmp_nvset(), 
 * ROUTINES: snmp_nvram_init(), v1v2cinfo(), snmp_preview(),
 * ROUTINES: snmp_logtrap(), snmp_print_value(), snmp_logoid(), 
 *
 * PORTABLE: YES
 */

#include "snmpport.h"    /* lengths for nvparms SNMP strings */

#ifdef INCLUDE_SNMP
#include "snmp_var.h"    /* lengths for nvparms SNMP strings */
#include "asn1.h"    /* lengths for nvparms SNMP strings */

#ifdef IN_MENUS
#include "in_utils.h"
#include "menu.h"
extern struct menu_op snmpmenu[];
extern int    sendtrap(void * pio);
extern int    sendalltraps(void * pio);
extern int    v1v2cinfo(void * pio);
#ifdef MIB_COUNTERS
extern int    snmp_stat(void * pio);
#endif
#endif   /* IN_MENUS */

#ifdef INCLUDE_NVPARMS
#include "nvparms.h"
#include "ip.h"
struct snmp_nvparam snmp_nvparms;
extern struct nvparm_format snmp_format;
#endif   /* INCLUDE_NVPARMS */


#include "snmp_imp.h"

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
#ifdef SNMP_TEST_SP_TRAP /* test enterprise specific trap */
extern void snmp_test_sp_trap(int version);
#endif
#endif

#ifdef INCLUDE_SNMPV3 
#include "../snmpv3/v3main.h"
#endif

#include "syslog.h"


/* FUNCTION: prep_snmp()
 * 
 * PARAMS: NONE
 *
 * RETURNS: Error Code or 0 for OK
 */
int prep_snmp(void)
{
int e = 0;

#ifdef IN_MENUS
   /* install the SNMP Agent commands */
   e = install_menu(&snmpmenu[0]);
#endif   /* IN_MENUS */

#ifdef INCLUDE_NVPARMS
   e = install_nvformat(&snmp_format, nv_formats);
   if(e)
   {
      dprintf("unable to install SNMP NVPARMS, reconfigure nv_formats[]\n");
      dtrap();
   }
#endif   /* INCLUDE_NVPARMS */
   return e;
}

#ifdef IN_MENUS

struct menu_op snmpmenu[] = {
   "snmp",      stooges,     "SNMP Agent Commands",
#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
   "trap",      sendtrap,    "send a v1/v2 trap(use:trap <type> <snmpversion>)",
   "alltraps",  sendalltraps,"send all v1/v2 traps",
#ifdef INCLUDE_NVPARMS
   "v1v2cinfo", v1v2cinfo,   "info. about v1/v2c agent (communities, etc.)",
#endif
#endif
#ifdef MIB_COUNTERS
   "snmp",      snmp_stat,   "display SNMP MIB counters",
#endif
   NULL,        NULL,        NULL,
   };


#ifdef INCLUDE_SNMPV1
extern   int   agent_on;

/* FUNCTION: agent()
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

int
agent()
{

   /* toggle snmp agent */
   if (agent_on)
      agent_on = FALSE;
   else
      agent_on = TRUE;

   printf("snmp agent turned %s\n", agent_on?"on":"off");
   return (0);
}

char     commstr[COMMSIZE];   /* default community string */



/* FUNCTION: setcommstr()
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
setcommstr(void * pio)     /* set default community string */
{
   char *   cp =  nextarg( ((GEN_IO)pio)->inbuf );
   int   len;

   if (!*cp)
   {
      printf("Community is \"%s\"; to change it please specify new one.\n",
       commstr);
      return -1;
   }

   if ((len = stlen(cp)) >= COMMSIZE)
   {
      printf("community strings are limited to %d characters\n", COMMSIZE);
      return -1;
   } 
   memcpy(commstr, cp, len);
   *(commstr+len) = '\0';        /* NULL terminate... */

   printf("Set default community string to \"%s\"\n", commstr);
      return 0;
}
#endif   /* INCLUDE_SNMPV1 */


#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))


/* FUNCTION: sendtrap()
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */


int
sendtrap(void * pio)
{
   int version=0; /* SNMP_VERSION_1 has the value 0 */
   int type=1;    /* warmstart by default */
   char *   cp =  nextarg( ((GEN_IO)pio)->inbuf );

   if (*cp)  /* both arguments are optional */
   {
      type=atoi(cp);  /* 1st arg is type of trap */
      if (type < 0 || type > 6) /* sanity check */
         type = 1;   /* use warmstart */
   
      cp = nextarg(cp);    /* see if user specified version */
      if (*cp)
      {
         version=atoi(cp)-1;
         if (version > 1)  /* sanity check */
            version=0; /* use version 1 */
      }
   }

   ns_printf(pio,"usage: trap <type> <version>\n");
   ns_printf(pio,"  type=0..6,  version=1/2\n");

#ifdef SNMP_TEST_SP_TRAP
   if (type==6) /* send enterprise specific trap */
   {
      snmp_test_sp_trap(version);
      ns_printf(pio,"enterprise specific trap sent (type=%d,version=%d)\n", 
         type, version+1);
      return 0;
   }
#endif

   if (snmp_trap(type, 0, 0, NULL,version))
      ns_printf(pio,"trap send err (type=%d,version=%d)\n",type,version+1);
   else
      ns_printf(pio,"trap sent (type=%d,version=%d)\n",type,version+1);
   return 0;
}

/* FUNCTION: sendalltraps()
 * 
 * Send 7 traps for SNMPv1 and 7 traps for SNMPv2c
 *    SNMP_TRAP_COLDSTART       0
 *    SNMP_TRAP_WARMSTART       1
 *    SNMP_TRAP_LINKDOWN        2
 *    SNMP_TRAP_LINKUP          3
 *    SNMP_TRAP_AUTHFAIL        4
 *    SNMP_TRAP_EGPNEIGHBORLOSS 5
 *    SNMP_TRAP_ENTERPRISESPECIFIC  6
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */


int
sendalltraps(void * pio)
{
   int i;

   ns_printf(pio,"Send all generic traps for SNMPv1 and SNMPv2c.\n");

   for (i=0; i<6; i++)
      snmp_trap(i, 0, 0, NULL,0);   /* send generic SNMPv1 traps */
   for (i=0; i<6; i++)
      snmp_trap(i, 0, 0, NULL,1);   /* send generic SNMPv2c traps */

#ifdef SNMP_TEST_SP_TRAP
      snmp_test_sp_trap(0);
      snmp_test_sp_trap(1);
#endif

   return 0;
}
#endif   /* (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C)) */

#ifdef MIB_COUNTERS
#ifndef SYSOBJECTID
#include "../snmp/snmp_var.h"
#endif   /* SYSOBJECTID */


/* FUNCTION: snmp_stat()
 *
 * dump the SNMP MIB group from RFC-1213.
 * 
 * PARAM1: void * pio - output stream
 *
 * RETURNS: 
 */

int
snmp_stat(void * pio)
{
   ns_printf(pio,"This build supports: ");
#ifdef INCLUDE_SNMPV1
   ns_printf(pio,"SNMPv1 ");
#endif
#ifdef INCLUDE_SNMPV2C
   ns_printf(pio,"SNMPv2c ");
#endif
#ifdef INCLUDE_SNMPV3
   ns_printf(pio,"SNMPv3 ");
#endif
   /* Now display all the SNMP counters. 2 counters are displayed
    * per line and hence the formatting is done accordingly.
    * "%-10ld" says that "display value in 10 bytes, right justified".
    * The largest value that can be shown is 0xffffffff = 4294967295
    * (requires 10 digits).
    */
   ns_printf(pio,"\nThe SNMP statistics are as follows\n");
   ns_printf(pio,"snmpInPkts            : %-10ld  snmpOutPkts            : %ld\n",
    SnmpMib.snmpInPkts, SnmpMib.snmpOutPkts );
   ns_printf(pio,"snmpInBadVersions     : %-10ld  snmpInBadCommunityNames: %ld\n",
    SnmpMib.snmpInBadVersions, SnmpMib.snmpInBadCommunityNames );
   ns_printf(pio,"snmpInBadCommunityUses: %-10ld  snmpInASNParseErrs     : %ld\n",
    SnmpMib.snmpInBadCommunityUses, SnmpMib.snmpInASNParseErrs );
   ns_printf(pio,"snmpInTooBigs         : %-10ld  snmpInNoSuchNames      : %ld\n",
    SnmpMib.snmpInTooBigs, SnmpMib.snmpInNoSuchNames );
   ns_printf(pio,"snmpInBadValues       : %-10ld  snmpInReadOnlys        : %ld\n",
    SnmpMib.snmpInBadValues, SnmpMib.snmpInReadOnlys );
   ns_printf(pio,"snmpInGenErrs         : %-10ld  snmpInTotalReqVars     : %ld\n",
    SnmpMib.snmpInGenErrs, SnmpMib.snmpInTotalReqVars );
   ns_printf(pio,"snmpInTotalSetVars    : %-10ld  snmpInGetRequests      : %ld\n",
    SnmpMib.snmpInTotalSetVars, SnmpMib.snmpInGetRequests );
   ns_printf(pio,"snmpInGetNexts        : %-10ld  snmpInSetRequests      : %ld\n",
    SnmpMib.snmpInGetNexts, SnmpMib.snmpInSetRequests );
   ns_printf(pio,"snmpInGetResponses    : %-10ld  snmpInTraps            : %ld\n",
    SnmpMib.snmpInGetResponses, SnmpMib.snmpInTraps );
   ns_printf(pio,"snmpOutTooBigs        : %-10ld  snmpOutNoSuchNames     : %ld\n",
    SnmpMib.snmpOutTooBigs, SnmpMib.snmpOutNoSuchNames );
   ns_printf(pio,"snmpOutBadValues      : %-10ld  snmpOutGenErrs         : %ld\n",
    SnmpMib.snmpOutBadValues, SnmpMib.snmpOutGenErrs );
   ns_printf(pio,"snmpOutGetRequests    : %-10ld  snmpOutGetNexts        : %ld\n",
    SnmpMib.snmpOutGetRequests, SnmpMib.snmpOutGetNexts );
   ns_printf(pio,"snmpOutSetRequests    : %-10ld  snmpOutGetResponses    : %ld\n",
    SnmpMib.snmpOutSetRequests, SnmpMib.snmpOutGetResponses );
   ns_printf(pio,"snmpOutTraps          : %-10ld  snmpEnableAuthenTraps  : %ld\n",
    SnmpMib.snmpOutTraps, SnmpMib.snmpEnableAuthenTraps );


   return 0;
}



#endif   /* MIB_COUNTERS */
#endif   /* IN_MENUS */

#ifdef INCLUDE_NVPARMS
/* Please see nvparms.h and nvparms.c regarding the usage of
 * the following datatypes and functions.
 */

extern   char  sysContact[SYS_STRING_MAX];
extern   char  sysName[SYS_STRING_MAX];
extern   char  sysLocation[SYS_STRING_MAX];

struct nvparm_info snmp_nvformats[] = 
{
   {"SNMP Get Community: %s\n"         , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.getcomm[0]     , NULL, }, 
   {"SNMP Set Community: %s\n"         , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.setcomm[0]     , NULL, }, 
   {"SNMP sysContact: %s\n"            , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.sysContact[0]  , NULL, }, 
   {"SNMP sysName: %s\n"               , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.sysName[0]     , NULL, }, 
   {"SNMP sysLocation: %s\n"           , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.sysLocation[0] , NULL, }, 
   {"SNMP Trap target1: %s\n"          , NVIP46DEC , NVBND_NOOP  , \
    &snmp_nvparms.trapTarget1    , NULL, }, 
   {"SNMP Trap Community1: %s\n"       , NVSTRING, MAX_NVSTRING, \
    &snmp_nvparms.trapcomm1[0]   , NULL, }, 
};

#define NUMSNMP_FORMATS  \
        (sizeof(snmp_nvformats)/sizeof(struct nvparm_info))

/* FUNCTION: snmp_nvset()
 * 
 * PARAM1: NV_FILE * fp
 *
 * RETURNS: Silent return of 0 for OK
 */
int snmp_nvset(NV_FILE * fp)
{
   int i=0;

   /* SNMP agent setup: */
   nv_fprintf(fp, snmp_nvformats[i++].pattern, snmp_nvparms.getcomm);
   nv_fprintf(fp, snmp_nvformats[i++].pattern, snmp_nvparms.setcomm);
   nv_fprintf(fp, snmp_nvformats[i++].pattern, sysContact);
   nv_fprintf(fp, snmp_nvformats[i++].pattern, sysName);
   nv_fprintf(fp, snmp_nvformats[i++].pattern, sysLocation);
   nv_fprintf(fp, snmp_nvformats[i++].pattern, print46_addr(&snmp_nvparms.trapTarget1));
   nv_fprintf(fp, snmp_nvformats[i++].pattern, snmp_nvparms.trapcomm1);

   return 0;
}

struct nvparm_format snmp_format = 
{
   NUMSNMP_FORMATS,
   &snmp_nvformats[0],
   snmp_nvset,
   NULL
};


/* FUNCTION: snmp_nvram_init()
 * 
 * snmp_nvram_init() 
 * Use the values specified in NV file.
 * The values have already been read into snmp_nvparms struct.
 * Copy them to appropriate globals.
 *
 * PARAM1: -
 *
 * RETURNS: Returns 0 if OK, else non-zero. 
 */

int
snmp_nvram_init(void)
{
   /* following variables are common to all SNMP agents */
   if ( strlen(snmp_nvparms.sysContact) > 0 )
      strcpy(sysContact,snmp_nvparms.sysContact);
   if ( strlen(snmp_nvparms.sysLocation) > 0 )
      strcpy(sysLocation,snmp_nvparms.sysLocation);
   if ( strlen(snmp_nvparms.sysName) > 0 )
      strcpy(sysName,snmp_nvparms.sysName);

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))

   /* communities not needed for SNMPV3 */
   if ( strlen(snmp_nvparms.getcomm) > 0 )
      strcpy(communities[1].name,snmp_nvparms.getcomm);
   if ( strlen(snmp_nvparms.setcomm) > 0 )
      strcpy(communities[0].name,snmp_nvparms.setcomm);

#ifdef ENABLE_SNMP_TRAPS
   trap_targets[0].ipaddr = snmp_nvparms.trapTarget1;
   inet_setport(&trap_targets[0].ipaddr, SNMP_TRAP_PORT);
   strcpy(trap_targets[0].community, snmp_nvparms.trapcomm1 ) ;
   trap_targets[0].commLen = strlen(trap_targets[0].community) ;
#endif

#endif /* SNMPv1, SNMPv2c */

   return 0;
}

#ifdef IN_MENUS

/* FUNCTION: v1v2cinfo()
 * 
 * PARAM1: void * pio - output stream
 *
 * RETURNS: 
 */

int v1v2cinfo(void * pio)
{
int i = 0;

   /* SNMP agent setup: */
   ns_printf(pio, snmp_nvformats[i++].pattern, snmp_nvparms.getcomm);
   ns_printf(pio, snmp_nvformats[i++].pattern, snmp_nvparms.setcomm);
   ns_printf(pio, snmp_nvformats[i++].pattern, sysContact);
   ns_printf(pio, snmp_nvformats[i++].pattern, sysName);
   ns_printf(pio, snmp_nvformats[i++].pattern, sysLocation);
   ns_printf(pio, snmp_nvformats[i++].pattern, print46_addr(&snmp_nvparms.trapTarget1));
   ns_printf(pio, snmp_nvformats[i++].pattern, snmp_nvparms.trapcomm1);

   return 0;
}
#endif /* IN_MENUS */
#endif /* INCLUDE_NVPARMS */


/* FUNCTION: snmp_preview()
 * 
 * Generate preview for received SET Request.
 * Parse all the received varbinds into vbinds.
 * - Fill in vbinds->objs[] array
 * - Initialize vbinds->num to contain "number of varbinds parsed".
 *
 * Initializes vbinds with the new values parsed.
 * All earlier values would be overwritten.
 *
 * PARAM1: u_char * data               - IN - start of varbinds
 * PARAM2: unsigned length             - IN - length of PDU
 * PARAM3: struct SnmpObjects * vbinds - OUT - all received varbinds
 *
 * RETURNS: 
 * 0            - SUCCESS
 * <err code>   - on error  ( -1 for now )
 */

int snmp_preview(u_char *data, unsigned length, struct SnmpObjects *vbinds)
{
   int i=0;       /* number of varbinds parsed */
   unsigned tmp;
   struct SnmpObject *obj; 
   vbinds->num=0;  /* number of varbinds present in vbinds */
   while (length)
   {
      /* parse the name, value pair */
      obj = &(vbinds->objs[i]);
      obj->namelen = MAX_NAME_LEN;
      data = snmp_parse_var_op(data, obj->name, &obj->namelen, 
         &obj->valuetype, &obj->valuelen, &obj->value, &length);
      if (data == NULL)
         return -1;
      /* obj->value points to ASN encoded value
       * - skip past "type" and "len" fields, so that "value"
       * points to received value.
       */
      tmp = data - obj->value;
      obj->value = asn_parse_header(obj->value, &tmp, &obj->valuetype);
      if (obj->value == NULL)
         return -1;
      i++;
      /* test which verifies that SET command contains valid VB ammount */
      if((i >= SNMP_MAX_OIDS) && (length > 0))  
         return -1 ;
   }
   vbinds->num=i;  /* number of varbinds present in vbinds */
   return 0;
}

/* FUNCTION: put_oid()
 * 
 * put the ascii decimal digits representing the value 
 * of the oid into a buffer.
 *
 * PARAM1: char* buf
 * PARAM2: oid oidval
 *
 * RETURNS: A pointer to the next buffer space available after the oid.
 */

char *   
put_oid(char* buf, oid oidval)
{
   sprintf(buf,"%ld",(long)oidval);    /* format decimal digits into buf */
   while (*buf != 0) buf++;            /* find end of digit string */
      return(buf);                        /* return pointer to trailing null */
}

#define  OID_BUF_SIZE   128   /* max length of display line */
static char oidbuf[OID_BUF_SIZE];   /* display buffer */

/* FUNCTION: print_oid()
 * 
 * format an oid string into a buffer for display. 
 * Buffer is static, and so must be used or copied before
 * the next call is made.
 *
 * PARAM1:  oid * oidp - Pointer to OID array (IN)
 * PARAM2: int len - Length of array (IN)
 *
 * REMARKS: This function has been picked from InstantStation code
 * No changes done.
 *
 * RETURNS: A pointer to the display buffer. 
 */

char * 
print_oid( oid * oidp,  /* pointer to an array of type oid */
     int   len)  /* number of oids in array */
{
   char *   cp;

   cp = oidbuf;

   /* pretty up "2B"s at the front of list */
   if (*oidp == 0x2b)
   {
      MEMCPY(cp, "1.3.", 4);
      cp += 4;
      oidp++;
      len--;
   }

   while (len--)
   {
      cp = put_oid(cp, *oidp++);  /* format number into buffer */
      if (cp > &oidbuf[OID_BUF_SIZE - 6])  /* don't overwrite past buffer */
         break;
      if (len) *cp++ = '.'; /* add dots between digits */
         }
   *cp = 0;   /* NULL terminate string */
   return(oidbuf);
}

#ifdef INCLUDE_SNMPV3 
/* FUNCTION: v3_print_oid()
 * 
 * format an oid string into a buffer for display. 
 *
 * PARAM1: SNMP_OID p_oid - Pointer to the OID (IN)
 * PARAM2: char *str - Pointer to char array to store the converted OID (OUT)
 *
 * REMARKS: This function has been derived from print_oid() of
 * InstantStation print_oid() has been kept for downward compatibility.
 *
 * RETURNS: SUCCESS or Error code
 */

int
v3_print_oid(SNMP_OID p_oid, char * str)
{
   char *   cp;
   oid *oidp = p_oid->subid;
   int   len   =  p_oid->len;

   cp = str;

   /* pretty up "2B"s at the front of list */
   if (*oidp == 0x2b)
   {
      MEMCPY(cp, "1.3.", 4);
      cp += 4;
      oidp++;
      len--;
   }

   while (len--)
   {
      cp = put_oid(cp, *oidp++);    /* format number into buffer */
      if (cp > &oidbuf[MAX_OID_SIZE - 6]) /* don't overwrite past buffer */
         break;
      if (len) *cp++ = '.';   /* add dots between digits */
         }
   *cp = 0;    /* NULL terminate string */

   return SUCCESS;
}
#endif

#ifdef SNMP_SYSLOG

/* FUNCTION: snmp_logtrap()
 * Send syslog message for a trap.
 * 
 * PARAM1: int type      - type of trap being sent
 * PARAM1: char * msg    - extra descr for enterprise spec. traps
 *
 * RETURNS: 
 */

static char * snmptrapdescr[7] = {
   "ColdStart",          /* 0 */
   "WarmStart",          /* 1 */
   "LinkDown",           /* 2 */
   "LinkUp",             /* 3 */
   "AuthFail",           /* 4 */
   "EgpNeighborLoss",    /* 5 */
   "TrapVenSpec",        /* 6 */
};

int
snmp_logtrap(int type, char * msg)
{
   syslog(LOG_NOTICE|LOG_CONSOLE, "ALERT[%s] %s", snmptrapdescr[type], msg);
   return 0;
}


/* FUNCTION: snmp_print_value()
 * Print the value into a buffer and return the buffer. 
 * 
 * Buffer is static to this function.
 *
 * PARAM1: u_char * var_val          - ASN encoded value
 * PARAM1: u_char  var_val_type      - type of value
 *
 * RETURNS: 
 */

char * snmp_print_value(u_char * var_val, /* ASN.1 encoded value of variable to set */
   u_char   var_val_type)     /* ASN.1 type of var_val */
{
   /* set an abritrarily large buffer parm for asn1 parsing. 
    * We don't use the returned value for this anyway.
    */
   unsigned   asn_buf_size = 1000;     /*  */
   unsigned   set_var_size = _MAX_PATH;  /* bytes in field to set */
   long tmpvalue=0;

   static char datastr[_MAX_PATH];
   static char datatmp[_MAX_PATH];
   static oid  dataoid[_MAX_PATH];

   datastr[0]=0;  /* null terminate */

   if (!var_val)
      return datastr;

   switch (var_val_type)
   {
   case ASN_INTEGER:
   case COUNTER:
   case GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
   case TIMETICKS:
      if (!asn_parse_int(var_val, &asn_buf_size, &var_val_type, 
         &tmpvalue, sizeof(long)))
      {
         break;
      }
      sprintf(datastr,"%ld",tmpvalue) ;

      break;
   case ASN_BIT_STR:
      if (!asn_parse_bits(var_val, &asn_buf_size, &var_val_type, 
         (unsigned char *)datatmp, &set_var_size))
      {
         break;
      }
      strcpy(datastr,datatmp);
         break;
   case ASN_OCTET_STR:
   case IPADDRESS:
   case OPAQUE:
      if (!asn_parse_string(var_val, &asn_buf_size, &var_val_type, 
         (unsigned char *)datatmp, &set_var_size))
      {
         break;
      }
      *(datatmp + set_var_size) = '\0'; /* null terminate the strings */
      strcpy(datastr,datatmp);
      break;
   case ASN_OBJECT_ID:
      if (!asn_parse_objid(var_val, &asn_buf_size, &var_val_type, 
         dataoid, &set_var_size))
      {
         break;
      }
      strcpy(datastr, print_oid(dataoid, set_var_size));
      break;
   }

   return datastr;
}


/* FUNCTION: snmp_logoid()
 * Send syslog message for an OID.
 * 
 * PARAM1: int type      - type of trap being sent
 * PARAM1: char * msg    - extra descr for enterprise spec. traps
 *
 * RETURNS: 
 */

static char * snmperrcode[19] =
{
   "success" ,            /* V3_VB_NOERROR              */
   "tooBig" ,             /* V3_VB_TOOBIG               */
   "noSuchName" ,         /* V3_VB_NOSUCHNAME           */
   "badValue" ,           /* V3_VB_BADVALUE             */
   "readOnly" ,           /* V3_VB_READONLY             */
   "genErr" ,             /* V3_VB_GENERR               */
   "noAccess" ,           /* V3_VB_NOACCESS             */
   "wrongType" ,          /* V3_VB_WRONGTYPE            */
   "wrongLength" ,        /* V3_VB_WRONGLENGTH          */
   "wrongEncoding" ,      /* V3_VB_WRONGENCODING        */
   "wrongValue" ,         /* V3_VB_WRONGVALUE           */
   "noCreation" ,         /* V3_VB_NOCREATION           */
   "inconsistentValue" ,  /* V3_VB_INCONSISTENTVALUE    */
   "resourceUnavailable" ,/* V3_VB_RESOURCEUNAVAILABLE  */
   "commitFailed" ,       /* V3_VB_COMMITFAILED         */
   "undoFailed" ,         /* V3_VB_UNDOFAILED           */
   "authorizationError" , /* V3_VB_AUTHORIZATIONERROR   */
   "notWritable" ,        /* V3_VB_NOTWRITABLE          */
   "inconsistentName" ,   /* V3_VB_INCONSISTENTNAME     */
};


int
snmp_logoid(char *community, oid * var_name, unsigned var_name_len, 
   long err, u_char *var_val, u_char valuetype)
{
   static char buf[150];
   static char valuebuf[100];

   sprintf(valuebuf, "SET=%s(%d); %s", snmperrcode[err], err, 
      snmp_print_value(var_val, valuetype));
   sprintf(buf, "SNMP[%s.%s] %s", community, 
      print_oid(var_name, var_name_len), valuebuf);
   
   syslog(LOG_NOTICE|LOG_SECURITY, buf);
   return 0;
}

#endif /* SNMP_SYSLOG */


#endif   /* INCLUDE_SNMP */
