/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1998-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * GraphMLbuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 */
 

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "GraphML.h"
#include "HTML.h"
#include "parser.h"
#include "style.h"

/* mapping table of attribute values */

static AttrValueMapping GraphMLAttrValueMappingTable[] =
{ 
   {GraphML_ATTR_arrowhead, TEXT("both"), GraphML_ATTR_arrowhead_VAL_both},
   {GraphML_ATTR_arrowhead, TEXT("end"), GraphML_ATTR_arrowhead_VAL_end_},
   {GraphML_ATTR_arrowhead, TEXT("none"), GraphML_ATTR_arrowhead_VAL_none_},
   {GraphML_ATTR_arrowhead, TEXT("start"), GraphML_ATTR_arrowhead_VAL_start},
   {GraphML_ATTR_linestyle_, TEXT("dashed"), GraphML_ATTR_linestyle__VAL_dashed_},
   {GraphML_ATTR_linestyle_, TEXT("dotted"), GraphML_ATTR_linestyle__VAL_dotted_},
   {GraphML_ATTR_linestyle_, TEXT("solid"), GraphML_ATTR_linestyle__VAL_solid_},
   {GraphML_ATTR_xml_space, TEXT("default"), GraphML_ATTR_xml_space_VAL_xml_space_default},
   {GraphML_ATTR_xml_space, TEXT("preserve"), GraphML_ATTR_xml_space_VAL_xml_space_preserve},
   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};

#define MaxMsgLength 200

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"
#include "XMLparser_f.h"


/*----------------------------------------------------------------------
   GraphMLGetDTDName
   Return in DTDname the name of the DTD to be used for parsing the
   content of element named elementName.
   This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLGetDTDName (STRING DTDname, STRING elementName)
#else
void      GraphMLGetDTDName (DTDname, elementName)
STRING DTDname;
STRING elementName;
 
#endif
{
   if (ustrcmp (elementName, TEXT("math")) == 0)
      ustrcpy (DTDname, TEXT("MathML"));
   else
      ustrcpy (DTDname, TEXT(""));
}

/*----------------------------------------------------------------------
   MapGraphMLAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void   MapGraphMLAttribute (CHAR_T *attrName, AttributeType *attrType,
			    CHAR_T* elementName, ThotBool *level, Document doc)
#else
void   MapGraphMLAttribute (Attr, attrType, elementName, level, doc)
CHAR_T        *attrName;
AttributeType *attrType;
CHAR_T        *elementName;
ThotBool      *level;
Document       doc;
#endif
{
  attrType->AttrSSchema = GetGraphMLSSchema (doc);
  MapXMLAttribute (GRAPH_TYPE, attrName, elementName, level, doc, &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapGraphMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MapGraphMLAttributeValue (CHAR_T* AttrVal, AttributeType attrType, int *value)
#else
void      MapGraphMLAttributeValue (AttrVal, attrType, value)
CHAR_T*             AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (GraphMLAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  GraphMLAttrValueMappingTable[i].ThotAttr != 0)
     i++;
   if (GraphMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
     do
       if (!ustrcasecmp (GraphMLAttrValueMappingTable[i].XMLattrValue,AttrVal))
	 *value = GraphMLAttrValueMappingTable[i].ThotAttrValue;
       else
	 i++;
     while (*value <= 0 && GraphMLAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   MapGraphMLEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void   MapGraphMLEntity (STRING entityName, STRING entityValue, STRING alphabet)
#else
void   MapGraphMLEntity (entityName, entityValue, alphabet)
STRING  entityName;
STRING  entityValue;
STRING  alphabet;
#endif
{
  entityValue[0] = EOS;
  *alphabet = EOS;
}

/*----------------------------------------------------------------------
   MapGraphMLEntityWithExpat
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void   MapGraphMLEntityWithExpat (STRING entityName,
				  int* entityValue,
				  STRING alphabet)
#else
void   MapGraphMLEntityWithExpat (entityName, entityValue, alphabet)
STRING  entityName;
int    *entityValue;
STRING  alphabet;
#endif
{
   *alphabet = EOS;
}

/*----------------------------------------------------------------------
   GraphMLEntityCreated
   A GraphML entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void    GraphMLEntityCreated (USTRING entityValue, Language lang,
			      STRING entityName, Document doc)
#else
void    GraphMLEntityCreated (entityValue, lang, entityName, doc)
USTRING     entityValue;
Language    lang;
STRING      entityName;
Document    doc;
#endif
{
}

/*----------------------------------------------------------------------
   GraphMLEntityCreatedWithExpat
   A GraphML entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  GraphMLEntityCreatedWithExpat (int entityValue, Language lang,
				     STRING entityName, ParserData *XmlContext)
#else
void  GraphMLEntityCreatedWithExpat (entityValue, lang, entityName, XmlContext)
int         entityValue;
Language    lang;
STRING      entityName;
ParserData *XmlContext;
#endif
{
}

/*----------------------------------------------------------------------
   ParseFillStrokeAttributes
   Create or update a specific presentation rule for element el that reflects
   the value of attribute attr, which is fill, stroke or stroke-width
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseFillStrokeAttributes (int attrType, Attribute attr, Element el, Document doc, ThotBool delete)
#else
void      ParseFillStrokeAttributes (attrType, attr, el, doc, delete)
int             attrType;
Attribute	attr;
Element		el;
Document	doc;
ThotBoool       delete;
#endif
{
#define buflen 50
   CHAR_T               css_command[buflen+20];
   int                  length;
   STRING               text;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* builds the equivalent CSS rule */
      if (attrType == GraphML_ATTR_fill)
	  usprintf (css_command, TEXT("fill: %s"), text);
      else if (attrType == GraphML_ATTR_stroke)
          usprintf (css_command, TEXT("stroke: %s"), text);
      else if (attrType == GraphML_ATTR_stroke_width)
          usprintf (css_command, TEXT("stroke-width: %s"), text);
      /* parse the CSS rule */
      ParseHTMLSpecificStyle (el, css_command, doc, 0, delete);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   CreateGraphicalLeaf
   Create a GRAPHICS_UNIT element as the last child of element el if it
   does not exist yet.
   Return that GRAPHICS_UNIT element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      CreateGraphicalLeaf (char shape, Element el, Document doc, ThotBool changeShape)
