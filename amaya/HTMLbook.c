/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Initialization functions and button functions of Amaya application.
 *
 * Authors: V. Quint, I. Vatton
 *          R. Guetari (W3C/INRIA) - Windows version.
 */


/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "AHTURLTools_f.h"
#include "print.h"
#include "css.h"

/* structure to register sub-documents in MakeBook function*/
typedef struct _SubDoc
  {
     struct _SubDoc  *SDnext;
     Element          SDel;
     char            *SDname;
  }SubDoc;

/* the structure used for the GetIncludedDocuments_callback function */
typedef struct _IncludeCtxt
{
  Element		div; /* enclosing element for the search */
  Element		link; /* current processed link */
  char		       *url; /* called url */
  char                 *name; /* the fragment name */
  struct _IncludeCtxt  *ctxt; /* the previous context */
} IncludeCtxt;

/* shared with windialogapi.c */
ThotBool         PrintURL;
ThotBool	 NumberLinks;
ThotBool	 WithToC;
ThotBool         IgnoreCSS;

static struct _SubDoc  *SubDocs;
static char             PSfile[MAX_PATH];
static char             PPrinter[MAX_PATH];
static char            *DocPrintURL;
static Document		DocPrint;
static int              PaperPrint;
static int              ManualFeed = PP_OFF;
static int              Orientation;
static int              PageSize;
static int              PagePerSheet;

#include "init_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "UIcss_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

static ThotBool GetIncludedDocuments ();

/*----------------------------------------------------------------------
  RedisplayDocument redisplays a view of a document
  ----------------------------------------------------------------------*/
void  RedisplayDocument(Document doc, View view)
{
  Element	       el;
  int		       position;
  int		       distance;

  /* get the current position in the document */
  position = RelativePosition (doc, &distance);
  TtaSetDisplayMode (doc, NoComputedDisplay);
  TtaSetDisplayMode (doc, DisplayImmediately);
  /* show the document at the same position */
  el = ElementAtPosition (doc, position);
  TtaShowElement (doc, view, el, distance);
}


/*----------------------------------------------------------------------
  RegisterSubDoc adds a new entry in SubDoc table.
  ----------------------------------------------------------------------*/
static void RegisterSubDoc (Element el, char *url)
{
  struct _SubDoc  *entry, *last;

  if (url == NULL || url[0] == EOS)
    return;

  entry = TtaGetMemory (sizeof (struct _SubDoc));
  entry->SDnext = NULL;
  entry->SDel = el;
  entry->SDname = TtaStrdup (url);

  if (SubDocs == NULL)
    SubDocs = entry;
  else
    {
      last = SubDocs;
      while (last->SDnext != NULL)
	last = last->SDnext;
      last->SDnext = entry;
    }
}


/*----------------------------------------------------------------------
  SearchSubDoc searches whether a document name is registered or not
  within the SubDoc table.
  Return the DIV element that correspond to the sub-document or NULL.
  ----------------------------------------------------------------------*/
static Element SearchSubDoc (char *url)
{
  Element          el;
  struct _SubDoc  *entry;
  ThotBool         docFound;

  if (url == NULL || url[0] == EOS)
    return (NULL);

  entry = SubDocs;
  docFound = FALSE;
  el = NULL;
  while (!docFound && entry != NULL)
    {
      docFound = (strcmp (url, entry->SDname) == 0);
      if (!docFound)
	entry = entry->SDnext;
      else
	/* document found -> return the DIV element */
	el = entry->SDel;
    }
  return (el);
}

/*----------------------------------------------------------------------
  FreeSubDocTable frees all entries in SubDoc table.
  ----------------------------------------------------------------------*/
static void FreeSubDocTable ()
{
  struct _SubDoc  *entry, *last;

  entry = SubDocs;
  while (entry != NULL)
    {
      last = entry;
      entry = entry->SDnext;
      TtaFreeMemory (last->SDname);
      TtaFreeMemory (last);
    }
  SubDocs = NULL;
}



/*----------------------------------------------------------------------
  SetInternalLinks
  Associate an InternalLink attribute with all anchor (A) elements of the
  document which designate an element in the same document.
  InternalLink is a Thot reference attribute that links a source and a
  target anchor and that allows P schemas to display and print cross-references
  ----------------------------------------------------------------------*/
