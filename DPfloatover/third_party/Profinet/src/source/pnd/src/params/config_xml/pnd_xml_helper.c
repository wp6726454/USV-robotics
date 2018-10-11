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
/*  F i l e               &F: pnd_xml_helper.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of XML Parser helper access functions                     */
/*                                                                           */
/*****************************************************************************/


#include "pnd_xml_std_interface.h"
#include "pnd_xml_cfg.h"
#include "pnd_xml_helper.h"
#include "pnd_sys.h"



///////////////////////////////////////////////////////////////////////////////
///
///  Returns element with given content with given path
///
///  @param [in]        parent (xml_element)    :  Parent element
///  @param [in]        child_name(LSA_CHAR *)  :  name of the cild element
///  @param [in]        content(LSA_CHAR *)     :  content of the target element
///
///  @return element(xm_element) : resulting element pointer
///
///////////////////////////////////////////////////////////////////////////////
xml_element xml_locate_element_by_content(xml_element parent, const LSA_CHAR * child_name, const LSA_CHAR * content)
{
    xml_element child;
    LSA_INT16 i = 0, result = 0;
    LSA_UINT16 child_count = 0;
    LSA_CHAR child_content[1000], name[1000];

    XML_ASSERT(parent != LSA_NULL);

    // get number of all child elements
    child_count = xml_stdint_get_child_count(parent);
    if (child_count == 0)
    return LSA_NULL;

    // get first child
    child = xml_stdint_get_next_element(parent, LSA_NULL);
    if(child == LSA_NULL)
        return LSA_NULL;

    //run through all child elements and find the first element with the required content
    for (i=0; i<child_count; i++)
    {
        // check the child's name
        xml_stdint_get_element_name(child, name);
        result = (LSA_INT16)PARS_strcmp (child_name, name);
        if(result == 0)
        {
            // check if this child has the requierd content
            xml_stdint_get_element_content(child, child_content);
            result = (LSA_INT16)PARS_strcmp (child_content, content);
        }

        if(result == 0)
        {
            return parent;
        }
        // get next child element
        child = xml_stdint_get_next_element(LSA_NULL, child);
    }

    return LSA_NULL; // no element found
}


///////////////////////////////////////////////////////////////////////////////
///
///  Returns element with with a child with a given value
///
///  @param [in]        parent (xml_element)        :  parent element
///  @param [in]        childs_child (LSA_CHAR *)   :  name of the child element
///  @param [in]        value(LSA_CHAR *)           :  content of the child element
///  @param [in]        index(LSA_UINT16)           :  index of the element
///
///  @return element(xm_element) : resulting element pointer
///
///////////////////////////////////////////////////////////////////////////////
xml_element xml_locate_element_by_child_content(xml_element parent, const LSA_CHAR * childs_child, const LSA_CHAR * childs_child_content, LSA_UINT16 index)
{
    xml_element child, element;
    LSA_INT16 i = 0, found_index = 0;
    LSA_UINT16 child_count = 0;

    XML_ASSERT(parent != LSA_NULL);
    XML_ASSERT(index != 0);

    // get number of all child elements
    child_count = xml_stdint_get_child_count(parent);
    if (child_count == 0)
        return LSA_NULL;

    if (index > child_count)
        return LSA_NULL;

    // get first child
    child = xml_stdint_get_next_element(parent, LSA_NULL);
    if(child == LSA_NULL)
        return LSA_NULL;
    //run through all child elements
    for (i=0; i<child_count; i++)
    {
        // check if this child has an element with childs_child and childs_child_value
        element = xml_locate_element_by_content(child, childs_child, childs_child_content);

        if(element != LSA_NULL)
        {
            found_index++;
            // if index has been reached, return element
            if (found_index == index)
                return element;
        }
        // get next child element
        child = xml_stdint_get_next_element(LSA_NULL, child);
    }

    return LSA_NULL; // no element found
}

///////////////////////////////////////////////////////////////////////////////
///
///  Converts string to unsigned32. 
///  If the rawstr starts with 0x or 0X then str handled as hex otherwise rawstr is converted as decimal. 
///  Status :LSA_TRUE if successful,LSA_FALSE if not
///
///  @param [in]        raw_str(LSA_CHAR) : string
///  @param [in]        hex(LSA_BOOL) : if LSA_TRUE, the string is in hex format
///  @param [out]       status(LSA_CHAR *): Status for the conversion
///
///  @return uValue(LSA_UINT32) : Converted value.
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT32 xml_string_to_u32(LSA_CHAR *rawstr,LSA_UINT8 *status, LSA_BOOL hex) 
{
    LSA_INT16 n = 0, count, sign = 1, str_len=0, multiplier=0;		
    LSA_INT16 digit[10];      
    LSA_CHAR *str=rawstr;
    LSA_UINT32 uValue = 0;  

    XML_ASSERT(rawstr != LSA_NULL);
    XML_ASSERT(status != LSA_NULL);

    if (*str=='-')
    {
        sign = -1;
        str++;
    }
    else if (*str=='+')
    {
        sign = 1;
        str++;
    }

    if (hex == LSA_TRUE)
    {
        multiplier = 16;
    }
    else 
        multiplier = 10;

    str_len = (LSA_INT16)xml_strlen(str);
    if (str_len>10)
        return LSA_FALSE;

    while (n < str_len) 
    {
        if (str[n]=='\0')
            break;
        if ((str[n] >= '0') && (str[n] <= '9' )) //if 0 to 9
            digit[n] = str[n] - '0';            //convert to int
        else if ((str[n] >='a') && (str[n] <= 'f')) //if a to f
            digit[n] = (str[n] -'a') + 10;      //convert to int
        else if ((str[n] >='A') && (str[n] <= 'F')) //if A to F
            digit[n] = (str[n] - 'A') + 10;      //convert to int
        else return LSA_FALSE;
        n++;
    }
    count = n;
    n = 0;
    while(n < count) 
    {     
        uValue = uValue * multiplier + digit[n];
        n++;   
    }
    *status = LSA_TRUE;
    return (sign * uValue);
}

///////////////////////////////////////////////////////////////////////////////
///
///  Converts hex formated string to byte array. 
///
///  @param [in]        str (LSA_CHAR*) : string
///  @param [in]        byte_array(LSA_CUINT8 *)    : memory location for byte array
///  @param [in]        max_len(LSA_INT): length in bytes
///
///  @return len(LSA_UINT16) : Status for the conversion
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT8 xml_hex_string_to_byte_array (LSA_CHAR *str, LSA_UINT8 *byte_array, LSA_UINT32 nr_of_bytes)
{
    LSA_UINT16 i = 0;
    LSA_CHAR byte[3] = {0};
    LSA_UINT32 value = 0;
    LSA_UINT8 status = 0;

    for (i=0; i<nr_of_bytes; i++)
    {
        xml_memcpy(byte, str+i*2, 2);
        byte[2] = 0;
        value = (LSA_INT16) xml_string_to_u32(byte, &status, LSA_TRUE);
        XML_ASSERT((status == 1) && (value <= 0xFF));
        byte_array[i] = (LSA_UINT8) value;
    }

    return (LSA_UINT16) status;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