#else
static Element      CreateGraphicalLeaf (shape, el, doc, changeShape)
char                shape;
Element             el;
Document            doc;
ThotBool            changeShape;
#endif
{
  ElementType	   elType;
  Element	   leaf, child;
  AttributeType    attrType;
  Attribute        attr;
  CHAR_T           oldShape;

  leaf = NULL;
  child = TtaGetLastChild (el);
  if (child != NULL)
    /* there is a child element */
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == GraphML_EL_GRAPHICS_UNIT)
	{
	  oldShape = TtaGetGraphicsShape (child);
	  leaf = child;
	  if (oldShape == EOS || (changeShape && oldShape != shape))
	    TtaSetGraphicsShape (child, shape, doc);
	}
    }

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = GraphML_ATTR_stroke;
  if (leaf == NULL)
    /* create the graphical element */
    {
      elType.ElTypeNum = GraphML_EL_GRAPHICS_UNIT;
      leaf = TtaNewElement (doc, elType);
      if (child == NULL)
	TtaInsertFirstChild (&leaf, el, doc);
      else
	TtaInsertSibling (leaf, child, FALSE, doc);
      TtaSetGraphicsShape (leaf, shape, doc);
    }
  /* set the attribute stroke */
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      /* it's a new attribute */
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, TEXT("1"), el, doc);
    }
  ParseFillStrokeAttributes (attrType.AttrTypeNum, attr, el, doc, FALSE);
  return leaf;
}

