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
/*  F i l e               &F: PARSP_Properties.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface description for the Variable Interface Service                 */
/*  ATTENTION! This is the Variable Service 2 Version!!!                     */
/*                                                                           */
/*****************************************************************************/

#ifndef PARS_VARIF_PROPERTIES_DEFINED
#define PARS_VARIF_PROPERTIES_DEFINED

/*----------------------------------------------------------------------------------------------------*\
|   Constants for Properties                                                Constants for Properties   |
\*----------------------------------------------------------------------------------------------------*/

/* BEGIN predifined properties - reserved entries up to 5000 */
#define    PARS_PROP_EMPTY                   0
#define    PARS_PROP_INVALID_TYPE            0xFFFFL    /* Ups no type :-) */
#define    PARS_PROP_DATATYPE                1          /* Item Canonical DataType */
#define    PARS_PROP_VALUE                   2          /* Item Value */
#define    PARS_PROP_QUALITY                 3          /* Item Quality */
#define    PARS_PROP_TIMESTAMP               4          /* Item Time stamp */
#define    PARS_PROP_ACCESSRIGHTS            5          /* Item Access Rights */
#define    PARS_PROP_SCANRATE                6          /* Server Scan Rate */
#define    PARS_PROP_EUTYPE                  7          /* Item EU Type */
#define    PARS_PROP_EUINFO                  8          /* Item EUInfo */
/* 9-99             Reserved for future use */
#define    PARS_PROP_ENGINEERINGUNITS      100          /* EU Units */
#define    PARS_PROP_DESCRIPTION           101          /* Item Description */
#define    PARS_PROP_HIGHEU                102          /* High EU */
#define    PARS_PROP_LOWEU                 103          /* Low EU */
#define    PARS_PROP_HIGHIR                104          /* High Instrument Range */
#define    PARS_PROP_LOWIR                 105          /* Low Instrument Range */
#define    PARS_PROP_CLOSELABEL            106          /* Contact Close Label */
#define    PARS_PROP_OPENLABEL             107          /* Contact Open Label */
#define    PARS_PROP_TIMEZONE              108          /* Item Timezone */
#define    PARS_PROP_MINIMUMVALUE          109          /* Minimum Value */
#define    PARS_PROP_MAXIMUMVALUE          110          /* Maximum Value */
#define    PARS_PROP_VALUEPRECISION        111          /* Value Precision */
#define    PARS_PROP_MAXAGE                112          /* Maximum Age of a cached Variable */
#define    PARS_PROP_DIAGNOSTICINFO        113          /* Extented Diagnosticinformation for a Variable - if applicable */
/* 113 - 399        Reserved for future use */
#define    PARS_PROP_START_USERDEFINED    5000          /* Begin of user defined properties */
/* END predefined properties - reserved entries up to 5000 */

/* If your provider(s) support additional properties not covert with the predefined you can do it here */
/* As an sample - the SimVarProvider has defined his own properties below */
/* The highest number of properties is 65534 (0xFFFE) */
/* Don't forget to extend the PARS_VARIF_g_PropertiesList below with meaningful entries!!! */
#define PARS_PROP_SIM_BEHAVIOR               1 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_NAME                   2 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_DELTA                  3 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_MODINTERVAL            4 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_LINK                   5 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_BROWSEINFOS            6 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_CALL                   7 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */

#define PARS_PROP_SIM_SENSORAKTOR_INDEX    101 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_EXT_SOURCENAME       102 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_EVENT_SOURCENAME     103 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_IS_HOSTED_FROM       104 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */
#define PARS_PROP_SIM_IS_RELATED_WITH      105 + PARS_PROP_START_USERDEFINED   /* private property of SimVarProvider */

typedef struct PARS_VARIF_S_PropertyTypes
{
    PARS_LPSTR      strQualifiedName;
    PARS_LPSTR      strNameSpaceType;
    PARS_DWORD      dwDataType;
    PARS_DWORD      dwPropertyType;
    PARS_LPSTR      strDescription;
} PARS_VARIF_T_PropertyTypes, *PARS_VARIF_PT_PropertyTypes;

extern const PARS_VARIF_T_PropertyTypes PARS_OMUTILS_g_PropertiesList[];

#endif /* PARS_VARIF_PROPERTIES_DEFINED */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

