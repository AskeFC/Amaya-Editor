/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Set of functions to parse CSS rules:
 * Each ParseCSS function calls one or more Thot style API function.
 *
 * Author: I. Vatton (INRIA)
 *
 */
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "XLink.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif

static char        *TargetDocumentURL = NULL;
static int          OldWidth;
static int          OldHeight;
#define buflen 50

#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#ifdef GRAPHML
#include "GraphMLbuilder_f.h"
#endif
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "Mathedit_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "tree.h"
#include "XHTMLbuilder_f.h"
#include "XLinkedit_f.h"

#ifdef _WINDOWS
extern HWND currentWindow;
#include "wininclude.h"
#endif /* _WINDOWS */

static ThotBool AttrHREFundoable = FALSE;

/*----------------------------------------------------------------------
   SetTargetContent sets the new value of Target.                  
  ----------------------------------------------------------------------*/
void                SetTargetContent (Document doc, Attribute attrNAME)
{
   int                 length;

   /* the document that issues the command Set target becomes the target doc */
   if (TargetDocumentURL != NULL)
      TtaFreeMemory (TargetDocumentURL);
   if (doc != 0)
      {
       length = strlen (DocumentURLs[doc]);
       TargetDocumentURL = TtaGetMemory (length + 1);
       strcpy (TargetDocumentURL, DocumentURLs[doc]);
     }

   if (TargetName != NULL)
     {
	TtaFreeMemory (TargetName);
	TargetName = NULL;
     }
   if (attrNAME != NULL)
     {
	/* get a buffer for the NAME */
	length = TtaGetTextAttributeLength (attrNAME);
	TargetName = TtaGetMemory (length + 1);
	/* copy the NAME attribute into TargetName */
	TtaGiveTextAttributeValue (attrNAME, TargetName, &length);
     }
}


/*----------------------------------------------------------------------
   LinkToPreviousTarget
   If current selection is within an anchor, change that link, otherwise
   create a link.
  ----------------------------------------------------------------------*/
void                LinkToPreviousTarget (Document doc, View view)
{
   Element             el;
   int                 firstSelectedChar, i;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       (TargetDocumentURL == NULL || TargetDocumentURL[0] == EOS))
     /* no target available */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       !strcmp (TargetDocumentURL, DocumentURLs[doc]))
     /* it's not a valid target */
     return;

   UseLastTarget = TRUE;
   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   if (el != NULL)
     {
       /* Look if there is an enclosing anchor element */
       el = SearchAnchor (doc, el, TRUE, TRUE);
       if (el == NULL)
	 {
	   /* The link element is a new created one */
	   IsNewAnchor = TRUE;
	   /* no anchor element, create a new link */
	   CreateAnchor (doc, view, TRUE);
	 }
       else
	 {
	   /* The link element already exists */
	   IsNewAnchor = FALSE;
	   /* There is an anchor. Just points to the last created target */
	   SetREFattribute (el, doc, TargetDocumentURL, TargetName);
	 }
     }
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AddAccessKey (NotifyAttribute *event)
{
   char              buffer[2];
   int                 length;

   /* get the access key */
   length = 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   if (length > 0)
     TtaAddAccessKey (event->document, (unsigned int)buffer[0], event->element);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
ThotBool RemoveAccessKey (NotifyAttribute *event)
{
   char              buffer[2];
   int                 length;

   /* get the access key */
   length = 2;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   if (length > 0)
     TtaRemoveAccessKey (event->document, (int)buffer[0]);
  return FALSE; /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   RemoveLink: destroy the link element and remove CSS rules when the
   link points to a CSS file.
  ----------------------------------------------------------------------*/
void RemoveLink (Element el, Document doc)
{
  ElementType	    elType;
  AttributeType     attrType;
  Attribute         attr;
  char              buffer[MAX_LENGTH];
  char              pathname[MAX_LENGTH], documentname[MAX_LENGTH];   
  int               length;

  /* Search the refered image */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (IsCSSLink (el, doc))
    {
      /* it's a link to a style sheet. Remove that style sheet */
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	{
	  /* copy the HREF attribute into the buffer */
	  length = MAX_LENGTH;
	  TtaGiveTextAttributeValue (attr, buffer, &length);
	  NormalizeURL (buffer, doc, pathname, documentname, NULL);
	  RemoveStyleSheet (pathname, doc, TRUE, TRUE);
	}
    }
}
/*----------------------------------------------------------------------
   DeleteLink                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteLink (NotifyElement * event)
{
  RemoveLink (event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   SetREFattribute  sets the HREF or CITE attribue of the element to      
   the concatenation of targetURL and targetName.
  ----------------------------------------------------------------------*/
void SetREFattribute (Element element, Document doc, char *targetURL,
		      char *targetName)
{
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr;
   SSchema	       HTMLSSchema;
   char               *value, *base;
   char                tempURL[MAX_LENGTH];
   int                 length;
   ThotBool            new, oldStructureChecking, isHTML;

   if (AttrHREFundoable)
      TtaOpenUndoSequence (doc, element, element, 0, 0);

   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   elType = TtaGetElementType (element);
   isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
   if (isHTML)
     {
       attrType.AttrSSchema = HTMLSSchema;
       if (elType.ElTypeNum == HTML_EL_Quotation ||
	   elType.ElTypeNum == HTML_EL_Block_Quote ||
	   elType.ElTypeNum == HTML_EL_INS ||
	   elType.ElTypeNum == HTML_EL_DEL)
	 attrType.AttrTypeNum = HTML_ATTR_cite;
       else
	 attrType.AttrTypeNum = HTML_ATTR_HREF_;
     }
   else
     {
       /* the origin of the link is not a HTML element */
       /* create a XLink link */
       attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
       if (!attrType.AttrSSchema)
	 attrType.AttrSSchema = TtaNewNature (doc, TtaGetDocumentSSchema (doc),
					      "XLink", "XLinkP");
       attrType.AttrTypeNum = XLink_ATTR_href_;
       /* create a xlink:type attribute with value "simple" */
       if (TtaIsLeaf (elType))
	 element = TtaGetParent (element);
       SetXLinkTypeSimple (element, doc, AttrHREFundoable);
     }
   attr = TtaGetAttribute (element, attrType);
   if (attr == 0)
     {
       /* create an attribute HREF for the element */
       attr = TtaNewAttribute (attrType);
       /* this element may be in a different namespace, so don't check
	  validity */
       oldStructureChecking = TtaGetStructureChecking (doc);
       TtaSetStructureChecking (0, doc);
       TtaAttachAttribute (element, attr, doc);
       TtaSetStructureChecking (oldStructureChecking, doc);
       new = TRUE;
     }
   else
     {
       new = FALSE;
       if (AttrHREFundoable)
	 TtaRegisterAttributeReplace (attr, element, doc);
     }

   /* build the complete target URL */
   if (targetURL != NULL && strcmp(targetURL, DocumentURLs[doc]))
     strcpy (tempURL, targetURL);
   else
     tempURL[0] = EOS;
   if (targetName != NULL)
     {
       strcat (tempURL, "#");
       strcat (tempURL, targetName);
     }

   if (tempURL[0] == EOS)
     {
       /* get a buffer for the attribute value */
       length = TtaGetTextAttributeLength (attr);
       if (length == 0)
	 /* no given value */
	 TtaSetAttributeText (attr, "XX", element, doc);
     }
   else
     {
       /* set the relative value or URL in attribute HREF */
       base = GetBaseURL (doc);
       value = MakeRelativeURL (tempURL, base);
       TtaFreeMemory (base);
       if (*value == EOS)
	 TtaSetAttributeText (attr, "./", element, doc);
       else
	 TtaSetAttributeText (attr, value, element, doc);
       TtaFreeMemory (value);
     }

   /* register the new value of the HREF attribute in the undo queue */
   if (AttrHREFundoable && new)
      TtaRegisterAttributeCreate (attr, element, doc);

   /* is it a link to a CSS file? */
   if (tempURL[0] != EOS)
      if (elType.ElTypeNum == HTML_EL_LINK && isHTML &&
	  (LinkAsCSS || IsCSSName (targetURL)))
	 {
	   LinkAsCSS = FALSE;
	   LoadStyleSheet (targetURL, doc, element, NULL, CSS_ALL);
	   attrType.AttrTypeNum = HTML_ATTR_REL;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr == 0)
	     {
	       /* create an attribute HREF for the element */
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (element, attr, doc);
	       new = TRUE;
	     }
	   else
	     {
	       new = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, "stylesheet", element, doc);
	   if (AttrHREFundoable && new)
	       TtaRegisterAttributeCreate (attr, element, doc);

	   attrType.AttrTypeNum = HTML_ATTR_Link_type;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr == 0)
	     {
	       /* create an attribute HREF for the element */
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (element, attr, doc);
	       new = TRUE;
	     }
	   else
	     {
	       new = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, "text/css", element, doc);	   
	   if (AttrHREFundoable && new)
	       TtaRegisterAttributeCreate (attr, element, doc);
	 }
   if (AttrHREFundoable)
     {
       TtaCloseUndoSequence (doc);
       AttrHREFundoable = FALSE;
     }
   TtaSetDocumentModified (doc);
   TtaSetStatus (doc, 1, " ", NULL);
}


