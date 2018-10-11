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
/*  F i l e               &F: pnd_xml_helper.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definitions for XML Parser helper functions	                 */
/*                                                                           */
/*****************************************************************************/


#ifndef PND_XML_HELPER_INCLUDED
#define PND_XML_HELPER_INCLUDED


LSA_UINT8   xml_hex_string_to_byte_array (LSA_CHAR *str, LSA_UINT8 *byte_array, LSA_UINT32 nr_of_bytes);
LSA_UINT32  xml_string_to_u32(LSA_CHAR *rawstr,LSA_UINT8 *status, LSA_BOOL hex);

xml_element xml_locate_element_by_content(xml_element parent, const LSA_CHAR * child_name, const LSA_CHAR * content);
xml_element xml_locate_element_by_child_content(xml_element parent, const LSA_CHAR * childs_child, const LSA_CHAR * childs_child_content, LSA_UINT16 index);
LSA_VOID update_rema(PARS_PND_REMA_DATA_TYPE PARS_remaData, PARS_LPSTR** remaFileData, LSA_UINT32* dataLength); //TODO
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_XML_HELPER_H */
