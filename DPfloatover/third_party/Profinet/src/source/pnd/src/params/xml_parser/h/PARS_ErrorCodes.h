/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: PARS_ErrorCodes.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Macros and constants for the error handling.                             */
/*                                                                           */
/*****************************************************************************/

#ifndef     __PARS_ERROR_INLCUDED
#define     __PARS_ERROR_INLCUDED

PARS_BEGIN_EXTERN_C

/*----------------------------------------------------------------------------------------------------*\
|                                                                                                      |
|        Every Function which will take part to the Error Handling must obey some                      |
|        implementation guidelines:                                                                    |
|                                                                                                      |
|        - The Function must have the return type: "PARS_RESULT"                                       |
|                                                                                                      |
|        - Direct after the definition of the local variables must be a:                               |
|                                                                                                      |
|                RETURN_INIT(FACILITY, SCOPE);                                                         |
|                                                                                                      |
|                The scope is a name for the function and only used                                    |
|                                                                                                      |
|        - to bail out make a:                                                                         |
|                BAIL_OUT(ERROR_CODE, DESCRIPTION);                                                    |
|                                                                                                      |
|          or                                                                                          |
|                BAIL_OUT_ACTUAL_ERROR();                                                              |
|                                                                                                      |
|        - in the second case, a default Code must be set via:                                         |
|                                                                                                      |
|                ACTUAL_ERROR(ERROR_CODE);                                                             |
|                                                                                                      |
|        - At the end of the Function must be the Clean Up area:                                       |
|                                                                                                      |
|                LANDING_ZONE;                                                                         |
|                this will return the last Error Code set, when landing Zone is normally reached       |
|                (not due a BAIL_OUT), the code in the landing zone will not be prozessed.             |
|                you can use the Landing Zone to Clean Up, free memory etc.                            |
|                                                                                                      |
|                                                                                                      |
|            or                                                                                        |
|                                                                                                      |
|                LANDING_ZONE2(any return Value);                                                      |
|                this will return the given value;                                                     |
|                                                                                                      |
|                                                                                                      |
|        - End the Landing zone with:                                                                  |
|                END_LANDING_ZONE;                                                                     |
|                                                                                                      |
|        DESCRIPTION is a Descriptive string, which is printed out to the Debug Console.               |
|        If you don't want a Description, give an empty string ("")                                    |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|                                                                                                      |
|        Format of the Return Code   PARS_RESULT                                                       |
|                                                                                                      |
|        ITs a 32Bit Value:                                                                            |
|                                                                                                      |
|  /--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--\  |
|  |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16||15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|  |
|  |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|  |
|  |-----7-----|-----6-----|-----5-----|-----4-----||-----3-----|-----2-----|-----1-----|-----0-----|  |
|  | E| S| C| C| C| C| C| C| C| C| C| C| C| C| C| C|| F| F| F| F| F| F| F| F| F| F| F| F| F| F| F| F|  |
|  \--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--/  |
|                                                                                                      |
|        E == 1 : Error Condition                                                (  1 Bit )            |
|        S == 1 : Standard Error Code, or == 0 : Component Specific Error Code   (  1 Bit )            |
|        C ^=   : The Error Code                                                 ( 14 Bit )            |
|        F ^=   : Facility (Code Component)                                      ( 16 Bit )            |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*/
/* Macro Descriptions                                                              Macro Descriptions */
/*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|                                                                                                      |
|        Macro Description:                                                                            |
|                                                                                                      |                                                                                                      |
|        BAIL_OUT(X,Y)       Directly jumps to the Landing Zone, X is the Error Code, Y a descriptive  |
|                            String.                                                                   |
|                                                                                                      |
|        SET_ACTUAL_ERROR(X) Sets the current Error Code to X, hereby the facility                     |
|                            (Set by RETURN_INIT) is not overwritten.                                  |
|                                                                                                      |
|        RETURN_ACTUAL_ERROR This is the replacement the return statement. The actual Error Code will  |
|                            be returned.                                                              |
|                                                                                                      |
|        ACTUAL_ERROR        addresses the Current Error Variable, no mapping is done, so a            |
|                            Facility Code could be redefined.                                         |
|                                                                                                      |
|        CHECK_NULL_PARAMETER(X)     Checks whether X is PARS_NULL. If so, the current Error Code is   |
|                                    set toMWE_INVALID_PARAMETER, and the Descriptive String is set    |
|                                    to X.After that directly jumps to the Landing Zone.               |
|                                                                                                      |
|        CHECK_MEMORY(X)             Checks, whether X is PARS_NULL. If so, the Current Error Code is  |
|                                    set to MWE_MEMORY_ERROR and the Descriptive String is set to X.   |
|                                    After that directly jumps to the landing zone.                    |
|                                                                                                      |
|        BAIL_OUT_ON_FAIL(X,Y)       The Error Code (X) will be set to the actual error. If this is    |
|                                    a FAILED value the Macro bails out an print the debug Message     |
|                                    (Y) to the debug console.                                         |
|        BAIL_OUT_ON_NULL(X,Y,Z)     If X==PARS_NULL then set the Error Condition to Y, the Description|
|                                    to Z, than jumps directly to the landing zone.                    |
|                                                                                                      |
|                                                                                                      |
|        LANDING_ZONE        In the landing zone you can fulfill clean tasks when you bailed out.      |
|                            The landing zone is _NOT_ processed when reached normally (not due a      |
|                            BAIL_OUT).                                                                |
|                            the landing zone is between LANDING_ZONE and END_LANDING_ZONE.            |
|                            After processing the landing zone, the last Error is returned (normally   |
|                            the Error code set by BAIL_OUT)                                           |
|                                                                                                      |
|        END_LANDING_ZONE    Marks the end of the landing zone.                                        |
|                                                                                                      |
|        Error Code Processing Macros:                                                                 |
|                                                                                                      |
|        MWE_FAILED(X)           is PARS_TRUE, when X is a FAILED Code.                                |
|        MWE_SUCCEEDED(X)        is PARS_TRUE, when X is NOT a FAIELD Code.                            |
|                                                                                                      |
|        MWE_MASK_FACILITY(X)    Only the Facility of X is returned. (for comparison)                  |
|        MWE_MASK_ERROR(X)       Only the Error Code (including E, S see above) is returned.           |
|        MWE_GET_HTTP_CODE(X)    returns the HTTP Code of X (i.e. 500 for internal Server Errors)      |
|        MWE_COMPARE_ERROR(X,Y)  Results PARS_TRUE if the Error Codes (E,S,C) of the both Parameter    |
|                                (X,Y) are the same.                                                   |
|                                                                                                      |
|                                                                                                      |
|                                                                                                      |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/

#define MAX_TRACE_DESCRIPTION_LENGTH        80

/*----------------------------------------------------------------------------------------------------*\
|   Facility Codes                                                                    Facility Codes   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*/
/* Attention! Changes of facilities only by master stack crew!                                        */
/*            If changes were made here, please update PARS_TraceApplication.c Facility List,         */
/*            else false Facility information will be traced out!                                     */
/*----------------------------------------------------------------------------------------------------*/

/* Facilities for Modules which are part of the Master stack */
/* ONLY Project Management has to add Facilities */
#define     MWE_FAC_NO_FAC           0x00000000L         /* No Facility given */
#define     MWE_FAC_ECB              0x00000001L         /* From ECB */
#define     MWE_FAC_MCB              0x00000002L         /* From MCB */
#define     MWE_FAC_CORE             0x00000003L         /* Rest of Core */
#define     MWE_FAC_DEFAPP           0x00000004L         /* Default Application */
#define     MWE_FAC_AUTH             0x00000005L         /* Authentication Library */
#define     MWE_FAC_FILESYS          0x00000006L         /* File system */
#define     MWE_FAC_EVENT            0x00000007L         /* Event Service */
#define     MWE_FAC_LIST_SERVICE     0x00000008L         /* List Management */
#define     MWE_FAC_LOADER_SERVICE   0x00000009L         /* Loader Service */
#define     MWE_FAC_SOCKIF           0x0000000AL         /* Socket Service*/
#define     MWE_FAC_TIMER_SERVICE    0x0000000BL         /* Timer Service */
#define     MWE_FAC_XML_SERVICE      0x0000000CL         /* XML Parser */
#define     MWE_FAC_VARIF            0x0000000DL         /* Variable Interface */
#define     MWE_FAC_HTTPCLNT_SERVICE 0x0000000EL         /* HTTP Client Service Interface */
#define     MWE_FAC_MWDCOM           0x0000000FL         /* DCom Service */
#define     MWE_FAC_WEBSERVICE       0x00000010L         /* WebService Service */
#define     MWE_FAC_MEMORY           0x00000011L         /* Memory stuff */
#define     MWE_FAC_MEMPOOL          0x00000012L         /* Memory Pool */
#define     MWE_FAC_MWSL             0x00000013L         /* MWSL Scripting Engine */
#define     MWE_FAC_MWSL_APP         0x00000014L         /* MWSL Scripting Engine */
/* Facilities for Modules which are not part of the Master stack */
/* ONLY Project Management has to add Facilities */
#define     MWE_FAC_PDIAG            0x00000020L         /* PDiag */
#define     MWE_FAC_WEBSIMOTION      0x00000021L         /* Web Simotion Application */
#define     MWE_FAC_OPC              0x00000022L         /* OPC Applications */
#define     MWE_FAC_BROWSE           0x00000023L         /* Browse Service Interface */
#define     MWE_FAC_SUBSCRIBE        0x00000024L         /* Subscription Service */
#define     MWE_FAC_SEM              0x00000025L         /* SEM Security module */

#define     MWE_FAC_CSSA             0x00000030L         /* CSSA */