/*----------------------------------------------------------------------
   ChangeTitle
   Update the TITLE for the document.
  ----------------------------------------------------------------------*/
void ChangeTitle (Document doc, View view)
{
   ElementType         elType;
   Element             el, child;
   Language            lang;
   int                 length;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   /* search the Title element */
   el = TtaGetMainRoot (doc);
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
     /* it's a HTML document */
     {
       elType.ElTypeNum = HTML_EL_TITLE;
       el = TtaSearchTypedElement (elType, SearchForward, el);
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   /* insert the text element */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   child = TtaNewElement (doc, elType);
	   TtaInsertFirstChild  (&child, el, doc);
	 }
       length = MAX_LENGTH;
       TtaGiveTextContent (child, Answer_text, &length, &lang);
       CurrentDocument = doc;
#ifndef _WINDOWS 
       TtaNewForm (BaseDialog + TitleForm, TtaGetViewFrame (doc, 1),
		   TtaGetMessage (1, BTitle), TRUE, 2, 'L', D_CANCEL);
       TtaNewTextForm (BaseDialog + TitleText, BaseDialog + TitleForm, "",
		       50, 1, FALSE);
       /* initialise the text field in the dialogue box */
       TtaSetTextForm (BaseDialog + TitleText, Answer_text);
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + TitleForm, FALSE);
#else /* _WINDOWS */
       CreateTitleDlgWindow (TtaGetViewFrame (doc, view), Answer_text);
#endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   SetNewTitle
   Update the TITLE for the document.
  ----------------------------------------------------------------------*/
void SetNewTitle (Document doc)
{
   ElementType         elType;
   Element             el, child;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   /* search the Title element */
   el = TtaGetMainRoot (doc);
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
     {
       elType.ElTypeNum = HTML_EL_TITLE;
       el = TtaSearchTypedElement (elType, SearchForward, el);
       child = TtaGetFirstChild (el);
       if (child)
	 {
	    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
	    TtaRegisterElementReplace (el, doc);
	    TtaSetTextContent (child, Answer_text, TtaGetDefaultLanguage (),
			       doc);
	    TtaCloseUndoSequence (doc);
	    TtaSetDocumentModified (doc);
	    SetWindowTitle (doc, doc, 0);
	    if (DocumentSource[doc])
	      SetWindowTitle (doc, DocumentSource[doc], 0);
	 }
     }
}

/*----------------------------------------------------------------------
   SelectDestination selects the destination of the el Anchor.     
  ----------------------------------------------------------------------*/
void SelectDestination (Document doc, Element el, ThotBool withUndo)
{
   Element             targetEl;
   ElementType	       elType;
   Document            targetDoc;
   Attribute           attr;
   AttributeType       attrType;
   char               *buffer = NULL;
   int                 length;
#ifndef _WINDOWS
   int                 i;
   char              s[MAX_LENGTH];
#endif
   ThotBool            isHTML;

   /* ask the user to select target document and target anchor */
   TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
   TtaClickElement (&targetDoc, &targetEl);
   if (targetDoc != 0)
     isHTML = !(strcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (targetDoc)),
			 "HTML"));
   else
     isHTML = FALSE;

   if (targetDoc != 0 && targetEl != NULL && DocumentURLs[targetDoc] != NULL)
     {
       if (isHTML)
	 {
	   /* get attrName of the enclosing end anchor */
	   attr = GetNameAttr (targetDoc, targetEl);
	   /* the document becomes the target doc */
	   SetTargetContent (targetDoc, attr);
	 }
       else
	 SetTargetContent (targetDoc, NULL);
     }
   else
     {
	targetDoc = doc;
	SetTargetContent (0, NULL);
     }

   AttrHREFelement = el;
   AttrHREFdocument = doc;
   AttrHREFundoable = withUndo;
   if (doc != targetDoc || TargetName != NULL)
     /* the user has clicked another document or a target element */
     /* create the attribute HREF or CITE */
     SetREFattribute (el, doc, TargetDocumentURL, TargetName);
   else
     /* the user has clicked the same document: pop up a dialogue box
	to allow the user to type the target URI */
     {
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
	/* Dialogue form to insert HREF name */

	/* If the anchor has an HREF attribute, put its value in the form */
	elType = TtaGetElementType (el);
	if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
	  /* it's an HTML element */
	  {
	    attrType.AttrSSchema = elType.ElSSchema;
	    /* search the HREF or CITE attribute */
	    if (elType.ElTypeNum == HTML_EL_Quotation ||
		elType.ElTypeNum == HTML_EL_Block_Quote ||
		elType.ElTypeNum == HTML_EL_INS ||
		elType.ElTypeNum == HTML_EL_DEL)
	      attrType.AttrTypeNum = HTML_ATTR_cite;
	    else
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  }
	else
	  {
	    attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
	    attrType.AttrTypeNum = XLink_ATTR_href_;
	  }
	attr = TtaGetAttribute (el, attrType);
	AttrHREFvalue[0] = EOS;
	if (attr != 0)
	  {
	     /* get a buffer for the attribute value */
	     length = TtaGetTextAttributeLength (attr);
	     buffer = TtaGetMemory (length + 1);
	     /* copy the HREF attribute into the buffer */
	     TtaGiveTextAttributeValue (attr, buffer, &length);
	     strcpy (AttrHREFvalue, buffer);
	     TtaFreeMemory (buffer);
	  }

#ifndef _WINDOWS
	/* Dialogue form for open URL or local */
	i = 0;
	strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
	
	TtaNewSheet (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),
		     TtaGetMessage (AMAYA, AM_ATTRIBUTE), 3, s,
		     TRUE, 2, 'L', D_CANCEL);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_LOCATION), 50, 1, TRUE);
	TtaNewLabel (BaseDialog + HREFLocalName,
		     BaseDialog + AttrHREFForm, " ");
	/* initialise the text field in the dialogue box */
	TtaSetTextForm (BaseDialog + AttrHREFText, AttrHREFvalue);
	strcpy (s, DirectoryName);
	strcat (s, DIR_STR);
	strcat (s, DocumentName);
	TtaSetDialoguePosition ();
	TtaShowDialogue (BaseDialog + AttrHREFForm, TRUE);
#else  /* _WINDOWS */
    CreateHRefDlgWindow (currentWindow, AttrHREFvalue, DocSelect, DirSelect, 2);
#endif  /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   GetNameAttr return the NAME attribute of the enclosing Anchor   
   element or the ID attribute of (an ascendant of) the selected element
   or NULL.
  ----------------------------------------------------------------------*/
Attribute           GetNameAttr (Document doc, Element selectedElement)
{
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   SSchema	       HTMLSSchema;

   attr = NULL;		/* no NAME attribute yet */
   if (selectedElement != NULL)
     {
        elType = TtaGetElementType (selectedElement);
	HTMLSSchema = TtaGetSSchema ("HTML", doc);
	attrType.AttrSSchema = HTMLSSchema;
	if (elType.ElSSchema == HTMLSSchema &&
	    elType.ElTypeNum == HTML_EL_Anchor)
	  el = selectedElement;
	else
	  {
	    elType.ElSSchema = HTMLSSchema;
	    elType.ElTypeNum = HTML_EL_Anchor;
	    el = TtaGetTypedAncestor (selectedElement, elType);
	  }

	if (el != NULL)
	  {
	     /* the ascending Anchor element has been found */
	     /* get the NAME attribute of element Anchor */
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attr = TtaGetAttribute (el, attrType);
	  }
	else
	  {
	    /* no ascending Anchor element */
	    /* get the ID attribute of the selected element */
	    attrType.AttrTypeNum = HTML_ATTR_ID;
	    attr = TtaGetAttribute (selectedElement, attrType);
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = MathML_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#ifdef GRAPHML
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema ("GraphML", doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = GraphML_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#endif
	  }
     }
   return (attr);
}


/*----------------------------------------------------------------------
   CreateTargetAnchor creates a NAME or ID attribute with a default    
   value for element el.
   If the withUndo parameter is true, we'll register the undo sequence.
   If the forceID parameter, we'll always use an ID attribute, rather
   than a NAME one in some cases.
  ----------------------------------------------------------------------*/
void CreateTargetAnchor (Document doc, Element el, ThotBool forceID,
			 ThotBool withUndo)
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elText;
   SSchema	       HTMLSSchema;
   Language            lang;
   char               *text;
   char               *url = TtaGetMemory (MAX_LENGTH);
   int                 length, i, space;
   ThotBool            found;
   ThotBool            withinHTML, new;

   elType = TtaGetElementType (el);
   withinHTML = !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML");

   /* get a NAME or ID attribute */
   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrSSchema = HTMLSSchema;
   if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor ||
		      elType.ElTypeNum == HTML_EL_MAP))
     {
       if (forceID)
	 attrType.AttrTypeNum = HTML_ATTR_ID;
       else
	 attrType.AttrTypeNum = HTML_ATTR_NAME;
     }
   else
     {
     if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = MathML_ATTR_id;
       }
     else