/*----------------------------------------------------------------------
   CreateGraphicLeaf
   Create a graphical leaf element as the last child of element el,
   according to the type of el.
   Returns the created (or existing) element.
   When returning, closed indicates whether the shape is closed or not.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element         CreateGraphicLeaf (Element el, Document doc, ThotBool *closed, int arrowHead)
#else
Element         CreateGraphicLeaf (el, doc, closed, arrowHead)
Element		el;
Document	doc;
ThotBool       *closed;
int		arrowHead;

#endif
{
   ElementType elType;
   Element     leaf;
   char        shape;

   leaf = NULL;
   *closed = FALSE;
   elType = TtaGetElementType (el);
   switch (elType.ElTypeNum)
     {
     case GraphML_EL_Spline:
       switch (arrowHead)
	 {
	 case GraphML_ATTR_arrowhead_VAL_none_:
	   shape = 'B';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_start:
	   shape = 'F';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_end_:
	   shape = 'A';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_both:
	   shape = 'D';
	   break;
	 default:
	   shape = 'B';
	   break;
	 }
       leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
       break;

     case GraphML_EL_ClosedSpline:
       leaf = CreateGraphicalLeaf ('s', el, doc, FALSE);
       *closed = TRUE;
       break;

     case GraphML_EL_rect:
       leaf = CreateGraphicalLeaf ('C', el, doc, FALSE);
       *closed = TRUE;
       break;

     case GraphML_EL_circle:
       leaf = CreateGraphicalLeaf ('a', el, doc, FALSE);
       *closed = TRUE;
       break;

     case GraphML_EL_ellipse:
       leaf = CreateGraphicalLeaf ('c', el, doc, FALSE);
       *closed = TRUE;
       break;

     case GraphML_EL_line_:
       switch (arrowHead)
	 {
	 case GraphML_ATTR_arrowhead_VAL_none_:
	   shape = 'g';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_start:
	   shape = 'x';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_end_:
	   shape = 'y';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_both:
	   shape = 'z';
	   break;
	 default:
	   shape = 'g';
	   break;
	 }
       leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
       break;

     case GraphML_EL_polyline:
       switch (arrowHead)
	 {
	 case GraphML_ATTR_arrowhead_VAL_none_:
	   shape = 'S';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_start:
	   shape = 'N';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_end_:
	   shape = 'U';
	   break;
	 case GraphML_ATTR_arrowhead_VAL_both:
	   shape = 'M';
	   break;
	 default:
	   shape = 'S';
	   break;
	 }
       leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
       break;

     case GraphML_EL_polygon:
       leaf = CreateGraphicalLeaf ('p', el, doc, FALSE);
       *closed = TRUE;
       break;

     default:
       break;
     }
   return leaf;
}

/*----------------------------------------------------------------------
   CreateEnclosingElement
   Create an element of type elType, insert it in document doc at the
   position of element el and moves el and its following siblings within
   this new element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     CreateEnclosingElement (Element el, ElementType elType, Document doc)
#else
static void     CreateEnclosingElement (el, elType, doc)
Element		el;
ElementType	elType;
Document	doc;

#endif
{
   Element	new, prev, next, child;

   new = TtaNewElement (doc, elType);
   TtaInsertSibling (new, el, TRUE, doc);
   next = el;
   TtaNextSibling (&next);
   TtaRemoveTree (el, doc);
   TtaInsertFirstChild (&el, new, doc);
   prev = el;
   while (next != NULL)
     {
     child = next;
     TtaNextSibling (&next);
     TtaRemoveTree (child, doc);
     TtaInsertSibling (child, prev, FALSE, doc);
     prev = child;
     }
}

/*----------------------------------------------------------------------
  SetGraphicDepths forces a depth to each SVG child.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             SetGraphicDepths (Document doc, Element el)
#else /* __STDC__*/
void             SetGraphicDepths (doc, el)
Document         doc;
Element          el;
#endif /* __STDC__*/
{
  Element              child;
  ElementType          elType;
  SSchema	       graphSchema;
  PresentationValue    pval;
  PresentationContext  ctxt;

  /* look for the SVG root */
  graphSchema = GetGraphMLSSchema (doc);
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != GraphML_EL_GraphML || elType.ElSSchema != graphSchema)
    {
      elType.ElTypeNum = GraphML_EL_GraphML;
      elType.ElSSchema = graphSchema;
      el = TtaGetTypedAncestor (el, elType);
    }
  /* set depth to all SVG elements */
  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation is not a CSS rule */
  ctxt->cssLevel = 0;
  ctxt->destroy = FALSE;
  pval.typed_data.value = 0; /*TtaGetDepth (el, doc, 1)*/
  pval.typed_data.real = FALSE;
  child = TtaGetLastChild (el);
  while (child)
    {
      TtaSetStylePresentation (PRDepth, child, NULL, ctxt, pval);
      TtaPreviousSibling (&child);
      pval.typed_data.value += 5;
    }
  TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
   CopyUseContent
   Copy the subtree pointed by the href URI as a subtree of element el,
   which is od type use.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  CopyUseContent (Element el, Document doc, STRING href)
