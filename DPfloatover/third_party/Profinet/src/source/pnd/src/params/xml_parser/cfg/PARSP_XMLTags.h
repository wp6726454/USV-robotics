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
/*  F i l e               &F: PARSP_XMLTags.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Defines                                                                  */
/*                                                                           */
/*****************************************************************************/

/*----------------------------------------------------------------------------------------------------*\
|   XML Defines                                                                          XML Defines   |
\*----------------------------------------------------------------------------------------------------*/
#define        XML_TAG                  "<?xml "          /* Start XML Tag (incl trail space!)        */
#define        DTD_START_TAG            "!DOCTYPE"        /* Start of DTD Tag without '['             */
#define        DTG_END_TAG              "]>"
#define        XML_ATTR_VERSION         "version"         /* Attribute Name "Version"                 */
#define        XML_ATTR_ENCODING        "encoding"        /* Attribute Name "Encoding"                */
#define        XML_ATTR_STANDALONE      "standalone"      /* Attribute Name "StandAlone"              */

#define        XML_ATTR_STANDALONE_VALUE_YES    "yes"
#define        XML_ATTR_STANDALONE_VALUE_NO     "no"

#define        START_OF_TAG             '<'
#define        END_OF_TAG               '>'
#define        XML_MAX_TAGNAME_LEN      255

/*----------------------------------------------------------------------------------------------------*\
|   XML Config File Defines                                                  XML Config File Defines   |
\*----------------------------------------------------------------------------------------------------*/
#define LOAD_XML_DOC            "/SERVERPAGES"          /* use this url to load a new XML Tree */
#define CONFIG_ROOT_TAG         "SERVERPAGES"           /* name of the Root tag of the config */

#define BASE_PATH               "BASE"                  /* tag of root path in XML File */
#define SYSTEM_BASE_PATH        "SYSTEM"                  /* tag of root path in XML File */

#define DEFAULT_LINK            "Default_Link"          /* Default Link to Local Dir */
#define SERVEROPTIONS           "SERVEROPTIONS"         /* tag of Serveroption in XML File*/
#define USERSECTION             "USERDATABASE"          /* Start of the UserDataBase */
#define CONFIG_BASE             "CONFIGURATION_DATA"    /* Where in  the XML is the Configuration data */

#define DEFAULTDOCUMENT         "DEFAULTDOCUMENT"       /* tag of name of default file */
#define HTTP_INIT_XML           "WebCfg.xml"            /* filename of XML File */
#define PORTNUMBER              "PORTNUMBER"            /* tagname for Portnumber*/
#define ALTERNATE_PORTNUMBER    "ALTERNATE_PORTNUMBER"  /* tagname for alternate Portnumber*/
#define ALTERNATE_SSL_PORTNUMBER "ALTERNATE_SSL_PORTNUMBER"  /* tagname for alternate Portnumber*/
#define TRACELEVEL              "TRACELEVEL"            /* initial trace level */
#define SSLPORTNUMBER           "SSLPORTNUMBER"         /* tagname for Portnumber*/
#define TIMEZONE                "TIMEZONE"              /* Timezone in +/- Minutes from gmt */
#define STRINGTABLE             "STRINGTABLE"           /* Tag of the String Table */
#define LANGUAGE_TAG            "LANGUAGE"
#define LANGUAGES_TAG           "LANGUAGES"

#define MIME_TYPE_BASE          "MIME_TYPES"            /* Base Tag of the Mime Types Group */
#define MIME_ATTR_NAME          "EXTENSION"             /* Tag Name of Extension */
#define MIME_ATTR_VALUE         "MIMETYPE"              /* Tag Name of Mime Type */
#define MIME_ATTR_FILTER        "FILTER"                /* Should Post filter used */
#define MIME_ATTR_YES           PARS_TRUE_STRING        /* Positive Value for BROWSEABLE_TAG */
#define MIME_TYPE_DEFAULT       "DEFAULT"               /* Extension of the default mime type */
#define ICON_ATTR_NAME          "ICON"                  /* Attribute Name for Icon Path */

#define UNKNOWN_TAG             "UNKNOWN"
#define APPLICATION_TAG         "APPLICATION"
#define FILTER_TAG              "FILTER"
#define SERVICE_TAG             "SERVICE"
#define LIBRARIES_TAG           "LIBRARIES"
#define LIBRARY_TAG             "LIBRARY"
#define STATIC_TAG              "STATIC"                /* Tag for specifying static linked WebServices */
#define WEBSERVICE_TAG          "WEBSERVICE"            /* Element for specifying WebService info */

#define PASSWORD_TAG            "PASSWORD"
#define LOCAL_LINK_ATTR         "LOCALLINK"
#define LOCAL_LINK_TAG          "LOCALLINK"
#define PREFER_EXTERNAL_ATTR    "PREFER_EXTERNAL"
#define PREFER_EXTERNAL_YES     PARS_TRUE_STRING
#define BROWSEABLE_TAG          "BROWSEABLE"            /* Is Directory Browseble */
#define BROWSEABLE_YES          PARS_TRUE_STRING        /* Positive Value for BROWSEABLE_TAG */
#define HREF_ATTR               "HREF"
#define PARS_XML_ATTR_FILEDATE  "FILEDATE"

#define READ_ONLY_ATTR          "READONLY"
#define HIDDEN_ATTR             "HIDDEN"

#define REALM_ATTR              "REALM"
#define ALGO_ATTR               "AUTH_ALGO"

#define WSDL_ATTR               "WSDL"                  /* Path to the WSDL file for a WebService */
#define DOC_ATTR                "DOC"                   /* Path to the Documentation file for a WebService */
#define WSDL_ATTR               "WSDL"                 /* Path to the WSDL file for a WebService */
#define URL_ATTR                "URL"

#define DEFAULT_TAG             "DEFAULT"
#define LOADPATH_TAG            "LOADPATH"
#define NAME_TAG                "NAME"
#define USER_TAGNAME            "USER"
#define A1_TAG                  "A1"
#define EXTERNAL_TAG            "EXTERN"
#define SRC_TAG                 "SRC"
#define BASE_TAG                "BASE"                  /* Base URL of a WebService */
#define WSDL_TAG                "WSDL"                  /* Path to the WSDL file for a WebService */
#define DOC_TAG                 "DOC"                   /* Path to the Documentation file for a WebService */

#define DEFAPP_BASE_TAG         "DEFAPP"
#define RESULT_CODE             "NAME"                  /* The Attribut to find HTTP Result Codes */
#define HTTP_RESULT_CODES       "HTTP_RESULT_CODES"
#define CODE                    "CODE"
#define SUBCODE_ATTR            "SUBCODE"

#define PARS_XML_ATTR_VALUE     "VALUE"

#define PARS_ADMIN_GROUP         "Administrators"

#define PARS_XML_ATTR_ENCODING   "ENCODING"
#define PARS_XML_VALUE_MBASE64   "MBASE64"
#define FILE_SIZE_ATTR           "SIZE"

#define MODIFY_ATTR              "MODIFY"
#define WRITE_ATTR               "WRITE"
#define READ_ATTR                "READ"
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