#ifdef GRAPHML
     if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "GraphML") == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = GraphML_ATTR_id;
       }
     else
#endif
       attrType.AttrTypeNum = HTML_ATTR_ID;
     }
   attr = TtaGetAttribute (el, attrType);

   if (attr == NULL)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	new = TRUE;
     }
   else
     {
     new = FALSE;
     if (withUndo)
        TtaRegisterAttributeReplace (attr, el, doc);
     }

   /* build a value for the new attribute */
   if (withinHTML && elType.ElTypeNum == HTML_EL_MAP)
     {
       /* mapxxx for a map element */
       strcpy (url, "map");
     }
   else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
     {
       /* linkxxx for a link element */
       strcpy (url, "link");
     }
   else
       /* get the content for other elements */
     {
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	elText = TtaSearchTypedElement (elType, SearchInTree, el);
	if (elText != NULL)
	  {
	    /* first word longer than 3 characters */
	    length = 50;
	    TtaGiveTextContent (elText, url, &length, &lang);
	    space = 0;
	    i = 0;
	    found = FALSE;
	    url[length++] = EOS;
	    while (!found && i < length)
	      {
		if (url[i] == ' ' || url[i] == EOS)
		  {
		    found = (i - space > 3 || (i != space && url[i] == EOS));
		    if (found)
		      {
			/* url = the word */
			if (i > space + 10)
			  /* limit the word length */
			  i = space + 10;
			url[i] = EOS;
			if (space != 0)
			  strcpy (url, &url[space]);
		      }
		    i++;
		    space = i;
		  }
		else if (url[i] == '_' ||
			 url[i] == ':' ||
			 ((unsigned int) url[i] >= 65 &&
			  (unsigned int) url[i] <= 90) ||
			 ((unsigned int) url[i] >= 81 &&
			  (unsigned int) url[i] <= 127) ||
			 (i > 0 && (unsigned int) url[i] >= 48 &&
			  (unsigned int) url[i] <= 57))
		  /* valid character for an ID */
		  i++;
		else
		  /* invalid name for an ID */
		  i = length;
	      }

	    if (!found)
	      {
		/* label of the element */
		text = TtaGetElementLabel (el);
		strcpy (url, text);
	      }
	  }
	else
	  {
	    /* get the element's label if there is no text */
	    text = TtaGetElementLabel (el);
	    strcpy (url, text);
	  }
     }
   /* copie the text into the NAME attribute */
   TtaSetAttributeText (attr, url, el, doc);
   TtaSetDocumentModified (doc);
   /* Check the attribute value to make sure that it's unique within */
   /* the document */
   MakeUniqueName (el, doc);
   /* set this new end-anchor as the new target */
   SetTargetContent (doc, attr);
   if (withUndo && new)
       TtaRegisterAttributeCreate (attr, el, doc);
   TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
   CreateAnchor creates a link or target element.                  
  ----------------------------------------------------------------------*/
void                CreateAnchor (Document doc, View view, ThotBool createLink)
{
  Element             first, last, el, next, parent;
  Element             parag, prev, child, anchor;
  SSchema             HTMLSSchema;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  int                 c1, cN, lg, i;
  ThotBool            noAnchor;

  parag = NULL;
  HTMLSSchema = TtaGetSSchema ("HTML", doc);
  dispMode = TtaGetDisplayMode (doc);
  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cN);

  /* Check whether the selected elements are a valid content for an anchor */
  elType = TtaGetElementType (first);
  if (elType.ElTypeNum == HTML_EL_Anchor &&
      TtaSameSSchemas (elType.ElSSchema, HTMLSSchema) &&
      first == last)
    /* add an attribute on the current anchor */
    anchor = first;
  else
    {
      /* check whether the selection is within an anchor */
      if (TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
	el = SearchAnchor (doc, first, TRUE, TRUE);
      else
	el = NULL;
      if (el != NULL)
	/* add an attribute on this anchor */
	anchor = el;
      else
	{
	  el = first;
	  noAnchor = FALSE;
	  
	  while (!noAnchor && el != NULL)
	    {
	      elType = TtaGetElementType (el);
	      if (!TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
		noAnchor = TRUE;
	      else if (elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
		  elType.ElTypeNum != HTML_EL_Teletype_text &&
		  elType.ElTypeNum != HTML_EL_Italic_text &&
		  elType.ElTypeNum != HTML_EL_Bold_text &&
		  elType.ElTypeNum != HTML_EL_Underlined_text &&
		  elType.ElTypeNum != HTML_EL_Struck_text &&
		  elType.ElTypeNum != HTML_EL_Big_text &&
		  elType.ElTypeNum != HTML_EL_Small_text &&
		  elType.ElTypeNum != HTML_EL_Emphasis &&
		  elType.ElTypeNum != HTML_EL_Strong &&
		  elType.ElTypeNum != HTML_EL_Def &&
		  elType.ElTypeNum != HTML_EL_Code &&
		  elType.ElTypeNum != HTML_EL_Sample &&
		  elType.ElTypeNum != HTML_EL_Keyboard &&
		  elType.ElTypeNum != HTML_EL_Variable &&
		  elType.ElTypeNum != HTML_EL_Cite &&
		  elType.ElTypeNum != HTML_EL_ABBR &&
		  elType.ElTypeNum != HTML_EL_ACRONYM &&
		  elType.ElTypeNum != HTML_EL_INS &&
		  elType.ElTypeNum != HTML_EL_DEL &&
		  elType.ElTypeNum != HTML_EL_PICTURE_UNIT &&
		  elType.ElTypeNum != HTML_EL_Applet &&
		  elType.ElTypeNum != HTML_EL_Object &&
		  elType.ElTypeNum != HTML_EL_Font_ &&
		  elType.ElTypeNum != HTML_EL_SCRIPT &&
		  elType.ElTypeNum != HTML_EL_MAP &&
		  elType.ElTypeNum != HTML_EL_Quotation &&
		  elType.ElTypeNum != HTML_EL_Subscript &&
		  elType.ElTypeNum != HTML_EL_Superscript &&
		  elType.ElTypeNum != HTML_EL_Span &&
		  elType.ElTypeNum != HTML_EL_BDO &&
		  elType.ElTypeNum != HTML_EL_IFRAME )
		noAnchor = TRUE;
	      if (el == last)
		el = NULL;
	      else
		TtaGiveNextSelectedElement (doc, &el, &i, &i);
	    }
	  
	  if (noAnchor)
	    {
	      if (createLink || el != NULL)
		{
		elType = TtaGetElementType (first);
		if (first == last && c1 == 0 && cN == 0 && createLink &&
		    !TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
		   /* a single element is selected and it's not a HTML elem
		      nor a character string */
		  {
		    if (UseLastTarget)
		      /* points to the last created target */
		      SetREFattribute (first, doc, TargetDocumentURL,
				       TargetName);
		    else
		      /* select the destination */
		      SelectDestination (doc, first, TRUE);
		  }
		else
		  /* cannot create an anchor here */
		  TtaSetStatus (doc, 1,
				TtaGetMessage (AMAYA, AM_INVALID_ANCHOR1),
				NULL);
		}
	      else
		  /* create an ID for target element */
		{
		  TtaOpenUndoSequence (doc, first, last, c1, cN);
		  CreateTargetAnchor (doc, first, FALSE, TRUE);
		  TtaCloseUndoSequence (doc);
		}
	      return;
	    }
	  /* check if the anchor to be created is within an anchor element */
	  else if (SearchAnchor (doc, first, TRUE, TRUE) != NULL ||
		   SearchAnchor (doc, last, TRUE, TRUE) != NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR2),
			    NULL);
	      return;
	    }

	  /* stop displaying changes that will be made in the document */
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);
	  /* remove selection before modifications */
	  TtaUnselect (doc);

	  TtaOpenUndoSequence (doc, first, last, c1, cN);

	  /* process the last selected element */
	  elType = TtaGetElementType (last);
	  if (cN > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      lg = TtaGetTextLength (last);
	      if (cN < lg)
		/* split the last text */
		{
		TtaRegisterElementReplace (last, doc);
		TtaSplitText (last, cN, doc);
		next = last;
		TtaNextSibling (&next);
		TtaRegisterElementCreate (next, doc);
		}
	    }
	  /* process the first selected element */
	  elType = TtaGetElementType (first);
	  if (c1 > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      /* split the first selected text element */
	      el = first;
	      TtaRegisterElementReplace (first, doc);
	      TtaSplitText (first, c1 - 1, doc);
	      TtaNextSibling (&first);
	      TtaRegisterElementCreate (first, doc);
	      if (last == el)
		{
		  /* we have to change last selection because the element
		     was split */
		  last = first;
		}
	    }

	  /* Create the corresponding anchor */
	  elType.ElTypeNum = HTML_EL_Anchor;
	  anchor = TtaNewElement (doc, elType);
	  if (createLink)
	    {
	      /* create an attribute HREF for the new anchor */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      attr = TtaGetAttribute (anchor, attrType);
	      if (attr == NULL)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (anchor, attr, doc);
		}
	    }
	  
	  /* Check if the first element is included within a paragraph */
	  elType = TtaGetElementType (TtaGetParent (first));
	  if (elType.ElTypeNum == HTML_EL_BODY ||
	      elType.ElTypeNum == HTML_EL_Division ||
	      elType.ElTypeNum == HTML_EL_Object_Content ||
	      elType.ElTypeNum == HTML_EL_Data_cell ||
	      elType.ElTypeNum == HTML_EL_Heading_cell ||
	      elType.ElTypeNum == HTML_EL_Block_Quote)
	    {
	      elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      parag = TtaNewElement (doc, elType);
	      TtaInsertSibling (parag, last, FALSE, doc);
	      TtaInsertFirstChild (&anchor, parag, doc);
	    }
	  else
	      TtaInsertSibling (anchor, last, FALSE, doc);
	  
	  /* move the selected elements within the new Anchor element */
	  child = first;
	  prev = NULL;
	  parent = TtaGetParent (anchor);
	  while (child != NULL)
	    {
	      /* prepare the next element in the selection, as the current
		 element will be moved and its successor will no longer
		 be accessible */
	      next = child;
	      TtaNextSibling (&next);
	      /* remove the current element */
	      TtaRegisterElementDelete (child, doc);
	      TtaRemoveTree (child, doc);
	      /* insert it as a child of the new anchor element */
	      if (prev == NULL)
		TtaInsertFirstChild (&child, anchor, doc);
	      else
		TtaInsertSibling (child, prev, FALSE, doc);
	      /* get the next element in the selection */
	      prev = child;
	      if (child == last || next == parent)
		/* avoid to move the previous element or the parent
		   of the anchor into the anchor */
		child = NULL;
	      else
		child = next;
	    }
	}
    }

  TtaSetDocumentModified (doc);
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  TtaSelectElement (doc, anchor);
  if (createLink )
    {
      if (UseLastTarget)
	/* points to the last created target */
	SetREFattribute (anchor, doc, TargetDocumentURL, TargetName);
      else
	/* Select the destination */
	SelectDestination (doc, anchor, FALSE);
      /* The anchor element must have an HREF attribute */
      /* create an attribute PseudoClass = link */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
      attr = TtaGetAttribute (anchor, attrType);
      if (attr == NULL)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (anchor, attr, doc);
	}
      TtaSetAttributeText (attr, "link", anchor, doc);
    }
  else
    CreateTargetAnchor (doc, anchor, FALSE, FALSE);

  if (parag)
    TtaRegisterElementCreate (parag, doc);
  else
    TtaRegisterElementCreate (anchor, doc);
  TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
   MakeUniqueName
   Check attribute NAME or ID in order to make sure that its value is unique
   in the document.
   If the NAME or ID is already used, add a number at the end of the value.
  ----------------------------------------------------------------------*/