#define     MWE_FAC_PORTING_LAYER  0x00000040L         /* All function of the porting layer */

/* common Facilities (used by modules which have no own facility */
/* ONLY Project Management has to add Facilities */
#define     MWE_FAC_SERVICE          0x00000050L         /* Generic Service */
#define     MWE_FAC_APPLICATION      0x00000051L         /* Generic Application */
#define     MWE_FAC_FILTER           0x00000052L         /* Generic Filter  */
#define     MWE_FAC_UNKNOWN          0x00000053L         /* Generic Unknown Facility */
#define     MWE_FAC_SIMAPP           0x00000054L         /* Read Variables per URL */
#define     MWE_FAC_SIMULATOR        0x00000055L         /* Process Variables simulation */

#define     MWE_FAC_ADDRESSMOD       0x00000060L         /* Address Modeller */
#define     MWE_FAC_DATAACCESS       0x00000061L         /* Data Access Service */
#define     MWE_FAC_OMUTILS          0x00000062L         /* Object Manager Utilities */
#define     MWE_FAC_SUBSCRIPTION     0x00000063L         /* Subscription Service */
#define     MWE_FAC_UAS              0x00000064L         /* UA Support Service */

#define     MWE_FAC_LANG_FILTER      0x00000065L        /* Language Filter */
#define     MWE_FAC_CT               0x00000066L        /* Config tool */

#define     MWE_FAC_DAISY            0x00000070L         /* Base daisy communication service */
#define     MWE_FAC_UPNP             0x00000071L         /* uPnP services */
#define     MWE_FAC_CTRLPNT          0x00000072L         /* uPnP Controller Point */

#define     MWE_FAC_SMTP             0x00000073L         /* SMTP Protocol */
#define     MWE_FAC_USEREDITOR       0x00000100L         /* User editor */
#define     MWE_FAC_WEBGS            0x00000101L         /* Web Graphics Service */
#define     MWE_FAC_CONDITION_MONITORING 0x00000102L     /* Condition monitoring communication module */

#define     MWE_FAC_PROACTOR         0x00000103L         /* Proactor framework */
#define     MWE_FAC_SINUMERIK        0x00000104L         /* sinumerik stuff */
#define     MWE_FAC_ODBC             0x00000105L         /* ODBC Access */
#define     MWE_FAC_ANP              0x00000106L         /* ANP Server Modules */

#define     MWE_FAC_HM               0x00000107L         /* handle management */

/*----------------------------------------------------------------------------------------------------*\
|   Standard Errors                                                                  Standard Errors   |
\*----------------------------------------------------------------------------------------------------*/
/* First define Parts of the Error Codes, so we can assemble the Error Codes afterwards */

#define     MWE_ERROR_CONDITION         (0x80000000L)
#define     MWE_STANDARD_ERROR          (0x40000000L)
/*#define     MWE_MAKE_ERROR(X)           (X<<16) */

/******************************************************************************************************/
/*                                                                                                    */
/* Reservations for Error Codes:                                                                      */
/*                                                                                                    */
/* Reserved Error Codes for Core:       0x0000 - 0x009F                                               */
/* Reserved Error Codes for Variables:  0x00E0 - 0x00FF                                               */
/*                                                                                                    */
/* Upper 2 Bits of Error Codes (0x0100 - 0x03FF) are reserved!!!                                      */
/*                                                                                                    */
/******************************************************************************************************/

/******************************************************************************************************/
/* Reserved Error Codes for Core:       0x0000 - 0x009F                                               */
/******************************************************************************************************/

#define MWE_NO_ERROR            0x00000000L                          /* E=0, S=0, C=0x0000, F=0x0000, */
#define MWE_MAKE_ERROR(X,Y)     ( MWE_ERROR_CONDITION | MWE_STANDARD_ERROR | ( ( ( ( X-300 )*0x30 )+0x101+Y )<<16 ) )
#define MWE_MAKE_SUCCESS(X,Y)   ( MWE_STANDARD_ERROR  | ( ( ( ( X-100 )*0x100 )+0x101+Y )<<16 ) )
/* -------------------------------------------------------------------------------------------------- */
/* --- Error Code Mappings to HTTP Errors                                                         --- */
/* -------------------------------------------------------------------------------------------------- */
/* See RFC 2068 for details of HTTP Error Codes                                                       */

/* -------------------------------------------------------------------------------------------------- */
/* --- Error codes: 0x0000 - 0x000F reserved, not mapped to HTTP Errors.                          --- */
/* -------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------- */
/* --- Error codes: 0x0101 - 0x3940 , mapped to HTTP Errors (0x30 per HTTP Code)                      */
/* -------------------------------------------------------------------------------------------------- */

/* --- 300 ---------------------------------------------------------------------------------- 300 --- */
/* HTTP 300: 0x0101 - 0x0130 MWE_HTTP_MULTIPLE_CHOICES                  --- 300 --------------------- */
#define     MWE_HTTP_MULTIPLE_CHOICES                                   MWE_MAKE_ERROR(300, 0)
/* HTTP 301: 0x0131 - 0x0160 MWE_HTTP_MOVED_PERMANENTLY                 --- 301 --------------------- */
#define     MWE_HTTP_MOVED_PERMANENTLY                                  MWE_MAKE_ERROR(301, 0)
/* HTTP 302: 0x0161 - 0x0190 MWE_HTTP_MOVED_TEMPORARILY                 --- 302 --------------------- */
#define     MWE_HTTP_MOVED_TEMPORARILY                                  MWE_MAKE_ERROR(302, 0)
/* HTTP 303: 0x0191 - 0x01C0 MWE_HTTP_SEE_OTHER                         --- 303 --------------------- */
#define     MWE_HTTP_SEE_OTHER                                          MWE_MAKE_ERROR(303, 0)
/* HTTP 304: 0x01C1 - 0x01F0 MWE_HTTP_NOT_MODIFIED                      --- 304 --------------------- */
#define     MWE_HTTP_NOT_MODIFIED                                       MWE_MAKE_ERROR(304, 0)
/* HTTP 305: 0x01F1 - 0x0220 MWE_HTTP_USE_PROXY                         --- 305 --------------------- */
#define     MWE_HTTP_USE_PROXY                                          MWE_MAKE_ERROR(305, 0)
/* --- 400 ---------------------------------------------------------------------------------- 400 --- */
/* HTTP 400: 0x13C1 - 0x13F0 MWE_HTTP_BAD_REQUEST                       --- 400 --------------------- */
#define     MWE_HTTP_BAD_REQUEST                                        MWE_MAKE_ERROR(400, 0)
#define     MWE_DIVIDE_BY_ZERO                                          MWE_MAKE_ERROR(400, 1)
#define     MWE_INVALID_URL                                             MWE_MAKE_ERROR(400, 2)
#define     MWE_SYNTAX_ERROR                                            MWE_MAKE_ERROR(400, 3)
#define     MWE_METHOD_NOT_IMPLEMENTED                                  MWE_MAKE_ERROR(400, 4)
#define     MWE_DISCONNECTED                                            MWE_MAKE_ERROR(400, 5)
#define     MWE_GRACEFUL_SHUTDOWN                                       MWE_MAKE_ERROR(400, 6)
#define     MWE_MALFORMED_HTTP                                          MWE_MAKE_ERROR(400, 7)
#define     MWE_PREFILTER_OVERRULES                                     MWE_MAKE_ERROR(400, 9)
#define     MWE_SOAP_INVALID_REQUEST                                    MWE_MAKE_ERROR(400, 10)
#define     MWE_SOAP_UNKNOWN_FUNCTION                                   MWE_MAKE_ERROR(400, 11)
#define     MWE_SOAP_MUSTUNDERSTAND                                     MWE_MAKE_ERROR(400, 12)
#define     MWE_PIPELINED_REQUEST                                       MWE_MAKE_ERROR(400, 13)

/* HTTP 401: 0x13F1 - 0x1420 MWE_HTTP_UNAUTHORIZED                      --- 401 --------------------- */
#define     MWE_HTTP_UNAUTHORIZED                                       MWE_MAKE_ERROR(401, 0)
#define     MWE_AUTH_REQUIRED                                           MWE_MAKE_ERROR(401, 1)
#define     MWE_JUST_RESERVED_HANDLE                                    MWE_MAKE_ERROR(401, 2)

/* HTTP 402: 0x1421 - 0x1450 MWE_HTTP_PAYMENT_REQUIRED                  --- 402 --------------------- */
#define     MWE_HTTP_PAYMENT_REQUIRED                                   MWE_MAKE_ERROR(402, 0)
#define     MWE_NO_LICENSE                                              MWE_MAKE_ERROR(402, 1)
/* HTTP 403: 0x1451 - 0x1480 MWE_HTTP_FORBIDDEN                         --- 403 --------------------- */
#define     MWE_HTTP_FORBIDDEN                                          MWE_MAKE_ERROR(403, 0)
#define     MWE_SECURITY_ERROR                                          MWE_MAKE_ERROR(403, 1)
#define     MWE_READ_ONLY                                               MWE_MAKE_ERROR(403, 2)
#define     MWE_FORBIDDEN                                               MWE_MAKE_ERROR(403, 3)
#define     MWE_KEYBER_BADTAG                                           MWE_MAKE_ERROR(403, 4)
#define     MWE_KEYBER_BADLENGTH                                        MWE_MAKE_ERROR(403, 5)
#define     MWE_KEYBER_BADTIME                                          MWE_MAKE_ERROR(403, 6)