#else
void  CopyUseContent (el, doc, href)
Element el;
Document doc;
Attribute attr;
STRING href;
#endif
{
  Element              source, curEl, copy, child, elFound;
  ElementType          elType;
  Attribute            attr;
  AttributeType        attrType;
  SearchDomain         direction;
  int                  i, length, oldStructureChecking;
  STRING               id;

  /* look for an element with an id attribute with the same value as the
     href attribute */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = GraphML_ATTR_id;
  /* search backwards first */
  direction = SearchBackward;
  source = NULL;
  if (href[0] == '#') /* handles only internal links */
    for (i = 1; i <= 2 && source == NULL; i++)
      {
	curEl = el;
	do
	  {
	    TtaSearchAttribute (attrType, direction, curEl, &elFound, &attr);
	    if (attr)
	      /* an id attribute has been found */
	      {
		/* get its value */
		length = TtaGetTextAttributeLength (attr);
		id = TtaAllocString (length + 1);
		TtaGiveTextAttributeValue (attr, id, &length);
		/* compare with the xlink:href attribute of the use element */
		if (!ustrcasecmp (&href[1], id))
		  /* same  values. we found it */
		  source = elFound;
		TtaFreeMemory (id);
	      }
	    curEl = elFound;
	  }
	while (elFound && !source);
	/* search forward if not found */
	direction = SearchForward;
      }
  if (source)
    /* the element to be copied in the use element has been found */
    {
      /* remove the old copy if there is one */
      child = TtaGetFirstChild (el);
      while (child)
	  if (TtaIsTranscludedElement (child))
	    /* that's the old copy. remove it */
	    {
	      TtaRemoveTree (child, doc);
	      child = NULL;
	    }
	  else
	    TtaNextSibling (&child);

      /* make a copy of the source element */
      copy = TtaNewTranscludedElement (doc, source);
      /* remove the id attribute from the copy */
      attr = TtaGetAttribute (copy, attrType);
      /* insert it as the last child of the use element */
      if (attr)
	TtaRemoveAttribute (copy, attr, doc);
      child = TtaGetLastChild (el);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      if (child)
	TtaInsertSibling (copy, child, FALSE, doc);
      else
	TtaInsertFirstChild (&copy, el, doc);
      TtaSetStructureChecking (oldStructureChecking, doc);
    }
}