void MakeUniqueName (Element el, Document doc)
{
  ElementType	    elType;
  AttributeType     attrType;
  Attribute         attr;
  Element	    image;
  char             *value;
  char              url[MAX_LENGTH];
  int               length, i;
  ThotBool          change, checkID;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  checkID = FALSE;
  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    /* it's an element from the XHTML namespace */
    if (elType.ElTypeNum == HTML_EL_Anchor ||
	elType.ElTypeNum == HTML_EL_MAP)
      /* it's an anchor or a map. Look for a NAME attribute */
      {
	attrType.AttrTypeNum = HTML_ATTR_NAME;
	attr = TtaGetAttribute (el, attrType);
	if (attr != 0)
	  /* the element has a NAME attribute. Check it and then check
	     if there is an ID too */
	  checkID = TRUE;
	else
	  /* no NAME. Look for an ID */
	  attrType.AttrTypeNum = HTML_ATTR_ID;
      }
    else
      /* Look for an ID attribute */
      attrType.AttrTypeNum = HTML_ATTR_ID;
  else
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
      /* it's an element from the MathML namespace, look for the
         id attribute from the same namespace */
      attrType.AttrTypeNum = MathML_ATTR_id;
    else
#ifdef GRAPHML
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "GraphML"))
	/* it's an element from the SVG namespace, look for the
	   id attribute from the same namespace */
	attrType.AttrTypeNum = GraphML_ATTR_id;
      else
#endif
	attrType.AttrTypeNum = 0;
  
  if (attrType.AttrTypeNum != 0)
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	/* the element has an attribute NAME or ID. Check it */
	{
	  length = TtaGetTextAttributeLength (attr) + 10;
	  value = TtaGetMemory (length);
	  change = FALSE;
	  if (value != NULL)
	    {
	      TtaGiveTextAttributeValue (attr, value, &length);
	      i = 0;
	      while (SearchNAMEattribute (doc, value, attr) != NULL)
		{
		  /* Yes. Avoid duplicate NAMEs */
		  change = TRUE;
		  i++;
		  sprintf (&value[length], "%d", i);
		}
	      
	      if (change)
		{
		  /* copy the element Label into the NAME attribute */
		  TtaSetAttributeText (attr, value, el, doc);
		  if (checkID)
		    /* It's an HTML anchor. We have just changed its NAME
		       attribute. Change its ID (if any) accordingly */
		    {
		      attrType.AttrTypeNum = HTML_ATTR_ID;
		      attr = TtaGetAttribute (el, attrType);
		      if (attr)
			TtaSetAttributeText (attr, value, el, doc);
		    }
		  if ((strcmp(TtaGetSSchemaName (elType.ElSSchema),
			       "HTML") == 0) &&
		      elType.ElTypeNum == HTML_EL_MAP)
		    /* it's a MAP element */
		    {
		      /* Search backward the refered image */
		      attrType.AttrTypeNum = HTML_ATTR_USEMAP;
		      TtaSearchAttribute (attrType, SearchBackward, el,
					  &image, &attr);
		      if (!attr)
			/* Not found. Search forward the refered image */
			TtaSearchAttribute (attrType, SearchForward, el,
					    &image, &attr);
		      if (attr && image)
			/* referred image found */
			{
			  i = MAX_LENGTH;
			  TtaGiveTextAttributeValue (attr, url, &i);
			  if (i == length+1 && !strncmp (&url[1], value,
							  length))
			    {
			      /* Change the USEMAP attribute of the image */
			      attr = TtaGetAttribute (image, attrType);
			      strcpy (&url[1], value);
			      TtaSetAttributeText (attr, url, image, doc);
			    }
			}
		    }
		}
	    }
	  TtaFreeMemory (value);
	}
    }
}

/*----------------------------------------------------------------------
  GetNextNode
  Returns the next node in the tree, using a complete traversal algorithm.
  ----------------------------------------------------------------------*/
static Element    GetNextNode (Element curr)
{
  Element el;

  if (!curr)
    return NULL;

  /* get the next child */
  el = TtaGetFirstChild (curr);
  if (!el)
    {
      /* get the next siblign, or parent
	 if there was no other sibling */
      el = TtaGetSuccessor (curr);
    }
  return el;
}

/*----------------------------------------------------------------------
  SearchTypedElementForward
  Searchs for a typed element and stops when it finds it or if the
  search reaches the last element.
  ----------------------------------------------------------------------*/
static Element    SearchTypedElementForward (ElementType elType_search, Element curr, Element last)
{
  ElementType elType;
  Element el;

  /* start by getting the next node */
  el = GetNextNode (curr);
  /*  continue browsing until we get an element
     of the searched type or the end condition is
     reached */
  while (el && el != last)
    {
      elType = TtaGetElementType (el);
      if (TtaSameTypes (elType_search, elType))
	  break;
      el = GetNextNode (el);
    }

  /* don't return el if it's equal to the last element parameter */
  return ((el != last) ? el : NULL);
}

/*----------------------------------------------------------------------
  CreateRemoveIDAttribute
  For all elements elName of a document, this functions eithers adds or 
  deletes an ID attribute. 
  The createID flag tells which operation must be done.
  The inSelection attribute says if we must apply the operation in the
  whole document or just in the current selection.
  If an element already has an ID attribute, a new one won't be created.
  TO DO: Use the thotmsg functions for the dialogs.
  ----------------------------------------------------------------------*/