void SetInternalLinks (Document document)
{
  Element	        el, div, link, target, sibling;
  ElementType		elType, linkType;
  Attribute		HrefAttr, IntLinkAttr;
  Attribute             attr, ExtLinkAttr;
  AttributeType	        attrType;
  char		       *text, *ptr, *url; 
  char                  number[10];
  char                  value[MAX_LENGTH];
  int			length, i, volume;
  int                   status, position;
  ThotBool              split;

  /* Remember the current status of the document */
  status = TtaIsDocumentModified (document);
  el = TtaGetMainRoot (document);
  volume = TtaGetElementVolume (el);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = HTML_EL_AnyLink;
  attrType.AttrSSchema = elType.ElSSchema;
  /* looks for all links in the document */
  link = el;
  while (link != NULL)
    {
      /* display the progression of the work */
      el = link;
      position = 0;
      while (el != NULL)
	{
	  sibling = el;
	  do
	    {
	      /* add volume of each previous element */
	      TtaPreviousSibling (&sibling);
	      if (sibling != NULL)
		position += TtaGetElementVolume (sibling);
	    }
	  while (sibling != NULL);
	  el = TtaGetParent (el);
	}
      sprintf (number, "%d", position*100/volume);
      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_UPDATED_LINK), number);
      TtaHandlePendingEvents ();
      link = TtaSearchTypedElement (elType, SearchForward, link);
      if (link != NULL)
	/* a link has been found */
	{
	  linkType = TtaGetElementType (link);
	  if (linkType.ElTypeNum == HTML_EL_Anchor)
	     attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  else
	     attrType.AttrTypeNum = HTML_ATTR_cite;
	  HrefAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_InternalLink;
	  IntLinkAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_ExternalLink;
	  ExtLinkAttr = TtaGetAttribute (link, attrType);
	  if (HrefAttr == NULL)
	    /* this element is not a link (no href or cite attribute) */
	    /* remove attributes InternalLink and ExternalLink if they
	       are present */
	    {
	      if (IntLinkAttr != NULL)
		TtaRemoveAttribute (link, IntLinkAttr, document);
	      if (ExtLinkAttr != NULL)
		TtaRemoveAttribute (link, ExtLinkAttr, document);	   
	    }
	  else
	    /* this element has an HREF or cite attribute */
	    {
	      length = TtaGetTextAttributeLength (HrefAttr);
	      text = TtaGetMemory (length + 1);
	      TtaGiveTextAttributeValue (HrefAttr, text, &length);

	      /* does an external link become an internal link ? */
	      if (document == DocBook && SubDocs != NULL)
		{
		  ptr = strrchr (text, '#');
		  url = text;
		  split = FALSE;
		  if (ptr == text)
		      /* a local link */
		      url = NULL;
		  else if (ptr != NULL)
		    {
		      /* split url and name part */
		      ptr[0] = EOS;
		      split = TRUE;
		    }

		  /* Is it a sub-document */
		  div = SearchSubDoc (url);
		  if (split)
		    /* retore the mark */
		    ptr[0] = '#';

		  if (div == NULL)
		    {
		      /* it's not a sub-document */
		      if (url == NULL)
			/* a local link */
			ptr = &text[1];
		      else
			/* still an externa; link */
			ptr = NULL;
		    }
		  else
		    {
		      /* this link becomes internal */
		      if (ptr != NULL)
			{
			  /* get the target name */
			  strcpy (value, ptr);
			  length = strlen (value);
			  /* check whether the name changed */
			  i = 0;
			  target = SearchNAMEattribute (document, &value[1], NULL);
			  while (target != NULL)
			    {
			      /* is it the right NAME */
			      if (TtaIsAncestor (target, div))
				target = NULL;
			      else
				{
				  /* continue the search */
				  i++;
				  sprintf (&value[length], "%d", i);
				  target = SearchNAMEattribute (document,
							&value[1], NULL);
				}
			    }
			}
		      else
			{
			  /* get the DIV name */
			  attrType.AttrTypeNum = HTML_ATTR_ID;
			  attr = TtaGetAttribute (div, attrType);
			  length = 200;
			  value[0] = '#';
			  TtaGiveTextAttributeValue (attr, &value[1], &length);
			}
		      ptr = &value[1];
		      TtaSetAttributeText (HrefAttr, value, link, document);
		    }
		}
	      else if (text[0] == '#')
		  ptr = &text[1];
	      else
		ptr = NULL;

	      if (ptr != NULL)
		/* it's an internal link. Attach an attribute InternalLink */
		/* to the link, if this attribute does not exist yet */
		{
		  if (IntLinkAttr == NULL)
		    {
		      attrType.AttrTypeNum = HTML_ATTR_InternalLink;
		      IntLinkAttr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (link, IntLinkAttr, document);
		    }
		  /* looks for the target element */
		  target = SearchNAMEattribute (document, ptr, NULL);
		  if (target != NULL)
		    /* set the Thot link */
		    TtaSetAttributeReference (IntLinkAttr, link, document,
					      target, document);
		}
	      else
		/* it's an external link */
		{
		  /* Remove the InternalLink attribute if it is present */
		  if (IntLinkAttr != NULL)
		    TtaRemoveAttribute (link, IntLinkAttr, document);
		  /* create an ExternalLink attribute if there is none */
		  if (ExtLinkAttr == NULL)
		    {
		      attrType.AttrTypeNum = HTML_ATTR_ExternalLink;
		      ExtLinkAttr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (link, ExtLinkAttr, document);
		    }
		}
	      TtaFreeMemory (text);
	    }
	}
    }
  /* Reset document status */
  if (!status)
    TtaSetDocumentUnmodified (document);
}

