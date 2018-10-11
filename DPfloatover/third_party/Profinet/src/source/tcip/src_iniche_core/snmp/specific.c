/*
 * v3.1 August 30, 2011 snmp/specific.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: specific.c
 *
 * Copyright 2000-04 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES: get_num_variables(), get_variables
 *
 * PORTABLE: yes
 *
 * Contains API for accessing all OIDs. 
 * specfic.c is provided to access num_variables, variables[] of 
 * snmpvars.c via functions get_num_variables() and get_variables_arr(). 
 */

/* 
 * REMARKS: This file is to be included from SNMPVARS.C
 * This is an exception to the normal practice of 
 * including only header files within a C file. The variables[] of 
 * snmpvars.c is put in a FAR_DATA segement (as it wouldn't fit in a 
 * DATA segment with all other modules), and hence special treatment 
 * is needed to access these variables from other modules. Ideally 
 * there shouldn't be any problem. But we are living in a non-ideal 
 * world :-). When num_variables are accessed directly in SNMP.C, 
 * Microsoft compiler compiles and links fine. But the generated code 
 * doesn't contain segment information. Hence the binary doesn't 
 * work. With Borland compiler, we get a linker error which reads 
 * something like "fixup overflow at SNMP_TEXT:0333; target = 
 * _VARIABLES in library file ..\snmp\snmp.lib in module SNMP" Hence 
 * this file, specfic.c is provided to access num_variables, 
 * variables[] of snmpvars.c via functions get_num_variables() and 
 * get_variables_arr(). Borland compiler needs the HUGE declaration 
 * for the return type of get_variables_arr(), otherwise it doesn't 
 * uses current DS, instead of the DS where variables[] is located. 
 */

#ifndef SPECIFIC_C
#define  SPECIFIC_C

/* Microsoft compiler is not neat in handing int variables across 
 * object files in a LARGE model. So when num_variables is accessed 
 * in getStatPtr() in snmp.c, invalid data segment is used, and hence 
 * incorrect value gets picked up. (Same is true with integer 
 * pointers also). Hence this function. 
 */


/* FUNCTION: get_num_variables()
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

int get_num_variables()
{
   int   num=sizeof(variables)/sizeof(struct  variable);

   return num;
}

/* FUNCTION: get_variables_arr()
 * 
 * PARAM1: void
 *
 * RETURNS: 
 */

/* Borland compiler gives "fixup error" when variables[] is used in snmp.c
 * So, we will use get_variables_arr() to access variables[]. 
 */

#ifdef __BORLANDC__
struct variable * _loadds get_variables_arr(void)
#else
struct variable *  get_variables_arr(void)
#endif
{
   return   variables;
}

#endif   /*  SPECIFIC_C */