void         CreateRemoveIDAttribute (char *elName, Document doc, ThotBool createID, ThotBool inSelection)
{
  Element             el, lastEl;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char             *schema_name;
  DisplayMode         dispMode;
  ThotBool            closeUndo;
  int                 i, j;

  /* the user must select something */
  if (inSelection && !TtaIsDocumentSelected (doc))
    {
      strcpy (IdStatus, "Nothing selected");
      return;
    }
  /* search for the elementType corresponding to the element name given
   by the user */
  GIType (elName, &elType, doc);
  if (elType.ElTypeNum == 0)
    {
      /* element name not found */
      strcpy (IdStatus, "Unknown element");
      return;
    }
  /* in function of the target elType, we choose the correct
     ATTR_ID value and schema */
  attrType.AttrTypeNum = 0;
  schema_name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (schema_name, "HTML"))
    {
      /* exception handling... we can't add an ID attribute everywhere
       in HTML documents */
      if (!(elType.ElTypeNum == HTML_EL_HTML
	    || elType.ElTypeNum == HTML_EL_HEAD
	    || elType.ElTypeNum == HTML_EL_TITLE
	    || elType.ElTypeNum == HTML_EL_BASE
	    || elType.ElTypeNum == HTML_EL_META
	    || elType.ElTypeNum == HTML_EL_SCRIPT
	    || elType.ElTypeNum == HTML_EL_STYLE_))
	attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  else if (!strcmp (schema_name, "MathML"))
    attrType.AttrTypeNum = MathML_ATTR_id;
  else if (!strcmp (schema_name, "GraphML"))
    attrType.AttrTypeNum = GraphML_ATTR_id;

  /* we didn't find an attribute or we can't put an ID attribute
     in this element */
  if (attrType.AttrTypeNum == 0)
    {
      strcpy (IdStatus, "DTD forbids it");
      return;
    }
  attrType.AttrSSchema = elType.ElSSchema;

  /* prepare the environment before doing the operation */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* set the first and last elements for the search */
  if (inSelection)
    {
      /* get the first and last elements of the selection */
      TtaGiveFirstSelectedElement (doc, &el, &i, &j);
       if (TtaIsSelectionEmpty ())
	 lastEl = el;
       else
	 TtaGiveLastSelectedElement (doc, &lastEl, &j, &i);
       /* and set the last element as the element just after
	  the selection */
       lastEl = TtaGetSuccessor (lastEl);
    }
  else
    {
      el = TtaGetMainRoot (doc);
      lastEl = NULL;
    }

  /*
  ** browse the tree and add the ID if it's missing. Variable i
  ** stores the number of changes we have done.
  */

  /* move to the first element that is of the chosen elementType */
  /* the element where we started is not of the chosen type */
  if (!TtaSameTypes (TtaGetElementType (el), elType))
    el = SearchTypedElementForward (elType, el, lastEl);

  if (TtaPrepareUndo (doc))
      closeUndo = FALSE;
  else
    {
      closeUndo = TRUE;
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
    }
  i = 0;
  while (el)
    {
      /* does the element have an ID attribute already? */
      attr = TtaGetAttribute (el, attrType);
      if (!attr && createID) /* add it */
	{
	  /* we reuse an existing Amaya function */
	  CreateTargetAnchor (doc, el, TRUE, TRUE);
	  i++;
	}
      else if (attr && !createID) /* delete it */
	{
	  TtaRegisterAttributeDelete (attr, el, doc);
	  TtaRemoveAttribute (el, attr, doc);
	  i++;
	}
      /* get the next element */
      el = SearchTypedElementForward (elType, el, lastEl);
    }

  if (closeUndo)
    TtaCloseUndoSequence (doc);

  /* reset the state of the document */
  if (i)
    TtaSetDocumentModified (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  sprintf (IdStatus, "%d elements changed", i);
}

/*----------------------------------------------------------------------
   CheckPseudoParagraph
   Element el has been created or pasted. If its a Pseudo_paragraph,
   it is turned into an ordinary Paragraph if it's not the first child
   of its parent.
   If the next sibiling is a Pseudo_paragraph, this sibling is turned into
   an ordinary Paragraph.
   Rule: only the first child of any element can be a Pseudo_paragraph.
  ----------------------------------------------------------------------*/
static void         CheckPseudoParagraph (Element el, Document doc)
{
  Element		prev, next, parent;
  Attribute             attr;
  ElementType		elType;
  
  elType = TtaGetElementType (el);
  if (!TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc)))
    /* it's not an HTML element */
    return;

  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the element is a Pseudo_paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev || attr)
        /* the Pseudo-paragraph is not the first element among its sibling */
        /* or it has attributes: turn it into an ordinary paragraph */
	{
	TtaRemoveTree (el, doc);
        ChangeElementType (el, HTML_EL_Paragraph);
	TtaInsertSibling (el, prev, FALSE, doc);
	}
    }
  else if (elType.ElTypeNum == HTML_EL_Paragraph)
    /* the element is a Paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev == NULL && attr == NULL)
        /* the Paragraph is the first element among its sibling and it has
	   no attribute */
        /* turn it into an Pseudo-paragraph if it's in a List_item or a
	   table cell. */
         {
	 parent = TtaGetParent (el);
	 if (parent)
	    {
	    elType = TtaGetElementType (parent);
	    if (elType.ElTypeNum == HTML_EL_List_Item ||
		elType.ElTypeNum == HTML_EL_Definition ||
		elType.ElTypeNum == HTML_EL_Data_cell ||
		elType.ElTypeNum == HTML_EL_Heading_cell)
		{
		TtaRemoveTree (el, doc);
		ChangeElementType (el, HTML_EL_Pseudo_paragraph);
		TtaInsertFirstChild (&el, parent, doc);
		}
	    }
	 }
    }
  next = el;
  TtaNextSibling (&next);
  if (next)
    {
      elType = TtaGetElementType (next);
      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	/* the next element is a Pseudo-paragraph */
	/* turn it into an ordinary paragraph */
	{
	  TtaRegisterElementReplace (next, doc);
	  TtaRemoveTree (next, doc);
	  ChangeElementType (next, HTML_EL_Paragraph);
	  TtaInsertSibling (next, el, FALSE, doc);
	}
    }
}

/*----------------------------------------------------------------------
   ElementCreated
   An element has been created in a HTML document.
  ----------------------------------------------------------------------*/
void                ElementCreated (NotifyElement * event)
{
  CheckPseudoParagraph (event->element, event->document);
}

/*----------------------------------------------------------------------
 ElementDeleted
 An element has been deleted. If it was the only child of element
 BODY, create a first paragraph.
 -----------------------------------------------------------------------*/
void ElementDeleted (NotifyElement *event)
{
  Element	child, el;
  ElementType	elType, childType;
  ThotBool	empty;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_BODY)
     {
     child = TtaGetFirstChild (event->element);
     empty = TRUE;
     while (empty && child)
	{
        elType = TtaGetElementType (child);
	if (elType.ElTypeNum != HTML_EL_Comment_ &&
	    elType.ElTypeNum != HTML_EL_Invalid_element)
	   empty = FALSE;
        else
	   TtaNextSibling (&child);
	}
     if (empty)
	{
	elType.ElTypeNum = HTML_EL_Paragraph;
	child = TtaNewTree (event->document, elType, "");
	TtaInsertFirstChild (&child, event->element, event->document);
	TtaRegisterElementCreate (child, event->document);
	do
	   {
	   el = TtaGetFirstChild (child);
	   if (el)
	      child = el;
	   }
	while (el);
	TtaSelectElement (event->document, child);
	}
     }
   /* if the deleted element was the first child of a LI, transform
      the new first child into a Pseudo-Paragraph if it's a Paragraph */
   else if (elType.ElTypeNum == HTML_EL_List_Item)
     /* the parent element is a List_Item */
     if (event->position == 0)
        /* the deleted element was the first child */
        {
        child = TtaGetFirstChild (event->element);
        if (child)
	   {
	   childType = TtaGetElementType (child);
	   if (childType.ElTypeNum == HTML_EL_Paragraph)
	      /* the new first child is a Paragraph */
	      {
	      TtaRegisterElementReplace (child, event->document);
	      TtaRemoveTree (child, event->document);
	      ChangeElementType (child, HTML_EL_Pseudo_paragraph);
	      TtaInsertFirstChild (&child, event->element, event->document);
	      }
	   }
        }
}


/*----------------------------------------------------------------------
   RegisterURLSavedElements
   Elements from document doc have been saved in the Thot Copy/Cut buffer.
   Save the URL of this document, to allow relative URIs contained
   in these elements to be adapted when they are pasted.
  ----------------------------------------------------------------------*/
void RegisterURLSavedElements (Document doc)
{
  if (SavedDocumentURL)
    TtaFreeMemory (SavedDocumentURL);
  SavedDocumentURL = GetBaseURL (doc);
}

/*----------------------------------------------------------------------
   ChangeURI
   Element el has been pasted in document doc. It comes from document
   originDocument and it has an href attribute (from the HTML or XLink
   namespace) that has to be updated. Update it according to the new
   context.
  ----------------------------------------------------------------------*/
