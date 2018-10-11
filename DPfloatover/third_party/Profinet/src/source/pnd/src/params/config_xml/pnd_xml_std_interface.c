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
/*  F i l e               &F: pnd_xml_std_interface.c                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of standard XML Parser access functions                   */
/*                                                                           */
/*****************************************************************************/

#include "pnd_xml_std_interface.h"
#include "pnd_int.h"


/*
 *  XML Parser is isolated from the rest of the XML 'Interpreter with a set of interface functions.
 *  Provided that this interface is supported XML Interpreter shall work with any XML Parser.
 */

///////////////////////////////////////////////////////////////////////////////
///  Global Service handle for XML Parser
///////////////////////////////////////////////////////////////////////////////
XMLServiceInterface*    pXMLService; 


///////////////////////////////////////////////////////////////////////////////
///
///  Initializes XML parser internal structures
///
///  @return void
///
///////////////////////////////////////////////////////////////////////////////
LSA_VOID xml_stdint_parser_init() 
{
    PARS_XML_InitializeStandaloneXMLService( PARS_SafeMalloc, PARS_SafeFree, &pXMLService);
}


///////////////////////////////////////////////////////////////////////////////
///
///  Parse the document in the given memory buffer and return doc handle.
///
///  @param [in]        buffer (LSA_CHAR *) : Buffer for the XML document
///
///  @return mydoc (xml_doc) : handle to the document
///
///////////////////////////////////////////////////////////////////////////////
xml_doc xml_stdint_parse_from_buffer(LSA_CHAR * buffer) 
{
    xml_doc mydoc;
    pXMLService->CreateSheet( buffer, PARS_FALSE, 0, &mydoc, PARS_NULL);
    return mydoc;
}

LSA_VOID xml_stdint_free_doc(xml_doc doc) 
{
    pXMLService->DeleteSheet(doc);
}


xml_element xml_stdint_get_root_element(xml_doc doc)
{
    return (xml_element)pXMLService->GetRootNode((PARS_XML_Sheet) doc);
}

