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
/*  F i l e               &F: pnd_xml_std_interface.h                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definitions for standard XML Parser access functions           */
/*                                                                           */
/*****************************************************************************/

#ifndef XML_PARSER_STD_INTERFACE_INCLUDED
#define XML_PARSER_STD_INTERFACE_INCLUDED

#include "PARSP_Utils.h"
#include "PARS_XML.h"
#include "lsa_cfg.h"
#include "pnd_xml_cfg.h"

//if not in lsa.cfg.h
#ifndef LSA_CHAR
#define LSA_CHAR LSA_INT8 
#endif

///////////////////////////////////////////////////////////////////////////////
///  XML Element type
///////////////////////////////////////////////////////////////////////////////
#define xml_element PARS_XML_Node
///////////////////////////////////////////////////////////////////////////////
///  XML Document type
///////////////////////////////////////////////////////////////////////////////
#define xml_doc  PARS_XML_Sheet


LSA_VOID    xml_stdint_parser_init();
xml_doc     xml_stdint_parse_from_buffer(LSA_CHAR * buffer);
LSA_VOID    xml_stdint_free_doc(xml_doc doc);

/*------element access function--------*/

xml_element xml_stdint_get_element_by_name(xml_element parent, const LSA_CHAR * name);
xml_element xml_stdint_get_element_by_attribute(xml_element parent, const LSA_CHAR * attribute, const LSA_CHAR * value) ;
xml_element xml_stdint_get_next_element(xml_element const parent, xml_element const element);
xml_element xml_stdint_get_root_element(xml_doc doc);
LSA_UINT32  xml_stdint_get_element_content(xml_element element,LSA_CHAR * buf);
LSA_UINT8*  xml_stdint_get_element_content2(xml_element element, LSA_UINT32* length);
LSA_UINT32  xml_stdint_get_attribute_value(xml_element element, const LSA_CHAR *attribute_name, LSA_CHAR *attribute_value, LSA_UINT16 num_char);
LSA_VOID    xml_stdint_get_element_name(xml_element element, LSA_CHAR * buf);
LSA_UINT16  xml_stdint_get_child_count(xml_element element);

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_XML_STD_INTERFACE_H */