void ChangeURI (Element el, Attribute attr, Document originDocument,
		Document doc)
{
  int      length, i, iName;
  char    *value, *base, *documentURI, *tempURI, *path;

  /* get a buffer for the URI */
  length = TtaGetTextAttributeLength (attr) + 1;
  value = TtaGetMemory (length);
  if (value)
    {
    tempURI = TtaGetMemory (MAX_LENGTH);
    if (tempURI)
      {
      iName = 0;
      /* get the URI itself */
      TtaGiveTextAttributeValue (attr, value, &length);
      if (value[0] == '#')
	  /* the target is in the original document */
	  /* convert the internal link into an external link */
	{
	  if (originDocument == 0)
	    /* origin document has been unloaded. Get the saved URL */
	    strcpy (tempURI, SavedDocumentURL);
	  else
	    strcpy (tempURI, DocumentURLs[originDocument]);
	}
      else
	{
	  /* the target element is in another document */
          documentURI = TtaGetMemory (MAX_LENGTH);
	  if (documentURI)
	    {
	    strcpy (documentURI, value);
	    /* looks for a '#' in the value */
	    i = length;
	    while (value[i] != '#' && i > 0)
	      i--;
	    if (i == 0)
		/* there is no '#' in the URI */
		value[0] = EOS;
	    else
	      {
		/* there is a '#' character in the URI */
		/* separate document name and element name */
		documentURI[i] = EOS;
		iName = i;
	      }
	    /* get the complete URI of the referred document */
	    /* Add the base if necessary */
	    path = TtaGetMemory (MAX_LENGTH);
	    if (path)
	      {
	      if (originDocument == 0)
	        NormalizeURL (documentURI, -1, tempURI, path, NULL);
	      else
	        NormalizeURL (documentURI, originDocument, tempURI, path,NULL);
	      TtaFreeMemory (path);
	      }
	    TtaFreeMemory (documentURI);
	    }
	}
      if (value[iName] == '#')
	{
	  if (!strcmp (tempURI, DocumentURLs[doc]))
	    /* convert external link into internal link */
	    strcpy (tempURI, &value[iName]);
	  else
	    strcat (tempURI, &value[iName]);
	}
      /* set the relative value or URI in attribute HREF */
      base = GetBaseURL (doc);
      if (base)
	{
        value = MakeRelativeURL (tempURI, base);
        TtaFreeMemory (base);
        }
      TtaSetAttributeText (attr, value, el, doc);
      TtaFreeMemory (tempURI);
      }
    TtaFreeMemory (value);
    }
}

/*----------------------------------------------------------------------
   ElementPasted
   This function is called for each element pasted by the user, and for
   each element within the pasted element.
   An element has been pasted in a HTML document.
   Check Pseudo paragraphs.
   If the pasted element has a NAME attribute, change its value if this
   NAME is already used in the document.
   If it's within the TITLE element, update the corresponding field in
   the Formatted window.
  ----------------------------------------------------------------------*/
void ElementPasted (NotifyElement * event)
{
  Document            originDocument, doc;
  Language            lang;
  Element             el, anchor, child, previous, nextchild, parent;
  ElementType         elType, parentType;
  AttributeType       attrType;
  Attribute           attr;
  SSchema             HTMLschema;
  CSSInfoPtr          css;
  char               *value;
  int                 length, oldStructureChecking;

  el = event->element;
  doc = event->document;
  HTMLschema = TtaGetSSchema ("HTML", doc);
  CheckPseudoParagraph (el, doc);
  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (el, doc);
  
  elType = TtaGetElementType (el);
  anchor = NULL;
  if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_Anchor)
      anchor = el;
  else if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_LINK)
    {
      /* check if it's a CSS link */
      CheckCSSLink (el, doc, HTMLschema);
    }
  else if (elType.ElSSchema == HTMLschema &&
	   elType.ElTypeNum == HTML_EL_STYLE_)
    {
      /* The pasted element is a STYLE element in the HEAD */
      /* Get its content */
      child = TtaGetFirstChild (el);
      length = TtaGetTextLength (child);
      value = TtaGetMemory (length + 1);
      TtaGiveTextContent (child, value, &length, &lang);
      /* parse the content */
      css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, NULL, NULL);
      ReadCSSRules (doc, css, value,
		    TtaGetElementLineNumber (child), FALSE);
      TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {  
      parent = TtaGetParent (event->element);
      parentType = TtaGetElementType (parent);
      if (TtaSameSSchemas (parentType.ElSSchema, HTMLschema) &&
          parentType.ElTypeNum == HTML_EL_TITLE)
         /* the parent of the pasted text is the TITLE element */
         /* That's probably the result of undoing a change in the TITLE */
         UpdateTitle (parent, doc);
    }
  else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      originDocument = (Document) event->position;
      if (originDocument > 0)
	{
	  /* remove USEMAP attribute */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != NULL)
	    TtaRemoveAttribute (el, attr, doc);
	  /* Change attributes SRC if the element comes from another */
	  /* document */
	  if (originDocument != doc)
	    {
	      /* the image has moved from one document to another */
	      /* get the SRC attribute of element IMAGE */
	      attrType.AttrTypeNum = HTML_ATTR_SRC;
	      attr = TtaGetAttribute (el, attrType);
	      if (attr != NULL)
		{
		  /* get a buffer for the SRC */
		  length = TtaGetTextAttributeLength (attr);
		  if (length > 0)
		    {
		      value = TtaGetMemory (MAX_LENGTH);
		      if (value != NULL)
			{
			  /* get the SRC itself */
			  TtaGiveTextAttributeValue (attr, value, &length);
			  /* update value and SRCattribute */
			  ComputeSRCattribute (el, doc, originDocument,
					       attr, value);
			}
		      TtaFreeMemory (value);
		    }
		}
	    }
	}
    }
  
  if (anchor != NULL)
    {
      TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      /* Is there a parent anchor? */
      parent = TtaGetTypedAncestor (el, elType);
      if (parent != NULL)
	{
	  /* Move anchor children and delete the anchor element */
	  child = TtaGetFirstChild (anchor);
	  previous = child;
	  TtaPreviousSibling (&previous);
	  
	  while (child != NULL)
	    {
	      nextchild = child;
	      TtaNextSibling (&nextchild);
	      TtaRemoveTree (child, doc);
	      TtaInsertSibling (child, anchor, TRUE, doc);
	      /* if anchor is the pasted element, it has been registered
		 in the editing history for the Undo command.  It will be
		 deleted, so its children have to be registered too. */
	      if (anchor == el)
		TtaRegisterElementCreate (child, doc);
	      child = nextchild;
	    }
	  TtaDeleteTree (anchor, doc);
	}
      else
	{
	  /* Change attributes HREF if the element comes from another */
	  /* document */
	  originDocument = (Document) event->position;
	  if (originDocument >= 0 && originDocument != doc)
	    {
	      /* the anchor has moved from one document to another */
	      /* get the HREF attribute of element Anchor */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      attr = TtaGetAttribute (anchor, attrType);
	      if (attr != NULL)
                 ChangeURI (anchor, attr, originDocument, doc);
	    }
	}
      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
      TtaSetDisplayMode (doc, DisplayImmediately);
    }
}

/*----------------------------------------------------------------------
   CheckNewLines
   Some new text has been pasted or typed in a text element. Check the
   NewLine characters and replace them by spaces, except in a PRE
 -----------------------------------------------------------------------*/
void CheckNewLines (NotifyOnTarget *event)
{
  Element     ancestor;
  ElementType elType;
  char       *content;
  int         length, i;
  Language    lang;
  ThotBool    changed, pre;

  if (!event->target)
     return;
  length = TtaGetTextLength (event->target);
  if (length == 0)
     return;

  /* is there a preformatted (or equivalent) ancestor? */
  pre = FALSE;
  ancestor = TtaGetParent (event->target);
  while (ancestor && !pre)
    {
    elType = TtaGetElementType (ancestor);
    if ((strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
        (elType.ElTypeNum == HTML_EL_STYLE_ ||
         elType.ElTypeNum == HTML_EL_SCRIPT ||
	 elType.ElTypeNum == HTML_EL_Preformatted))
       pre = TRUE;
    else
       ancestor = TtaGetParent (ancestor);
    }
  if (pre)
     /* there is a preformatted ancestor. Don't change anything */
     return;

  /* replace every new line in the content of the element by a space */
  length++;
  content = TtaGetMemory (length);
  TtaGiveTextContent (event->target, content, &length, &lang);
  changed = FALSE;
  for (i = 0; i < length; i++)
     if (content[i] == (char) EOL)
       {
        content[i] = SPACE;
	changed = TRUE;
       }
  if (changed)
     /* at least 1 new line has been replaced by a space */
     TtaSetTextContent (event->target, content, lang, event->document);
  TtaFreeMemory (content);
}

/*----------------------------------------------------------------------
   CreateTarget creates a target element.                          
  ----------------------------------------------------------------------*/
void                CreateTarget (Document doc, View view)
{
   CreateAnchor (doc, view, FALSE);
}


/*----------------------------------------------------------------------
   UpdateAttrID
   An ID attribute has been created, modified or deleted.
   If it's a creation or modification, check that the ID is a unique name
   in the document.
   If it's a deletion for a SPAN element, remove that element if it's
   not needed.
  ----------------------------------------------------------------------*/
void                UpdateAttrID (NotifyAttribute * event)
{
   Element	firstChild, lastChild;

   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document, &firstChild,
			  &lastChild);
   else
      {
      MakeUniqueName (event->element, event->document);
      if (event->event == TteAttrCreate)
         /* if the ID attribute is on a text string, create a SPAN element that
         encloses this text string and move the ID attribute to that SPAN
         element */
         AttrToSpan (event->element, event->attribute, event->document);
      }
}


/*----------------------------------------------------------------------
   CoordsModified  updates x_ccord, y_coord, width, height or      
   polyline according to the new coords value.             
  ----------------------------------------------------------------------*/
void                CoordsModified (NotifyAttribute * event)
{
   ParseAreaCoords (event->element, event->document);
}


/*----------------------------------------------------------------------
   GraphicsModified        updates coords attribute value          
   according to the new coord value.                       
  ----------------------------------------------------------------------*/