/*----------------------------------------------------------------------
  CheckPrintingDocument reinitialize printing parameters as soon as
  the printing document changes.
  ----------------------------------------------------------------------*/
static void CheckPrintingDocument (Document document)
{
  char         docName[MAX_LENGTH];
  char        *ptr; 
  char         suffix[MAX_LENGTH];
  int            lg;

  if (DocPrint != document || DocPrintURL == NULL ||
      strcmp(DocPrintURL, DocumentURLs[document]))
    {
      /* initialize print parameters */
      TtaFreeMemory (DocPrintURL);
      DocPrint = document;
      DocPrintURL = TtaStrdup (DocumentURLs[document]);
      
      /* define the new default PS file */
      ptr = TtaGetEnvString ("APP_TMPDIR");
      if (ptr != NULL && TtaCheckDirectory (ptr))
	strcpy (PSfile, ptr);
      else
	strcpy (PSfile, TtaGetDefEnvString ("APP_TMPDIR"));
      lg = strlen (PSfile);
      if (PSfile[lg - 1] == DIR_SEP)
	PSfile[--lg] = EOS;
      strcpy (docName, TtaGetDocumentName (document));
      TtaExtractSuffix (docName, suffix);
      sprintf (&PSfile[lg], "%c%s.ps", DIR_SEP, docName);
      TtaSetPsFile (PSfile);
    }
}

/*----------------------------------------------------------------------
   PrintDocument prints the document using predefined parameters.
   ----------------------------------------------------------------------*/  
static void PrintDocument (Document doc, View view)
{
  AttributeType      attrType;
  ElementType        elType;
  Attribute          attr;
  Element            el, docEl;
  char              *files, *dir;
  char               viewsToPrint[MAX_PATH];
  ThotBool           status, textFile;

  textFile = (DocumentTypes[doc] == docText ||
              DocumentTypes[doc] == docSource ||
	      DocumentTypes[doc] == docCSS);

  /* initialize printing information */
  CheckPrintingDocument (doc);
  strcpy (viewsToPrint, "Formatted_view ");
  if (DocumentTypes[doc] == docHTML && WithToC)
    strcat (viewsToPrint, "Table_of_contents ");
  
  if (textFile)
    {
      if (PageSize == PP_A4)
	{
	  if (Orientation == PP_Landscape)
	    TtaSetPrintSchema ("TextFilePL");
	  else
	    TtaSetPrintSchema ("TextFilePP");
	}
      else
	{
	  if (Orientation == PP_Landscape)
	    TtaSetPrintSchema ("TextFileUSL");
	  else
	    TtaSetPrintSchema ("TextFilePPUS");
	}
    }
  else if (DocumentTypes[doc] == docSVG)
    TtaSetPrintSchema ("SVGP");
  else if (DocumentTypes[doc] == docMath)
    TtaSetPrintSchema ("MathMLP");
  else if (DocumentTypes[doc] == docAnnot)
    TtaSetPrintSchema ("AnnotP");
  else if (DocumentTypes[doc] == docHTML && NumberLinks)
    /* display numbered links */
    {
      /* associate an attribute InternalLink with all anchors refering
	 a target in the same document.  This allows P schemas to work
	 properly */
      SetInternalLinks (DocPrint);
      if (PageSize == PP_A4)
	{
	  if (Orientation == PP_Landscape)
	    TtaSetPrintSchema ("HTMLPLL");
	  else
	    TtaSetPrintSchema ("HTMLPLP");
	}
      else
	{
	  if (Orientation == PP_Landscape)
	    TtaSetPrintSchema ("HTMLUSLL");
	  else
	    TtaSetPrintSchema ("HTMLPLPUS");
	}
      strcat (viewsToPrint, "Links_view ");
    }
  else if (PageSize == PP_A4)
    {
      if (Orientation == PP_Landscape)
	TtaSetPrintSchema ("HTMLPL");
      else
	TtaSetPrintSchema ("HTMLPP");
    }
  else
    {
      if (Orientation == PP_Landscape)
	TtaSetPrintSchema ("HTMLUSL");
      else
	TtaSetPrintSchema ("HTMLPPUS");
    }    
  
  status = TtaIsDocumentModified (doc);

  if (textFile || DocumentTypes[doc] == docImage ||
      DocumentTypes[doc] == docHTML)
    {
      /* post or remove the PrintURL attribute */
      attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
      elType.ElSSchema = attrType.AttrSSchema;
      if (textFile)
	{
	  elType. ElTypeNum = TextFile_EL_TextFile;
	  attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
	}
      else
	{
	  elType. ElTypeNum = HTML_EL_HTML;
	  attrType.AttrTypeNum = HTML_ATTR_PrintURL;
	}
      docEl = TtaGetMainRoot (doc);
      el = TtaSearchTypedElement (elType, SearchForward, docEl);
      attr = TtaGetAttribute (el, attrType);
      if (!attr && PrintURL)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (el, attr, doc);
	}
      if (attr && !PrintURL)
	TtaRemoveAttribute (el, attr, doc);
    }
  
  /* get the path dir where css files have to be stored */
  if ((DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docSVG) &&
      !IgnoreCSS)
    {
      TtaGetPrintNames (&files, &dir);
      /* store css files and get the list of names */
      files = CssToPrint (doc, dir);
    }
  else
    files = NULL;
  TtaPrint (DocPrint, viewsToPrint, files);
  if (files)
    TtaFreeMemory (files);
  if (!status)
    TtaSetDocumentUnmodified (doc);
}