/*----------------------------------------------------------------------
   GraphMLElementComplete
   Check the Thot structure of the GraphML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLElementComplete (Element el, Document doc, int *error)
#else
void      GraphMLElementComplete (el, doc)
Element		el;
Document	doc;
int             *error
#endif
{
   ElementType		elType, parentType, newType;
   Element		child, parent, new, leaf;
   AttributeType        attrType;
   Attribute            attr;
   int                  length;
   PRule		fillPatternRule, newPRule;
   SSchema	        GraphMLSSchema;
   STRING               text, href;
   ThotBool		closedShape, parseCSS;

   *error = 0;
   elType = TtaGetElementType (el);
   GraphMLSSchema = GetGraphMLSSchema (doc);
   if (elType.ElSSchema != GraphMLSSchema)
     /* this is not a GraphML element. It's the HTML element <XMLGraphics>, or
	any other element containing a GraphML expression */
     {
     if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
	/* this element is empty. Create a GraphML element as it's child */
	{
	newType.ElSSchema = GraphMLSSchema;
	newType.ElTypeNum = GraphML_EL_GraphML;
	new = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the GraphML element */
        newType.ElTypeNum = GraphML_EL_GraphicsElement;
	child = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&child, new, doc);
	}
     }
   else
     {
     /* if the parent element is defined by a different SSchema, insert
        a GraphML root element between the element and its parent */
     parent = TtaGetParent (el);
     parentType = TtaGetElementType (parent);
     if (parentType.ElSSchema != elType.ElSSchema)
       {
        if (elType.ElTypeNum != GraphML_EL_GraphML)
	  {
	  newType.ElSSchema = GraphMLSSchema;
	  newType.ElTypeNum = GraphML_EL_GraphML;
	  CreateEnclosingElement (el, newType, doc);
	  }
	else
	  /* apply a deph rule to each child */
	  SetGraphicDepths (doc, el);
       }

     switch (elType.ElTypeNum)
       {
       case GraphML_EL_image:
	 /* it's an image element, create a PICTURE_UNIT child */
	 /* create the graphical element */
	 newType.ElSSchema = elType.ElSSchema;
	 newType.ElTypeNum = GraphML_EL_PICTURE_UNIT;
	 leaf = TtaNewElement (doc, newType);
	 TtaInsertFirstChild (&leaf, el, doc);
	 break;

       case GraphML_EL_use_:
	 /* it's a use element, make a transclusion of the element addressed
	    by its xlink_href attribute after the last child */
         /* first, get the xlink:href attribute */
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = GraphML_ATTR_xlink_href;
	 attr = TtaGetAttribute (el, attrType);
	 if (attr)
	   /* the use element has a xlink:href attribute */
	   {
	     /* get its value */
	     length = TtaGetTextAttributeLength (attr);
	     href = TtaAllocString (length + 1);
	     TtaGiveTextAttributeValue (attr, href, &length);
	     CopyUseContent (el, doc, href);
	     TtaFreeMemory (href);
	   }
	 break;

       case GraphML_EL_style__:
	 /* it's a style element, parse its contents as a style sheet */
	 /* Search the type attribute */
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = GraphML_ATTR_type;
	 attr = TtaGetAttribute (el, attrType);
	 parseCSS = FALSE;
	 if (attr == NULL)
	   /* no type attribute. Assume CSS by default */
	   parseCSS = TRUE;
	 else
	   /* the style element has a type attribute */
	   /* get its value */
	   {
	     length = TtaGetTextAttributeLength (attr);
	     text = TtaAllocString (length + 1);
	     TtaGiveTextAttributeValue (attr, text, &length);
	     if (!ustrcasecmp (text, TEXT("text/css")))
	       parseCSS = TRUE;
	     TtaFreeMemory (text);
	   }
	 if (parseCSS)
	   {
	     text = GetStyleContents (el);
	     if (text)
	       {
		 ReadCSSRules (doc, NULL, text, FALSE);
		 TtaFreeMemory (text);
	       }
	   }
	 break;
 
       default:
	 /* if it's a graphic primitive, create a GRAPHIC_UNIT leaf as a child
	    of the element, if it has not been done when creating attributes
	    (points, arrowhead, rx, ry) */
	 leaf = CreateGraphicLeaf (el, doc, &closedShape, 0);
	 /* if it's a closed shape, move the FillPattern rule to that leaf */
	 if (closedShape && leaf)
	   {
	     fillPatternRule = TtaGetPRule (el, PRFillPattern);
	     if (fillPatternRule != NULL)
	       {
		 newPRule = TtaCopyPRule (fillPatternRule);
		 TtaAttachPRule (leaf, newPRule, doc);
		 TtaRemovePRule (el, fillPatternRule, doc);
	       }
	   }
	 break;
       }
     }
}

/*----------------------------------------------------------------------
 UpdatePositionOfPoly
 Set position, width and height for an element polyline, Spline,
 polygon or ClosedSpline.
 Change coords of control points accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void     UpdatePositionOfPoly (Element el, Document doc, int minX, int minY, int maxX, int maxY)
#else /* __STDC__*/
void     UpdatePositionOfPoly (el, doc, minX, minY, maxX, maxY)
Element  el;
Document doc;
int      minX;
int      minY;
int      maxX;
int      maxY;
#endif /* __STDC__*/
{
   PRule                pRule;
   Element              leaf;
   int			height, width, i, nbPoints, x, y;
   TypeUnit		unit;
   PresentationValue    pval;
   PresentationContext  ctxt;

   leaf = TtaGetFirstChild (el);
   ctxt = TtaGetSpecificStyleContext (doc);
   /* the specific presentation is not a CSS rule */
   ctxt->cssLevel = 0;
   ctxt->destroy = FALSE;
   pval.typed_data.unit = STYLE_UNIT_PX;

   unit = UnPixel;
   width = maxX - minX;
   height = maxY - minY;
   nbPoints = TtaGetPolylineLength (leaf);
   for (i = 1; i <= nbPoints; i++)
      {
      TtaGivePolylinePoint (leaf, i, unit, &x, &y);
      TtaModifyPointInPolyline (leaf, i, unit, x-minX, y-minY, doc);
      }
   TtaChangeLimitOfPolyline (leaf, unit, width, height, doc);

   pRule = TtaGetPRule (el, PRHorizPos);
   if (pRule)
     {
     x = TtaGetPRuleValue (pRule);
     unit = TtaGetPRuleUnit (pRule);
     }
   else
     x = 0;
   pval.typed_data.value = x+minX;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);

   pRule = TtaGetPRule (el, PRVertPos);
   if (pRule)
     {
     y = TtaGetPRuleValue (pRule);
     unit = TtaGetPRuleUnit (pRule);
     }
   else
     y = 0;
   pval.typed_data.value = y+minY;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);

   pval.typed_data.value = width;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRWidth, el, NULL, ctxt, pval);
          
   pval.typed_data.value = height;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRHeight, el, NULL, ctxt, pval);

   TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
   CreatePoints
   Process the points attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            CreatePoints (Attribute attr, Element el, Document doc)