/* HTTP 404: 0x1481 - 0x14B0 MWE_HTTP_NOT_FOUND                         --- 404 --------------------- */
#define     MWE_HTTP_NOT_FOUND                                          MWE_MAKE_ERROR(404, 0)
#define     MWE_NOT_FOUND                                               MWE_MAKE_ERROR(404, 0)
#define     MWE_USER_NOT_FOUND                                          MWE_MAKE_ERROR(404, 1)
#define     MWE_COMPILED_FILE_NOT_FOUND                                 MWE_MAKE_ERROR(404, 2)

/* HTTP 405: 0x14B1 - 0x1510 MWE_HTTP_METHOD_NOT_ALLOWED                --- 405 --------------------- */
#define     MWE_HTTP_METHOD_NOT_ALLOWED                                 MWE_MAKE_ERROR(405, 0)
/* HTTP 406: 0x1511 - 0x1540 MWE_HTTP_NOT_ACCEPTABLE                    --- 406 --------------------- */
#define     MWE_HTTP_NOT_ACCEPTABLE                                     MWE_MAKE_ERROR(406, 0)
/* HTTP 407: 0x1541 - 0x1570 MWE_HTTP_PROXY_AUTHENTICATION_REQUIRED     --- 407 --------------------- */
#define     MWE_HTTP_PROXY_AUTHENTICATION_REQUIRED                      MWE_MAKE_ERROR(407, 0)
/* HTTP 408: 0x1571 - 0x15A0 MWE_HTTP_REQUEST_TIMEOUT                   --- 408 --------------------- */
#define     MWE_HTTP_REQUEST_TIMEOUT                                    MWE_MAKE_ERROR(408, 0)
/* HTTP 409: 0x15A1 - 0x15D0 MWE_HTTP_CONFLICT                          --- 409 --------------------- */
#define     MWE_HTTP_CONFLICT                                           MWE_MAKE_ERROR(409, 0)
/* HTTP 410: 0x15D1 - 0x1600 MWE_HTTP_GONE                              --- 410 --------------------- */
#define     MWE_HTTP_GONE                                               MWE_MAKE_ERROR(410, 0)
/* HTTP 411: 0x1601 - 0x1630 MWE_HTTP_LENGTH_REQUIRED                   --- 411 --------------------- */
#define     MWE_HTTP_LENGTH_REQUIRED                                    MWE_MAKE_ERROR(411, 0)
/* HTTP 412: 0x1631 - 0x1660 MWE_HTTP_PRECONDITION_FAILED               --- 412 --------------------- */
#define     MWE_HTTP_PRECONDITION_FAILED                                MWE_MAKE_ERROR(412, 0)
#define     MWE_DIRECTORY_NOT_EMPTY                                     MWE_MAKE_ERROR(412, 1)
#define     MWE_INVALID_COMPILED_FILE                                   MWE_MAKE_ERROR(412, 2)

/* HTTP 413: 0x1661 - 0x1690 MWE_HTTP_REQUEST_ENTITY_TOO_LARGE          --- 413 --------------------- */
#define     MWE_HTTP_REQUEST_ENTITY_TOO_LARGE                           MWE_MAKE_ERROR(413, 0)
#define     MWE_DISK_FULL                                               MWE_MAKE_ERROR(413, 1)
#define     MWE_WRITE_ERROR                                             MWE_MAKE_ERROR(413, 2)
#define     MWE_ALREADY_EXISTS                                          MWE_MAKE_ERROR(413, 3)

/* HTTP 414: 0x1691 - 0x16C0 MWE_HTTP_REQUEST_URI_TOO_LONG              --- 414 --------------------- */
#define     MWE_HTTP_REQUEST_URI_TOO_LONG                               MWE_MAKE_ERROR(414, 0)
/* HTTP 415: 0x16C1 - 0x16F0 MWE_HTTP_UNSUPPORTED_MEDIA_TYPE            --- 415 --------------------- */
#define     MWE_HTTP_UNSUPPORTED_MEDIA_TYPE                             MWE_MAKE_ERROR(415, 0)
/* HTTP 422: 0x16F1 - 0x1720 MWE_HTTP_UNPROCESSABLE_ENTITY              --- 422 --------------------- */
#define     MWE_HTTP_UNPROCESSABLE_ENTITY                               MWE_MAKE_ERROR(422, 0)
/* HTTP 423: 0x1721 - 0x1750 MWE_HTTP_LOCKED                            --- 422 --------------------- */
#define     MWE_HTTP_LOCKED                                             MWE_MAKE_ERROR(423, 0)
/* HTTP 424: 0x1751 - 0x1780 MWE_HTTP_FAILED_DEPENDENCY                 --- 422 --------------------- */
#define     MWE_HTTP_FAILED_DEPENDENCY                                  MWE_MAKE_ERROR(424, 0)

/* --- 500 ---------------------------------------------------------------------------------- 500 --- */
/* HTTP 500: 0x2681 - 0x26B0 MWE_HTTP_INTERNAL_SERVER_ERROR             --- 500 --------------------- */
#define     MWE_HTTP_INTERNAL_SERVER_ERROR                              MWE_MAKE_ERROR(500, 0)
#define     MWE_INTERNAL_ERROR                                          MWE_MAKE_ERROR(500, 1)
#define     MWE_INVALID_PARAMETER                                       MWE_MAKE_ERROR(500, 2)
#define     MWE_UNEXPECTED                                              MWE_MAKE_ERROR(500, 3)
#define     MWE_CONNECTION_CLOSED                                       MWE_MAKE_ERROR(500, 4)
#define     MWE_SOCKET_ERROR                                            MWE_MAKE_ERROR(500, 5)
#define     MWE_MEMORY_ERROR                                            MWE_MAKE_ERROR(500, 6)
#define     MWE_ASYNC_IO_ERROR                                          MWE_MAKE_ERROR(500, 7)
#define     MWE_READ_ERROR                                              MWE_MAKE_ERROR(500, 8)
#define     MWE_SERVICE_IN_USE                                          MWE_MAKE_ERROR(500, 9)
#define     MWE_SHUTTING_DOWN                                           MWE_MAKE_ERROR(500, 10)
#define     MWE_BUFFER_TOO_SMALL                                        MWE_MAKE_ERROR(500, 11)
#define     MWE_FAIL                                                    MWE_MAKE_ERROR(500, 12)
#define     MWE_UNEXPECTED_READ_EVENT                                   MWE_MAKE_ERROR(500, 13)
#define     MWE_CANCELED                                                MWE_MAKE_ERROR(500, 14)

/* HTTP 501: 0x26B1 - 0x26E0 MWE_HTTP_NOT_IMPLEMENTED                   --- 501 --------------------- */
#define     MWE_HTTP_NOT_IMPLEMENTED                                    MWE_MAKE_ERROR(501, 0)
#define     MWE_NOT_IMPLEMENTED                                         MWE_MAKE_ERROR(501, 1)
/* HTTP 502: 0x26E1 - 0x2710 MWE_HTTP_BAD_GATEWAY                       --- 502 --------------------- */
#define     MWE_HTTP_BAD_GATEWAY                                        MWE_MAKE_ERROR(502, 0)
/* HTTP 503: 0x2711 - 0x2740 MWE_HTTP_SERVICE_UNAVAILABLE               --- 503 --------------------- */
#define     MWE_HTTP_SERVICE_UNAVAILABLE                                MWE_MAKE_ERROR(503, 0)
#define     MWE_SERVER_OVERLOADED                                       MWE_MAKE_ERROR(503, 1)
/* HTTP 504: 0x2741 - 0x2770 MWE_HTTP_GATEWAY_TIMEOUT                   --- 504 --------------------- */
#define     MWE_HTTP_GATEWAY_TIMEOUT                                    MWE_MAKE_ERROR(504, 0)
/* HTTP 505: 0x2771 - 0x27A0 MWE_HTTP_VERSION_NOT_SUPPORTED             --- 505 --------------------- */
#define     MWE_HTTP_VERSION_NOT_SUPPORTED                              MWE_MAKE_ERROR(505, 0)
#define     MWE_VERSION_NOT_SUPPORTED                                   MWE_MAKE_ERROR(505, 1)

/* -------------------------------------------------------------------------------------------------- */
/* --- Errorcodes: 0x2861 - 0x2890 variable handling and opc.                                     --- */
/* -------------------------------------------------------------------------------------------------- */

/* HTTP 505: 0x2861 - 0x2890 MWE_HTTP_VERSION_NOT_SUPPORTED             --- 510 --------------------- */
/* general variable error. (should not be used!)*/
#define     MWE_VARIABLE_FAIL                                           MWE_MAKE_ERROR(510, 0)
/*The item name doesn't conform the server's syntax."*/
#define     MWE_INVALIDITEMID                                           MWE_MAKE_ERROR(510, 1)
/*The item path doesn't conform the server's syntax."*/
#define     MWE_INVALIDITEMPATH                                         MWE_MAKE_ERROR(510, 2)
/*The server does not recognize the passed property ID."*/
#define     MWE_INVALIDPID                                              MWE_MAKE_ERROR(510, 3)
/*The value is out of range."*/
#define     MWE_RANGE                                                   MWE_MAKE_ERROR(510, 4)
/*The operation timed out."*/
#define     MWE_TIMEDOUT                                                MWE_MAKE_ERROR(510, 5)
/**
 *  The define MWE_UNKNOWNITEMID is used for two purposes:
 *  1.) A provider may return this value if it should deliver the value for a variable
 *      that he doesn't know.
 *  2.) This value is the initial value used in the transport struct to indicate that no
 *      one has already touched the associated variable. This way, a provider can decide
 *      if it has a look at the variable (status is MWE_UNKNOWNITEMID), or if it can skip
 *      the attribute (status is not MWE_UNKNOWNITEMID).
 */