/*----------------------------------------------------------------------
   PrintAs prints the document using predefined parameters.
   ----------------------------------------------------------------------*/  
void PrintAs (Document doc, View view)
{
#ifdef _WINDOWS
  DocPrint = doc;
  ReusePrinterDC ();
#else /* _WINDOWS */
  PrintDocument (doc, view);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   CallbackImage handle return of Print form.                   
  ----------------------------------------------------------------------*/
void CallbackPrint (int ref, int typedata, char *data)
{
  int                 val;

  val = (int) data;
  switch (ref - BasePrint)
    {
    case FormPrint:
      TtaDestroyDialogue (BasePrint + FormPrint);
      switch (val)
	{
	case 1:
	  TtaSetPrintCommand (PPrinter);
	  TtaSetPsFile (PSfile);
	  /* update the environment variable */
	  TtaSetEnvString ("THOTPRINT", PPrinter, TRUE);
	  PrintDocument (DocPrint, 1);
	  break;
	default:
	  break;
	}
      break;
    case PrintOptions:
      switch (val)
	{
	case 0:
	  /* Manual feed option */
	  if (ManualFeed == PP_ON)
	    ManualFeed = PP_OFF;
	  else
	    ManualFeed = PP_ON;
	  TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
	  break;
	case 1:
	  /* Toc option */
	  WithToC = !WithToC;
	  break;
	case 2:
	  /* NumberLinks option */
	  NumberLinks = !NumberLinks;
	case 3:
	  /* URL option */
	  PrintURL = !PrintURL;
	  break;
	case 4:
	  /* CSS option */
	  IgnoreCSS = !IgnoreCSS;
	  break;
	}
      break;
    case PaperFormat:
      /* page size submenu */
      switch (val)
	{
	case 0:
	  PageSize = PP_A4;
	  break;
	case 1:
	  PageSize = PP_US;
	  break;
	}
      TtaSetPrintParameter (PP_PaperSize, PageSize);
      break;
    case PaperOrientation:
      /* orientation submenu */
      Orientation = val;
      TtaSetPrintParameter (PP_Orientation, Orientation);
      break;
    case PPagesPerSheet:
      /* pages per sheet submenu */
      switch (val)
	{
	case 0:
	  PagePerSheet = 1;
	  break;
	case 1:
	  PagePerSheet = 2;
	  break;
	case 2:
	  PagePerSheet = 4;
	  break;
	}
      TtaSetPrintParameter (PP_PagesPerSheet, PagePerSheet);
      break;
    case PrintSupport:
      /* paper print/save PostScript submenu */
      switch (val)
	{
	case 0:
	  if (PaperPrint == PP_PS)
	    {
	      PaperPrint = PP_PRINTER;
#ifndef _WINDOWS
	      TtaSetTextForm (BasePrint + PPrinterName, PPrinter);
#endif /* !_WINDOWS */
	      TtaSetPrintParameter (PP_Destination, PaperPrint);
	    }
	  break;
	case 1:
	  if (PaperPrint == PP_PRINTER)
	    {
	      PaperPrint = PP_PS;
#ifndef _WINDOWS
	      TtaSetTextForm (BasePrint + PPrinterName, PSfile);
#endif /* !_WINDOWS */
	      TtaSetPrintParameter (PP_Destination, PaperPrint);
	    }
	  break;
	}
      break;
    case PPrinterName:
      if (data[0] != '\0')
	{
	if (PaperPrint == PP_PRINTER)
	    /* text capture zone for the printer name */
	    strncpy (PPrinter, data, MAX_PATH);
	else
	  /* text capture zone for the name of the PostScript file */
	  strncpy (PSfile, data, MAX_PATH);
	}
      break;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitPrint (void)
{
  char* ptr;

   BasePrint = TtaSetCallback (CallbackPrint, PRINT_MAX_REF);
   DocPrint = 0;
   DocPrintURL = NULL;

   /* read default printer variable */
   ptr = TtaGetEnvString ("THOTPRINT");
   if (ptr == NULL)
     strcpy (PPrinter, "");
   else
     strcpy (PPrinter, ptr);
   TtaSetPrintCommand (PPrinter);
   PaperPrint = PP_PRINTER;
   TtaSetPrintParameter (PP_Destination, PaperPrint);

   /* define the new default PrintSchema */
   NumberLinks = FALSE;
   WithToC = FALSE;
   IgnoreCSS = FALSE;
   PrintURL = TRUE;
   PageSize = TtaGetPrintParameter (PP_PaperSize);	  
   TtaSetPrintSchema ("");
   /* no manual feed */
   ManualFeed = PP_OFF;
   TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
   PagePerSheet = 1;
   TtaSetPrintParameter (PP_PagesPerSheet, PagePerSheet);
}

/*----------------------------------------------------------------------
  SetupAndPrint sets printing parameters and starts the printing process
  ----------------------------------------------------------------------*/
void SetupAndPrint (Document doc, View view)
{
#ifndef _WINDOWS
  char           bufMenu[MAX_LENGTH];
  int            i;
#endif /* _WINDOWS */
  ThotBool       textFile;

  textFile = (DocumentTypes[doc] == docText || DocumentTypes[doc] == docCSS);
  /* Print form */
  CheckPrintingDocument (doc);

#ifndef _WINDOWS
  TtaNewSheet (BasePrint + FormPrint, TtaGetViewFrame (doc, view), 
	       TtaGetMessage (LIB, TMSG_LIB_PRINT), 1,
	       TtaGetMessage (AMAYA, AM_BUTTON_PRINT), FALSE, 3, 'L', D_CANCEL);

  /* Paper format submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
  TtaNewSubmenu (BasePrint + PaperFormat, BasePrint + FormPrint, 0,
		 TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, TRUE);
  if (PageSize == PP_US)
    TtaSetMenuForm (BasePrint + PaperFormat, 1);
  else
    TtaSetMenuForm (BasePrint + PaperFormat, 0);
  
  /* Orientation submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_PORTRAIT));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_LANDSCAPE));
  TtaNewSubmenu (BasePrint + PaperOrientation, BasePrint + FormPrint, 0,
		 TtaGetMessage (AMAYA, AM_ORIENTATION), 2, bufMenu, NULL, TRUE);
  if (Orientation == PP_Landscape)
    TtaSetMenuForm (BasePrint + PaperOrientation, 1);
  else
    TtaSetMenuForm (BasePrint + PaperOrientation, 0);
  /* Pages per sheet submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_1_PAGE_SHEET));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_2_PAGE_SHEET));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_4_PAGE_SHEET));
  TtaNewSubmenu (BasePrint + PPagesPerSheet, BasePrint + FormPrint, 0,
		 TtaGetMessage (LIB, TMSG_REDUCTION), 3, bufMenu, NULL, TRUE);
  if (PagePerSheet == 1)
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 0);
  else if (PagePerSheet == 2)
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 1);
  else
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 2);
    
  /* Print to paper/ Print to file submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
  TtaNewSubmenu (BasePrint + PrintSupport, BasePrint + FormPrint, 0,
		 TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);

  /* PaperPrint selector */
  TtaNewTextForm (BasePrint + PPrinterName, BasePrint + FormPrint, NULL, 30, 1, TRUE);
  if (PaperPrint == PP_PRINTER)
    {
      TtaSetMenuForm (BasePrint + PrintSupport, 0);
      TtaSetTextForm (BasePrint + PPrinterName, PPrinter);
    }
  else
    {
      TtaSetMenuForm (BasePrint + PrintSupport, 1);
      TtaSetTextForm (BasePrint + PPrinterName, PSfile);
    }

  /* The toggle */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_TOC));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_URL));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_WITH_CSS));
  TtaNewToggleMenu (BasePrint + PrintOptions, BasePrint + FormPrint,
		    TtaGetMessage (LIB, TMSG_OPTIONS), 5, bufMenu, NULL, FALSE);
  if (ManualFeed == PP_ON)
    TtaSetToggleMenu (BasePrint + PrintOptions, 0, TRUE);
  else
    TtaSetToggleMenu (BasePrint + PrintOptions, 0, FALSE);
  TtaSetToggleMenu (BasePrint + PrintOptions, 1, WithToC);
  TtaSetToggleMenu (BasePrint + PrintOptions, 2, NumberLinks);
  TtaSetToggleMenu (BasePrint + PrintOptions, 3, PrintURL);
  TtaSetToggleMenu (BasePrint + PrintOptions, 4, IgnoreCSS);
  
  /* activates the Print form */
  TtaShowDialogue (BasePrint+FormPrint, FALSE);
  if (textFile)
    {
      /* invalid dialogue entries */
      TtaRedrawMenuEntry (BasePrint + PrintOptions, 1, NULL, -1, FALSE);
      TtaRedrawMenuEntry (BasePrint + PrintOptions, 2, NULL, -1, FALSE);
    }