#else
void            CreatePoints (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   Element		leaf;
   TypeUnit		unit;
   STRING		text, ptr;
   int			length, x, y, nbPoints, maxX, maxY, minX, minY, i;
   ThotBool		closed;

   /* create (or get) the Graphics leaf according to the element type */
   leaf = CreateGraphicLeaf (el, doc, &closed, 0);
   if (leaf == NULL)
      return;

   /* text attribute. Get its value */
   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* first, delete all points in the polyline */
      nbPoints = TtaGetPolylineLength (leaf);
      for (i = 1; i <= nbPoints; i++)
	  TtaDeletePointInPolyline (leaf, i, doc);

      TtaGiveTextAttributeValue (attr, text, &length);
      ptr = text;
      nbPoints = 0;
      minX = minY = 32000;
      maxX = maxY = 0;
      unit = UnPixel;
      while (*ptr != EOS)
         {
         x = y = 0;
         usscanf (ptr, TEXT("%d"), &x);
         if (x > maxX)
            maxX = x;
         if (x < minX)
   	    minX = x;
         ptr = SkipInt (ptr);
         ptr = SkipSep (ptr);
         if (ptr)
            usscanf (ptr, TEXT("%d"), &y);
         if (y > maxY)
            maxY = y;
         if (y < minY)
   	    minY = y;
         ptr = SkipInt (ptr);
         ptr = SkipSep (ptr);
         nbPoints++;
         TtaAddPointInPolyline (leaf, nbPoints, unit, x, y, doc);
         }
      UpdatePositionOfPoly (el, doc, 0, 0, maxX, maxY);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParseCoordAttribute
   Create or update a specific presentation rule for element el that reflects
   the value of the x, y, cx, cy, x1, x2, y1, y2, dx, or dy attribute attr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseCoordAttribute (Attribute attr, Element el, Document doc)
#else
void      ParseCoordAttribute (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   int                  length, attrKind, ruleType;
   STRING               text, ptr;
   AttributeType        attrType;
   PresentationValue    pval;
   PresentationContext  ctxt;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* get the value of the x or y attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = TtaSkipWCBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	{
	  /* decide of the presentation rule to be created or updated */
	  TtaGiveAttributeType (attr, &attrType, &attrKind);
	  if (attrType.AttrTypeNum == GraphML_ATTR_x)
	    ruleType = PRHorizPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_y)
	    ruleType = PRVertPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_cx)
	    ruleType = PRHorizPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_cy)
	    ruleType = PRVertPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_x1)
	    ruleType = PRHorizPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_y1)
	    ruleType = PRVertPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_x2)
	    ruleType = PRWidth;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_y2)
	    ruleType = PRHeight;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_dx)
	    ruleType = PRHorizPos;
	  else if (attrType.AttrTypeNum == GraphML_ATTR_dy)
	    ruleType = PRVertPos;
	  else
	    return;
	  ctxt = TtaGetSpecificStyleContext (doc);
	  /* the specific presentation is not a CSS rule */
	  ctxt->cssLevel = 0;
	  ctxt->destroy = FALSE;
	  TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
	  TtaFreeMemory (ctxt);
	}
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParseWidthHeightAttribute
   Create or update a specific presentation rule for element el that reflects
   the value of attribute attr, which is width_, height_, r, rx, or ry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   ParseWidthHeightAttribute (Attribute attr, Element el, Document doc, ThotBool delete)