#define     MWE_UNKNOWNITEMID                                           MWE_MAKE_ERROR(510, 6)
/*The item's access path is not known to the server."*/
#define     MWE_UNKNOWNITEMPATH                                         MWE_MAKE_ERROR(510, 7)
/*The Item is write only and cannot be read or returned in a Write response."*/
#define     MWE_WRITEONLY                                               MWE_MAKE_ERROR(510, 8)
/*Server does not support writing quality / timestamp."*/
#define     MWE_NOTSUPPORTED                                            MWE_MAKE_ERROR(510, 9)
/*The subscription is not known for the specified handle.*/
#define     MWE_NOSUBSCRIPTION                                          MWE_MAKE_ERROR(510, 10)
/*The Item is read only and cannot be written to.*/
#define     MWE_READONLY                                                MWE_MAKE_ERROR(510, 11)
/*The server cannot convert the data between the requested data type and the canonical data type."*/
#define     MWE_BADTYPE                                                 MWE_MAKE_ERROR(510, 12)
/*The filter string was not valid."*/
#define     MWE_INVALIDFILTER                                           MWE_MAKE_ERROR(510, 13)
/*The server is currently processing another SubscriptionPolledRefresh from that client."*/
#define     MWE_BUSY                                                    MWE_MAKE_ERROR(510, 14)
/*The continuation point (browse) is not valid."*/
#define     MWE_INVALIDCONTINUATIONPOINT                                MWE_MAKE_ERROR(510, 15)
/*The time delay until the specified holdtime is too long (server specific)."*/
#define     MWE_INVALIDHOLDTIME                                         MWE_MAKE_ERROR(510, 16)
/*No item list has been passed in the request."*/
#define     MWE_NOITEMLIST                                              MWE_MAKE_ERROR(510, 17)
/*The operation failed, because the server is in the wrong state."*/
#define     MWE_SERVERSTATE                                             MWE_MAKE_ERROR(510, 18)
/*The operation failed, because the server is in the wrong state."*/
#define     MWE_OBSOLETE_PROVIDER                                       MWE_MAKE_ERROR(510, 19)
/*The value is time stamped older than the requested maximum age. "*/
#define     MWE_STALEVALUE                                              MWE_MAKE_ERROR(510, 20)
/*Error in writing the quality for an item."*/
#define     MWE_Q_OUTOFRANGE                                            MWE_MAKE_ERROR(510, 21)
/*Error in writing the timestamp for an item."*/
#define     MWE_T_OUTOFRANGE                                            MWE_MAKE_ERROR(510, 22)
/*Error in writing the 'Bits' for an item."*/
#define     MWE_B_OUTOFRANGE                                            MWE_MAKE_ERROR(510, 23)
/* The operation failed, because the provider is in the wrong state */
#define     MWE_PROVIDERSTATE                                           MWE_MAKE_ERROR(510, 24)
/* The operation failed because the passed group handle was expired */
#define     MWE_INVALIDGROUP                                            MWE_MAKE_ERROR(510, 26)
/* The operation failed because the passed browse direction is invalid */
#define     MWE_INVALIDBROWSEDIRECTION                                  MWE_MAKE_ERROR(510, 27)
/* The operation failed because the passed view ID is invalid */
#define     MWE_UNKNOWNVIEWID                                           MWE_MAKE_ERROR(510, 28)
/* The operation failed because the passed reference type ID is invalid */
#define     MWE_INVALIDREFTYPEID                                        MWE_MAKE_ERROR(510, 29)
/* The operation failed because an invalid browsename was presented */
#define     MWE_INVALID_BROWSENAME                                      MWE_MAKE_ERROR(510, 30)
/** The requested operation has no match to return */
#define     MWE_NO_MATCH                                                MWE_MAKE_ERROR(510, 31)

/* -------------------------------------------------------------------------------------------------- */
/* --- Error codes: 0x3941 - 0x3FFF reserved Error codes.                                         --- */
/* -------------------------------------------------------------------------------------------------- */

/*----------------------------------------------------------------------------------------------------*\
|   Standard Successful Return Codes                                Standard Successful Return Codes   |
\*----------------------------------------------------------------------------------------------------*/

/* --- 200 ---------------------------------------------------------------------------------- 200 --- */
/* HTTP 200: 0x0101 - 0x0130 MWE_HTTP_OK                                --- 200 --------------------- */
#define     MWS_ASYNC_IO_COMPLETE                                       MWE_MAKE_SUCCESS(200, 1)
#define     MWS_ASYNC_IO_AGAIN                                          MWE_MAKE_SUCCESS(200, 2)
#define     MWS_ASYNC_IO_PENDING                                        MWE_MAKE_SUCCESS(200, 3)
#define     MWS_CLEANUP_COMPLETE                                        MWE_MAKE_SUCCESS(200, 4)
#define     MWS_CLEANUP_AGAIN                                           MWE_MAKE_SUCCESS(200, 5)
#define     MWS_NEW_TIMEOUT                                             MWE_MAKE_SUCCESS(200, 6)
#define     MWS_VALID_LICENSE                                           MWE_MAKE_SUCCESS(200, 7)
#define     MWS_SOCKET_EVENT_OCCURED                                    MWE_MAKE_SUCCESS(200, 8)
#define     MWS_SOCKET_TIMEOUT                                          MWE_MAKE_SUCCESS(200, 9)
#define     MWS_SOCKET_CONNECT_BLOCK                                    MWE_MAKE_SUCCESS(200, 10)
#define     MWS_TRUNCATED                                               MWE_MAKE_SUCCESS(200, 11)
#define     MWS_LAST_ELEMENT                                            MWE_MAKE_SUCCESS(200, 12)
#define     MWS_FILE_IS_EXTERNAL                                        MWE_MAKE_SUCCESS(200, 13)
#define     MWS_MODULE_ALREADY_LOADED                                   MWE_MAKE_SUCCESS(200, 14)
#define     MWS_VALID_USER_HANDLE                                       MWE_MAKE_SUCCESS(200, 15)
#define     MWS_DIED                                                    MWE_MAKE_SUCCESS(200, 16)

/*----------------------------------------------------------------------------------------------------*\
|   Success Codes for Variables                                      Success Codes for Variables       |
\*----------------------------------------------------------------------------------------------------*/
/*A value passed to WRITE was accepted but the output was clamped."*/
#define     MWS_CLAMP                                                   MWE_MAKE_SUCCESS(200, 20)
/*The server does not support the requested data rate but will use the closest available rate."*/
#define     MWS_UNSUPPORTEDRATE                                         MWE_MAKE_SUCCESS(200, 21)
/*Not every detected change has been returned since the server's buffer reached its limit and had to purge our the oldest data */
#define     MWS_DATAQUEUEOVERFLOW                                       MWE_MAKE_SUCCESS(200, 22)
/*The operation completed partially*/
#define     MWS_FALSE                                                   MWE_MAKE_SUCCESS(200, 23)
/* For SSL, Application close of the connection identified, signal end of SSL connection */
#define     MWS_PREMATURELY_CLOSED_CONNECTION                           MWE_MAKE_SUCCESS(200, 24)
/*Error in writing the 'Bits' for an item."*/
#define     MWE_S_DONTTOUCH                                             MWE_MAKE_SUCCESS(200, 25)
#define     MWE_S_EVENTUPDATE                                           MWE_MAKE_SUCCESS(200, 26)
#define     MWE_S_NEVERTOUCH                                            MWE_MAKE_SUCCESS(200, 27)

/*----------------------------------------------------------------------------------------------------*\
|   Module Specific Errors                                                    Module Specific Errors   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|   Filter Management                                                              Filter Management   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_FILTER_NEXT_FILTER                                      MWE_MAKE_SUCCESS(200, 30)
#define     MWS_FILTER_RETURN_DONE                                      MWE_MAKE_SUCCESS(200, 31)
#define     MWS_APPLICATION_NEXT_APP                                    MWE_MAKE_SUCCESS(200, 32)
#define     MWS_APPLICATION_DONE                                        MWE_MAKE_SUCCESS(200, 33)
#define     MWS_NOT_TRACED                                              MWE_MAKE_SUCCESS(200, 34)

/*----------------------------------------------------------------------------------------------------*\
|   Conversion Service                                                            Conversion Service   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_CHAR_TRUNCATED_TO_ASCII                                 MWE_MAKE_SUCCESS(200, 40)

/*----------------------------------------------------------------------------------------------------*\
|   HTTPClient Service                                                            HTTPClient Service   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_NO_CONTENT_LENGTH_SPECIFIED                             MWE_MAKE_SUCCESS(200, 50)

/*----------------------------------------------------------------------------------------------------*\
|   ObjectManager                                                                      ObjectManager   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_DEFAULT_USED                                            MWE_MAKE_SUCCESS(200, 60)
#define     MWS_ALREADY_EXISTING                                        MWE_MAKE_SUCCESS(200, 61)
#define     MWS_CREATED_NEW                                             MWE_MAKE_SUCCESS(200, 62)
#define     MWS_REFERENCES_LEFT                                         MWE_MAKE_SUCCESS(200, 63)
#define     MWS_NO_MORE_REFERENCES                                      MWE_MAKE_SUCCESS(200, 64)
/** a specified provider id was successfully applied to a node id */
#define     MWS_PROVIDERID_TAKEN                                        MWE_MAKE_SUCCESS(200, 65)
/** a specified provider id was not applied to a node id because a more qualified one did already exist */
#define     MWS_PROVIDERID_REJECTED                                     MWE_MAKE_SUCCESS(200, 66)