#else  /* _WINDOWS */
  CreatePrintDlgWindow (TtaGetViewFrame (doc, view), PSfile);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  UpdateURLsInSubtree
  Update NAMEs and URLs in subtree of el element, to take into account
  the move from one document to another.
  If a NAME attribute already exists in the new document, it is changed
  to avoid duplicate names.
  Transform the HREF and SRC attribute to make them independent from their
  former base.
  ----------------------------------------------------------------------*/
static void UpdateURLsInSubtree (NotifyElement *event, Element el)
{
  Element             nextEl, child;
  ElementType         elType;
  SSchema             HTMLschema;

  nextEl = TtaGetFirstChild (el);
  HTMLschema = TtaGetSSchema ("HTML", event->document);
  elType.ElSSchema = HTMLschema;
  while (nextEl != NULL)
    {
      event->element = nextEl;
      ElementPasted (event);

      /* manage included links and anchors */
      elType.ElTypeNum = HTML_EL_Anchor;
      child = TtaSearchTypedElement (elType, SearchInTree, nextEl);
      while (child)
	{
	  event->element = child;
	  ElementPasted (event);
	  child = TtaSearchTypedElementInTree (elType, SearchForward, nextEl, child);
	}

      /* manage included links and anchors */
      elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
      child = TtaSearchTypedElement (elType, SearchInTree, nextEl);
      while (child)
	{
	  event->element = child;
	  ElementPasted (event);
	  child = TtaSearchTypedElementInTree (elType, SearchForward, nextEl, child);
	}
      TtaNextSibling (&nextEl);
    }
}