///////////////////////////////////////////////////////////////////////////////
///
///  Return the element with given name
///
///  @param [in]        parent(xml_element) : Element in which search shall take place
///  @param [in]        name(LSA_CHAR*) : name
///
///  @return element (xml_element) : Handle of element with given name. 
///  If such an element does not exist, LSA_NULL is returned.
///
///////////////////////////////////////////////////////////////////////////////
xml_element xml_stdint_get_element_by_name(xml_element parent, const LSA_CHAR * name) 
{
    xml_element  element=LSA_NULL;
    pXMLService->FindNodeByTagname(parent, 
                                    PARS_XML_NAVIGATE_DOWN, 
                                    (PARS_LPSTR)name, 
                                    xml_strlen (name), 
                                    &element);
    return element;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Return the element with given attribute value
///
///  @param [in]        parent(xml_element) : element in which seacrh shall take place
///  @param [in]        name(LSA_CHAR*) : attribute
///  @param [in]        name(LSA_CHAR*) : value
///
///  @return element (xml_element) : Handle of element with given name. 
///  If such an element does not exist, LSA_NULL is returned.
///
///////////////////////////////////////////////////////////////////////////////
xml_element xml_stdint_get_element_by_attribute(xml_element parent, const LSA_CHAR * attribute, const LSA_CHAR * value) 
{
    xml_element  element=LSA_NULL;
    pXMLService->FindNodeByAttribute(parent,
                                    PARS_XML_NAVIGATE_DOWN,
                                    (PARS_LPSTR)attribute,
                                    strlen (attribute),
                                    (PARS_LPSTR)value,
                                    strlen (value),
                                    &element);
    return element;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Return the next element. 
///  If parent param value is given, first element in down the the given element is returned.
///  If element param value is given, next element besides the element is returned.
///
///  @param [in]        parent(xml_element) : Parent element in which search shall take place
///  @param [in]        element(xml_element) : Neighbouring element after which search shall take place
///
///  @return element (xml_element) : 
///  If such an element does not exist, LSA_NULL is returned.
///
///////////////////////////////////////////////////////////////////////////////
xml_element xml_stdint_get_next_element(xml_element const parent, xml_element const element)
{
    xml_element next_element=element;
    if (next_element == PARS_NULL)
    {
        if (parent == PARS_NULL)
        {
            return PARS_NULL;
        }
        pXMLService->GetNode(parent, PARS_XML_NAVIGATE_DOWN, PARS_XML_NODE_TYPE_ELEMENT, &next_element);
    }
    else
    {
        pXMLService->GetNode(element, PARS_XML_NAVIGATE_NEXT, PARS_XML_NODE_TYPE_ELEMENT, &next_element);
    }
    return next_element;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Retrieves name of given element
///
///  @param [in]        element(xml_element) 
///  @param [out]       buf (LSA_CHAR*) : name of element is copied to the buffer
///
///  @return LSA_VOID 
///
///////////////////////////////////////////////////////////////////////////////
void xml_stdint_get_element_name(xml_element element,LSA_CHAR * buf)
{
    PARS_DWORD dwContentLen=0;
    LSA_CHAR *  loc_buf;
    pXMLService->GetTagName(element, 0 /*PARS_XML_MEM_FLAG_GIVE_A_COPY*/ , &loc_buf, &dwContentLen);
    PARS_memcpy(buf,(LSA_CHAR *)loc_buf, dwContentLen);
    buf[dwContentLen]=0;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Retrieves the content of given element
///
///  @param [in]        element(xml_element) 
///  @param [out]       buf (LSA_CHAR*) : content of the element is copied to the buffer
///
///  @return LSA_VOID 
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT32 xml_stdint_get_element_content(xml_element element,LSA_CHAR * buf)
{
    PARS_DWORD dwContentLen=0;
    LSA_CHAR *  loc_buf;
    pXMLService->GetContent(element, 0 /*PARS_XML_MEM_FLAG_GIVE_A_COPY*/ , &loc_buf, &dwContentLen);
    PARS_memcpy(buf,(LSA_CHAR *)loc_buf,dwContentLen);
    buf[dwContentLen]=0;
    return dwContentLen;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Returns the pointer to the content of given element
///
///  @param [in]        element(xml_element) 
///  @param [out]       length (LSA_UINT32*) : length of the returned content in bytes
///
///  @return pointer to the content
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT8* xml_stdint_get_element_content2(xml_element element, LSA_UINT32* length)
{
    PARS_DWORD  content_length;
    LSA_CHAR*   loc_buf;
    LSA_UINT8*  content;

    content_length = 0;

    pXMLService->GetContent(element, 0 /*PARS_XML_MEM_FLAG_GIVE_A_COPY*/ , &loc_buf, &content_length);
    content = (LSA_UINT8*)xml_malloc( content_length+1 );
    XML_ASSERT( content != 0 );
    PARS_memcpy(content, (LSA_CHAR *)loc_buf, content_length);
    content[content_length]=0;
    pXMLService->GetRealContent((LSA_CHAR*)content);
    //*length = content_length;
    *length = xml_strlen((LSA_CHAR*)content);
    return content;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Retrieves the content of given element
///
///  @param [in]        element(xml_element)
///  @param [in]        attribute_name (LSA_CHAR*) : name of the atribute
///  @param [in]        attribute_value (LSA_CHAR*) : attribute value is copied to the buffer
///  @param [in]        num_char (LSA_UINT16*) : max character length of atribute
///
///  @return string length 
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT32 xml_stdint_get_attribute_value(xml_element element, const LSA_CHAR *attribute_name, LSA_CHAR *attribute_value, LSA_UINT16 num_char)
{
    PARS_DWORD attribute_length = 0;
    LSA_CHAR *  loc_buf;
    pXMLService->QueryAttributeByName(element, 0 /*PARS_XML_MEM_FLAG_GIVE_A_COPY*/ , (PARS_LPSTR)attribute_name, xml_strlen (attribute_name), LSA_FALSE, &loc_buf, &attribute_length);

    // number of characters, returned by QueryAttributeByName() must not be larger than the lenght of the allocated character string "attribute_name"
    XML_ASSERT( attribute_length < num_char );

    PARS_memcpy(attribute_value,(LSA_CHAR *)loc_buf,attribute_length);
    attribute_value[attribute_length]=0;
    return attribute_length;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Returns number of children of a given element
///
///  @param [in]        element(xml_element) 
///
///  @return child_count(LSA_UINT16) : Child count 
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT16 xml_stdint_get_child_count(xml_element   element)
{
    PARS_DWORD child_count = 0;
    pXMLService->GetChildCount(element, PARS_XML_NODE_TYPE_ALLNODES , &child_count);
    return (LSA_UINT16)child_count;
}

///////////////////////////////////////////////////////////////////////////////
///
///  Initializes XML parser internal structures
///
///  @return void
///
///////////////////////////////////////////////////////////////////////////////
LSA_VOID update_rema(PARS_PND_REMA_DATA_TYPE PARS_remaData, PARS_LPSTR** remaFileData, PNIO_UINT32* dataLength)
{
	pXMLService->Generate_Rema_XML_Buffer(PARS_remaData, remaFileData, (PARS_DWORD*)dataLength);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/