/*----------------------------------------------------------------------------------------------------*\
|   DAV                                                                                          DAV   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_HTTP_CREATED                                            MWE_MAKE_SUCCESS(201, 1)
#define     MWS_HTTP_NO_CONTENT                                         MWE_MAKE_SUCCESS(204, 1)
#define     MWS_HTTP_MULTISTATUS                                        MWE_MAKE_SUCCESS(207, 1)

/*----------------------------------------------------------------------------------------------------*\
|   SUBS                                                                                        SUBS   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_SUBS_KEEPALIVE                                          MWE_MAKE_SUCCESS(200, 70)
#define     MWS_SUBS_NOTTOUCHED                                         MWE_MAKE_SUCCESS(200, 71)

/*----------------------------------------------------------------------------------------------------*\
|   ODBC Management                                                                  ODBC Management   |
\*----------------------------------------------------------------------------------------------------*/
#define     MWS_SQL_NEED_DATA                                           MWE_MAKE_SUCCESS(200, 72)
#define     MWS_SQL_SUCC_INFO                                           MWE_MAKE_SUCCESS(200, 73)
#define     MWS_SQL_SUCCEED                                             MWE_MAKE_SUCCESS(200, 74)
#define     MWE_SQL_FAILED                                              MWE_MAKE_ERROR(500, 15)
#define     MWE_SQL_INV_HANDLE                                          MWE_MAKE_ERROR(500, 16)
#define     MWE_SQL_NO_DATA                                             MWE_MAKE_ERROR(500, 17)
#define     MWE_SQL_EXECUTING                                           MWE_MAKE_ERROR(500, 18)

/*----------------------------------------------------------------------------------------------------*\
|   Access Macros                                                                      Access Macros   |
\*----------------------------------------------------------------------------------------------------*/

#define     MWE_FAILED(x)                ((x&0x80000000L)!=0)     /* PARS_TRUE if Code has failed condition */
#define     MWE_SUCCEEDED(x)             ((x&0x80000000L)==0)     /* PARS_TRUE if Code has success condition */

#define     MWE_MASK_FACILITY(x)         (x&0x0000FFFFL)        /* Extracts the facility Code */
#define     MWE_MASK_ERROR(x)            (x&0xFFFF0000L)
#define     MWE_MASK_CODE(x)             (x&0xFFFF0000L)
#define     MWE_GET_ERROR(x)             ((x&0xFFFF0000L)>>16)
#define     MWE_GET_TRACE_LEVEL(x)       ((x&0x07FF0000L)>>16)
#define     MWE_SET_TRACE_LEVEL(x)       ((x&0x0000FFFFL)<<16)
#define     MWE_SET_FACILITY_TL(x)       ((x&0x0000FFFFL)<<16)

#define     MWE_COMPARE_ERROR(X,Y)       ((MWE_MASK_ERROR(X))==(MWE_MASK_ERROR(Y)))

/*----------------------------------------------------------------------------------------------------*\
|   Support Macros                                                                    Support Macros   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|   TRACE OUTPUT                                                                        TRACE OUTPUT   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_DEBUG_OUT
 #define PARS_TRACE(TL, DESCRIPTION)                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION)
 #define PARS_TRACE_0(TL, DESCRIPTION)                      if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION)
 #define PARS_TRACE_1(TL, DESCRIPTION, P1)                  if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, P1)
 #define PARS_TRACE_2(TL, DESCRIPTION, P1, P2)              if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, P1, P2)
 #define PARS_TRACE_3(TL, DESCRIPTION, P1, P2, P3)          if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, P1, P2, P3)
 #define PARS_TRACE_4(TL, DESCRIPTION, P1, P2, P3, P4)      if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, P1, P2, P3, P4)
 #define PARS_TRACE_5(TL, DESCRIPTION, P1, P2, P3, P4, P5)  if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, P1, P2, P3, P4, P5)
 #define PARS_TRACE_BUFFER(TL, DESCRIPTION, DESCRIPTIONLEN) if (PARS_g_pCoreContext) PARS_g_pCoreContext->TraceBuffer( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto), DESCRIPTION, DESCRIPTIONLEN)

 #define PARS_PORT_TRACE(TL, DESCRIPTION)                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION)
 #define PARS_PORT_TRACE_0(TL, DESCRIPTION)                      if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION)
 #define PARS_PORT_TRACE_1(TL, DESCRIPTION, P1)                  if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION, P1)
 #define PARS_PORT_TRACE_2(TL, DESCRIPTION, P1, P2)              if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION, P1, P2)
 #define PARS_PORT_TRACE_3(TL, DESCRIPTION, P1, P2, P3)          if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION, P1, P2, P3)
 #define PARS_PORT_TRACE_4(TL, DESCRIPTION, P1, P2, P3, P4)      if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION, P1, P2, P3, P4)
 #define PARS_PORT_TRACE_5(TL, DESCRIPTION, P1, P2, P3, P4, P5)  if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(MWE_FAC_PORTING_LAYER), DESCRIPTION, P1, P2, P3, P4, P5)
#else /* PARS_DEBUG_OUT */
 #define PARS_TRACE(TL, DESCRIPTION)
 #define PARS_TRACE_0(TL, DESCRIPTION)
 #define PARS_TRACE_1(TL, DESCRIPTION, P1)
 #define PARS_TRACE_2(TL, DESCRIPTION, P1, P2)
 #define PARS_TRACE_3(TL, DESCRIPTION, P1, P2, P3)
 #define PARS_TRACE_4(TL, DESCRIPTION, P1, P2, P3, P4)
 #define PARS_TRACE_5(TL, DESCRIPTION, P1, P2, P3, P4, P5)
 #define PARS_TRACE_BUFFER(TL, DESCRIPTION, DESCRIPTIONLEN)

 #define PARS_PORT_TRACE(TL, DESCRIPTION)
 #define PARS_PORT_TRACE_0(TL, DESCRIPTION)
 #define PARS_PORT_TRACE_1(TL, DESCRIPTION, P1)
 #define PARS_PORT_TRACE_2(TL, DESCRIPTION, P1, P2)
 #define PARS_PORT_TRACE_3(TL, DESCRIPTION, P1, P2, P3)
 #define PARS_PORT_TRACE_4(TL, DESCRIPTION, P1, P2, P3, P4)
 #define PARS_PORT_TRACE_5(TL, DESCRIPTION, P1, P2, P3, P4, P5)

#endif /* PARS_DEBUG_OUT */

#ifdef _DEBUG
  #define PARS_ASSERT(EXPRESSION,DESCRIPTION) if(!(EXPRESSION)) { PARS_TRACE_0(PARS_DEBUG_LEVEL_INFO, DESCRIPTION);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
  #define PARS_ASSERT_1(EXPRESSION,DESCRIPTION,P1) if(!(EXPRESSION)) { PARS_TRACE_1(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
  #define PARS_ASSERT_2(EXPRESSION,DESCRIPTION,P1,P2) if(!(EXPRESSION)) { PARS_TRACE_2(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
  #define PARS_ASSERT_3(EXPRESSION,DESCRIPTION,P1,P2,P3) if(!(EXPRESSION)) { PARS_TRACE_3(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
  #define PARS_ASSERT_4(EXPRESSION,DESCRIPTION,P1,P2,P3,P4) if(!(EXPRESSION)) { PARS_TRACE_4(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3, P4);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
  #define PARS_ASSERT_5(EXPRESSION,DESCRIPTION,P1,P2,P3,P4,P5) if(!(EXPRESSION)) { PARS_TRACE_5(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3, P4, P5);PARS_DebugBreak(PARS_DEBUG_LEVEL_WARNING);}
#else /* _DEBUG */
#  if PARS_DEBUG_OUT
    #define PARS_ASSERT(EXPRESSION,DESCRIPTION) if(!(EXPRESSION)) PARS_TRACE_0(PARS_DEBUG_LEVEL_INFO, DESCRIPTION);
    #define PARS_ASSERT_1(EXPRESSION,DESCRIPTION,P1) if(!(EXPRESSION)) PARS_TRACE_1(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1);
    #define PARS_ASSERT_2(EXPRESSION,DESCRIPTION,P1,P2) if(!(EXPRESSION)) PARS_TRACE_2(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2);
    #define PARS_ASSERT_3(EXPRESSION,DESCRIPTION,P1,P2,P3) if(!(EXPRESSION)) PARS_TRACE_3(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3);
    #define PARS_ASSERT_4(EXPRESSION,DESCRIPTION,P1,P2,P3,P4) if(!(EXPRESSION)) PARS_TRACE_4(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3, P4);
    #define PARS_ASSERT_5(EXPRESSION,DESCRIPTION,P1,P2,P3,P4,P5) if(!(EXPRESSION)) PARS_TRACE_5(PARS_DEBUG_LEVEL_INFO, DESCRIPTION, P1, P2, P3, P4, P5);
#  else /* PARS_DEBUG_OUT */
    #define PARS_ASSERT(EXPRESSION,DESCRIPTION)
    #define PARS_ASSERT_1(EXPRESSION,DESCRIPTION,P1)
    #define PARS_ASSERT_2(EXPRESSION,DESCRIPTION,P1,P2)
    #define PARS_ASSERT_3(EXPRESSION,DESCRIPTION,P1,P2,P3)
    #define PARS_ASSERT_4(EXPRESSION,DESCRIPTION,P1,P2,P3,P4)
    #define PARS_ASSERT_5(EXPRESSION,DESCRIPTION,P1,P2,P3,P4,P5)
#  endif /* PARS_DEBUG_OUT */
#endif /* _DEBUG */