/*----------------------------------------------------------------------
  MoveDocumentBody
  Copy the elements contained in the BODY of the document sourceDoc at the
  position of the element el in the document destDoc.
  Delete the element containing el and all its empty ancestors.
  If deleteTree is TRUE, copied elements are deleted from the source
  document.
  Return the root element that delimits the new inserted part (a div).
  ----------------------------------------------------------------------*/
static Element MoveDocumentBody (Element el, Document destDoc,
				 Document sourceDoc, char *target,
				 char *url, ThotBool deleteTree)
{
  Element	   root, ancestor, elem, firstInserted, div;
  Element          lastInserted, srce, copy, old, parent, sibling;
  ElementType	   elType;
  NotifyElement    event;
  int		   checkingMode;
  ThotBool         isID;

  div = NULL;
  if (target != NULL)
    {
      /* locate the target element within the source document */
      root = SearchNAMEattribute (sourceDoc, target, NULL);
      elType = TtaGetElementType (root);
      isID = (elType.ElTypeNum != HTML_EL_Anchor &&
	      elType.ElTypeNum != HTML_EL_MAP);
    }
  else
    {
      isID = FALSE;
      /* get the BODY element of source document */
      root = TtaGetMainRoot (sourceDoc);
      elType = TtaGetElementType (root);
      elType.ElTypeNum = HTML_EL_BODY;
      root = TtaSearchTypedElement (elType, SearchForward, root);
    }

  if (root != NULL)
    {
      /* don't check the abstract tree against the structure schema */
      checkingMode = TtaGetStructureChecking (destDoc);
      TtaSetStructureChecking (0, destDoc);
      /* get elem, the ancestor of el which is a child of a DIV or BODY
	 element in the destination document. The copied elements will be
	 inserted just before this element. */
      elem = el;
      do
	{
	  ancestor = TtaGetParent (elem);
	  if (ancestor != NULL)
	    {
	      elType = TtaGetElementType (ancestor);
	      if (elType.ElTypeNum == HTML_EL_BODY ||
		  elType.ElTypeNum == HTML_EL_Division)
		ancestor = NULL;
	      else
		elem = ancestor;
	    }
	}
      while (ancestor != NULL);
      parent = TtaGetParent (elem);

      /* insert a DIV element */
      elType.ElTypeNum = HTML_EL_Division;
      lastInserted = TtaNewElement (destDoc, elType);
      TtaInsertSibling (lastInserted, elem, TRUE, destDoc);
      /* this delimits the new inserted part of the document */
      RegisterSubDoc (lastInserted, url);
      CreateTargetAnchor (destDoc, lastInserted, FALSE, FALSE);
      div = lastInserted;

      /* do copy */
      firstInserted = NULL;
      if (isID)
	srce = root;
      else
	srce = TtaGetFirstChild (root);
      while (srce != NULL)
	{
	  copy = TtaCopyTree (srce, sourceDoc, destDoc, parent);
	  if (copy != NULL)
	    {
	      if (firstInserted == NULL)
		/* this is the first copied element. Insert it before elem */
		{
		  TtaInsertFirstChild (&copy, lastInserted, destDoc);
		  firstInserted = copy;
		}
	      else
		/* insert the new copied element after the element previously
		   copied */
		TtaInsertSibling (copy, lastInserted, FALSE, destDoc);
	      lastInserted = copy;
	      /* update the NAMEs and URLs in the copied element */
	      event.document = destDoc;
	      event.position = sourceDoc;
	      UpdateURLsInSubtree(&event, copy);
	    }
	  /* get the next element in the source document */
	  old = srce;
	  TtaNextSibling (&srce);
	  if (deleteTree)
	    TtaDeleteTree (old, sourceDoc);
	  /* Stop here if the target points to a specific element with an ID */
	  if (isID)
	    srce = NULL;
	}
      
      /* delete the element(s) containing the link to the copied document */
      /* delete the parent element of el and all empty ancestors */
      elem = TtaGetParent (el);
      do
	{
	  sibling = elem;
	  TtaNextSibling (&sibling);
	  if (sibling == NULL)
	    {
	      sibling = elem;
	      TtaPreviousSibling (&sibling);
	      if (sibling == NULL)
		elem = TtaGetParent (elem);
	    }
	}
      while (sibling == NULL);
      TtaDeleteTree (elem, destDoc);
      /* restore previous chacking mode */
      TtaSetStructureChecking ((ThotBool)checkingMode, destDoc);
    }
  /* return the address of the new division */
  return (div);
}