void                GraphicsModified (NotifyAttribute * event)
{
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute	       attr;
   char		       buffer[15];

   el = event->element;
   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* update the associated map */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_IntWidthPxl)
	 {
	   UpdateImageMap (el, event->document, OldWidth, -1);
	   OldWidth = -1;
	   /* update attribute Width__ */
	   attrType.AttrSSchema = event->attributeType.AttrSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Width__;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr)
	      {
	      sprintf (buffer, "%d",
			TtaGetAttributeValue (event->attribute));
	      TtaSetAttributeText (attr, buffer, el, event->document);
	      }
	 }
       else
	 {
	   UpdateImageMap (el, event->document, -1, OldHeight);
	   OldHeight = -1;
	 }
     }
   else
     {
       if (elType.ElTypeNum != HTML_EL_AREA)
	 {
	 el = TtaGetParent (el);
	 elType = TtaGetElementType (el);
	 }
       if (elType.ElTypeNum == HTML_EL_AREA)
	 SetAreaCoords (event->document, el, event->attributeType.AttrTypeNum);
     }
}

/*----------------------------------------------------------------------
   StoreWidth IntWidthPxl will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool            StoreWidth (NotifyAttribute * event)
{
  ElementType	     elType;
  int                h;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &OldWidth,&h);
  else
    OldWidth = -1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   StoreHeight height_ will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool            StoreHeight (NotifyAttribute * event)
{
  ElementType	     elType;
  int                w;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &w,
		    &OldHeight);
  else
     OldHeight = -1;
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   AttrWidthDelete         An attribute Width__ will be deleted.   
   Delete the corresponding attribute IntWidthPercent or   
   IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
ThotBool            AttrWidthDelete (NotifyAttribute * event)
{
   AttributeType       attrType;
   Attribute           attr;

   StoreHeight (event);
   attrType = event->attributeType;
   attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
   attr = TtaGetAttribute (event->element, attrType);
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntWidthPercent;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr != NULL)
      TtaRemoveAttribute (event->element, attr, event->document);
   return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   AttrWidthModifed  An attribute Width__ has been created or modified.
   Create the corresponding attribute IntWidthPercent or IntWidthPxl.
  ----------------------------------------------------------------------*/
void AttrWidthModified (NotifyAttribute * event)
{
  char               *buffer;
  int                 length;

  length = buflen - 1;
  buffer = TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrWidthPercentPxl (buffer, event->element, event->document,
			     OldWidth);
  TtaFreeMemory (buffer);
  OldWidth = -1;
}

/*----------------------------------------------------------------------
   an HTML attribute "size" has been created for a Font element.   
   Create the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
void AttrFontSizeCreated (NotifyAttribute * event)
{
   char               *buffer = TtaGetMemory (buflen);
   int                 length;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (event->document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (event->document, DeferredDisplay);

   length = buflen - 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   CreateAttrIntSize (buffer, event->element, event->document);
   TtaSetDisplayMode (event->document, dispMode);
   TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   an HTML attribute "size" has been deleted for a Font element.   
   Delete the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
ThotBool AttrFontSizeDelete (NotifyAttribute * event)
{
   AttributeType       attrType;
   Attribute           attr;

   attrType = event->attributeType;
   attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
   attr = TtaGetAttribute (event->element, attrType);
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr != NULL)
      TtaRemoveAttribute (event->element, attr, event->document);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   an attribute color, TextColor or BackgroundColor has been       
   created or modified.                                            
  ----------------------------------------------------------------------*/
void AttrColorCreated (NotifyAttribute * event)
{
   char            *value = TtaGetMemory (buflen);
   int              length;

   value[0] = EOS;
   length = TtaGetTextAttributeLength (event->attribute);
   if (length >= buflen)
      length = buflen - 1;
   if (length > 0)
      TtaGiveTextAttributeValue (event->attribute, value, &length);

   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLSetBackgroundColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLSetBackgroundImage (event->document, event->element, STYLE_REPEAT,
			      value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLSetForegroundColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLSetAlinkColor (event->document, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLSetAvisitedColor (event->document, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLSetAactiveColor (event->document, value);
   TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
   an attribute color, TextColor or BackgroundColor is being       
   deleted.                                                        
  ----------------------------------------------------------------------*/
ThotBool            AttrColorDelete (NotifyAttribute * event)
{
   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLResetBackgroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLResetBackgroundImage (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLResetForegroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLResetAlinkColor (event->document);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLResetAvisitedColor (event->document);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLResetAactiveColor (event->document);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   An element List_Item has been created or pasted. Set its        
   IntItemStyle attribute according to its surrounding elements.   
  ----------------------------------------------------------------------*/
void                ListItemCreated (NotifyElement * event)
{
   SetAttrIntItemStyle (event->element, event->document);
}

/*----------------------------------------------------------------------
   Set the IntItemStyle attribute of all List_Item elements in the 
   el subtree.                                                     
  ----------------------------------------------------------------------*/
static void         SetItemStyleSubtree (Element el, Document doc)
{
   ElementType         elType;
   Element             child;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_List_Item)
      SetAttrIntItemStyle (el, doc);
   child = TtaGetFirstChild (el);
   while (child != NULL)
     {
	SetItemStyleSubtree (child, doc);
	TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   An element Unnumbered_List or Numbered_List has changed type.   
   Set the IntItemStyle attribute for all enclosed List_Items      
  ----------------------------------------------------------------------*/
void                ListChangedType (NotifyElement * event)
{
   SetItemStyleSubtree (event->element, event->document);
}

/*----------------------------------------------------------------------
   An attribute BulletStyle or NumberStyle has been created,       
   deleted or modified for a list. Create or updated the           
   corresponding IntItemStyle attribute for all items of the list. 
  ----------------------------------------------------------------------*/
void                UpdateAttrIntItemStyle (NotifyAttribute * event)
{
   Element             child;

   child = TtaGetFirstChild (event->element);
   while (child != NULL)
     {
	SetAttrIntItemStyle (child, event->document);
	TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   An attribute ItemStyle has been created, updated or deleted.    
   Create or update the corresponding IntItemStyle attribute.      
  ----------------------------------------------------------------------*/
void                AttrItemStyle (NotifyAttribute * event)
{
   Element             el;

   el = event->element;
   while (el != NULL)
     {
	SetAttrIntItemStyle (el, event->document);
	TtaNextSibling (&el);
     }
}

/*----------------------------------------------------------------------
   GlobalAttrInMenu
   Called by Thot when building the Attributes menu.
   Prevent Thot from including a global attribute in the menu if the selected
   element do not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool            GlobalAttrInMenu (NotifyAttribute * event)
{
   ElementType         elType;
   SSchema	       HTMLSSchema;
   char               *attr;

   elType = TtaGetElementType (event->element);
   attr = GetXMLAttributeName (event->attributeType, elType, event->document);
   if (attr[0] == EOS)
      return TRUE;	/* don't put an invalid attribute in the menu */

   /* handle only Global attributes */
   if (event->attributeType.AttrTypeNum != HTML_ATTR_ID &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Class &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Style_ &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Title &&
       event->attributeType.AttrTypeNum != HTML_ATTR_dir &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onclick &&
       event->attributeType.AttrTypeNum != HTML_ATTR_ondblclick &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmousedown &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseup &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseover &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmousemove &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseout &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeypress &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeydown &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeyup &&
       event->attributeType.AttrTypeNum != HTML_ATTR_xml_space)
     /* it's not a global attribute. Accept it */
     return FALSE;

   HTMLSSchema = TtaGetSSchema ("HTML", event->document);
   if (TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
     {
      /* it's a HTML element */

       /* BASE and SCRIPT do not accept any global attribute */
       if (elType.ElTypeNum == HTML_EL_BASE ||
	   elType.ElTypeNum == HTML_EL_SCRIPT ||
	   elType.ElTypeNum == HTML_EL_Element)
	 return TRUE;

       /* BASEFONT and PARAM accept only ID */
       if (elType.ElTypeNum == HTML_EL_BaseFont ||
	   elType.ElTypeNum == HTML_EL_Parameter)
	 return (event->attributeType.AttrTypeNum != HTML_ATTR_ID);

       /* coreattrs */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_ID ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_Class ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_Style_ ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_Title)
	 {
	 if (elType.ElTypeNum == HTML_EL_HEAD ||
	     elType.ElTypeNum == HTML_EL_TITLE ||
	     elType.ElTypeNum == HTML_EL_META ||
	     elType.ElTypeNum == HTML_EL_STYLE_ ||
	     elType.ElTypeNum == HTML_EL_HTML)
	   /* HEAD, TITLE, META, STYLE and HTML don't accept coreattrs */
	   return TRUE;
	 else
	   return FALSE; /* let Thot perform normal operation */
	 }
       /* i18n */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_dir ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_Langue)
	 {
	 if (elType.ElTypeNum == HTML_EL_BR ||
	     elType.ElTypeNum == HTML_EL_Applet ||
	     elType.ElTypeNum == HTML_EL_Horizontal_Rule ||
	     elType.ElTypeNum == HTML_EL_FRAMESET ||
	     elType.ElTypeNum == HTML_EL_FRAME ||
	     elType.ElTypeNum == HTML_EL_IFRAME)
	   return TRUE;
	 else
	   return FALSE;
	 }
       /* events */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_onclick ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_ondblclick ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onmousedown ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onmouseup ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onmouseover ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onmousemove ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onmouseout ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onkeypress ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onkeydown ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_onkeyup)
	 if (elType.ElTypeNum == HTML_EL_BDO ||
	     elType.ElTypeNum == HTML_EL_Font_ ||
	     elType.ElTypeNum == HTML_EL_BR ||
	     elType.ElTypeNum == HTML_EL_Applet ||
	     elType.ElTypeNum == HTML_EL_FRAMESET ||
	     elType.ElTypeNum == HTML_EL_FRAME ||
	     elType.ElTypeNum == HTML_EL_IFRAME ||
	     elType.ElTypeNum == HTML_EL_HEAD ||
	     elType.ElTypeNum == HTML_EL_TITLE ||
	     elType.ElTypeNum == HTML_EL_META ||
	     elType.ElTypeNum == HTML_EL_STYLE_ ||
	     elType.ElTypeNum == HTML_EL_HTML ||
	     elType.ElTypeNum == HTML_EL_ISINDEX)
	   return TRUE;
       /* xml:space attribute */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_xml_space)
	 /* be careful here -- document may not have been received yet */
	 if (DocumentMeta[event->document] == NULL
	     || DocumentMeta[event->document]->xmlformat == FALSE)
	   return TRUE;	 

       return FALSE;
     }
   return TRUE;	/* don't put an invalid attribute in the menu */
}

/*----------------------------------------------------------------------
   AttrNAMEinMenu
   doesn't display NAME in Reset_Input and Submit_Input
  ----------------------------------------------------------------------*/
ThotBool            AttrNAMEinMenu (NotifyAttribute * event)
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;

   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_Reset_Input ||
       elType.ElTypeNum == HTML_EL_Submit_Input)
      /* Attribute menu for an element Reset_Input or Submit_Input */
      /* prevent Thot from including an entry for that attribute */
      return TRUE;
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* check if it's an input element */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_IsInput;
       attr = TtaGetAttribute (event->element, attrType);
       if (attr)
	 return FALSE;		/* let Thot perform normal operation */
       else
	 return TRUE;		/* not allowed on standard pictures */
     }
   else
      return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  The Emphasis button or menu item has been clicked
  ----------------------------------------------------------------------*/
void                SetOnOffEmphasis (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Emphasis);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontstyle);
    }
}