/*----------------------------------------------------------------------------------------------------*\
|   RETURN_INIT                                                                          RETURN_INIT   |
\*----------------------------------------------------------------------------------------------------*/
 /**
  * @def RETURN_INIT(X,Y)
  *  Initializes the Return Code handling, must be set direct after Variable
  *  Declaration. X is the facility of the Function, Y is a descriptive String
  *  named Scope. This is to find the source of an Error in the Debug Output.
  */

#if PARS_DEBUG_OUT
 #if PARS_DEBUG_MODE_CALL_TRACE
  #if !PARS_DEBUG_MODE_CALL_TRACE_DISABLE
   #if PARS_CHECK_STACK_SIZE
    #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT   _retAuto        = FACILITY;               \
                                                static PARS_CHAR* _achScope;                            \
                                                if (PARS_g_pCoreContext) {                              \
                                                PARS_g_pCoreContext->CheckStack();                      \
                                                PARS_g_pCoreContext->Trace(_retAuto, #SCOPE " entered Facility " #FACILITY" (%04x).\n", (FACILITY&0x0000FFFFL));\
                                                }                                                       \
                                                _achScope = SCOPE;                                                \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
   #else /* PARS_CHECK_STACK_SIZE */
    #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT   _retAuto        = FACILITY;               \
                                                static PARS_CHAR* _achScope;                            \
                                                if (PARS_g_pCoreContext) {                              \
                                                  PARS_g_pCoreContext->Trace(_retAuto, #SCOPE " entered Facility " #FACILITY" (%04x).\n", (FACILITY&0x0000FFFFL));\
                                                }                                                       \
                                                _achScope = SCOPE;                                                \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
   #endif  /* PARS_CHECK_STACK_SIZE */
  #else /* !PARS_DEBUG_MODE_CALL_TRACE_DISABLE */
   #if PARS_CHECK_STACK_SIZE
    #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT   _retAuto        = FACILITY;               \
                                                static PARS_CHAR* _achScope;                            \
                                                if (PARS_g_pCoreContext) {                              \
                                                PARS_g_pCoreContext->CheckStack();                      \
                                                }                                                       \
                                                _achScope = SCOPE;                                                \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
   #else /* PARS_CHECK_STACK_SIZE */
    #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT   _retAuto        = FACILITY;               \
                                                static PARS_CHAR* _achScope;                            \
                                                _achScope = SCOPE;                                      \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
   #endif /* PARS_CHECK_STACK_SIZE */
  #endif /* PARS_DEBUG_MODE_CALL_TRACE_DISABLE */
 #else /* PARS_DEBUG_MODE_CALL_TRACE */
  #if PARS_CHECK_STACK_SIZE
   #define     RETURN_INIT(FACILITY, SCOPE)     PARS_RESULT   _retAuto        = FACILITY;               \
                                                static PARS_CHAR* _achScope;                            \
                                                _achScope = SCOPE;                                                \
                                                if (PARS_g_pCoreContext) {                              \
                                                PARS_g_pCoreContext->CheckStack();                      \
                                                }                                                       \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
   #else /* PARS_CHECK_STACK_SIZE */
    #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT  _retAuto    = FACILITY;                    \
                                                static PARS_CHAR* _achScope;                            \
                                                _achScope = SCOPE;                                                \
                                                if ( _retAuto == MWE_SYNTAX_ERROR ) goto _MWE_RETURN;
   #endif /* PARS_CHECK_STACK_SIZE */
  #endif /* PARS_DEBUG_MODE_CALL_TRACE */
 #else /* PARS_DEBUG_OUT */
  #if PARS_CHECK_STACK_SIZE
   #define     RETURN_INIT(FACILITY, SCOPE)     PARS_RESULT   _retAuto        = FACILITY;               \
                                                if (PARS_g_pCoreContext) {                              \
                                                PARS_g_pCoreContext->CheckStack();                      \
                                                }                                                       \
                                                 if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
  #else /* PARS_CHECK_STACK_SIZE */
   #define     RETURN_INIT(FACILITY, SCOPE)    PARS_RESULT   _retAuto        = FACILITY;                \
                                                if (_retAuto==MWE_SYNTAX_ERROR) goto _MWE_RETURN;
  #endif /* PARS_CHECK_STACK_SIZE */
#endif /* PARS_DEBUG_OUT */

/*----------------------------------------------------------------------------------------------------*\
|   BAIL OUT                                                                                BAIL OUT   |
\*----------------------------------------------------------------------------------------------------*/
#define     BAIL_OUT_SILENT(ERROR_CODE)             {                                                                           \
                                                        _retAuto    = (MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE)); \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                    }

#if PARS_DEBUG_OUT
  #define   BAIL_OUT_0( TL, ERROR_CODE, DESCRIPTION)  {                                                                         \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto),   \
                                                                           "\"%s\" BAILED OUT with code: %08X: %s",             \
                                                                           _achScope,                                           \
                                                                           _retAuto,                                            \
                                                                           DESCRIPTION);                                        \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                      }

  #define   BAIL_OUT_1(TL, ERROR_CODE, DESCRIPTION, P1){                                                                        \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto),   \
                                                                         "\"%s\" BAILED OUT (Code: %08X): " DESCRIPTION,        \
                                                                         _achScope,                                             \
                                                                         _retAuto,                                              \
                                                                         P1);                                                   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                      }

  #define   BAIL_OUT_2(TL, ERROR_CODE, DESCRIPTION, P1, P2){                                                                    \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto),   \
                                                                         "\"%s\" BAILED OUT (Code: %08X): " DESCRIPTION,        \
                                                                         _achScope,                                             \
                                                                         _retAuto,                                              \
                                                                         P1, P2);                                               \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }
  #define   BAIL_OUT_3(TL, ERROR_CODE, DESCRIPTION, P1, P2, P3){                                                                \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto),   \
                                                                         "\"%s\" BAILED OUT (Code: %08X): " DESCRIPTION,        \
                                                                         _achScope,                                             \
                                                                         _retAuto,                                              \
                                                                         P1, P2, P3);                                           \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }
  #define   BAIL_OUT_4(TL, ERROR_CODE, DESCRIPTION, P1, P2, P3, P4){                                                            \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(TL)|MWE_MASK_FACILITY(_retAuto),   \
                                                                         "\"%s\" BAILED OUT (Code: %08X): " DESCRIPTION,        \
                                                                         _achScope,                                             \
                                                                         _retAuto,                                              \
                                                                         P1, P2, P3, P4);                                       \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }

  #define   BAIL_OUT(ERROR_CODE, DESCRIPTION)       BAIL_OUT_0( PARS_DEBUG_LEVEL_BAIL_OUT, ERROR_CODE, DESCRIPTION)

  #define   BAIL_OUT_ACTUAL_ERROR(DESCRIPTION)      {                                                                           \
                                                        if (PARS_g_pCoreContext) PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),   \
                                                                         "\"%s\" BAILED OUT (Code: %08X):\n%s",                 \
                                                                         _achScope,                                             \
                                                                         _retAuto,                                              \
                                                                         DESCRIPTION);                                          \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                    }

#else /* PARS_DEBUG_OUT */
  #define   BAIL_OUT_0(TL, ERROR_CODE, DESCRIPTION)   {                                                                         \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                      }

  #define   BAIL_OUT_1(TL, ERROR_CODE, DESCRIPTION, P1){                                                                        \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                      }

  #define   BAIL_OUT_2(TL, ERROR_CODE, DESCRIPTION, P1, P2){                                                                    \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }
  #define   BAIL_OUT_3(TL, ERROR_CODE, DESCRIPTION, P1, P2, P3){                                                                \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }
  #define   BAIL_OUT_4(TL, ERROR_CODE, DESCRIPTION, P1, P2, P3, P4){                                                            \
                                                        _retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE);   \
                                                        PARS_BREAK_ON_BAILOUT(TL)                                               \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                        }

  #define   BAIL_OUT(ERROR_CODE, DESCRIPTION)       BAIL_OUT_0(PARS_DEBUG_LEVEL_BAIL_OUT, ERROR_CODE, DESCRIPTION)

  #define   BAIL_OUT_ACTUAL_ERROR                   {                                                                           \
                                                        PARS_BREAK_ON_BAILOUT(PARS_DEBUG_LEVEL_INFO)                            \
                                                        goto    _MWE_LANDING_ZONE;                                              \
                                                    }
#endif /* PARS_DEBUG_OUT */

#define     SET_ACTUAL_ERROR(ERROR_CODE)            (_retAuto    = MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE))

#define     RETURN_ACTUAL_ERROR                     goto    _MWE_RETURN;

#define     RETURN(ERROR_CODE)                      {                                                                           \
                                                        _retAuto    = (MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE)); \
                                                        goto    _MWE_RETURN;                                                    \
                                                    }

#if      PARS_DEBUG_MODE_CALL_TRACE
#define     RETURN2(X)                          {                                                                               \
                                                    if (PARS_g_pCoreContext)                                                    \
                                                    {                                                                           \
                                                    PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),    \
                                                                     "\"%s\" returned.\n",                                      \
                                                                     _achScope);                                                \
                                                    }                                                                           \
                                                    _retAuto=_retAuto;                                                          \
                                                    return X;                                                                   \
                                                }
#else   /* PARS_DEBUG_MODE_CALL_TRACE */
#define     RETURN2(X)                          _retAuto=_retAuto;                                                              \
                                                return X;
#endif  /* PARS_DEBUG_MODE_CALL_TRACE */