/*----------------------------------------------------------------------
  CloseMakeBook
  ----------------------------------------------------------------------*/
static void CloseMakeBook (Document document)
{
  ResetStop (document);
  /* update internal links */
  SetInternalLinks (document);
  /* if the document changed force the browser mode */
  if (SubDocs)
    SetBrowserEditor (document);
  /* remove registered  sub-documents */
  FreeSubDocTable ();
  DocBook = 0;
  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
}


/*----------------------------------------------------------------------
  GetIncludedDocuments_callback finishes the GetIncludedDocuments procedure
  ----------------------------------------------------------------------*/
void   GetIncludedDocuments_callback (int newdoc, int status, 
				      char *urlName,
				      char *outputfile, 
				      AHTHeaders *http_headers,
				      void * context)
{
  Element		link, div;
  IncludeCtxt          *ctx, *prev;
  char		       *url, *ptr;
  ThotBool              found = FALSE;

  /* restore GetIncludedDocuments's context */
  ctx = (IncludeCtxt *) context;  
  if (!ctx)
    return;

  div = NULL;
  link = ctx->link;
  ptr = ctx->name;
  url = ctx->url;
  if (url)
    {
      if (newdoc && newdoc != DocBook)
	{
	  /* it's not the DocBook itself */
	  /* copy the target document at the position of the link */
	  TtaSetDocumentModified (DocBook);
	  div = MoveDocumentBody (link, DocBook, newdoc, ptr, url,
				  (ThotBool)(newdoc == IncludedDocument));
	}
      /* global variables */
      FreeDocumentResource (IncludedDocument);
      TtaCloseDocument (IncludedDocument);
      IncludedDocument = 0;
    }

  if (div != NULL)
    {
      /* new starting point for the search */
      ctx->link = div;
      found = GetIncludedDocuments (div, div, DocBook, ctx);
    }
  while (!found && ctx)
    {
      /* this sub-document has no more inclusion, examine the caller */
      div = ctx->div;
      link = ctx->link;
      prev = ctx->ctxt;
      TtaFreeMemory (url);
	  url = NULL;
      TtaFreeMemory (ctx);
      ctx = prev;
      found = GetIncludedDocuments (div, link, DocBook, ctx);
    }
  if (!found)
    /* all links are now managed */
    CloseMakeBook (DocBook);
}