#else
ThotBool   ParseWidthHeightAttribute (attr, el, doc, delete)
Attribute  attr;
Element	   el;
Document   doc;
ThotBool   delete;

#endif
{
   AttributeType	attrType;
   ElementType          elType;
   Element              child;
   int			length, attrKind, ruleType;
   STRING		text, ptr;
   PresentationValue    pval;
   PresentationContext  ctxt;
   ThotBool             ret;

   ret = FALSE; /* let Thot perform normal operation */
   elType = TtaGetElementType (el);
   text = NULL;
   if (attr && !delete)
     {
       length = TtaGetTextAttributeLength (attr) + 2;
       text = TtaAllocString (length);
       if (!text)
	 return ret;
     }
   ctxt = TtaGetSpecificStyleContext (doc);
   /* the specific presentation is not a CSS rule */
   ctxt->cssLevel = 0;
   ctxt->destroy = FALSE;
   /* decide of the presentation rule to be created or updated */
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   if (attrType.AttrTypeNum == GraphML_ATTR_width_)
     ruleType = PRWidth;
   else if (attrType.AttrTypeNum == GraphML_ATTR_height_)
     ruleType = PRHeight;
   else if (attrType.AttrTypeNum == GraphML_ATTR_r)
     ruleType = PRWidth;
   else if (attrType.AttrTypeNum == GraphML_ATTR_rx)
     if (elType.ElTypeNum == GraphML_EL_rect)
       ruleType = PRXRadius;
     else
       ruleType = PRWidth;
   else if (attrType.AttrTypeNum == GraphML_ATTR_ry)
     if (elType.ElTypeNum == GraphML_EL_rect)
       ruleType = PRYRadius;
     else
       ruleType = PRHeight;
   else
     ruleType = PRWidth;
   if (delete)
      /* attribute deleted */
      if (ruleType != PRXRadius && ruleType != PRYRadius)
	/* attribute madatory. Do not delete */
	ret = TRUE;
      else
	{
	   /* that's the radius of a rounded corner. Get the graphics leaf
	      which has the specific presentation rule to be removed */
	  child = TtaGetFirstChild (el);
	  while (child &&
		 TtaGetElementType(child).ElTypeNum != GRAPHICS_UNIT)
	    TtaNextSibling (&child);
	  pval.typed_data.value = 0;
          pval.typed_data.unit = STYLE_UNIT_PX;
	  ctxt->destroy = FALSE;
	  TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
	  ctxt->destroy = TRUE;
	  TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
	  ret = FALSE; /* let Thot perform normal operation */
	}
   else
      /* attribute created or modified */
      {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length); 
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = TtaSkipWCBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	 {
	 if (ruleType != PRXRadius && ruleType != PRYRadius)
	   /* it's not attribute ry or ry for a rectangle */
	   {
	     if (pval.typed_data.value == 0)
	       /* a value of 0 disables rendering of this element */
	       ruleType = PRVisibility;
	     else
	       {
		 /* if there was a value of 0 previously, enable rendering */
		 ctxt->destroy = TRUE;
		 TtaSetStylePresentation (PRVisibility, el, NULL, ctxt, pval);
		 ctxt->destroy = FALSE;
	       }
	   }
	 if ((elType.ElTypeNum == GraphML_EL_ellipse ||
	      elType.ElTypeNum == GraphML_EL_circle) &&
	     (attrType.AttrTypeNum == GraphML_ATTR_r ||
	      attrType.AttrTypeNum == GraphML_ATTR_rx ||
	      attrType.AttrTypeNum == GraphML_ATTR_ry))
	   /* that's the radius of a circle or an ellipse,
	      multiply the value by 2 to set the width or height of the box */
	   pval.typed_data.value *= 2;
         if (ruleType == PRXRadius || ruleType == PRYRadius)
	   /* that's the radius of a rounded corner. Get the graphics leaf
	      which will receive the specific presentation rule */
	   {
	     child = TtaGetFirstChild (el);
	     while (child &&
		    TtaGetElementType(child).ElTypeNum != GRAPHICS_UNIT)
	        TtaNextSibling (&child);
	     el = child;
	   }
         /* set the specific presentation value of the box */
	 if (el)
	    TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
	 }
      if (text)
	TtaFreeMemory (text);
      }
   TtaFreeMemory (ctxt);
   return ret;
}