#define     ACTUAL_ERROR                        _retAuto
/*----------------------------------------------------------------------------------------------------*\
|   Special BAIL_OUTs                                                              Special BAIL_OUTs   |
\*----------------------------------------------------------------------------------------------------*/
#define     CHECK_NULL_PARAMETER(PARAMETER)     if (((PARS_LPVOID)PARAMETER) == PARS_NULL)                                      \
                                                {                                                                               \
                                                    BAIL_OUT(MWE_INVALID_PARAMETER, " Null Parameter: " #PARAMETER "\n");       \
                                                }
#define     CHECK_NULL_FUNCTION(PARAMETER)     if( (PARAMETER) == PARS_NULL)                                                    \
                                                {                                                                               \
                                                    BAIL_OUT(MWE_INVALID_PARAMETER, " Null Parameter: " #PARAMETER "\n");       \
                                                }
#define     CHECK_SERVICE(PARAMETER, SERVICE)   if (((PARS_LPVOID)PARAMETER) == PARS_NULL)                                      \
                                                {                                                                               \
                                                    BAIL_OUT(MWE_INVALID_PARAMETER, " Needed Service not found: " #SERVICE "\n");       \
                                                }

#define     CHECK_NULL_PARAMETER_SILENT(PARAMETER)  if (((PARS_LPVOID)PARAMETER) == PARS_NULL)                                  \
                                                {                                                                               \
                                                    BAIL_OUT_SILENT(MWE_INVALID_PARAMETER);            \
                                                }

#define     BAIL_OUT_ON_FAIL_SILENT(ERROR_CODE) _retAuto    = (MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE));         \
                                                if (MWE_FAILED(_retAuto))                                                       \
                                                {                                                                               \
                                                    BAIL_OUT_SILENT(_retAuto);                                                  \
                                                }

#define     BAIL_OUT_ON_NULL_SILENT(PTR, ERROR_VALUE)                                                                           \
                                                if ((PTR) == 0)                                                                 \
                                                {                                                                               \
                                                    BAIL_OUT_SILENT(ERROR_VALUE);                                               \
                                                }

#define     CHECK_FUNCTION_POINTER(PARAMETER)   if (((PARS_FPVOID)PARAMETER) == PARS_NULL)                                      \
                                                {                                                                               \
                                                    BAIL_OUT(MWE_INVALID_PARAMETER, " Function Pointer is NULL: " #PARAMETER "\n");  \
                                                }
#define     CHECK_ZERO_PARAMETER(PARAMETER)     if ((PARAMETER) == 0)                                                           \
                                                {                                                                               \
                                                    BAIL_OUT(MWE_INVALID_PARAMETER, " Scalar Parameter is zero: " #PARAMETER "\n");  \
                                                }
#define     CHECK_MEMORY(PARAMETER)             if ((PARAMETER) == PARS_NULL)                                                   \
                                                {                                                                               \
                                                    BAIL_OUT( MWE_MEMORY_ERROR, " Memory Error:" #PARAMETER "\n");              \
                                                }

#define     BAIL_OUT_ON_FAIL(ERROR_CODE,DESCR)  _retAuto    = (MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE));         \
                                                if (MWE_FAILED(_retAuto))                                                       \
                                                {                                                                               \
                                                    if (PARS_strlen(DESCR)!=0)                                                  \
                                                    {                                                                           \
                                                        BAIL_OUT(_retAuto, " Failed: " DESCR);                                  \
                                                    }                                                                           \
                                                    else                                                                        \
                                                    {                                                                           \
                                                        BAIL_OUT_SILENT(_retAuto);                                              \
                                                    }                                                                           \
                                                }

#define     BAIL_OUT_ON_FAIL_EX(ERROR_CODE, TL, DESCR)                                                                          \
                                                _retAuto    = (MWE_MASK_FACILITY(_retAuto)|MWE_MASK_ERROR(ERROR_CODE));         \
                                                if (MWE_FAILED(_retAuto))                                                       \
                                                {                                                                               \
                                                    if (PARS_strlen(DESCR)!=0)                                                  \
                                                    {                                                                           \
                                                        BAIL_OUT_0( TL, _retAuto, " Failed: " DESCR);                           \
                                                    }                                                                           \
                                                    else                                                                        \
                                                    {                                                                           \
                                                        BAIL_OUT_SILENT(_retAuto);                                              \
                                                    }                                                                           \
                                                }

#define     BAIL_OUT_ON_NULL(PTR, ERROR_VALUE,DESCR)                                                                            \
                                                if ((PTR) == 0)                                                                 \
                                                {                                                                               \
                                                    if (PARS_strlen(DESCR)!=0)                                                  \
                                                    {                                                                           \
                                                        BAIL_OUT(ERROR_VALUE,#PTR " was PARS_NULL " DESCR);                     \
                                                    }                                                                           \
                                                    else                                                                        \
                                                    {                                                                           \
                                                        BAIL_OUT_SILENT(ERROR_VALUE);                                           \
                                                    }                                                                           \
                                                }

#define     BAIL_OUT_ON_NULL_EX(PTR, TL, ERROR_VALUE,DESCR)                                                                     \
                                                {                                                                               \
                                                    if ((PTR) == 0)                                                             \
                                                    {                                                                           \
                                                        BAIL_OUT_0(TL, ERROR_VALUE,#PTR " was PARS_NULL " DESCR);               \
                                                    }                                                                           \
                                                }

#define     BAIL_OUT_IF_TRUE(EXPR, ERROR_VALUE, DESCR)                                                                          \
                                                if ((EXPR)!=PARS_FALSE)                                                         \
                                                {                                                                               \
                                                    BAIL_OUT(ERROR_VALUE, #EXPR " was LSA_TRUE: " DESCR);                           \
                                                }

#define     BAIL_OUT_IF_TRUE_EX(EXPR, TL, ERROR_VALUE, DESCR)                                                                   \
                                                if ((EXPR)!=PARS_FALSE)                                                         \
                                                {                                                                               \
                                                    BAIL_OUT_0(TL, ERROR_VALUE, #EXPR " was LSA_TRUE: " DESCR);                     \
                                                }

#define     BAIL_OUT_IF_FALSE(EXPR, ERROR_VALUE, DESCR)                                                                         \
                                                if ((EXPR)==PARS_FALSE)                                                         \
                                                {                                                                               \
                                                    BAIL_OUT(ERROR_VALUE, #EXPR " was LSA_FALSE: " DESCR);                          \
                                                }

#define     BAIL_OUT_IF_FALSE_EX(EXPR, TL, ERROR_VALUE, DESCR)                                                                  \
                                                if ((EXPR)==PARS_FALSE)                                                         \
                                                {                                                                               \
                                                    BAIL_OUT_0(TL, ERROR_VALUE, #EXPR " was LSA_FALSE: " DESCR);                    \
                                                }

#define     IGNORE_RESULT_CODE(X)               (PARS_VOID)(X)
/*----------------------------------------------------------------------------------------------------*\
|   LANDING_ZONE                                                                        LANDING_ZONE   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_DEBUG_OUT
 #if PARS_DEBUG_MODE_CALL_TRACE
  #if !PARS_DEBUG_MODE_CALL_TRACE_DISABLE
   #define     LANDING_ZONE                     _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),                \
                                                                 "\"%s\" returned (Code: %08X).\n",                             \
                                                                 _achScope,                                                     \
                                                                 _retAuto);                                                     \
                                                return _retAuto;                                                                \
                                                _MWE_LANDING_ZONE:
   #define     TRANSPARENT_LANDING_ZONE         _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),                \
                                                                 "\"%s\" returned (Code: %08X).\n",                             \
                                                                 _achScope,                                                     \
                                                                 _retAuto);                                                     \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE2(X)                 _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),                \
                                                                 "\"%s\" returned.\n",                                          \
                                                                 _achScope);                                                    \
                                                return X;                                                                       \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE_VOID                _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                PARS_g_pCoreContext->Trace( MWE_SET_TRACE_LEVEL(PARS_DEBUG_LEVEL_BAIL_OUT)|MWE_MASK_FACILITY(_retAuto),                \
                                                                 "\"%s\" returned.\n",                                          \
                                                                 _achScope);                                                    \
                                                return ;                                                                        \
                                                _MWE_LANDING_ZONE:
  #else /* ifndef     PARS_DEBUG_MODE_CALL_TRACE_DISABLE  */
   #define     LANDING_ZONE                     _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return _retAuto;                                                                \
                                                _MWE_LANDING_ZONE:
   #define     TRANSPARENT_LANDING_ZONE         _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE2(X)                 _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return X;                                                                       \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE_VOID                _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return;                                                                         \
                                                _MWE_LANDING_ZONE:
  #endif /* ifndef     PARS_DEBUG_MODE_CALL_TRACE_DISABLE  */
 #else /* PARS_DEBUG_MODE_CALL_TRACE */
  #define     LANDING_ZONE                      _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return _retAuto;                                                                \
                                                _MWE_LANDING_ZONE:
   #define     TRANSPARENT_LANDING_ZONE         _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE2(X)                 _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return X;                                                                       \
                                                _MWE_LANDING_ZONE:
   #define     LANDING_ZONE_VOID                _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return;                                                                         \
                                                _MWE_LANDING_ZONE:
 #endif /* PARS_DEBUG_MODE_CALL_TRACE */
#else /* PARS_DEBUG_OUT */
 #define     LANDING_ZONE                       _MWE_RETURN:                                                                    \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) {goto _MWE_LANDING_ZONE;};            \
                                                return _retAuto;                                                                \
                                                _MWE_LANDING_ZONE:
 #define     TRANSPARENT_LANDING_ZONE           _MWE_RETURN:                                                                    \
                                                goto _MWE_JUMPOVER;                                                             \
                                                goto _MWE_LANDING_ZONE;                                                         \
                                                _MWE_LANDING_ZONE:                                                              \
                                                _MWE_JUMPOVER:
 #define     LANDING_ZONE2(X)                   _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return X;                                                                       \
                                                _MWE_LANDING_ZONE:
 #define     LANDING_ZONE_VOID                  _MWE_RETURN:                                                                    \
                                                _retAuto=_retAuto;                                                              \
                                                if(PARS_g_pCoreContext==PARS_ILLEGAL_PTR) goto _MWE_LANDING_ZONE;               \
                                                return;                                                                         \
                                                _MWE_LANDING_ZONE:
#endif /* PARS_DEBUG_OUT */

/*----------------------------------------------------------------------------------------------------*\
|   END LANDING ZONE                                                                END LANDING ZONE   |
\*----------------------------------------------------------------------------------------------------*/
#define     END_LANDING_ZONE                    return _retAuto;
#define     END_LANDING_ZONE2(X)                return (X);
#define     END_LANDING_ZONE_VOID               return;

/* the X parameter in the following macros is the module name */

#define IMPLEMENT_LOCAL_SHUTDOWN_CONTROL(X)     static PARS_BOOL    PARS_g_bShutDownMode##X         = PARS_FALSE;               \
                                                static PARS_LPVOID  PARS_g_csShutDown##X            = PARS_NULL;                \
                                                static PARS_DWORD   PARS_g_dwBlockShutDownMode##X   = 0;

#define IMPLEMENT_GLOBAL_SHUTDOWN_CONTROL(X)    PARS_BOOL    PARS_g_bShutDownMode##X                = PARS_FALSE;               \
                                                PARS_LPVOID  PARS_g_csShutDown##X                   = PARS_NULL;                \
                                                PARS_DWORD   PARS_g_dwBlockShutDownMode##X          = 0;

#define GLOBAL_SHUTDOWN_CONTROL(X)              extern PARS_BOOL    PARS_g_bShutDownMode##X;                                    \
                                                extern PARS_LPVOID  PARS_g_csShutDown##X;                                       \
                                                extern PARS_DWORD   PARS_g_dwBlockShutDownMode##X;

#define IS_SHUTDOWN_MODE(X)                     (PARS_g_bShutDownMode##X != PARS_FALSE)
#if PARS_USE_SYNCHRONISATION
/* #pragma message ( "Synchronization used for Shutdown mode" ) */
#define BLOCK_SHUTDOWN(X)                       {                                                                                                   \
                                                    if(PARS_g_pCoreContext)(PARS_g_pCoreContext->EnterCriticalSection(PARS_g_csShutDown##X);        \
                                                    if (PARS_FALSE!=PARS_g_bShutDownMode##X)                                                        \
                                                    {                                                                                               \
                                                        if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);     \
                                                        BAIL_OUT_ON_FAIL (MWE_SHUTTING_DOWN, "Server is shutting down.\n");                         \
                                                    }                                                                                               \
                                                    PARS_g_dwBlockShutDownMode##X++;                                                                \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);         \
                                                }

#define UNBLOCK_SHUTDOWN(X)                     {   if(PARS_g_pCoreContext)PARS_g_pCoreContext->EnterCriticalSection(PARS_g_csShutDown##X);         \
                                                    if (PARS_FALSE!=PARS_g_bShutDownMode##X)                                                        \
                                                    {                                                                                               \
                                                        if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);     \
                                                        BAIL_OUT_ON_FAIL (MWE_SHUTTING_DOWN, "Server is shutting down.\n");                         \
                                                    }                                                                                               \
                                                    PARS_g_dwBlockShutDownMode##X--;                                                                \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);         \
                                                }

#define CHECK_SHUTDOWN_MODE(X)                  {   if(PARS_g_pCoreContext)PARS_g_pCoreContext->EnterCriticalSection(PARS_g_csShutDown##X);         \
                                                    if (PARS_g_bShutDownMode##X!=PARS_FALSE) SET_ACTUAL_ERROR(MWE_SHUTTING_DOWN);                   \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);         \
                                                    BAIL_OUT_ON_FAIL_EX (ACTUAL_ERROR, PARS_DEBUG_LEVEL_HINT, "INFO: Server is shutting down.\n");  \
                                                }

#define START_SHUTDOWN_MODE(X)                  {                                                                                                   \
                                                    PARS_g_bShutDownMode##X = PARS_TRUE;                                                            \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->EnterCriticalSection(PARS_g_csShutDown##X);         \
                                                    PARS_g_bShutDownMode##X = PARS_TRUE;                                                            \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);         \
                                                }

#define FINALIZE_SHUTDOWN(X,Y,Z)                {                                                                                                   \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->EnterCriticalSection(PARS_g_csShutDown##X);         \
                                                    if ( ( (Z==PARS_FALSE) && (Y>0) ) || (PARS_g_dwBlockShutDownMode##X > 0) )                      \
                                                    {                                                                                               \
                                                        if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);     \
                                                        RETURN (MWS_CLEANUP_AGAIN);                                                                 \
                                                    }                                                                                               \
                                                    _retAuto = MWS_CLEANUP_COMPLETE;                                                                \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->LeaveCriticalSection(PARS_g_csShutDown##X);         \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->DeleteCriticalSection(PARS_g_csShutDown##X);        \
                                                    PARS_g_csShutDown##X = PARS_NULL;                                                               \
                                                }

#define CLEANUP_SHUTDOWN(X)                     if(PARS_g_pCoreContext)PARS_g_pCoreContext->DeleteCriticalSection(PARS_g_csShutDown##X);            \
                                                PARS_g_csShutDown##X = PARS_NULL;

#if PARS_INIT_GLOBALS_MANUALLY
    #define INITIALIZE_SHUTDOWN_MODE(X)        {                                                                                                    \
                                                    if(PARS_g_pCoreContext) PARS_g_pCoreContext->CreateCriticalSection(&PARS_g_csShutDown##X);      \
                                                    PARS_g_bShutDownMode##X                      = PARS_FALSE;                                      \
                                                    PARS_g_dwBlockShutDownMode##X                = 0;                                               \
                                                }
#else /* PARS_INIT_GLOBALS_MANUALLY  */
    #define INITIALIZE_SHUTDOWN_MODE(X)         {                                                                                                   \
                                                    if (PARS_g_csShutDown##X != PARS_NULL)                                                          \
                                                    {                                                                                               \
                                                        return MWE_UNEXPECTED;                                                                      \
                                                    }                                                                                               \
                                                    if(PARS_g_pCoreContext)PARS_g_pCoreContext->CreateCriticalSection(&PARS_g_csShutDown##X);       \
                                                    PARS_g_bShutDownMode##X                      = PARS_FALSE;                                      \
                                                    PARS_g_dwBlockShutDownMode##X                = 0;                                               \
                                                }
#endif /* PARS_INIT_GLOBALS_MANUALLY  */

#else /* PARS_USE_SYNCHRONISATION */
/* #pragma message ( "Unsynchronized shutdown mode!!!" ) */
#define BLOCK_SHUTDOWN(X)                       {                                                                                       \
                                                    if (PARS_FALSE!=PARS_g_bShutDownMode##X)                                            \
                                                    {                                                                                   \
                                                        BAIL_OUT_ON_FAIL (MWE_SHUTTING_DOWN, "Server is shutting down.\n");             \
                                                    }                                                                                   \
                                                    PARS_g_dwBlockShutDownMode##X++;                                                    \
                                                }

#define UNBLOCK_SHUTDOWN(X)                     {   if (PARS_FALSE!=PARS_g_bShutDownMode##X)                                             \
                                                    {                                                                                    \
                                                        BAIL_OUT_ON_FAIL (MWE_SHUTTING_DOWN, "Server is shutting down.\n");              \
                                                    }                                                                                    \
                                                    PARS_g_dwBlockShutDownMode##X--;                                                     \
                                                }

#define CHECK_SHUTDOWN_MODE(X)                  {   if (PARS_g_bShutDownMode##X!=PARS_FALSE) SET_ACTUAL_ERROR(MWE_SHUTTING_DOWN);        \
                                                    BAIL_OUT_ON_FAIL_EX (ACTUAL_ERROR, PARS_DEBUG_LEVEL_HINT, "INFO: Server is shutting down.\n");  \
                                                }

#define START_SHUTDOWN_MODE(X)                  {                                                                                                   \
                                                    PARS_g_bShutDownMode##X = PARS_TRUE;                                                            \
                                                }

#define FINALIZE_SHUTDOWN(X,Y,Z)                {                                                                                                   \
                                                    if ( ( (Z==PARS_FALSE) && (Y>0) ) || (PARS_g_dwBlockShutDownMode##X > 0) )                       \
                                                    {                                                                                               \
                                                        RETURN (MWS_CLEANUP_AGAIN);                                                                 \
                                                    }                                                                                               \
                                                    _retAuto = MWS_CLEANUP_COMPLETE;                                                                \
                                                }

#if PARS_INIT_GLOBALS_MANUALLY
    #define INITIALIZE_SHUTDOWN_MODE(X)        {                                                                                                    \
                                                    PARS_g_bShutDownMode##X                      = PARS_FALSE;                                      \
                                                    PARS_g_dwBlockShutDownMode##X                = 0;                                               \
                                                }
#else /* PARS_INIT_GLOBALS_MANUALLY  */
    #define INITIALIZE_SHUTDOWN_MODE(X)         {                                                                                                   \
                                                    if (PARS_g_csShutDown##X != PARS_NULL)                                                          \
                                                    {                                                                                               \
                                                        return MWE_UNEXPECTED;                                                                      \
                                                    }                                                                                               \
                                                }
#endif /* PARS_INIT_GLOBALS_MANUALLY  */

#define CLEANUP_SHUTDOWN(X)
#endif /* PARS_USE_SYNCHRONISATION */

PARS_END_EXTERN_C

#endif /* __PARS_ERROR_INLCUDED */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

