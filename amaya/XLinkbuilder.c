/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * XLinkbuilder
 *
 * Author: V. Quint
 *
 */
 

#define THOT_EXPORT extern
#include "amaya.h"
#include "XLink.h"
#include "parser.h"

/* mapping table of attribute values */
static AttrValueMapping XLinkAttrValueMappingTable[] =
{ 
   {XLink_ATTR_actuate, "onLoad", XLink_ATTR_actuate_VAL_onLoad},
   {XLink_ATTR_actuate, "onRequest", XLink_ATTR_actuate_VAL_onRequest},
   {XLink_ATTR_actuate, "undefined", XLink_ATTR_actuate_VAL_undefined},

   {XLink_ATTR_show, "embed", XLink_ATTR_show_VAL_embed},
   {XLink_ATTR_show, "new", XLink_ATTR_show_VAL_new},
   {XLink_ATTR_show, "replace", XLink_ATTR_show_VAL_replace},
   {XLink_ATTR_show, "undefined", XLink_ATTR_show_VAL_undefined},

   {XLink_ATTR_type, "arc", XLink_ATTR_type_VAL_arc},
   {XLink_ATTR_type, "extended", XLink_ATTR_type_VAL_extended},
   {XLink_ATTR_type, "locator", XLink_ATTR_type_VAL_locator},
   {XLink_ATTR_type, "none", XLink_ATTR_type_VAL_none_},
   {XLink_ATTR_type, "resource", XLink_ATTR_type_VAL_resource},
   {XLink_ATTR_type, "simple", XLink_ATTR_type_VAL_simple},
   {XLink_ATTR_type, "title", XLink_ATTR_type_VAL_title_},

   {0, "", 0}			/* Last entry. Mandatory */
};

#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   MapXLinkAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     MapXLinkAttribute (CHAR_T *attrName, AttributeType *attrType,
			    CHAR_T *elementName, ThotBool *level, Document doc)
#else
void     MapXLinkAttribute (Attr, attrType, elementName, level, doc)
CHAR_T        *attrName;
AttributeType *attrType;
CHAR_T*        elementName;
ThotBool      *level;
Document       doc;
#endif
{
  attrType->AttrSSchema = GetXLinkSSchema (doc);
  MapXMLAttribute (XLINK_TYPE, attrName, elementName, level, doc, &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapXLinkAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MapXLinkAttributeValue (CHAR_T* AttrVal, AttributeType attrType, int *value)
#else
void                MapXLinkAttributeValue (AttrVal, attrType, value)
CHAR_T*             AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (XLinkAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  XLinkAttrValueMappingTable[i].ThotAttr != 0)
      i++;
   if (XLinkAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
      do
	if (!ustrcasecmp (XLinkAttrValueMappingTable[i].XMLattrValue, AttrVal))
	   *value = XLinkAttrValueMappingTable[i].ThotAttrValue;
	else
	   i++;
      while (*value <= 0 && XLinkAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   XLinkAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      XLinkAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      XLinkAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType	attrType;
   int			attrKind;

   TtaGiveAttributeType (attr, &attrType, &attrKind);

   switch (attrType.AttrTypeNum)
     {
     case XLink_ATTR_actuate:
	break;

     default:
	break;
     }
}

/* end of module */