/*----------------------------------------------------------------------
   ParseTransformAttribute
   Parse the value of a transform attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseTransformAttribute (Attribute attr, Element el, Document doc, ThotBool delete)
#else
void      ParseTransformAttribute (attr, el, doc, delete)
Attribute	attr;
Element		el;
Document	doc;
ThotBool        delete

#endif
{
   int                  length, x, y;
   STRING               text, ptr;
   PresentationValue    pval;
   PresentationContext  ctxt;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text)
      {
      /* get the content of the translate attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute content */
      /* look only for the "translate" part */
      ptr = strstr (text, "translate");
      if (ptr)
	{
        x = 0;  y = 0;
        ptr += 9;
        ptr = TtaSkipWCBlanks (ptr);
        if (*ptr == TEXT('('))
	  {
          ctxt = TtaGetSpecificStyleContext (doc);
	  ptr++;
          ptr = ParseCSSUnit (ptr, &pval);
	  if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	    {
	    /* the specific presentation is not a CSS rule */
	    ctxt->cssLevel = 0;
            ctxt->destroy = delete;
	    TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);
	    }	    
	  ptr = TtaSkipWCBlanks (ptr);
          if (*ptr == TEXT(','))
	    {
	    ptr++;
	    ptr = ParseCSSUnit (ptr, &pval);
	    if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	      {
	      ctxt->cssLevel = 0;
              ctxt->destroy = delete;
	      TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);
	      }
	    ptr = TtaSkipWCBlanks (ptr);
	    }
	  else if (*ptr == TEXT(')'))
	    {
	    ptr++;
	    pval.typed_data.unit = STYLE_UNIT_PX;
	    pval.typed_data.value = 0;
	    ctxt->cssLevel = 0;
	    ctxt->destroy = delete;
	    TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);
	    }
          TtaFreeMemory (ctxt);
	  }
	}
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   GraphMLAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      GraphMLAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType	attrType;
   ElementType          elType;
   Element		leaf;
   int			attrKind, value;
   ThotBool		closed;

   TtaGiveAttributeType (attr, &attrType, &attrKind);

   switch (attrType.AttrTypeNum)
     {
     case GraphML_ATTR_height_:
     case GraphML_ATTR_width_:
     case GraphML_ATTR_r:
	ParseWidthHeightAttribute (attr, el, doc, FALSE);
	break;
     case GraphML_ATTR_fill:
     case GraphML_ATTR_stroke:
     case GraphML_ATTR_stroke_width:
        ParseFillStrokeAttributes (attrType.AttrTypeNum, attr, el, doc, FALSE);
	break;
     case GraphML_ATTR_transform:
        ParseTransformAttribute (attr, el, doc, FALSE);
	break;
     case GraphML_ATTR_points:
	CreatePoints (attr, el, doc);
	break;
     case GraphML_ATTR_arrowhead:
	value = TtaGetAttributeValue (attr);
	leaf = CreateGraphicLeaf (el, doc, &closed, value);
	break;
     case GraphML_ATTR_x:
     case GraphML_ATTR_y:
     case GraphML_ATTR_cx:
     case GraphML_ATTR_cy:
     case GraphML_ATTR_x1:
     case GraphML_ATTR_y1:
     case GraphML_ATTR_x2:
     case GraphML_ATTR_y2:
     case GraphML_ATTR_dx:
     case GraphML_ATTR_dy:
	ParseCoordAttribute (attr, el, doc);
	break;
     case GraphML_ATTR_rx:
     case GraphML_ATTR_ry:
        elType = TtaGetElementType (el);
	if (elType.ElTypeNum == GraphML_EL_rect)
	  /* attribute rx or ry for a rect element.
	     create the GRAPHICS_UNIT child to put the corresponding
             specific presentation rule on it */
	   leaf = CreateGraphicLeaf (el, doc, &closed, 0);	   
	ParseWidthHeightAttribute (attr, el, doc, FALSE);
	break;
     default:
	break;
     }
}

/* end of module */