/*----------------------------------------------------------------------
  GetIncludedDocuments
  Look forward within the element el, starting from element link, for a 
  link (A) with attribute rel="chapter" or rel="subdocument" and replace
  that link by the contents of the target document.
  Return TRUE if one inclusion is launched.
  ----------------------------------------------------------------------*/
static ThotBool  GetIncludedDocuments (Element el, Element link,
				       Document document, IncludeCtxt *prev)
{
  ElementType           elType;
  Attribute		attr;
  AttributeType	        attrType;
  Document		newdoc;
  IncludeCtxt          *ctx = NULL;
  char		       *text, *ptr, *url = NULL;
  int			length;
  ThotBool              found = FALSE;

  /* look for anchors with the attribute rel within the element  el */
  attr = NULL;
  attrType.AttrSSchema = TtaGetSSchema ("HTML", document);
  elType.ElSSchema = attrType.AttrSSchema;
  elType.ElTypeNum = HTML_EL_Anchor;

  /* Get only one included file each time */
  while (link && attr == NULL)
    {
      link = TtaSearchTypedElementInTree (elType, SearchForward, el, link);
      if (link)
	{
	  attrType.AttrTypeNum = HTML_ATTR_REL;
	  attr = TtaGetAttribute (link, attrType);
	}
      if (attr)
	{
	  length = TtaGetTextAttributeLength (attr);
	  text = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attr, text, &length);
	  /* Valid rel values are rel="chapter" or rel="subdocument" */
	  if (strcasecmp (text, "chapter") &&
	      strcasecmp (text, "subdocument"))
	    attr = NULL;
	  TtaFreeMemory (text);
	}
  
      if (attr)
	{
	  /* a link with attribute rel="Chapter" has been found */
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  attr = TtaGetAttribute (link, attrType);
	}
      if (attr)
	/* this link has an attribute HREF */
	{
	  length = TtaGetTextAttributeLength (attr);
	  text = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attr, text, &length);
	  ptr = strrchr (text, '#');
	  url = text;
	  if (ptr != NULL)
	    {
	      if (ptr == text)
		url = NULL;
	      /* link to a particular position within a remote document */
	      ptr[0] = EOS;
	      ptr = &ptr[1];
	    }
		  
	  if (url != NULL)
	    /* this link designates an external document */
	    {
	      /* create a new document and loads the target document */
	      IncludedDocument = TtaNewDocument ("HTML", "tmp");
	      if (IncludedDocument != 0)
		{
		  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_FETCHING), url);
		  ctx = TtaGetMemory (sizeof (IncludeCtxt));
		  ctx->div =  el;
		  ctx->link = link;
		  ctx->url = url; /* the URL of the document */
		  ctx->name = ptr;
		  ctx->ctxt = prev; /* previous context */
		  /* Get the reference of the calling document */
		  SetStopButton (document);
		  newdoc = GetHTMLDocument (url, NULL, IncludedDocument,
					    document, CE_MAKEBOOK, FALSE, 
					    (void *) GetIncludedDocuments_callback,
					    (void *) ctx);
		  found = TRUE;
		}
	    }
	  else
	    TtaFreeMemory (text);
	}
    }
  return (found);
}


/*----------------------------------------------------------------------
  MakeBook
  Replace all links in a document which have an attribute REL="chapter"
  or REL="subdocument" by the corresponding target document.
  ----------------------------------------------------------------------*/
void MakeBook (Document document, View view)
{
  Element	    root, body;
  ElementType	    elType;

  /* stops all current transfers on this document */
  StopTransfer (document, 1);
  /* simulate a transfert in the main document */
  DocBook = document;
  IncludedDocument = 0;
  root = TtaGetMainRoot (document);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchForward, root);

  if (body)
    GetIncludedDocuments (body, body, document, NULL);
}