/*----------------------------------------------------------------------
  The Strong button or menu item has been clicked
  ----------------------------------------------------------------------*/
void                SetOnOffStrong (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Strong);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontweight);
    }
}

/*----------------------------------------------------------------------
  SetOnOffCite
  ----------------------------------------------------------------------*/
void                SetOnOffCite (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Cite);
}


/*----------------------------------------------------------------------
  SetOnOffDefinition
  ----------------------------------------------------------------------*/
void                SetOnOffDefinition (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Def);
}


/*----------------------------------------------------------------------
  The Code button or menu item has been clicked
  ----------------------------------------------------------------------*/
void                SetOnOffCode (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Code);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontfamily);
    }
}


/*----------------------------------------------------------------------
  SetOnOffVariable
  ----------------------------------------------------------------------*/
void                SetOnOffVariable (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Variable);
}


/*----------------------------------------------------------------------
  SetOnOffSample
  ----------------------------------------------------------------------*/
void                SetOnOffSample (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Sample);
}


/*----------------------------------------------------------------------
  SetOnOffKeyboard
  ----------------------------------------------------------------------*/
void                SetOnOffKeyboard (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Keyboard);
}


/*----------------------------------------------------------------------
  SetOnOffAbbr
  ----------------------------------------------------------------------*/
void                SetOnOffAbbr (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_ABBR);
}


/*----------------------------------------------------------------------
  SetOnOffAcronym
  ----------------------------------------------------------------------*/
void                SetOnOffAcronym (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_ACRONYM);
}


/*----------------------------------------------------------------------
  SetOnOffINS
  ----------------------------------------------------------------------*/
void                SetOnOffINS (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_INS);
}


/*----------------------------------------------------------------------
  SetOnOffDEL
  ----------------------------------------------------------------------*/
void                SetOnOffDEL (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_DEL);
}


/*----------------------------------------------------------------------
  SetOnOffItalic
  ----------------------------------------------------------------------*/
void                SetOnOffItalic (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Italic_text);
}


/*----------------------------------------------------------------------
  SetOnOffBold
  ----------------------------------------------------------------------*/
void                SetOnOffBold (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Bold_text);
}


/*----------------------------------------------------------------------
  SetOnOffTeletype
  ----------------------------------------------------------------------*/
void                SetOnOffTeletype (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Teletype_text);
}


/*----------------------------------------------------------------------
  SetOnOffBig
  ----------------------------------------------------------------------*/
void                SetOnOffBig (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Big_text);
}


/*----------------------------------------------------------------------
  SetOnOffSmall
  ----------------------------------------------------------------------*/
void                SetOnOffSmall (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Small_text);
}


/*----------------------------------------------------------------------
  SetOnOffSub
  ----------------------------------------------------------------------*/
void                SetOnOffSub (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Subscript);
}


/*----------------------------------------------------------------------
  SetOnOffSup
  ----------------------------------------------------------------------*/
void                SetOnOffSup (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Superscript);
}


/*----------------------------------------------------------------------
  SetOnOffQuotation
  ----------------------------------------------------------------------*/
void                SetOnOffQuotation (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Quotation);
}


/*----------------------------------------------------------------------
  SetOnOffBDO
  ----------------------------------------------------------------------*/
void                SetOnOffBDO (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_BDO);
}


/*----------------------------------------------------------------------
  SearchAnchor return the enclosing Anchor element with:
  - a HREF attribute if link is TRUE
  - a NAME attribute if name is TRUE
  ----------------------------------------------------------------------*/
Element    SearchAnchor (Document doc, Element element, ThotBool link, ThotBool name)
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elAnchor;
   int                 typeNum;
   SSchema             HTMLschema;

   attr = NULL;
   elAnchor = NULL;
   HTMLschema = TtaGetSSchema ("HTML", doc);
   if (!HTMLschema)
     return NULL;
   elType = TtaGetElementType (element);
   if (link && elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT &&
       TtaSameSSchemas (elType.ElSSchema, HTMLschema))
      /* search an ancestor of type AREA */
      typeNum = HTML_EL_AREA;
   else
      /* search an ancestor of type Anchor */
      typeNum = HTML_EL_Anchor;

   if (elType.ElTypeNum == typeNum &&
       TtaSameSSchemas (elType.ElSSchema, HTMLschema))
      elAnchor = element;
   else
     {
	elType.ElTypeNum = typeNum;
	elType.ElSSchema = HTMLschema;
	elAnchor = TtaGetTypedAncestor (element, elType);
     }

   attrType.AttrSSchema = HTMLschema;
   if (link)
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
   else
      attrType.AttrTypeNum = HTML_ATTR_NAME;
   if (!link || !name)
     while (elAnchor != NULL && attr == NULL)
       {
	 /* get the attribute of element Anchor */
	 attr = TtaGetAttribute (elAnchor, attrType);
	 if (attr == NULL)
	   /* we are not looking for any anchor */
	   elAnchor = TtaGetTypedAncestor (elAnchor, elType);
       }
   return elAnchor;
}

/*----------------------------------------------------------------------
   UpdateAtom : on X-Windows, update the content of atom           
   BROWSER_HISTORY_INFO with title and url of current doc  
   c.f: http://zenon.inria.fr/koala/colas/browser-history/       
  ----------------------------------------------------------------------*/
void UpdateAtom (Document doc, char *url, char *title)
{
#ifndef _GTK
#ifndef _WINDOWS
   static Atom         property_name = 0;
   char               *v;
   int                 v_size;
   Display            *dpy = TtaGetCurrentDisplay ();
   ThotWindow          win;
   ThotWidget	       frame;

   frame = TtaGetViewFrame (doc, 1);
   if (frame == 0)
      return;
   win = XtWindow (XtParent (XtParent (XtParent (frame))));
   /* 13 is strlen("URL=0TITLE=00") */
   v_size = strlen (title) + strlen (url) + 13;
   v = TtaGetMemory (v_size);
   sprintf (v, "URL=%s%cTITLE=%s%c", url, 0, title, 0);
   if (!property_name)
      property_name = XInternAtom (dpy, "BROWSER_HISTORY_INFO", FALSE);
   XChangeProperty (dpy, win, property_name, XA_STRING, 8, PropModeReplace,
		    v, v_size);
   TtaFreeMemory (v);
#endif /* !_WINDOWS */
#endif /* !_GTK */
}

/*----------------------------------------------------------------------
   The user has modified the contents of element TITLE. Update the    
   the Title field on top of the window.                           
  ----------------------------------------------------------------------*/
void                TitleModified (NotifyOnTarget * event)
{
   UpdateTitle (event->element, event->document);
}
