/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Author: I. Vatton
 *         R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

/* Included headerfiles */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "zlib.h"
#include "profiles.h"
#ifdef _GTK
#include "gtkdialogue_box.h"
#endif /* _GTK */

#ifdef _WINDOWS
#include "resource.h"
#else /* _WINDOWS */
#include "stopN.xpm"
#include "stopR.xpm"
#include "save.xpm"
#include "saveNo.xpm"
#include "find.xpm"
#include "Reload.xpm"
#include "Browser.xpm"
#include "Editor.xpm"
#include "Print.xpm"
#include "Back.xpm"
#include "BackNo.xpm"
#include "Forward.xpm"
#include "ForwardNo.xpm"
#include "I.xpm"
#include "INo.xpm"
#include "B.xpm"
#include "BNo.xpm"
#include "T.xpm"
#include "TNo.xpm"
#include "H1.xpm"
#include "H1No.xpm"
#include "H2.xpm"
#include "H2No.xpm"
#include "H3.xpm"
#include "H3No.xpm"
#include "Num.xpm"
#include "NumNo.xpm"
#include "Bullet.xpm"
#include "BulletNo.xpm"
#include "Image.xpm"
#include "ImageNo.xpm"
#include "DL.xpm"
#include "DLNo.xpm"
#include "Link.xpm"
#include "LinkNo.xpm"
#include "Table.xpm"
#include "TableNo.xpm"
#include "home.xpm"
#endif /* !_WINDOWS */

#ifdef AMAYA_PLUGIN
#include "plugin.h"
#ifndef _WINDOWS
#include "Plugin.xpm"
#endif /* !_WINDOWS */
#endif /* AMAYA_PLUGIN */

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */

#ifdef _WINDOWS
/*
#ifndef __GNUC__
#include <direct.h>
#endif 
*/
extern HWND     FrMainRef [12];
extern int      currentFrame;
extern CHAR_T   wTitle [MAX_LENGTH];
int             Window_Curs;

CHAR_T DocToOpen [MAX_LENGTH];
/* extern bmpID;  */
#endif /* _WINDOWS */

#include "helpmenu.h"

static int          AmayaInitialized = 0;
static ThotBool     NewFile = FALSE;
static int          NewDocType = 0;
/* we have to mark the initial loading status to avoid to re-open the
   document view twice */
static int          Loading_method = CE_INIT;

#ifndef _WINDOWS
static ThotIcon       stopR;
static ThotIcon       stopN;
static ThotIcon       iconSave;
static ThotIcon       iconSaveNo;
static ThotIcon       iconFind;
static ThotIcon       iconReload;
static ThotIcon       iconI;
static ThotIcon       iconINo;
static ThotIcon       iconB;
static ThotIcon       iconBNo;
static ThotIcon       iconT;
static ThotIcon       iconTNo;
static ThotIcon       iconImage;
static ThotIcon       iconImageNo;
static ThotIcon       iconBack;
static ThotIcon       iconBackNo;
static ThotIcon       iconForward;
static ThotIcon       iconForwardNo;
static ThotIcon       iconH1;
static ThotIcon       iconH1No;
static ThotIcon       iconH2;
static ThotIcon       iconH2No;
static ThotIcon       iconH3;
static ThotIcon       iconH3No;
static ThotIcon       iconPrint;
static ThotIcon       iconBullet;
static ThotIcon       iconBulletNo;
static ThotIcon       iconNum;
static ThotIcon       iconNumNo;
static ThotIcon       iconDL;
static ThotIcon       iconDLNo;
static ThotIcon       iconLink;
static ThotIcon       iconLinkNo;
static ThotIcon       iconTable;
static ThotIcon       iconTableNo;
static ThotIcon       iconBrowser;
static ThotIcon       iconEditor;
static ThotIcon       iconHome;
#ifdef AMAYA_PLUGIN
static ThotIcon       iconPlugin;
#endif /* AMAYA_PLUGIN */
#endif /* _WINDOWS */

#ifdef _WINDOWS
#define stopR          0
#define stopN          0
#define iconBack       1
#define iconBackNo     1
#define iconForward    2
#define iconForwardNo  2
#define iconReload     3
#define iconSave       4
#define iconSaveNo     4
#define iconPrint      5
#define iconFind       6
#define iconI          7
#define iconINo        7
#define iconB          8
#define iconBNo        8
#define iconT          9
#define iconTNo        9
#define iconImage     11
#define iconImageNo   11
#define iconH1        12
#define iconH1No      12
#define iconH2        13
#define iconH2No      13
#define iconH3        14
#define iconH3No      14
#define iconBullet    15
#define iconBulletNo  15
#define iconNum       16
#define iconNumNo     16
#define	iconDL        17
#define	iconDLNo      17
#define iconLink      18
#define iconLinkNo    18
#define iconTable     19
#define iconTableNo   19
#ifdef AMAYA_PLUGIN
#define iconPlugin    20
#endif /* AMAYA_PLUGIN */
#define iconBrowser   23
#define iconEditor    23
#define iconHome      10

extern int       currentFrame;
extern int       menu_item;
extern CHAR_T    LostPicturePath [512];

static ThotBool  itemChecked = FALSE;

#include "wininclude.h"
#endif /* _WINDOWS */

#include "css_f.h"
#include "HTMLhistory_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "trans_f.h"
#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "Mathedit_f.h"
#ifdef GRAPHML
#include "Graphedit_f.h"
#endif /* GRAPHML */
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "MENUconf_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "templates_f.h"
#include "transparse_f.h"
#include "ustring.h"
#include "XMLparser_f.h"
#include "Xml2thot_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

extern void InitMathML ();

#ifdef AMAYA_PLUGIN
extern void CreateFormPlugin (Document, View);
#endif /* AMAYA_PLUGIN */

/* the structure used for storing the context of the 
   GetHTMLDocument_callback function */
typedef struct _GETHTMLDocument_context {
  Document   doc;
  Document   baseDoc;
  ThotBool   ok;
  ThotBool   history;
  ThotBool   local_link;
  CHAR_T*    target;
  CHAR_T*    documentname;
  CHAR_T*    initial_url;
  CHAR_T*     form_data;
  ClickEvent method;
  CHAR_T*    tempdocument;
  TTcbf*     cbf;
  void*      ctx_cbf;
} GETHTMLDocument_context;

/* the structure used for storing the context of the 
   Reload_callback function */
typedef struct _RELOAD_context {
  Document newdoc;
  CHAR_T* documentname;
  CHAR_T* form_data;
  ClickEvent method;
  int position;	/* volume preceding the the first element to be shown */
  int distance; /* distance from the top of the window to the top of this
		   element (% of the window height) */
} RELOAD_context;

typedef enum
{
  OpenDocBrowser,
  HrefAttrBrowser,
} TypeBrowserFile;

TypeBrowserFile WidgetParent;


/*----------------------------------------------------------------------
   DocumentMetaClear
   Clears the dynamically allocated memory associated to a metadata
   element. Doesn't free the element or clears any of its other elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DocumentMetaClear (DocumentMetaDataElement *me)
#else
void                DocumentMetaClear (me)
DocumentMetaDataElement *me;

#endif
{
  if (me->form_data)
    {
      TtaFreeMemory (me->form_data);
      me->form_data = NULL;
    }
  if (me->initial_url)
    {
      TtaFreeMemory (me->initial_url);
      me->initial_url = NULL;
    }
}


/*----------------------------------------------------------------------
   IsDocumentLoaded returns the document identification if the        
   corresponding document is already loaded or 0.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            IsDocumentLoaded (CHAR_T* documentURL, CHAR_T* form_data)
#else
Document            IsDocumentLoaded (documentURL, form_data)
CHAR_T*             documentURL;
CHAR_T*              form_data;

#endif
{
  int               i;
  ThotBool          found;

  if (!documentURL)
    return ((Document) None);

  i = 1;
  found = FALSE;
  /* look for the URL into the list of downloaded documents */
  while (!found && i < DocumentTableLength)
    {
      if (DocumentURLs[i])
	{
	  /* compare the url */
	  found = (!ustrcmp (documentURL, DocumentURLs[i]) ||
		   (DocumentMeta[i]->initial_url && !ustrcmp (documentURL, DocumentMeta[i]->initial_url)));
	  /* compare the form_data */
	  if (found && (!((!form_data && !DocumentMeta[i]->form_data) ||
		 (form_data && DocumentMeta[i]->form_data &&
		  !ustrcmp (form_data, DocumentMeta[i]->form_data)))))
	    found = FALSE;	  
	}
      if (!found)
	i++;
    }
  
  if (found)
    /* document is found */
    return ((Document) i);
  else
    /* document is not found */ 
    return ((Document) None);
}

/*----------------------------------------------------------------------
  CanReplaceCurrentDocument
  Return TRUE if the document has not been modified of if the user
  agrees to loose the changes he/she has made.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool		CanReplaceCurrentDocument (Document document, View view)
#else
ThotBool		CanReplaceCurrentDocument (document, view)
Document	document;
View		view;
#endif
{
   ThotBool	ret;

   ret = TRUE;
   if (TtaIsDocumentModified (document) ||
       (DocumentSource[document] &&
	TtaIsDocumentModified (DocumentSource[document])))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	  {
	    TtaSetDocumentUnmodified (document);
	    if (DocumentSource[document])
	      TtaSetDocumentUnmodified (DocumentSource[document]);
	  }
	else
	   ret = FALSE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   ExtractParameters extract parameters from document nane.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExtractParameters (CHAR_T* aName, CHAR_T* parameters)
#else
void                ExtractParameters (aName, parameters)
CHAR_T*             aName;
CHAR_T*             parameters;
#endif
{
   int              lg, i;
   CHAR_T*          ptr;
   CHAR_T*          oldptr;

   if (!parameters || !aName)
     /* bad parameters */
     return;

   parameters[0] = WC_EOS;
   lg = ustrlen (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = ustrrchr (oldptr, TEXT('?'));
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = WC_EOS;
	     if (i != lg)
		ustrcpy (parameters, oldptr);
	  }
     }
}

/*----------------------------------------------------------------------
   FileExistTarget
   Removes the URL target separator ('#') before verifying if a file
   exists.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     FileExistTarget (CHAR_T* filename)
#else
static ThotBool     FileExistTarget (filename)
CHAR_T*             filename;
#endif
{
  CHAR_T *ptr;
  ThotBool result;

  ptr = ustrrchr (filename, TEXT('#'));
  if (ptr)
    *ptr = TEXT('\0');

  result = TtaFileExist (filename);

  if (ptr)
    *ptr = TEXT('#');

  return result;
}

/*----------------------------------------------------------------------
  SetArrowButton
  Change the appearance of the Back (if back == TRUE) or Forward button
  for a given document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetArrowButton (Document document, ThotBool back, ThotBool on)
#else
void                SetArrowButton (document, back, on)
Document            document;
ThotBool		    back;
ThotBool		    on;
#endif
{
  int		index;
  ThotBool      state;
  ThotIcon	picture;

  if (back)
    {
      index = 2;
      if (on)
	{
	  state   = TRUE;
	  picture = iconBack;
	  TtaSetItemOn (document, 1, File, BBack);
	}
      else
	{
          state = FALSE;
	  picture = iconBackNo;
	  TtaSetItemOff (document, 1, File, BBack);
	}
    }
  else
    {
      index = 3;
      if (on)
	{
	  state = TRUE;
	  picture = iconForward;
	  TtaSetItemOn (document, 1, File, BForward);
        }
      else
	{
	  state = FALSE;
	  picture = iconForwardNo;
	  TtaSetItemOff (document, 1, File, BForward);
	}
    }
  TtaChangeButton (document, 1, index, picture, state);
}

/*----------------------------------------------------------------------
   ResetStop resets the stop button state                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResetStop (Document document)
#else
void                ResetStop (document)
Document            document;
#endif
{
  if (FilesLoading[document] != 0)
    FilesLoading[document]--;
  if (FilesLoading[document] == 0)
    /* The last object associated to the document has been loaded */
    {
      if (TtaGetViewFrame (document, 1) != 0) 
	/* this document is displayed */
	{
	  if(!(DocNetworkStatus[document] & AMAYA_NET_ERROR) &&
	     (DocNetworkStatus[document] & AMAYA_NET_ACTIVE))
	    /* if there was no error message, display the LOADED message */
	    TtaSetStatus (document, 1,
			  TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
	  TtaChangeButton (document, 1, iStop, stopN, FALSE);
	}
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
}

/*----------------------------------------------------------------------
   ActiveTransfer initialize the current transfer                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ActiveTransfer (Document document)
#else
void                ActiveTransfer (document)
Document            doc;
#endif
{
  DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
  FilesLoading[document] = 1;
  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
    TtaChangeButton (document, 1, iStop, stopR, TRUE);
}

/*----------------------------------------------------------------------
   SetStopButton Activates the stop button if it's turned off
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetStopButton (Document document)
#else
void                SetStopButton (document)
Document            doc;
#endif
{
  if (document == 0)
    return;
  else if (document != DocBook)
    {
      if (DocNetworkStatus[document] != AMAYA_NET_ACTIVE)
	DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
      if (FilesLoading[document] == 0)
	FilesLoading[document] = 1;
    }

  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
    TtaChangeButton (document, 1, iStop, stopR, TRUE);
}


/*----------------------------------------------------------------------
   SetFormReadWrite
   Set ReadWrite access to input elements
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	SetFormReadWrite (Element el, Document doc)
#else
static void	SetFormReadWrite (el, doc)
Element         el;
Document        doc;
#endif
{
   ElementType         elType;
   Element             child, next;

   while (el != NULL)
     {
       /* look at all elements within this form */
       elType = TtaGetElementType (el);
       child = TtaGetFirstChild (el);
       next = el;
       TtaNextSibling (&next);
       switch (elType.ElTypeNum)
	 {
	 case HTML_EL_Input:
	 case HTML_EL_Text_Input:
	 case HTML_EL_Password_Input:
	 case HTML_EL_File_Input:
	 case HTML_EL_Option:
	 case HTML_EL_Text_Area:	/* it's a Text_Area */
	   TtaSetAccessRight (child, ReadWrite, doc);
	   child = NULL;
	   break;
	 default:
	   break;
	 }
       if (child != NULL)
	 SetFormReadWrite (child, doc);
       el = next;
     }
}


/*----------------------------------------------------------------------
   SetDocumentReadOnly
   Set the whole document in ReadOnly mode except input elements
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	SetDocumentReadOnly (Document doc)
#else
static void	SetDocumentReadOnly (doc)
Document        doc;
#endif
{
   ElementType         elType;
   Element             el, elForm;

  TtaSetDocumentAccessMode (doc, 0);
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
    {
      elType.ElTypeNum = HTML_EL_Form;
      elForm = TtaSearchTypedElement (elType, SearchForward, el);
      while (elForm != NULL)
	{
	  /* there is a form */
	  el = TtaGetFirstChild (elForm);
	  if (el != NULL)
	    SetFormReadWrite (el, doc);
	  elForm = TtaSearchTypedElement (elType, SearchForward, elForm);
	}
    }
}

/*----------------------------------------------------------------------
   Update the save button and corresponding menu entry according to the
   document status.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      DocStatusUpdate (Document document, ThotBool modified)
#else
void      DocStatusUpdate (document, modified)
Document  doc;
ThotBool  modified;
#endif
{
  Document    otherDoc;

  if (modified && TtaGetDocumentAccessMode (document))
    /* the document has been modified and is not in Read-Only mode */
    {
       TtaSetItemOn (document, 1, File, BSave);
       TtaChangeButton (document, 1, iSave, iconSave, TRUE);
       /* if we have a pair source/structured document allow synchronization */
       otherDoc = DocumentSource[document];
       if (!otherDoc)
	 otherDoc = GetDocFromSource (document);
       if (otherDoc)
	 {
	   TtaSetItemOn (document, 1, File, BSynchro);
	   TtaSetItemOn (otherDoc, 1, File, BSynchro);
	 }
    }
  else
    {
       TtaSetItemOff (document, 1, File, BSave);
       TtaChangeButton (document, 1, iSave, iconSaveNo, FALSE);
       /* if we have a pair source/structured document allow synchronization */
       otherDoc = DocumentSource[document];
       if (!otherDoc)
	 otherDoc = GetDocFromSource (document);
       if (otherDoc)
	 {
	   TtaSetItemOff (document, 1, File, BSynchro);
	   TtaSetItemOff (otherDoc, 1, File, BSynchro);
	 }
    }
}

/*----------------------------------------------------------------------
  UpdateBrowserMenus
  Update windows menus for the Browser mode              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  UpdateBrowserMenus (Document document)
#else
static void  UpdateBrowserMenus (document)
Document  doc;
#endif
{
  View    view;

#ifdef _WINDOWS 
  WIN_TtaSwitchButton (document, 1, iEditor, iconEditor,
		       TB_INDETERMINATE, TRUE);
#else  /* _WINDOWS */
  TtaChangeButton (document, 1, iEditor, iconBrowser, TRUE);
#endif /* _WINDOWS */

  TtaSetToggleItem (document, 1, Edit_, TEditMode, FALSE);

  TtaSetItemOff (document, 1, File, BSave);
  TtaSetItemOff (document, 1, File, BSynchro);
  TtaSetItemOff (document, 1, Edit_, BUndo);
  TtaSetItemOff (document, 1, Edit_, BRedo);
  TtaSetItemOff (document, 1, Edit_, BCut);
  TtaSetItemOff (document, 1, Edit_, BPaste);
  TtaSetItemOff (document, 1, Edit_, BClear);
  
  TtaChangeButton (document, 1, iSave, iconSaveNo, FALSE);

  if (DocumentTypes[document] == docHTML ||
      DocumentTypes[document] == docSVG ||
      DocumentTypes[document] == docMath ||
      DocumentTypes[document] == docImage)
    {
      TtaChangeButton (document, 1, iI, iconINo, FALSE);
      TtaChangeButton (document, 1, iB, iconBNo, FALSE);
      TtaChangeButton (document, 1, iT, iconTNo, FALSE);
      TtaChangeButton (document, 1, iImage, iconImageNo, FALSE);
      TtaChangeButton (document, 1, iH1, iconH1No, FALSE);
      TtaChangeButton (document, 1, iH2, iconH2No, FALSE);
      TtaChangeButton (document, 1, iH3, iconH3No, FALSE);
      TtaChangeButton (document, 1,iBullet, iconBulletNo, FALSE);
      TtaChangeButton (document, 1,iNum, iconNumNo, FALSE);
      TtaChangeButton (document, 1,iDL, iconDLNo, FALSE);
      TtaChangeButton (document, 1, iLink, iconLinkNo, FALSE);
      TtaChangeButton (document, 1, iTable, iconTableNo, FALSE);
      SwitchIconMath (document, 1, FALSE);

#ifdef GRAPHML
      SwitchIconGraph (document, 1, FALSE);
#endif /* GRAPHML */
      
      TtaSetItemOff (document, 1, Edit_, BSpellCheck);
      TtaSetItemOff (document, 1, Edit_, BTransform);
      TtaSetMenuOff (document, 1, Types);
      TtaSetMenuOff (document, 1, Links);
      TtaSetMenuOff (document, 1, Style);
      TtaSetItemOff (document, 1, Special, BMakeBook);

#ifdef ANNOTATIONS
      TtaSetMenuOff (document, 1, Annotations_);
#endif /* ANNOTATIONS */

      view = TtaGetViewFromName (document, "Structure_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOff (document, view, Edit_, BCut);
	  TtaSetItemOff (document, view, Edit_, BPaste);
	  TtaSetItemOff (document, view, Edit_, BClear);
	  TtaSetItemOff (document, view, Edit_, BSpellCheck);
	  TtaSetItemOff (document, view, Edit_, BTransform);
	  TtaSetMenuOff (document, view, StructTypes);
	  TtaSetMenuOff (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Alternate_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOff (document, view, Edit_, BCut);
	  TtaSetItemOff (document, view, Edit_, BPaste);
	  TtaSetItemOff (document, view, Edit_, BClear);
	  TtaSetItemOff (document, view, Edit_, BSpellCheck);
	  TtaSetMenuOff (document, view, StructTypes);
	  TtaSetMenuOff (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Links_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOff (document, view, Edit_, BCut);
	  TtaSetItemOff (document, view, Edit_, BPaste);
	  TtaSetItemOff (document, view, Edit_, BClear);
	  TtaSetItemOff (document, view, Edit_, BSpellCheck);
	  TtaSetItemOff (document, view, Edit_, BTransform);
	  TtaSetMenuOff (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Table_of_contents");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOff (document, view, Edit_, BCut);
	  TtaSetItemOff (document, view, Edit_, BPaste);
	  TtaSetItemOff (document, view, Edit_, BClear);
	  TtaSetItemOff (document, view, Edit_, BSpellCheck);
	  TtaSetItemOff (document, view, Edit_, BTransform);
	  TtaSetMenuOff (document, view, Types);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateEditorMenus 
   Update windows menus for the Editor mode              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  UpdateEditorMenus (Document document)
#else
static void  UpdateEditorMenus (document)
Document  doc;
#endif
{
  View    view;
  
#ifdef _WINDOWS 
  WIN_TtaSwitchButton (document, 1, iEditor, iconEditor,
		       TB_INDETERMINATE, FALSE);
#else  /* _WINDOWS */
  TtaChangeButton (document, 1, iEditor, iconEditor, TRUE);
#endif /* _WINDOWS */

  TtaSetToggleItem (document, 1, Edit_, TEditMode, TRUE);

  TtaSetItemOn (document, 1, Edit_, BUndo);
  TtaSetItemOn (document, 1, Edit_, BRedo);
  TtaSetItemOn (document, 1, Edit_, BCut);
  TtaSetItemOn (document, 1, Edit_, BPaste);
  TtaSetItemOn (document, 1, Edit_, BClear);
  
  if (DocumentTypes[document] == docHTML ||
      DocumentTypes[document] == docSVG ||
      DocumentTypes[document] == docImage)
    {
      TtaChangeButton (document, 1, iI, iconI, TRUE);
      TtaChangeButton (document, 1, iB, iconB, TRUE);
      TtaChangeButton (document, 1, iT, iconT, TRUE);
      TtaChangeButton (document, 1, iImage, iconImage, TRUE);
      TtaChangeButton (document, 1, iH1, iconH1, TRUE);
      TtaChangeButton (document, 1, iH2, iconH2, TRUE);
      TtaChangeButton (document, 1, iH3, iconH3, TRUE);
      TtaChangeButton (document, 1,iBullet, iconBullet, TRUE);
      TtaChangeButton (document, 1,iNum, iconNum, TRUE);
      TtaChangeButton (document, 1,iDL, iconDL, TRUE);
      TtaChangeButton (document, 1, iLink, iconLink, TRUE);
      TtaChangeButton (document, 1, iTable, iconTable, TRUE);
      SwitchIconMath (document, 1, TRUE);

#ifdef GRAPHML
      SwitchIconGraph (document, 1, TRUE);
#endif /* GRAPHML */

      TtaSetItemOn (document, 1, Edit_, BSpellCheck);
      TtaSetItemOn (document, 1, Edit_, BTransform);
      TtaSetMenuOn (document, 1, Types);
      TtaSetMenuOn (document, 1, Links);
      TtaSetMenuOn (document, 1, Style);
      TtaSetItemOn (document, 1, Special, BMakeBook);

#ifdef ANNOTATIONS
      TtaSetMenuOn (document, 1, Annotations_);
#endif /* ANNOTATIONS */
      
      view = TtaGetViewFromName (document, "Structure_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOn (document, view, Edit_, BCut);
	  TtaSetItemOn (document, view, Edit_, BPaste);
	  TtaSetItemOn (document, view, Edit_, BClear);
	  TtaSetItemOn (document, view, Edit_, BSpellCheck);
	  TtaSetItemOn (document, view, Edit_, BTransform);
	  TtaSetMenuOn (document, view, StructTypes);
	  TtaSetMenuOn (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Alternate_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOn (document, view, Edit_, BCut);
	  TtaSetItemOn (document, view, Edit_, BPaste);
	  TtaSetItemOn (document, view, Edit_, BClear);
	  TtaSetItemOn (document, view, Edit_, BSpellCheck);
	  TtaSetMenuOn (document, view, StructTypes);
	  TtaSetMenuOn (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Links_view");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOn (document, view, Edit_, BCut);
	  TtaSetItemOn (document, view, Edit_, BPaste);
	  TtaSetItemOn (document, view, Edit_, BClear);
	  TtaSetItemOn (document, view, Edit_, BSpellCheck);
	  TtaSetItemOn (document, view, Edit_, BTransform);
	  TtaSetMenuOn (document, view, Types);
	}
      view = TtaGetViewFromName (document, "Table_of_contents");
      if (view != 0 && TtaIsViewOpened (document, view))
	{
	  TtaSetItemOn (document, view, Edit_, BCut);
	  TtaSetItemOn (document, view, Edit_, BPaste);
	  TtaSetItemOn (document, view, Edit_, BClear);
	  TtaSetItemOn (document, view, Edit_, BSpellCheck);
	  TtaSetItemOn (document, view, Edit_, BTransform);
	  TtaSetMenuOn (document, view, Types);
	}
    }
}

/*----------------------------------------------------------------------
   ChangeToBrowserMode
   This mode is similar to Browser mode except for 
   the variable ReadOnlyDocument[document]
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void    ChangeToBrowserMode (Document document)
#else
void    ChangeToBrowserMode (document)
Document    doc;
#endif
{
   Document  docSel;

   docSel = TtaGetSelectedDocument ();
   if (docSel == document)
     TtaUnselect (document);

   /* =============> The document is in Read-Only mode now */
   /* change the document status */
   SetDocumentReadOnly (document);
   /* update windows menus */
   UpdateBrowserMenus (document);
}

/*----------------------------------------------------------------------
   Change the Browser/Editor mode                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void    SetBrowserEditor (Document document)
#else
void    SetBrowserEditor (document)
Document    doc;
#endif
{
   Document  docSel;

   docSel = TtaGetSelectedDocument ();
   if (docSel == document)
     TtaUnselect (document);

   if (TtaGetDocumentAccessMode (document))
     {
       /* =============> The document is in Read-Only mode now */
       /* change the document status */
       SetDocumentReadOnly (document);
       ReadOnlyDocument[document] = TRUE;
       /* update windows menus */
       UpdateBrowserMenus (document);
     }
   else
     {
       /* =============> The document is in Read-Write mode now */
       /* change the document status */
       if (TtaIsDocumentModified (document))
	 DocStatusUpdate (document, TRUE);
       /* change the document status */
       ReadOnlyDocument[document] = FALSE;
       TtaSetDocumentAccessMode (document, 1);
       /* update windows menus */
       UpdateEditorMenus (document);
     }
}

/*----------------------------------------------------------------------
   UpdateTransfer updates the status of the current transfer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateTransfer (Document document)
#else
void                UpdateTransfer (document)
Document            doc;

#endif
{
  FilesLoading[document]++;
}

/*----------------------------------------------------------------------
   StopTransfer stops the current transfer                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopTransfer (Document document, View view)
#else
void                StopTransfer (document, view)
Document            doc;
View                view;

#endif
{
  if (document == 0)
    return;
  else if (document == DocBook)
    {
      /* Make Book function stopped */
      DocBook = 0;
      /* stop transfer of the sub-document */
      StopRequest (document);
      StopRequest (IncludedDocument);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
  else if (DocNetworkStatus[document] & AMAYA_NET_ACTIVE)
    {
      if (TtaGetViewFrame (document, 1) != 0)
	TtaChangeButton (document, 1, 1, stopN, FALSE);
      StopRequest (document);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
      TtaSetStatus (document, 1, 
		    TtaGetMessage (AMAYA, AM_LOAD_ABORT), 
		    NULL);
    }
}


/*----------------------------------------------------------------------
   TextURL                                                      
   The Address text field in a document window has been modified by the user
   Load the corresponding document in that window.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TextURL (Document document, View view, CHAR_T* text)
#else
static void         TextURL (document, view, text)
Document            doc;
View                view;
CHAR_T*              text;

#endif
{
  CHAR_T           *s = NULL;
  CHAR_T           *url;
  ThotBool          change;

  change = FALSE;
  if (text)
    {
      /* remove any trailing '\n' chars that may have gotten there
	 after a cut and pase */
      change = RemoveNewLines (text);

      if (!IsW3Path (text))
	{
	  s = TtaAllocString (MAX_LENGTH);
	  change = NormalizeFile (text, s, AM_CONV_NONE);
	  if (!TtaFileExist (s))
	    {
	      /* It is not a valid URL */
	      /* TtaSetTextZone (document, view, 1, DocumentURLs[document]); */
	      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			    text);
	      TtaFreeMemory (s);
	      return;
	    }
	  url = s;
	}
      else
	url = text;
 
      if (!CanReplaceCurrentDocument (document, view))
	{
	  /* restore the previous value @@ */
	  TtaSetTextZone (document, view, 1, DocumentURLs[document]);
	  /* abort the command */
	  return;
	}

      /* do the same thing as a callback form open document form */
      if (change)
	{
	  /* change the text value */
	  TtaSetTextZone (document, view, 1, url);
	  CallbackDialogue (BaseDialog + URLName, STRING_DATA, url);
	}
      else
	CallbackDialogue (BaseDialog + URLName, STRING_DATA, url);

      if (s)
	TtaFreeMemory (s);

      InNewWindow = FALSE;
      CurrentDocument = document;
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
    }
}

/*----------------------------------------------------------------------
   SetWindowTitle
   Change the title of all windows (if view == 0) of document targetDoc
   or only the title of the window associated with the specified view.
   if it's not 0.
   The new title is the content of the TITLE element of document sourceDoc.
   If document sourceDoc does not have any TITLE element, nothing happen.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetWindowTitle (Document sourceDoc, Document targetDoc, View view)
#else
void         SetWindowTitle (sourceDoc, targetDoc, view)
Document            sourceDoc;
Document            targetDoc;
View                view;
#endif
{
   ElementType         elType;
   Element             el, child;
   Language            lang;
   CHAR_T*              text;
   int                 length;

   el = TtaGetMainRoot (sourceDoc);
   elType.ElSSchema = TtaGetDocumentSSchema (sourceDoc);
   if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
     /* sourceDoc is a HTML document */
     {
       /* search the Title element in sourceDoc */
       elType.ElTypeNum = HTML_EL_TITLE;
       el = TtaSearchTypedElement (elType, SearchForward, el);
       if (el)
	 {
	   child = TtaGetFirstChild (el);
	   if (child)
	     {
	       length = TtaGetTextLength (child) + 1;
	       text = TtaAllocString (length);
	       TtaGiveTextContent (child, text, &length, &lang);
               TtaChangeWindowTitle (targetDoc, view, text);
	       TtaFreeMemory (text);
	     }
	 }
     }
}

/*----------------------------------------------------------------------
  InitFormAnswer
  Dialogue form for answering text, user name and password
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitFormAnswer (Document document, View view, const CHAR_T* auth_realm, CHAR_T* server)
#else
void                InitFormAnswer (document, view, auth_realm, server)
Document            document;
View                view;
const CHAR_T       *auth_realm;
CHAR_T             *server;

#endif
{
#ifndef _WINDOWS
   CHAR_T* label;

   TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION),
	       TRUE, 1, 'L', D_CANCEL);

   label = TtaAllocString (((server) ? ustrlen (server) : 0)
			   + ustrlen (TtaGetMessage (AMAYA, 
						     AM_AUTHENTICATION_REALM_SERVER))
			   + ((auth_realm) ? ustrlen (auth_realm) : 0)
			   + 20); /*a bit more than enough memory */
   usprintf (label, TtaGetMessage (AMAYA, AM_AUTHENTICATION_REALM_SERVER),
	     ((auth_realm) ? auth_realm : TEXT("")), 
	     ((server) ? server : TEXT("")));
   TtaNewLabel (BaseDialog + RealmText, BaseDialog + FormAnswer,
		label);
   TtaFreeMemory (label);
   
   TtaNewTextForm (BaseDialog + NameText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);
   TtaNewTextForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameText, Answer_name);
   TtaSetTextForm (BaseDialog + PasswordText, Answer_password);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
   TtaWaitShowDialogue ();
   if (UserAnswer &&
       (Answer_name[0] == WC_EOS || Answer_password[0] == WC_EOS))
     {
       /* no login name or password, retry */
       TtaSetTextForm (BaseDialog + NameText, Answer_name);
       TtaSetTextForm (BaseDialog + PasswordText, Answer_password);
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
       TtaWaitShowDialogue ();
     }
#else /* _WINDOWS */
   CreateAuthenticationDlgWindow (TtaGetViewFrame (document, view), (STRING)auth_realm,
				  server);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  InitInfo
  Displays a message box with the given info text
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       InitInfo (CHAR_T* label, CHAR_T* info)
#else
void       InitInfo (label, info)
CHAR_T    *label;
CHAR_T    *info;
#endif
{
  if (!info || *info == WC_EOS)
    return;
#ifdef _WINDOWS   
  MessageBox (NULL, info, label, MB_OK);
#else /* !_WINDOWS */
  TtaDisplayMessage (CONFIRM, info, NULL);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      InitConfirm3L (Document document, View view, CHAR_T *label1, CHAR_T *label2, CHAR_T *label3, ThotBool withCancel)
#else
void      InitConfirm3L (document, view, label1, label2, label3, withCancel)
Document  document;
View      view;
CHAR_T   *label1;
CHAR_T   *label2;
CHAR_T   *label3;
ThotBool  withCancel;
#endif
{
#ifndef _WINDOWS
  /* Confirm form */
  if (withCancel)
    TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),  
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 3, 'L', D_CANCEL);
  else
    TtaNewDialogSheet (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
		       TtaGetMessage(LIB, TMSG_LIB_CONFIRM),
		       1,  TtaGetMessage(LIB, TMSG_LIB_CONFIRM),
		       FALSE, 3, 'L');
   /* open as many label widgets as \n we find in the label */
   if (label1 && *label1 != WC_EOS)
     TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmForm, label1);
   /* open as many label widgets as \n we find in the label */
   if (label2 && *label2 != WC_EOS)
     TtaNewLabel (BaseDialog + Label2, BaseDialog + ConfirmForm, label2);
   if (label3 && *label3  != WC_EOS)
     TtaNewLabel (BaseDialog + Label3, BaseDialog + ConfirmForm, label3);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
#else  /* _WINDOWS */
   CreateInitConfirm3LDlgWindow (TtaGetViewFrame (document, view),
				 BaseDialog + ConfirmForm,
				 TtaGetMessage (LIB, TMSG_LIB_CONFIRM), label1,
				 label2, label3, withCancel);
#endif /* _WINDOWS */
}

static ThotBool critic = FALSE;
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             InitConfirm (Document document, View view, CHAR_T* label)
#else
void             InitConfirm (document, view, label)
Document         document;
View             view;
CHAR_T*          label;
#endif
{
#ifndef _WINDOWS
   /* Confirm form */

   /* JK: This widget can't be called twice, but it happens when downloading a
      document with protected images. This is a quick silution to avoid the
      sigsev, although it doesn't fix the problem */
   if (critic)
     return;
   else critic = TRUE;
   TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
	       TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_CANCEL);
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
   /* remove the critic section */
   critic = FALSE;
#else  /* _WINDOWS */
   CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view),
			       BaseDialog + ConfirmForm,
			       TtaGetMessage (LIB, TMSG_LIB_CONFIRM), label);
#endif /* _WINDOWS */
}


#ifndef _WINDOWS
/*-------------------------------------------------------------------------
  BrowserForm
  Initializes a form that ask the URI of the opened or new created document.
  -------------------------------------------------------------------------*/
#ifdef __STDC__
static void   BrowserForm (Document doc, View view, CHAR_T* urlname)
#else
static void   BrowserForm (doc, view, urlnale)
Document      doc;
View          view;
CHAR_T       *urlname
#endif
{
   CHAR_T    s[MAX_LENGTH];
   int       i;
   CHAR_T    tempfile[MAX_LENGTH];

   /* Dialogue form for open URL or local */
   i = 0;
   ustrcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));
   
   TtaNewSheet (BaseDialog + FileBrowserForm, TtaGetViewFrame (doc, 1),
		TtaGetMessage (AMAYA, AM_FILE_BROWSER), 3, s,
		TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + FileBrowserText, BaseDialog + FileBrowserForm,
		   TtaGetMessage (AMAYA, AM_SELECTION), 50, 1, TRUE);
   TtaNewLabel (BaseDialog + FileBrowserLocalName,
		BaseDialog + FileBrowserForm, " ");

   /* initialise the text fields in the dialogue box */
   tempfile[0] = WC_EOS;
   if (urlname[0] != WC_EOS)
       NormalizeFile (urlname, tempfile, AM_CONV_NONE);
   
   if (tempfile[0] != WC_EOS && !IsW3Path (tempfile))
     { 
       if (TtaCheckDirectory (tempfile))
	 {
	   ustrcpy (DirectoryName, tempfile);
	   DocumentName[0] = WC_EOS;
	 }
       else
	 TtaExtractName (tempfile, DirectoryName, DocumentName);
       if (DirectoryName[0] == WC_EOS)
	 {
	   ugetcwd (DirectoryName, MAX_LENGTH);
	   DocumentName[0] = WC_EOS;
	 }
       ustrcpy (s, DirectoryName);
       ustrcat (s, DIR_STR);
       ustrcat (s, DocumentName);
     }
   else
     {
       if (tempfile[0] != WC_EOS && IsW3Path (tempfile))
	 {
	   ugetcwd (DirectoryName, MAX_LENGTH);
	   DocumentName[0] = WC_EOS;
	 }
       else
	 {
	   if (DirectoryName[0] == WC_EOS)
	     {
	       ugetcwd (DirectoryName, MAX_LENGTH);
	       DocumentName[0] = WC_EOS;
	     }
	 }
       ustrcpy (s, DirectoryName);
       ustrcat (s, DIR_STR);
       ustrcat (s, DocumentName);
     }
   TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + BrowserDirSelect, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES),
		     BaseDialog + BrowserDocSelect);
   TtaNewTextForm (BaseDialog + FileBrowserFilter,
		   BaseDialog + FileBrowserForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);

   TtaSetTextForm (BaseDialog + FileBrowserText, s);
   TtaSetTextForm (BaseDialog + FileBrowserFilter, ScanFilter);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FileBrowserForm, FALSE);
}
#endif

/*----------------------------------------------------------------------
  InitOpenDocForm initializes a form that ask the URI of the opened or
  new created document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        InitOpenDocForm (Document document, View view, CHAR_T* title)
#else
static void        InitOpenDocForm (document, view, title)
Document           document;
View               view;
CHAR_T*             title;
#endif
{
   CHAR_T            s[MAX_LENGTH];
#ifndef _WINDOWS
#ifdef _GTK
   ThotWidget        dialog_new;
#endif /* _GTK */
   int               i;

   CurrentDocument = document;
   /* Dialogue form for open URL or local */
   i = 0;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_OPEN_URL));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));

#ifdef _GTK
   dialog_new = create_dialog_new (title);
   gtk_widget_show (dialog_new);
#else /* _GTK */
   TtaNewSheet (BaseDialog + OpenForm, TtaGetViewFrame (document, view),
		title, 2, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + URLName, BaseDialog + OpenForm,
		   TtaGetMessage (AMAYA, AM_LOCATION), 50, 1, TRUE);
   TtaNewLabel (BaseDialog + LocalName, BaseDialog + OpenForm, " ");
   if (LastURLName[0] != WC_EOS)
      TtaSetTextForm (BaseDialog + URLName, LastURLName);
   else
     {
	ustrcpy (s, DirectoryName);
	ustrcat (s, DIR_STR);
	ustrcat (s, DocumentName);
	TtaSetTextForm (BaseDialog + URLName, s);
     }
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + OpenForm, TRUE);
#endif /* _GTK */
#else /* _WINDOWS */

   CurrentDocument = document;
   if (LastURLName[0] != WC_EOS)
      usprintf (s, TEXT("%s"), LastURLName);
   else
     usprintf (s, TEXT("%s%c%s"), DirectoryName, DIR_SEP, DocumentName);
   CreateOpenDocDlgWindow (TtaGetViewFrame (document, view), title, s, DocSelect, DirSelect, 2);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenDoc (Document document, View view)
#else
void                OpenDoc (document, view)
Document            document;
View                view;

#endif
{
   if (CanReplaceCurrentDocument (document, view))
     {
       /* load the new document */
       InNewWindow = FALSE;
       InitOpenDocForm (document, view, TtaGetMessage (1, BOpenDoc));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenDocInNewWindow (Document document, View view)
#else
void                OpenDocInNewWindow (document, view)
Document            document;
View                view;

#endif
{
   InNewWindow = TRUE;
   InitOpenDocForm (document, view, TtaGetMessage (1, BOpenInNewWindow));
}


/*----------------------------------------------------------------------
  OpenNew: create a new document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        OpenNew (Document document, View view, int docType)
#else
void        OpenNew (document, view, docType)
Document   document;
View       view;
int        docType;
#endif
{
  CHAR_T  tempfile[MAX_LENGTH];
  int       i;

  /* create a new document */
  InNewWindow = TRUE;
  NewFile = TRUE;
  NewDocType = docType;

  if (NewDocType == docHTML)
    {
      /* generate a default name for the new document */
      if (LastURLName[0] != WC_EOS)
	{
	  TtaExtractName (LastURLName, tempfile, DocumentName);
	  if (IsW3Path (LastURLName))
	  {
		i = ustrlen (tempfile);
		if (tempfile[i - 1] == TEXT (':'))
		  /* LastURLName is the root of the server */
		  i = ustrlen (LastURLName);
	    usprintf (&LastURLName[i], TEXT("%cNew.html"), WC_URL_SEP);
	  }
	  else
	    usprintf (LastURLName, TEXT("%s%cNew.html"), tempfile, WC_DIR_SEP);
	}
      else
	ustrcpy (DocumentName, TEXT("New.html"));
      InitOpenDocForm (document, view, TtaGetMessage (1, BHtml));
    }
  else if (NewDocType == docMath)
    {
      /* generate a default name for the new document */
      if (LastURLName[0] != WC_EOS)
	{
	  TtaExtractName (LastURLName, tempfile, DocumentName);
	  if (IsW3Path (LastURLName))
	  {
		i = ustrlen (tempfile);
		if (tempfile[i - 1] == TEXT (':'))
		  /* LastURLName is the root of the server */
		  i = ustrlen (LastURLName);
	    usprintf (&LastURLName[i], TEXT("%cNew.mml"), WC_URL_SEP);
	  }
	  else
	    usprintf (LastURLName, TEXT("%s%cNew.mml"), tempfile, WC_DIR_SEP);
	}
      else
	ustrcpy (DocumentName, TEXT("New.mml"));
      InitOpenDocForm (document, view, TtaGetMessage (1, BMathml));
    }
  else if (NewDocType == docSVG)
    {
      /* generate a default name for the new document */
      if (LastURLName[0] != WC_EOS)
	{
	  TtaExtractName (LastURLName, tempfile, DocumentName);
	  if (IsW3Path (LastURLName))
	  {
		i = ustrlen (tempfile);
		if (tempfile[i - 1] == TEXT (':'))
		  /* LastURLName is the root of the server */
		  i = ustrlen (LastURLName);
	    usprintf (&LastURLName[i], TEXT("%cNew.svg"), WC_URL_SEP);
	  }
	  else
	    usprintf (LastURLName, TEXT("%s%cNew.svg"), tempfile, WC_DIR_SEP);
	}
      else
	ustrcpy (DocumentName, TEXT("New.svg"));
      InitOpenDocForm (document, view, TtaGetMessage (1, BSvg));
    }
  else
    {
      if (LastURLName[0] != WC_EOS)
	{
	  TtaExtractName (LastURLName, tempfile, DocumentName);
	  usprintf (LastURLName, TEXT("%s%cNew.css"), tempfile, WC_DIR_SEP);
	}
      else
	ustrcpy (DocumentName, TEXT("New.css"));
      InitOpenDocForm (document, view, TtaGetMessage (1, BCss));
    }
}

/*----------------------------------------------------------------------
  Load the Home page
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            GoToHome (Document doc, View view)
#else
void            GoToHome (doc, view)
Document        doc;
View            view;
#endif
{
  CHAR_T   *s, *lang;

  s = TtaGetEnvString ("HOME_PAGE");
  lang = TtaGetVarLANG ();
  if (s == NULL)
    {
      s = TtaGetEnvString ("THOTDIR");
      usprintf (LastURLName, TEXT("%s%camaya%c%s.%s"), s, DIR_SEP, DIR_SEP, AMAYA_PAGE, lang);
      
      if (!TtaFileExist (LastURLName))
	usprintf (LastURLName, TEXT("%s%camaya%c%s"), s, DIR_SEP, DIR_SEP, AMAYA_PAGE);
    }
  else
    ustrcpy (LastURLName, s);
  InNewWindow = FALSE;
  CurrentDocument = doc;
  CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
}


/*----------------------------------------------------------------------
   InitDocView prepares the main view of a new document.
   logFile is TRUE if the new view is created to display a log file
   sourceOfDoc is not zero when we're opening the source view of a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document     InitDocView (Document doc,
			  CHAR_T* docname,
			  DocumentType docType,
			  Document sourceOfDoc,
			  ThotBool readOnly)
#else
Document     InitDocView (doc, docname, docType, sourceOfDoc, readOnly)
Document     doc;
CHAR_T*      docname;
DocumentType docType;
Document     sourceOfDoc;
ThotBool     readOnly;
#endif
{
  View                mainView, structView, altView, linksView, tocView;
  Document            old_doc;
  CHAR_T             *tmp;
  int                 x, y, w, h;
  int                 requested_doc;
  ThotBool            isOpen, reinitialized;
#ifdef _WINDOWS

  Window_Curs = IDC_WINCURSOR;
#endif /* _WINDOWS */

  /* is there any editing function available */
  if (!TtaCanEdit ())
    /* change the document status */
    readOnly = TRUE;
  
  /* previous document */
  old_doc = doc;

  if (doc != 0 && docType != docLog && !TtaIsDocumentModified (doc))
    /* the new document will replace another document in the same window */
    {
      /* keep in memory if the closed document is in read-only mode */
      if (ReadOnlyDocument[doc])
	readOnly = TRUE;
      if (DocumentTypes[doc] == docHTML ||
	  DocumentTypes[doc] == docSVG ||
	  DocumentTypes[doc] == docMath)
	{
	  TtaSetToggleItem (doc, 1, Views, TShowMapAreas, FALSE);
	  TtaSetToggleItem (doc, 1, Special, TSectionNumber, FALSE);
	  /* close the Alternate view if it is open */
	  altView = TtaGetViewFromName (doc, "Alternate_view");
	  if (altView != 0 && TtaIsViewOpened (doc, altView))
	    TtaCloseView (doc, altView);
	  /* close the Structure view if it is open */
	  structView = TtaGetViewFromName (doc, "Structure_view");
	  if (structView != 0 && TtaIsViewOpened (doc, structView))
	    TtaCloseView (doc, structView);
	  /* close the Links view if it is open */
	  linksView = TtaGetViewFromName (doc, "Links_view");
	  if (linksView != 0 && TtaIsViewOpened (doc, linksView))
	    TtaCloseView (doc, linksView);
	  /* close the Table_of_contents view if it is open */
	  tocView = TtaGetViewFromName (doc, "Table_of_contents");
	  if (tocView != 0 && TtaIsViewOpened (doc, tocView))
	    TtaCloseView (doc, tocView);
	}
        /* remove the current selection */
	TtaUnselect (doc);
	UpdateContextSensitiveMenus (doc);
	TtaFreeView (doc, 1);
	isOpen = TRUE;
	/* use the same document identifier */
	requested_doc = doc;
	old_doc = 0;	/* the previous document doesn't exist any more */
	/* The toolkit has to do its job now */
	TtaHandlePendingEvents ();
     }
   else
     {
       /* open the new document in a fresh window */
       isOpen = FALSE;
       requested_doc = 0;
     }

   /* open the document */
   if (docType == docText ||
       docType == docCSS ||
       docType == docSource ||
       docType == docLog)
     doc = TtaInitDocument (TEXT("TextFile"), docname, requested_doc);
   else if (docType == docAnnot)
     doc = TtaInitDocument (TEXT("Annot"), docname, requested_doc);
   else if (docType == docSVG)
     doc = TtaInitDocument (TEXT("GraphML"), docname, requested_doc);
   else if (docType == docMath)
     doc = TtaInitDocument (TEXT("MathML"), docname, requested_doc);
   else
     doc = TtaInitDocument (TEXT("HTML"), docname, requested_doc);
   if (doc >= DocumentTableLength)
     {
       TtaCloseDocument (doc);
       doc = 0;
     }
   else if (doc > 0)
     {
       /* assign a presentation model to the document */
       if (docType == docText ||
	   docType == docCSS ||
           docType == docSource ||
	   docType == docLog)
	   TtaSetPSchema (doc, TEXT("TextFileP"));
       else if (docType == docAnnot)
	   TtaSetPSchema (doc, TEXT("AnnotP"));
       else if (docType == docSVG)
	   TtaSetPSchema (doc, TEXT("GraphMLP"));
       else if (docType == docMath)
	   TtaSetPSchema (doc, TEXT("MathMLP"));
       /* @@ shouldn't we have a Color and BW case for annots too? */
       else
	 {
	   if (TtaGetScreenDepth () > 1)
	     TtaSetPSchema (doc, TEXT("HTMLP"));
	   else
	     TtaSetPSchema (doc, TEXT("HTMLPBW"));
	 }

       TtaSetNotificationMode (doc, 1);
       /* get the geometry of the main view */
       if (docType == docAnnot)
	 tmp = "Annot_Formatted_view";
       else if (sourceOfDoc && docType == docSource)
	 tmp = "Source_view";
       else
	 tmp = "Formatted_view";
       TtaGetViewGeometry (doc, tmp, &x, &y, &w, &h);
       if (docType == docLog)
	 {
	   x += 100;
	   y += 60;
	   h = 300;
	   w = 550;
	 }
       else if (docType == docMath)
	 {
	   h = 300;
	   w = 550;
	 }
       /* change the position slightly to avoid hiding completely the main
	  view of other documents */
       x = x + (doc - 1) * 10;
       y = y + (doc - 1) * 10;
       /* open the main view */
       mainView = TtaOpenMainView (doc, x, y, w, h);
       
       if (mainView == 0)
	 {
	   TtaCloseDocument (doc);
	   return (0);
	 }

       /* By default no log file */
       TtaSetItemOff (doc, 1, Special, BShowLogFile);
       if (docType == docLog)
	 {
	   TtaSetItemOff (doc, 1, File, BHtml);
	   TtaSetItemOff (doc, 1, File, BMathml);
	   TtaSetItemOff (doc, 1, File, BSvg);
	   TtaSetItemOff (doc, 1, File, BTemplate);
	   TtaSetItemOff (doc, 1, File, BCss);
	   TtaSetItemOff (doc, 1, File, BOpenDoc);
	   TtaSetItemOff (doc, 1, File, BOpenInNewWindow);
	   TtaSetItemOff (doc, 1, File, BReload);
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   TtaSetItemOff (doc, 1, File, BExit);
	   TtaSetMenuOff (doc, 1, Edit_);
	   TtaSetMenuOff (doc, 1, Types);
	   TtaSetMenuOff (doc, 1, Links);
	   TtaSetMenuOff (doc, 1, Views);
	   TtaSetMenuOff (doc, 1, Style);
	   TtaSetMenuOff (doc, 1, Special);
	   TtaSetMenuOff (doc, 1, Attributes_);
#ifdef ANNOTATIONS
	   TtaSetMenuOff (doc, 1, Annotations_);
#endif /* ANNOTATIONS */
	   TtaSetMenuOff (doc, 1, Help_);
	   TtcSwitchButtonBar (doc, 1); /* no button bar */
	   TtcSwitchCommands (doc, 1); /* no command open */
	 }
       else if (!isOpen)
	 /* use a new window */
	 {
	   /* Create all buttons */
	   iStop =TtaAddButton (doc, 1, stopN, StopTransfer,"StopTransfer",
				TtaGetMessage (AMAYA, AM_BUTTON_INTERRUPT),
				TBSTYLE_BUTTON, FALSE);
	   iBack = TtaAddButton (doc, 1, iconBackNo, GotoPreviousHTML,
				 "GotoPreviousHTML",
				 TtaGetMessage (AMAYA, AM_BUTTON_PREVIOUS),
				 TBSTYLE_BUTTON, FALSE);
	   iForward = TtaAddButton (doc, 1, iconForwardNo, GotoNextHTML,
				    "GotoNextHTML",
				    TtaGetMessage (AMAYA, AM_BUTTON_NEXT),
				    TBSTYLE_BUTTON, FALSE);
	   iReload = TtaAddButton (doc, 1, iconReload, Reload, "Reload",
				   TtaGetMessage (AMAYA, AM_BUTTON_RELOAD),
				   TBSTYLE_BUTTON, TRUE);
	   iHome = TtaAddButton (doc, 1, iconHome, GoToHome, "GoToHome",
				 TtaGetMessage (AMAYA, AM_BUTTON_HOME),
				 TBSTYLE_BUTTON, TRUE);
	   iEditor = TtaAddButton (doc, 1, iconEditor, SetBrowserEditor,
				   "SetBrowserEditor",
				   TtaGetMessage (AMAYA, AM_BUTTON_BrowseEdit),
				   TBSTYLE_BUTTON, TRUE);
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iSave = TtaAddButton (doc, 1, iconSaveNo, SaveDocument,
				 "SaveDocument",
				 TtaGetMessage (AMAYA, AM_BUTTON_SAVE),
				 TBSTYLE_BUTTON, FALSE);
#ifdef _WINDOWS
	   iPrint = TtaAddButton (doc, 1, iconPrint, SetupAndPrint,  "SetupAndPrint",
				  TtaGetMessage (AMAYA, AM_BUTTON_PRINT),
				  TBSTYLE_BUTTON, TRUE);
#else /* _WINDOWS */
	   iPrint = TtaAddButton (doc, 1, iconPrint, PrintAs,  "PrintAs",
				  TtaGetMessage (AMAYA, AM_BUTTON_PRINT),
				  TBSTYLE_BUTTON, TRUE);
#endif /* WINDOWS */
	   iFind = TtaAddButton (doc, 1, iconFind, TtcSearchText,
				 "TtcSearchText", 
				 TtaGetMessage (AMAYA, AM_BUTTON_SEARCH),
				 TBSTYLE_BUTTON, TRUE);
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iI =  TtaAddButton (doc, 1, iconI, SetOnOffEmphasis,
			       "SetOnOffEmphasis",
			       TtaGetMessage (AMAYA, AM_BUTTON_ITALICS),
			       TBSTYLE_CHECK, TRUE);
	   iB =  TtaAddButton (doc, 1, iconB, SetOnOffStrong, "SetOnOffStrong",
			       TtaGetMessage (AMAYA, AM_BUTTON_BOLD),
			       TBSTYLE_CHECK, TRUE);
	   iT = TtaAddButton (doc, 1, iconT, SetOnOffCode, "SetOnOffCode",
			      TtaGetMessage (AMAYA, AM_BUTTON_CODE),
			      TBSTYLE_CHECK, TRUE);
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iImage = TtaAddButton (doc, 1, iconImage, CreateImage,
				  "CreateImage", 
				  TtaGetMessage (AMAYA, AM_BUTTON_IMG),
				  TBSTYLE_BUTTON, TRUE);
	   iH1 = TtaAddButton (doc, 1, iconH1, CreateHeading1,
			       "CreateHeading1", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H1),
			       TBSTYLE_BUTTON, TRUE);
	   iH2 = TtaAddButton (doc, 1, iconH2, CreateHeading2,
			       "CreateHeading2", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H2),
			       TBSTYLE_BUTTON, TRUE);
	   iH3 = TtaAddButton (doc, 1, iconH3, CreateHeading3,
			       "CreateHeading3", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H3),
			       TBSTYLE_BUTTON, TRUE);
	   iBullet = TtaAddButton (doc, 1, iconBullet, CreateList,
				   "CreateList",
				   TtaGetMessage (AMAYA, AM_BUTTON_UL),
				   TBSTYLE_BUTTON, TRUE);
	   iNum = TtaAddButton (doc, 1, iconNum, CreateNumberedList,
				"CreateNumberedList",
				TtaGetMessage (AMAYA, AM_BUTTON_OL),
				TBSTYLE_BUTTON, TRUE);
	   iDL = TtaAddButton (doc, 1, iconDL, CreateDefinitionList,
			       "CreateDefinitionList",
			       TtaGetMessage (AMAYA, AM_BUTTON_DL),
			       TBSTYLE_BUTTON, TRUE);
	   iLink = TtaAddButton (doc, 1, iconLink, CreateOrChangeLink,
				 "CreateOrChangeLink",
				 TtaGetMessage (AMAYA, AM_BUTTON_LINK),
				 TBSTYLE_BUTTON, TRUE);
	   iTable = TtaAddButton (doc, 1, iconTable, CreateTable,
				  "CreateTable",
				  TtaGetMessage (AMAYA, AM_BUTTON_TABLE),
				  TBSTYLE_BUTTON, TRUE);
	   
#ifdef AMAYA_PLUGIN 
#ifndef _WINDOWS
	   TtaAddButton (doc, 1, iconPlugin, TtaCreateFormPlugin,
			 "TtaCreateFormPlugin",
			 TtaGetMessage (AMAYA, AM_BUTTON_PLUGIN),
			 TBSTYLE_BUTTON, TRUE);
#endif /* !_WINDOWS */
#endif /* AMAYA_PLUGIN */
	   AddMathButton (doc, 1);
#ifdef GRAPHML
	   AddGraphicsButton (doc, 1);
#endif /* GRAPHML */
	   if (docType == docAnnot)
	     TtcSwitchCommands (doc, 1); /* no command open */
	   else
	     TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL), TRUE,
			   TextURL);

	   /* initial state for menu entries */
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   TtaSetToggleItem (doc, 1, Views, TShowButtonbar, TRUE);
	   TtaSetToggleItem (doc, 1, Views, TShowTextZone, TRUE);
	   TtaSetToggleItem (doc, 1, Views, TShowMapAreas, FALSE);
	   TtaSetToggleItem (doc, 1, Views, TShowTargets, FALSE);
	   TtaSetMenuOff (doc, 1, Attributes_);

	   /* if we open the new document in a new view, control */
	   /* is transferred from previous document to new document */
	   if (old_doc != doc && old_doc != 0)
	     {
	       ResetStop (old_doc);
	       /* clear the status line of previous document */
	       TtaSetStatus (old_doc, 1, TEXT(" "), NULL);
	       ActiveTransfer (doc);
	     }
	 }
     }

   /* do we have to redraw buttons and menus? */
   reinitialized = FALSE;
   if ((docType == docHTML || docType == docImage || docType == docSVG ) &&
       (DocumentTypes[doc] != docHTML || DocumentTypes[doc] != docImage ||
	DocumentTypes[doc] != docSVG))
     /* we need to update menus and buttons */
     reinitialized = TRUE;
   else if ((docType == docCSS || docType == docText) &&
	    (DocumentTypes[doc] != docCSS || DocumentTypes[doc] != docText))
     /* we need to update menus and buttons */
     reinitialized = TRUE;
   else if (docType == docMath ||
#ifdef ANNOTATIONS
	    docType == docAnnot ||
#endif /* ANNOTATIONS */
	    docType == docSource)
     reinitialized = TRUE;

   /* store the new document type */
   DocumentTypes[doc] = docType;
   /* set the document in Read-Only mode */
   if (readOnly)
     ReadOnlyDocument[doc] = TRUE;
   
   if (reinitialized || !isOpen)
     {
     /* now update menus and buttons according to the document status */
     if ((DocumentTypes[doc] == docText ||
	  DocumentTypes[doc] == docCSS ||
	  DocumentTypes[doc] == docSource) ||
	 (ReadOnlyDocument[doc] &&
	  (DocumentTypes[doc] == docHTML ||
	   DocumentTypes[doc] == docImage ||
	   DocumentTypes[doc] == docSVG ||
	   DocumentTypes[doc] == docMath)))
       {
	 TtaChangeButton (doc, 1, iI, iconINo, FALSE);
	 TtaChangeButton (doc, 1, iB, iconBNo, FALSE);
	 TtaChangeButton (doc, 1, iT, iconTNo, FALSE);
	 TtaChangeButton (doc, 1, iImage, iconImageNo, FALSE);
	 TtaChangeButton (doc, 1, iH1, iconH1No, FALSE);
	 TtaChangeButton (doc, 1, iH2, iconH2No, FALSE);
	 TtaChangeButton (doc, 1, iH3, iconH3No, FALSE);
	 TtaChangeButton (doc, 1, iBullet, iconBulletNo, FALSE);
	 TtaChangeButton (doc, 1, iNum, iconNumNo, FALSE);
	 TtaChangeButton (doc, 1, iDL, iconDLNo, FALSE);
	 TtaChangeButton (doc, 1, iLink, iconLinkNo, FALSE);
	 TtaChangeButton (doc, 1, iTable, iconTableNo, FALSE);

	 SwitchIconMath (doc, 1, FALSE);
	 SwitchIconGraph (doc, 1, FALSE);

	 TtaSetItemOff (doc, 1, Edit_, BTransform);
	 TtaSetMenuOff (doc, 1, Types);
	 TtaSetMenuOff (doc, 1, Links);
	 TtaSetMenuOff (doc, 1, Style);
	 TtaSetItemOff (doc, 1, Special, BMakeBook);
	 TtaSetMenuOff (doc, 1, Attributes_);

	 if (ReadOnlyDocument[doc])
	   {
	     /* the document is in ReadOnly mode */
	     TtaSetItemOff (doc, 1, Edit_, BUndo);
	     TtaSetItemOff (doc, 1, Edit_, BRedo);
	     TtaSetItemOff (doc, 1, Edit_, BCut);
	     TtaSetItemOff (doc, 1, Edit_, BPaste);
	     TtaSetItemOff (doc, 1, Edit_, BClear);
	     TtaSetItemOff (doc, 1, Edit_, BSpellCheck);
	     TtaSetToggleItem (doc, 1, Edit_, TEditMode, FALSE);
	     TtaSetItemOff (doc, 1, Special, TSectionNumber);
#ifdef ANNOTATIONS
             TtaSetMenuOff (doc, 1, Annotations_);
#endif /* ANNOTATIONS */
#ifdef _WINDOWS 
	     WIN_TtaSwitchButton (doc, 1, iEditor, iconEditor,
				  TB_INDETERMINATE, TRUE);
#else  /* _WINDOWS */
	     TtaChangeButton (doc, 1, iEditor, iconBrowser, TRUE);
#endif /* _WINDOWS */
	   }
	 else
	   TtaSetToggleItem (doc, 1, Edit_, TEditMode, TRUE);

	 if ((DocumentTypes[doc] == docText ||
	      DocumentTypes[doc] == docCSS ||
	      DocumentTypes[doc] == docSource) ||
	     (ReadOnlyDocument[doc] &&
	      (DocumentTypes[doc] == docSVG ||
	       DocumentTypes[doc] == docMath)))
	   {
	     TtaSetItemOff (doc, 1, Views, TShowMapAreas);
	     TtaSetItemOff (doc, 1, Views, TShowTargets);
	     TtaSetItemOff (doc, 1, Views, BShowStructure);
	     TtaSetItemOff (doc, 1, Views, BShowAlternate);
	     TtaSetItemOff (doc, 1, Views, BShowLinks);
	     TtaSetItemOff (doc, 1, Views, BShowToC);
	     TtaSetItemOff (doc, 1, Views, BShowSource);
	   }
       }
     else if (DocumentTypes[doc] == docHTML ||
	      DocumentTypes[doc] == docImage ||
	      DocumentTypes[doc] == docSVG ||
	      DocumentTypes[doc] == docMath)
       {
	 if (reinitialized)
	   {
	     /* the document is in ReadWrite mode */
#ifdef _WINDOWS 
	     WIN_TtaSwitchButton (doc, 1, iEditor, iconEditor,
				  TB_INDETERMINATE, FALSE);
#else  /* _WINDOWS */
	     TtaChangeButton (doc, 1, iEditor, iconEditor, TRUE);
#endif /* _WINDOWS */
	     TtaChangeButton (doc, 1, iSave, iconSaveNo, FALSE);
	     TtaChangeButton (doc, 1, iI, iconI, TRUE);
	     TtaChangeButton (doc, 1, iB, iconB, TRUE);
	     TtaChangeButton (doc, 1, iT, iconT, TRUE);
	     TtaChangeButton (doc, 1, iImage, iconImage, TRUE);
	     TtaChangeButton (doc, 1, iH1, iconH1, TRUE);
	     TtaChangeButton (doc, 1, iH2, iconH2, TRUE);
	     TtaChangeButton (doc, 1, iH3, iconH3, TRUE);
	     TtaChangeButton (doc, 1, iBullet, iconBullet, TRUE);
	     TtaChangeButton (doc, 1, iNum, iconNum, TRUE);
	     TtaChangeButton (doc, 1, iDL, iconDL, TRUE);
	     TtaChangeButton (doc, 1, iLink, iconLink, TRUE);
	     TtaChangeButton (doc, 1, iTable, iconTable, TRUE);
	     SwitchIconMath (doc, 1, TRUE);
#ifdef GRAPHML
	     SwitchIconGraph (doc, 1, TRUE);
#endif /* GRAPHML */

	     TtaSetItemOn (doc, 1, Edit_, BTransform);
	     TtaSetMenuOn (doc, 1, Types);
	     TtaSetMenuOn (doc, 1, Links);
	     TtaSetMenuOn (doc, 1, Style);
	     TtaSetItemOn (doc, 1, Special, TSectionNumber);
	     TtaSetItemOn (doc, 1, Special, BMakeBook);
	     TtaSetMenuOn (doc, 1, Attributes_);

 	     TtaSetItemOn (doc, 1, Views, TShowMapAreas);
	     TtaSetItemOn (doc, 1, Views, TShowTargets);
	     TtaSetItemOn (doc, 1, Views, BShowStructure);
	     TtaSetItemOn (doc, 1, Views, BShowAlternate);
	     TtaSetItemOn (doc, 1, Views, BShowLinks);
	     TtaSetItemOn (doc, 1, Views, BShowToC);
	     TtaSetItemOn (doc, 1, Views, BShowSource);
	   }
	 TtaSetToggleItem (doc, 1, Edit_, TEditMode, TRUE);
       }
     }
   return (doc);
}

/*----------------------------------------------------------------------
  CreateHTMLContainer creates an HTML container for an image
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CreateHTMLContainer (CHAR_T* pathname, CHAR_T* docname,
				 CHAR_T* tempfile, ThotBool local)
#else
static void CreateHTMLContainer (pathname, docname, tempfile, local)
CHAR_T* pathname;
CHAR_T* docname;
CHAR_T* tempfile;
ThotBool local;
#endif 
{
  FILE *file;
  CHAR_T* tempfile_new;
  CHAR_T* ptr;

  if (!local)
    {
      /* Rename the current downloaded file (an image) so that we can
	 find it easily next time around. 
	 The convention is to change the image's extension to 'html',
	 and give the HTML's container the image's extension */
      tempfile_new = TtaAllocString (ustrlen (tempfile) + ustrlen (docname) + 10);
      ustrcpy (tempfile_new, tempfile);
      ptr = ustrrchr (tempfile_new, WC_DIR_SEP);
      ptr++;
      ustrcpy (ptr, docname);
      ptr = ustrrchr (tempfile_new, TEXT('.'));
      if (ptr)
	{
	  ptr++;
	  ustrcpy (ptr, TEXT("html"));
	}
      else
	ustrcat (tempfile_new, TEXT(".html"));
      TtaFileUnlink (tempfile_new);
#ifndef _WINDOWS
      rename (tempfile, tempfile_new);
#else /* _WINDOWS */
      if (urename (tempfile, tempfile_new)  != 0)
	usprintf (tempfile_new, TEXT("%s"), tempfile); 
#endif /* _WINDOWS */
      urename (tempfile, tempfile_new);
      TtaFreeMemory (tempfile_new);
    }
  /* create a temporary file for the container and make Amaya think
     that it is the current downloaded file */
  file = ufopen (tempfile, TEXT("w"));
  if (local)
    fprintf (file, "<html><head></head><body>"
	     "<img src=\"%s\"></body></html>", pathname);
  else
    fprintf (file, "<html><head></head><body>"
	     "<img src=\"internal:%s\"></body></html>", pathname);
  fclose (file);
}

/*----------------------------------------------------------------------
  MoveImageFile moves an image file (related to an HTML container) from 
  one directory to another
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void MoveImageFile (Document source_doc, Document dest_doc,
			   CHAR_T* documentname)
#else
static void MoveImageFile (source_doc, dest_doc, documentname)
Document source_doc;
Document dest_doc;
CHAR_T* documentname;
#endif
{
  CHAR_T* source;
  CHAR_T* target;
  CHAR_T* imagefile;
  CHAR_T* ptr;
  
  /* generate the name of the file where is stored the image */
  imagefile = TtaAllocString (ustrlen (documentname) + 6);
  ustrcpy (imagefile, documentname);
  ptr = ustrrchr (imagefile, TEXT('.'));
  if (!ptr)
     ustrcat (imagefile, TEXT(".html"));
  else
     ustrcpy (&(ptr[1]), TEXT("html"));

  /* create the source and dest file names */
  source = TtaAllocString (ustrlen (TempFileDirectory) + ustrlen (imagefile) + 6);
  usprintf (source, TEXT("%s%c%d%c%s"), 
	   TempFileDirectory, WC_DIR_SEP, source_doc, WC_DIR_SEP, imagefile);
  target = TtaAllocString (ustrlen (TempFileDirectory) + ustrlen (imagefile) + 6);
  usprintf (target, TEXT("%s%c%d%c%s"), 
	   TempFileDirectory, WC_DIR_SEP, dest_doc, WC_DIR_SEP, imagefile);

  /* move the file */
  TtaFileCopy (source, target);
  TtaFileUnlink (source);

  TtaFreeMemory (source);
  TtaFreeMemory (target);
  TtaFreeMemory (imagefile);
}

/*----------------------------------------------------------------------
  LoadHTMLDocument parses the new document and stores its path (or
  URL) into the document table.
  For a local loading, the parameter tempfile must be an empty string.
  For a remote loading, the parameter tempfile gives the file name that
  contains the current copy of the remote file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Document     LoadHTMLDocument (Document doc, CHAR_T* pathname, CHAR_T* form_data, CHAR_T* initial_url, int method, CHAR_T* tempfile, CHAR_T* documentname, AHTHeaders *http_headers, ThotBool history)
#else
static Document     LoadHTMLDocument (doc, pathname, form_data, initial_url, method, tempfile, documentname, http_headers, history)
Document            doc;
CHAR_T*             pathname;
CHAR_T*             form_data;
CHAR_T*             initial_url;
int                 method;
CHAR_T*             tempfile;
CHAR_T*             documentname;
AHTHeaders*         http_headers
ThotBool            history;
#endif
{
  CSSInfoPtr          css;
  Document            newdoc = 0;
  DocumentType        docType;
  CHARSET             charset, httpcharset;
  CHARSET             metacharset = UNDEFINED_CHARSET;
  CHAR_T*             charEncoding;
  CHAR_T*             tempdocument;
  CHAR_T*             tempdir;
  CHAR_T*             s;
  CHAR_T*             content_type;
  STRING              profile;
  int                 i, j;
  int                 parsingLevel;
  ThotBool            unknown;
  ThotBool            plainText;
  ThotBool            xmlDec, withDoctype, isXML;
  DocumentType        thotType;

  docType = docText;
  unknown = TRUE;
  tempdir = tempdocument = NULL;
  content_type = HTTP_headers (http_headers, AM_HTTP_CONTENT_TYPE);
  /* check if there is an XML declaration with a charset declaration */
  if (tempfile[0] != WC_EOS)
    CheckDocHeader (tempfile, &xmlDec, &withDoctype,
		    &isXML, &parsingLevel, &charset, &thotType);
  else
    CheckDocHeader (pathname, &xmlDec, &withDoctype,
		    &isXML, &parsingLevel, &charset, &thotType);

  if (charset == UNDEFINED_CHARSET && isXML && thotType == docHTML)
    {
      if (tempfile[0] != WC_EOS)
	CheckCharsetInMeta (tempfile, &metacharset);
      else
	CheckCharsetInMeta (pathname, &metacharset);
    }
  
  if (content_type == NULL || content_type[0] == EOS)
    /* no content type */
    {
      /* check file name extension */
      if (parsingLevel != L_Other || IsHTMLName (pathname))
	{
	  /* it seems to be an HTML document */
	  docType = docHTML;
	  parsingLevel = L_Transitional;
	  unknown = FALSE;
	}
      else if (isXML || IsXMLName (pathname))
	{
	  /* it seems to be an XML document */
	  isXML = TRUE;
	  docType = thotType;
	  unknown = FALSE;
	}
      else if (IsMathMLName (pathname))
	{
	  /* it seems to be an XML document */
	  isXML = TRUE;
	  docType = docMath;
	  unknown = FALSE;
	}
       else if (IsSVGName (pathname))
	{
	  /* it seems to be an XML document */
	  isXML = TRUE;
	  docType = docSVG;
	  unknown = FALSE;
	}
     else if (IsCSSName (pathname))
	{
	  docType = docCSS;
	  unknown = FALSE;
	}
      else if (IsImageName (pathname))
	{
	  /* It's a local image file that we can display. We change the 
	     doctype flag so that we can create an HTML container later on */
	  docType = docImage;
	  unknown = FALSE;
	  parsingLevel = L_Transitional;
	}
      }
   else
     /* the server returned a content type */
     {
       i = 0;
       while (content_type[i] != URL_SEP && content_type[i] != EOS)
	 i++;
       if (content_type[i] == URL_SEP)
	 {
	   content_type[i] = EOS;
	   j = i+1;
	   while (content_type[j] != TEXT(';') && content_type[j] != EOS)
	     j++;
	   if (content_type[j] == TEXT(';'))
	     content_type[j] = EOS;
	   if (!ustrcasecmp (content_type, TEXT("text")))
	     {
	       if (!ustrncasecmp (&content_type[i+1], TEXT("html"), 4))
		 {
		   /* it's an HTML document */
		   docType = docHTML;
		   if (parsingLevel == L_Other)
		     parsingLevel = L_Transitional;
		   unknown = FALSE;
		 }
	       else if (!ustrncasecmp (&content_type[i+1], TEXT("xhtml"), 5))
		 {
		   /* it's an XHTML document */
		   docType = docHTML;
		   if (parsingLevel == L_Other)
		     parsingLevel = L_Transitional;
		   unknown = FALSE;
		 }
	       else if (!ustrncasecmp (&content_type[i+1], TEXT("xml"), 3))
		 {
		   /* it's an XML document */
		   isXML = TRUE;
		   if (thotType == docSVG || thotType == docMath)
		     docType = thotType;
		   else if (parsingLevel == L_Other)
		     docType = docText;
		   else
		     docType = docHTML;
		   unknown = FALSE;
		 }
	       else if (!ustrncasecmp (&content_type[i+1], TEXT("css"), 3))
		 {
		   docType = docCSS;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }
	       else if (!ustrncasecmp (&content_type[i+1], TEXT("mathml"), 6) ||
			!ustrncasecmp (&content_type[i+1], TEXT("x-mathml"), 8))
		 {
		   /* it's an XML document */
		   isXML = TRUE;
		   docType = docMath;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }
	       else
		 {
		   docType = docText;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }
	     }
	   else if (!ustrcasecmp (content_type, TEXT("application")))
	     {
	       if (!ustrncasecmp (&content_type[i+1], TEXT("x-sh"), 4))
		 {
		   docType = docText;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }	     
	       else if (!ustrncasecmp (&content_type[i+1], TEXT("mathml"), 6))
		 {
		   /* it's an XML document */
		   isXML = TRUE;
		   docType = docMath;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }
	     }
	   else if (!ustrcasecmp (content_type, TEXT("image")))
	     {
	       if (!ustrncasecmp (&content_type[i+1], TEXT("svg"), 3))
		 {
		   /* it's an XML document */
		   isXML = TRUE;
		   docType = docSVG;
		   parsingLevel = L_Other;
		   unknown = FALSE;
		 }
	       else
		 {
		   /* get a pointer to the type ('/' substituted with an EOS
		      earlier in this function */
		   i = 0;
		   while (content_type[i])
		     i++;
		   i++;
		   /* we'll generate a HTML document */
		   if (IsImageType (&content_type[i]))
		     {
		       docType = docImage;
		       unknown = FALSE;
		       parsingLevel = L_Transitional;
		     }
		 }
	     }
	 }
     }

  if (unknown && tempfile[0] != WC_EOS)
    {
      /* The document is not an HTML file and cannot be parsed */
      /* rename the temporary file */
      ustrcpy (SavingFile, tempfile);
      SavingDocument = 0;
      SavingObject = 0;
      tempdocument = TtaAllocString (MAX_LENGTH);
      TtaExtractName (pathname, tempfile, tempdocument);
      /* reinitialize directories and document lists */
      ustrcpy (pathname, DirectoryName);
      ustrcat (pathname, WC_DIR_STR);
      ustrcat (pathname, tempdocument);
      ustrcpy (SavePath, DirectoryName);
      ustrcpy (SaveName, tempdocument);
      ResetStop (doc);
      InitSaveObjectForm (doc, 1, SavingFile, pathname);
    }
  else if (pathname[0] != WC_EOS)
    {
      if (method != CE_MAKEBOOK)
	{
	  /* do not register and open the document view */
	  if (DocumentURLs[doc])
	    {
	      /* save the URL of the old document in the history, if the same
		 window is reused, i.e. if the old document has not been
		 modified */
	      if (history && !TtaIsDocumentModified (doc))
		AddDocHistory (doc, DocumentURLs[doc], 
			       DocumentMeta[doc]->initial_url,
			       DocumentMeta[doc]->form_data,
			       DocumentMeta[doc]->method);
	    }

	  /*
	   * free the previous document and prepare the window
	   * for the new one
	   */
	  if (method == CE_RELATIVE && docType == docCSS)
	    {
	      /* display the CSS in a new window */
	      newdoc = InitDocView (0, documentname, docType, 0, FALSE);
	      ResetStop (doc);
	      /* clear the status line of previous document */
	      TtaSetStatus (doc, 1, TEXT(" "), NULL);
	      ActiveTransfer (newdoc);
	    }
#ifdef ANNOTATIONS
	  else if (method == CE_ANNOT && docType == docHTML)
	    {
	      docType = docAnnot;
	      method = CE_RELATIVE;
	      newdoc = doc;
	      /* @@ IV: we are not currently able to use the XML parser for 
		 annotations */
	      isXML = FALSE;
	    }
#endif /* ANNOTATIONS */
	  else if (method == CE_LOG)
	    {
	      docType = docLog;
	      newdoc = doc;
	    }
	  else if (method != CE_INIT || docType != docHTML)
	    newdoc = InitDocView (doc, documentname, docType, 0, FALSE);
	  else
	    newdoc = doc;
	}
      else
	newdoc = doc;
	
      if (docType == docImage)
      /* create an HTML container */
	{
	  if (content_type)
	    /* it's an image downloaded from the web */
	    CreateHTMLContainer (pathname, documentname, tempfile, FALSE);
	  else 
	    {
	      /* It's a local image file */
	      usprintf (tempfile, TEXT("%s%c%d%c%s"), TempFileDirectory,
			WC_DIR_SEP, 0, WC_DIR_SEP, TEXT("contain.html"));
	      CreateHTMLContainer (pathname, documentname, tempfile, TRUE);
	    }
	}

      /* what we have to do if doc and targetDocument are different */
      if (tempfile[0] != WC_EOS)
	{
	  /* It is a document loaded from the Web */
	  if (!TtaFileExist (tempfile))
	    {
	      /* Nothing is loaded */
	      ResetStop (doc);
	      return (0);
	    }
	  /* we have to rename the temporary file */
	  /* allocate and initialize tempdocument */
	  tempdocument = GetLocalPath (newdoc, pathname);
	  TtaFileUnlink (tempdocument);
	  if (doc != newdoc)
	    {
	      /* now we can rename the local name of a remote document */
	      TtaFileCopy (tempfile, tempdocument);
	      TtaFileUnlink (tempfile);
	      /* if it's an IMAGEfile, we copy it too to the new directory */
	      if (DocumentTypes[newdoc] == docImage)
		MoveImageFile (doc, newdoc, documentname);
	    }
	  else if (DocumentTypes[newdoc] == docCSS)
	      TtaFileCopy (tempfile, tempdocument);
	    /* now we can rename the local name of a remote document */
	  else
	    /* now we can rename the local name of a remote document */
	    urename (tempfile, tempdocument);
	}
      else
	{
	  /* store a copy of the local document */
	  /* allocate and initialize tempdocument */
	  tempdocument = GetLocalPath (newdoc, pathname);
	  TtaFileCopy (pathname, tempdocument);
	}

      /* store a copy of CSS files in the directory 0 */
      if (DocumentTypes[newdoc] == docCSS)
	{ 
	  css = SearchCSS (0, pathname);
	  if (css == NULL)
	    {
	      /* store a copy of this new CSS context in .amaya/0 */
	      s = GetLocalPath (0, pathname);
	      TtaFileCopy (tempdocument, s);
	      /* initialize a new CSS context */
	      if (UserCSS && !ustrcmp (pathname, UserCSS))
		AddCSS (newdoc, 0, CSS_USER_STYLE, NULL, s);
	      else
		AddCSS (newdoc, 0, CSS_EXTERNAL_STYLE, pathname, s);
	      TtaFreeMemory (s);
	    }
	  else
	    css->doc = newdoc;
	}
      
      /* save the document name into the document table */
      s = TtaWCSdup (pathname);
      if (DocumentURLs[newdoc] != NULL)
	{
	  TtaFreeMemory (DocumentURLs[newdoc]);
	  DocumentURLs[newdoc] = NULL;
	}
      /* if the document was already loaded, warn the user */
      if (IsDocumentLoaded (s, form_data))
	InitConfirm3L (newdoc, 1, TEXT("Warning: this document is already loaded in another Window"), TEXT("Saving that instance may cause a lost update problem"), NULL, FALSE);
      DocumentURLs[newdoc] = s;

      /* save the document's formdata into the document table */
      if (DocumentMeta[newdoc] != NULL)
	DocumentMetaClear (DocumentMeta[newdoc]);
      else
	DocumentMeta[newdoc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
      DocumentMeta[newdoc]->form_data = TtaWCSdup (form_data);
      if (initial_url && ustrcmp (pathname, initial_url))
	DocumentMeta[newdoc]->initial_url = TtaWCSdup (initial_url);
      else
	DocumentMeta[newdoc]->initial_url = NULL;
      DocumentMeta[newdoc]->method = (ClickEvent) method;
      DocumentMeta[newdoc]->put_default_name = FALSE;
      DocumentMeta[newdoc]->xmlformat = isXML;
      DocumentSource[newdoc] = 0;

      /* Set character encoding */
      charEncoding = HTTP_headers (http_headers, AM_HTTP_CHARSET);
      httpcharset = TtaGetCharset (charEncoding);
      if (charset != UNDEFINED_CHARSET)
	TtaSetDocumentCharset (newdoc, charset);
      else if (httpcharset != UNDEFINED_CHARSET)
	TtaSetDocumentCharset (newdoc, httpcharset);
      else if (metacharset != UNDEFINED_CHARSET)
	TtaSetDocumentCharset (newdoc, metacharset);
      
      if (TtaGetViewFrame (newdoc, 1) != 0)
	/* this document is displayed */
	{
	  /* concatenate the URL and its form_data and then
	     display it on the amaya URL box */
	  i = ustrlen (pathname) + 5;
	  if (form_data && method != CE_FORM_POST)
	    i += ustrlen (form_data);
	  s = TtaAllocString (i);

	  if (form_data && method != CE_FORM_POST)
	    usprintf (s, TEXT("%s?%s"), pathname, form_data);
	  else
	    ustrcpy (s, pathname);

	  TtaSetTextZone (newdoc, 1, 1, s);
	  TtaFreeMemory (s);
	}

      tempdir = TtaAllocString (MAX_LENGTH);
      TtaExtractName (tempdocument, tempdir, documentname);
      /* Now we forget the method CE_INIT. It's a standard method */
      if (DocumentMeta[newdoc]->method == CE_INIT)
	DocumentMeta[newdoc]->method = CE_ABSOLUTE;

      /* check the current profile */
      profile = TtaGetEnvString ("Profile");
      if (parsingLevel == L_Other)
	ParsingLevel[newdoc] = parsingLevel;
      else
	{
	  if (!ustrncmp (profile, TEXT("XHTML"), 5))
	    /* force the XML parsing */
	    DocumentMeta[newdoc]->xmlformat = TRUE;
	  if (!ustrncmp (profile, TEXT("XHTML-basic"), 10))
	    ParsingLevel[newdoc] = L_Basic;
	  else if (!ustrncmp (profile, TEXT("XHTML-strict"), 10))
	    ParsingLevel[newdoc] = L_Strict;
	  else
	    ParsingLevel[newdoc] = L_Transitional;
	}

      if (docType == docSVG || docType == docMath)
	plainText = FALSE;
      else
	plainText = (parsingLevel == L_Other);

      /* Calls the corresponding parser */
      if (DocumentMeta[newdoc]->xmlformat && !plainText)
	StartXmlParser (newdoc,	tempdocument, documentname,
			tempdir, pathname, xmlDec, withDoctype);
      else
	StartParser (newdoc, tempdocument, documentname, tempdir, pathname, plainText);
      
      TtaFreeMemory (tempdir);

      /* Set the document read-only when needed */
      if (ReadOnlyDocument[newdoc])
	SetDocumentReadOnly (newdoc);

      if (!plainText)
	{
	if (HTMLErrorsFound || XMLErrorsFound)
	  TtaSetItemOn (newdoc, 1, Special, BShowLogFile);
	else
	  TtaSetItemOff (newdoc, 1, Special, BShowLogFile);
	}
      if (newdoc != doc)
	/* the document is displayed in a different window */
	/* reset the history of the new window */
	InitDocHistory (newdoc);
        /* hide template entry if no template server is configured */
      if (TtaGetEnvString ("TEMPLATE_URL") == NULL)
 	TtaSetItemOff (newdoc, 1, File, BTemplate);
#ifdef ANNOTATIONS
      /* auto-load the annotations associated with the document */
      if (!plainText && DocumentTypes[newdoc] != docAnnot)
	ANNOT_AutoLoad (newdoc, 1);
#endif /* ANNOTATIONS */
    }
  TtaFreeMemory (tempdocument);
  return (newdoc);
}

/*----------------------------------------------------------------------
  Reload_callback
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Reload_callback (int doc, int status, CHAR_T* urlName,
                                     CHAR_T* outputfile, AHTHeaders *http_headers,
				       void * context)
#else  /* __STDC__ */
void                Reload_callback (doc, status, urlName, outputfile, 
				       http_headers, context)
int doc;
int status;
CHAR_T* urlName;
CHAR_T* outputfile;
AHTHeaders *http_headers;
void *context;

#endif
{
  Document newdoc;
  CHAR_T* pathname;
  CHAR_T* tempfile;
  CHAR_T* documentname;
  CHAR_T* form_data;
  CHAR_T* initial_url;
  ClickEvent method;
  Document res;
  Element el;
  RELOAD_context *ctx;
  ThotBool stopped_flag = FALSE;

  /* restore the context associated with the request */
  ctx = (RELOAD_context *) context;
  documentname = ctx->documentname;
  newdoc = ctx->newdoc;
  form_data = ctx->form_data;
  method = ctx->method;

  tempfile = outputfile;

  pathname = TtaAllocString (MAX_LENGTH);
  ustrcpy (pathname, urlName);
  
  if (status == 0)
     {
       TtaSetCursorWatch (0, 0);

       /* a bit of acrobatics so that we can retain the initial_url
	  without reallocating memory */
       initial_url = DocumentMeta[doc]->initial_url;
       DocumentMeta[doc]->initial_url = NULL;

       /* parse and display the document, res contains the new document
	identifier, as given by the thotlib */
       res = LoadHTMLDocument (newdoc, pathname, form_data, NULL, method,
			       tempfile, documentname, http_headers, FALSE);

       if (res == 0)
	 {
	   /* cannot load the document */
	   ResetStop(newdoc);
	   newdoc = 0;
	   TtaFreeMemory (initial_url);
	 }
       else if (newdoc != res)
	 {
	   newdoc = res;
	   /* restore the initial_url */
	   DocumentMeta[newdoc]->initial_url = initial_url;
#ifdef ANNOTATIONS
	   /* if it's an annotation, add the existing metadata */
	   if (DocumentTypes[newdoc] == docAnnot)
	     ANNOT_ReloadAnnotMeta (newdoc);
#endif /* ANNOTATIONS */
	 }
       
       if (newdoc)
	 {
	   W3Loading = 0;		/* loading is complete now */
	   TtaHandlePendingEvents ();
	   /* fetch and display all images referred by the document */
	   stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_NOCACHE | AMAYA_LOAD_IMAGE);
	   if (stopped_flag == FALSE) 
	     {
	       TtaResetCursor (0, 0);
	       /* show the document at the same position as before Reload */
	       el = ElementAtPosition (newdoc, ctx->position);
	       TtaShowElement (newdoc, 1, el, ctx->distance);
	     }
	 }
     }

  if (stopped_flag == FALSE && newdoc)
    ResetStop(newdoc);
  TtaFreeMemory (pathname);
  TtaFreeMemory (documentname);
  if (form_data)
    TtaFreeMemory (form_data);
  TtaFreeMemory (ctx);
}


/*----------------------------------------------------------------------
  Reload
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Reload (Document doc, View view)
#else
void                Reload (doc, view)
Document            doc;
View                view;

#endif
{
   CHAR_T*              tempfile;
   CHAR_T*              pathname;
   CHAR_T*              documentname;
   int                 toparse;
   CHAR_T*             form_data;
   ClickEvent          method;
   int                 mode;
   int		       position;
   int		       distance;
   RELOAD_context     *ctx;

   if (DocumentURLs[doc] == NULL)
      /* the document has not been loaded yet */
      return;

   if (DocumentTypes[doc] == docSource)
      /* don't reload a source document */
      return;

   /* abort all current exchanges concerning this document */
   StopTransfer (doc, 1);

   if (!CanReplaceCurrentDocument (doc, view))
      /* abort the command */
      return;

   /* reload the document */
   pathname = TtaAllocString (MAX_LENGTH);
   documentname = TtaAllocString (MAX_LENGTH);
   /* if the document is a template, restore the template script URL */
   if (DocumentMeta[doc]->method == CE_TEMPLATE)
      ReloadTemplateParams (&(DocumentURLs[doc]), &(DocumentMeta[doc]->method));
   NormalizeURL (DocumentURLs[doc], 0, pathname, documentname, NULL);

   if (!IsW3Path (pathname) && !TtaFileExist (pathname))
     {
       /* Free Memory */
       TtaFreeMemory (pathname);
       TtaFreeMemory (documentname);
       /* cannot reload this document */
       return;
     }

   if (DocumentMeta[doc]->form_data)
     form_data = TtaWCSdup (DocumentMeta[doc]->form_data);
   else
     form_data = NULL;
   method = DocumentMeta[doc]->method;
   if (DocumentTypes[doc] == docAnnot)
     method = method | CE_ANNOT;

   /* get the current position in the document */
   position = RelativePosition (doc, &distance);

   W3Loading = doc;	/* this document is currently in load */
   mode = AMAYA_ASYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;

   if (method == CE_FORM_POST)
     mode |= AMAYA_FORM_POST;

   tempfile = TtaAllocString (MAX_LENGTH);
   tempfile[0] = EOS;
   toparse = 0;
   ActiveTransfer (doc);
   /* Create the context for the callback */

   ctx = TtaGetMemory (sizeof (RELOAD_context));
   ctx->newdoc = doc;
   ctx->documentname = documentname;
   ctx->form_data = form_data;
   ctx->method = method;
   ctx->position = position;
   ctx->distance = distance;

   if (IsW3Path (pathname))
     {
       /* load the document from the Web */
       toparse = GetObjectWWW (doc, pathname, form_data, tempfile, 
			       mode,
			       NULL, NULL, (void *) Reload_callback, 
			       (void *) ctx, YES, NULL);
     }
   else if (TtaFileExist (pathname))
     Reload_callback (doc, 0, pathname, tempfile, NULL, (void *) ctx);

   TtaFreeMemory (tempfile);
   TtaFreeMemory (pathname);
   
   TtaHandlePendingEvents ();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowTargets (Document document, View view)
#else
void                ShowTargets (document, view)
Document            document;
View                view;
#endif
{
  int               visibility;
  View		    tocView;

  visibility = TtaGetSensibility (document, view);
  if (visibility == 4)
    {
      visibility = 5;
      TtaSetToggleItem (document, 1, Views, TShowTargets, FALSE);
    }
  else
    {
      visibility = 4;
      TtaSetToggleItem (document, 1, Views, TShowTargets, TRUE);
    }
  /* Change visibility threshold in the main view */
  TtaSetSensibility (document, view, visibility);
  /* Change visibility threshold in the table of content view if it's open */
  tocView = TtaGetViewFromName (document, "Table_of_contents");
  if (tocView && TtaIsViewOpened (document, tocView))
     TtaSetSensibility (document, tocView, visibility);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ZoomIn (Document document, View view)
#else
void                ZoomIn (document, view)
Document            document;
View                view;
#endif
{
  int               zoom, zoomVal;
  CHAR_T*            zoomStr;

  zoom = TtaGetZoom (document, view);
  if (zoom < 10)
    {
      zoom++;
      TtaSetZoom (document, view, zoom);
    }

  /* compare to the standard value? */
  zoomStr = TtaGetEnvString ("ZOOM");
  if (zoomStr == NULL)
    zoomVal = 0;
  else
    {
      usscanf (zoomStr, TEXT("%d"), &zoomVal);
      if (zoomVal > 10 || zoomVal < -10)
	zoomVal = 0;
    }
  if (zoom > zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomIn, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomIn, FALSE);
  if (zoom < zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomOut, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomOut, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ZoomOut (Document document, View view)
#else
void                ZoomOut (document, view)
Document            document;
View                view;
#endif
{
  int               zoom, zoomVal;
  CHAR_T*            zoomStr;

  zoom = TtaGetZoom (document, view);
  if (zoom > -10)
    {
      zoom--;
      TtaSetZoom (document, view, zoom);
    }

  /* compare to the standard value? */
  zoomStr = TtaGetEnvString ("ZOOM");
  if (zoomStr == NULL)
    zoomVal = 0;
  else
    {
      usscanf (zoomStr, TEXT("%d"), &zoomVal);
      if (zoomVal > 10 || zoomVal < -10)
	zoomVal = 0;
    }
  if (zoom > zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomIn, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomIn, FALSE);
  if (zoom < zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomOut, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomOut, FALSE);
}

/*----------------------------------------------------------------------
  ShowSource
  Display the source code of an HTML page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowSource (Document document, View view)
#else
void                ShowSource (document, view)
Document            document;
View                view;
#endif
{
   CHAR_T*        tempdocument;
   CHAR_T*       s;
   CHAR_T	     documentname[MAX_LENGTH];
   CHAR_T	     tempdir[MAX_LENGTH];
   Document	     sourceDoc;
   NotifyElement event;

   if (!DocumentURLs[document])
     /* the document is not loaded yet */
     return;
   if (DocumentTypes[document] != docHTML &&
       DocumentTypes[document] != docSVG &&
       DocumentTypes[document] != docMath)
     /* it's not an HTML or an XML document */
     return;
   if (DocumentSource[document])
     /* the source code of this document is already shown */
     /* raise its window */
     TtaRaiseView (DocumentSource[document], 1);
   else
     {
     /* save the current state of the document into the temporary file */
     tempdocument = GetLocalPath (document, DocumentURLs[document]);
     if (TtaIsDocumentModified (document))
       {
	 if (DocumentTypes[document] == docHTML)
	   {
	     SetNamespacesAndDTD (document);
	     if (DocumentMeta[document]->xmlformat)
	       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						    TEXT("HTMLTX"));
	     else
	       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						    TEXT("HTMLT"));
	   }
	 else if (DocumentTypes[document] == docSVG)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						TEXT("GraphMLT"));
	 else if (DocumentTypes[document] == docMath)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						TEXT("MathMLT"));
       }
     TtaExtractName (tempdocument, tempdir, documentname);
     /* open a window for the source code */
     sourceDoc = InitDocView (0, documentname, docSource, document, FALSE);   
     if (sourceDoc > 0)
       {
	 DocumentSource[document] = sourceDoc;
	 s = TtaWCSdup (DocumentURLs[document]);
	 DocumentURLs[sourceDoc] = s;
	 DocumentMeta[sourceDoc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
	 DocumentMeta[sourceDoc]->form_data = NULL;
	 DocumentMeta[sourceDoc]->initial_url = NULL;
	 DocumentMeta[sourceDoc]->method = CE_ABSOLUTE;
	 DocumentMeta[sourceDoc]->put_default_name =
	                            DocumentMeta[document]->put_default_name;
	 DocumentMeta[sourceDoc]->xmlformat = FALSE;
	 DocumentTypes[sourceDoc] = docSource;
	 DocNetworkStatus[sourceDoc] = AMAYA_NET_INACTIVE;
	 StartParser (sourceDoc, tempdocument, documentname, tempdir,
		      tempdocument, TRUE);
	 SetWindowTitle (document, sourceDoc, 0);
	 /* Set the document read-only when needed */
	 if (ReadOnlyDocument[document])
	   {
	     ReadOnlyDocument[sourceDoc] = TRUE;
	     SetDocumentReadOnly (sourceDoc);
	   }
	 TtcSwitchButtonBar (sourceDoc, 1); /* no button bar */
	 TtcSwitchCommands (sourceDoc, 1); /* no command open */
	 TtaSetItemOff (sourceDoc, 1, File, New1);
	 TtaSetItemOff (sourceDoc, 1, File, BHtml);
	 TtaSetItemOff (sourceDoc, 1, File, BMathml);
	 TtaSetItemOff (sourceDoc, 1, File, BSvg);
	 TtaSetItemOff (sourceDoc, 1, File, BTemplate);
	 TtaSetItemOff (sourceDoc, 1, File, BCss);
	 TtaSetItemOff (sourceDoc, 1, File, BOpenDoc);
	 TtaSetItemOff (sourceDoc, 1, File, BOpenInNewWindow);
	 TtaSetItemOff (sourceDoc, 1, File, BReload);
	 TtaSetItemOff (sourceDoc, 1, File, BSaveAs);
	 TtaSetItemOff (sourceDoc, 1, Edit_, BSpellCheck);
	 TtaSetItemOff (sourceDoc, 1, Edit_, BTransform);
	 TtaSetItemOff (sourceDoc, 1, Views, TShowButtonbar);
	 TtaSetItemOff (sourceDoc, 1, Views, TShowTextZone);
	 TtaSetMenuOff (sourceDoc, 1, Special);
	 TtaSetMenuOff (sourceDoc, 1, Help_);

    	 /* Switch the synchronization entry */
    	 if (TtaIsDocumentModified (document))
    	    DocStatusUpdate (document, TRUE);
	 /* Synchronize selections */
	 event.document = document;
	 SynchronizeSourceView (&event);
       }
     TtaFreeMemory (tempdocument);
     }
}

/*----------------------------------------------------------------------
  ShowStructure
  Open the structure view(s) of an HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowStructure (Document document, View view)
#else
void                ShowStructure (document, view)
Document            document;
View                view;
#endif
{
   View                structView;
   int                 x, y, w, h;
   CHAR_T              structureName[30];

   ustrcpy (structureName, TEXT("Structure_view"));  
   structView = TtaGetViewFromName (document, structureName);
   if (structView != 0 && TtaIsViewOpened (document, structView))
     TtaRaiseView (document, structView);
   else
     {
       TtaGetViewGeometry (document, structureName, &x, &y, &w, &h);
       structView = TtaOpenView (document, structureName, x, y, w, h);
       
       if (structView != 0)
	 {
	   TtcSwitchButtonBar (document, structView); /* no button bar */
	   TtcSwitchCommands (document, structView); /* no command open */
	   if (ReadOnlyDocument[document])
	     {
	       TtaSetItemOff (document, structView, Edit_, BCut);
	       TtaSetItemOff (document, structView, Edit_, BPaste);
	       TtaSetItemOff (document, structView, Edit_, BClear);
	       TtaSetItemOff (document, structView, Edit_, BSpellCheck);
	       TtaSetItemOff (document, structView, Edit_, BTransform);
	       TtaSetMenuOff (document, structView, StructTypes);
	       TtaSetMenuOff (document, structView, Types);
	       TtaSetMenuOff (document, structView, Attributes_);
	     }
	 }
     }
   SetWindowTitle (document, document, 0);
}

/*----------------------------------------------------------------------
  ShowAlternate
  Open the Alternate view of an HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowAlternate (Document document, View view)
#else
void                ShowAlternate (document, view)
Document            document;
View                view;

#endif
{
   View                altView;
   int                 x, y, w, h;

   altView = TtaGetViewFromName (document, "Alternate_view");
   if (view == altView)
      TtaRaiseView (document, view);
   else if (altView != 0 && TtaIsViewOpened (document, altView))
      TtaRaiseView (document, altView);
   else
     {
	TtaGetViewGeometry (document, "Alternate_view", &x, &y, &w, &h);
	altView = TtaOpenView (document, TEXT("Alternate_view"), x, y, w, h);
	if (altView != 0)
	  {
	    SetWindowTitle (document, document, altView);
	    TtcSwitchButtonBar (document, altView); /* no button bar */
	    TtcSwitchCommands (document, altView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, altView, Edit_, BCut);
		TtaSetItemOff (document, altView, Edit_, BPaste);
		TtaSetItemOff (document, altView, Edit_, BClear);
		TtaSetItemOff (document, altView, Edit_, BSpellCheck);
		TtaSetItemOff (document, altView, Edit_, BTransform);
		TtaSetMenuOff (document, altView, Types);
		TtaSetMenuOff (document, altView, Attributes_);
	      }
	  }
     }
}


/*----------------------------------------------------------------------
  ShowLinks
  Open the Links view of an HTML document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowLinks (Document document, View view)
#else
void                ShowLinks (document, view)
Document            document;
View                view;

#endif
{
   View                linksView;
   int                 x, y, w, h;

   linksView = TtaGetViewFromName (document, "Links_view");
   if (view == linksView)
      TtaRaiseView (document, view);
   else if (linksView != 0 && TtaIsViewOpened (document, linksView))
      TtaRaiseView (document, linksView);
   else
     {
	TtaGetViewGeometry (document, "Links_view", &x, &y, &w, &h);
	linksView = TtaOpenView (document, TEXT("Links_view"), x, y, w, h);
	if (linksView != 0)
	  {
	    SetWindowTitle (document, document, linksView);
	    TtcSwitchButtonBar (document, linksView); /* no button bar */
	    TtcSwitchCommands (document, linksView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, linksView, Edit_, BCut);
		TtaSetItemOff (document, linksView, Edit_, BPaste);
		TtaSetItemOff (document, linksView, Edit_, BClear);
		TtaSetItemOff (document, linksView, Edit_, BSpellCheck);
		TtaSetItemOff (document, linksView, Edit_, BTransform);
		TtaSetMenuOff (document, linksView, Types);
		TtaSetMenuOff (document, linksView, Attributes_);
	      }
	  }
     }
}


/*----------------------------------------------------------------------
  ShowToC
  Open the Table of content view of an HTML document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowToC (Document document, View view)
#else
void                ShowToC (document, view)
Document            document;
View                view;

#endif
{
   View                tocView;
   int                 x, y, w, h;

   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view == tocView)
      TtaRaiseView (document, view);
   else if (tocView != 0 && TtaIsViewOpened (document, tocView))
      TtaRaiseView (document, tocView);
   else
     {
	TtaGetViewGeometry (document, "Table_of_contents", &x, &y, &w, &h);
	tocView = TtaOpenView (document, TEXT("Table_of_contents"), x, y, w, h);
	if (tocView != 0)
	  {
	    SetWindowTitle (document, document, tocView);
	    TtcSwitchButtonBar (document, tocView); /* no button bar */
	    TtcSwitchCommands (document, tocView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, tocView, Edit_, BCut);
		TtaSetItemOff (document, tocView, Edit_, BPaste);
		TtaSetItemOff (document, tocView, Edit_, BClear);
		TtaSetItemOff (document, tocView, Edit_, BSpellCheck);
		TtaSetItemOff (document, tocView, Edit_, BTransform);
		TtaSetMenuOff (document, tocView, Types);
		TtaSetMenuOff (document, tocView, Attributes_);
	      }
	  }
     }
}


/*----------------------------------------------------------------------
   ViewToClose                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ViewToClose (NotifyDialog * event)
#else
ThotBool            ViewToClose (event)
NotifyDialog       *event;

#endif
{
   Document      document;
   View          view, structView, altView, linksView, tocView;

   view = event->view;
   document = event->document;
   structView = TtaGetViewFromName (document, "Structure_view");
   altView = TtaGetViewFromName (document, "Alternate_view");
   linksView = TtaGetViewFromName (document, "Links_view");
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view != 1)
     /* let Thot perform normal operation */
     return FALSE;
   else
     /* closing main view */
     if (!CanReplaceCurrentDocument (document, view))
	/* abort the command and don't let Thot perform normal operation */
	return TRUE;

   if (structView != 0 && TtaIsViewOpened (document, structView))
     TtaCloseView (document, structView);
   if (altView != 0 && TtaIsViewOpened (document, altView))
     TtaCloseView (document, altView);
   if (linksView != 0 && TtaIsViewOpened (document, linksView))
     TtaCloseView (document, linksView);
   if (tocView != 0 && TtaIsViewOpened (document, tocView))
     TtaCloseView (document, tocView);
   /* let Thot perform normal operation */
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void GetHTMLDocument_callback (int newdoc, int status, CHAR_T* urlName, CHAR_T* outputfile, AHTHeaders *http_headers, void * context)
#else  /* __STDC__ */
void GetHTMLDocument_callback (newdoc, status, urlName, outputfile, http_headers, context)
int       newdoc;
int       status;
CHAR_T*   urlName;
CHAR_T*    outputfile;
AHTHeaders *http_headers;
void*     context;

#endif
{
   Element             elFound;
   Document            doc;
   Document            baseDoc;
   Document            res;
   CHAR_T*             tempfile;
   CHAR_T*             target;
   CHAR_T*             pathname;
   CHAR_T*             initial_url;
   CHAR_T*             documentname;
   CHAR_T*             form_data;
   ClickEvent          method;
   CHAR_T*             tempdocument;
   CHAR_T*             s;
   int                 i;
   ThotBool	       history;
   ThotBool            ok;
   ThotBool            stopped_flag = FALSE;
   ThotBool            local_link;
   GETHTMLDocument_context *ctx;
   TTcbf              *cbf;
   void               *ctx_cbf;

   /* restore GETHTMLDocument's context */  
   ctx = (GETHTMLDocument_context *) context;

   if (!ctx)
     return;

   baseDoc = ctx->baseDoc;
   doc = ctx->doc;
   ok = ctx->ok;
   history = ctx->history;
   target = ctx->target;
   documentname = ctx->documentname;
   initial_url = ctx->initial_url;
   form_data = ctx->form_data;
   method = ctx->method;
   tempdocument = ctx->tempdocument;
   cbf = ctx->cbf;
   ctx_cbf = ctx->ctx_cbf;
   local_link = ctx->local_link;
   
   pathname = TtaAllocString (MAX_LENGTH + 1);
   ustrncpy (pathname, urlName, MAX_LENGTH);
   pathname[MAX_LENGTH] = WC_EOS;
   tempfile = TtaAllocString (MAX_LENGTH + 1);
   if (outputfile != NULL)
     {
       ustrncpy (tempfile, outputfile, MAX_LENGTH);
       tempfile[MAX_LENGTH] = EOS;
     }
   else
     tempfile[0] = EOS;
   
   if (ok && !local_link)
     {
       /* memorize the initial newdoc value in doc because LoadHTMLDocument */
       /* will open a new document if newdoc is a modified document */
       if (status == 0)
	 {
	   if (IsW3Path (pathname))
	     NormalizeURL (pathname, 0, tempdocument, documentname, NULL);
	   
	   /* parse and display the document */
	   res = LoadHTMLDocument (newdoc, pathname, form_data, 
				   initial_url, method,
				   tempfile, documentname,
				   http_headers, history);
	   W3Loading = 0;		/* loading is complete now */
	   if (res == 0)
	     {
	       /* cannot load the document */
	       ResetStop(newdoc);
	       newdoc = 0;
	       ok = FALSE;
	     }
	   else if (newdoc != res)
	     newdoc = res;
	   
	   if (ok)
	     /* fetch and display all images referred by the document */
	     stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE);
	 }
       else
	 {
	   if (DocumentURLs[newdoc] == NULL)
	     {
	       /* save the document name into the document table */
	       s = TtaWCSdup (pathname);
	       DocumentURLs[newdoc] = s;
	       TtaSetTextZone (newdoc, 1, 1, s);
	       /* save the document's formdata into the document table */
	       if (DocumentMeta[newdoc])
		   DocumentMetaClear (DocumentMeta[(int) newdoc]);
	       else
		 DocumentMeta[newdoc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
	       DocumentMeta[newdoc]->form_data = TtaWCSdup (form_data);
	       if (ustrcmp (pathname, initial_url))
		 DocumentMeta[newdoc]->initial_url = TtaWCSdup (initial_url);
	       else
		 DocumentMeta[newdoc]->initial_url = NULL;
	       DocumentMeta[newdoc]->method = method;
	       DocumentMeta[newdoc]->put_default_name = FALSE;
	       DocumentMeta[newdoc]->xmlformat = FALSE;
	       DocumentSource[newdoc] = 0;
	       ResetStop(newdoc);
	     }
	   W3Loading = 0;	/* loading is complete now */
	 }
 
       if (ok && !stopped_flag)
	 ResetStop(newdoc);
     }

#ifdef ANNOTATIONS
   /* if it's an annotation, add the existing metadata */
   if (DocumentTypes[newdoc] == docAnnot)
     ANNOT_LoadAnnotation (baseDoc, newdoc);
#endif /* ANNOTATIONS */
   /* select the target if present */
   if (ok && !stopped_flag && target != NULL && target[0] != EOS &&
       newdoc != 0)
     {
       /* attribute HREF contains the NAME of a target anchor */
       elFound = SearchNAMEattribute (newdoc, target, NULL);
       if (elFound != NULL)
	 {
	   /* show the target element in all views */
	   for (i = 1; i < 4; i++)
	     if (TtaIsViewOpened (newdoc, i))
	       TtaShowElement (newdoc, i, elFound, 0);
	 }
     }

   /*** if stopped_flag == true, how to deal with cbf? */
   if (cbf)
     (*cbf) (newdoc, status, pathname, tempfile, NULL, ctx_cbf);

   TtaFreeMemory (target);
   TtaFreeMemory (documentname);
   TtaFreeMemory (initial_url);
   TtaFreeMemory (pathname);
   TtaFreeMemory (tempfile);
   TtaFreeMemory (tempdocument);
   if (form_data)
     TtaFreeMemory (form_data);
   TtaFreeMemory (ctx);
}


/*----------------------------------------------------------------------
  GetHTMLDocument loads the document if it is not loaded yet and    
  calls the parser if the document can be parsed.
    - documentPath: can be relative or absolute address.
    - form_data: the text to be posted.
    - doc: the document which can be removed if not updated.
    - baseDoc: the document which documentPath is relative to.
    - CE_event: CE_FORM_POST for a post request, CE_RELATIVE for a double 
      click.
    - history: record the URL in the browsing history
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            GetHTMLDocument (const CHAR_T* documentPath, CHAR_T* form_data, Document doc, Document baseDoc, ClickEvent CE_event, ThotBool history, TTcbf *cbf, void *ctx_cbf)
#else
Document            GetHTMLDocument (documentPath, const CHAR_T* form_data, doc, baseDoc, CE_event, history, void *cbf, void *ctx_cbf)
CHAR_T*              documentPath;
CHAR_T*              form_data;
Document            doc;
Document            baseDoc;
ClickEvent          CE_event;
ThotBool            history;
TTcbf              *cbf;
void               *ctx_cbf;
#endif
{
   Document            newdoc;
   CSSInfoPtr          css;
   CHAR_T*             tempfile;
   CHAR_T*             tempdocument;
   CHAR_T*             parameters;
   CHAR_T*             target;
   CHAR_T*             pathname;
   CHAR_T*             documentname;
   CHAR_T*             content_type = NULL;
   int                 toparse;
#if 0
   int                 slash;
#endif
   int                 mode;
   int                 docType;
   ThotBool            ok;
   GETHTMLDocument_context *ctx = NULL;

   /* Extract parameters if necessary */
   if (ustrlen (documentPath) > MAX_LENGTH - 1) 
     {
       TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_TOO_LONG_URL),
		     TEXT("512"));
       return (0);
     }
   else
     /* clean up the status line */
     TtaSetStatus (baseDoc, 1, TEXT(" "), NULL);
 
   ok = TRUE;
   tempdocument = TtaAllocString (MAX_LENGTH);
   target       = TtaAllocString (MAX_LENGTH);
   documentname = TtaAllocString (MAX_LENGTH);
   parameters   = TtaAllocString (MAX_LENGTH);
   tempfile     = TtaAllocString (MAX_LENGTH);
   tempfile[0]  = WC_EOS;
   pathname     = TtaAllocString (MAX_LENGTH);

   ustrcpy (tempdocument, documentPath);
   ExtractParameters (tempdocument, parameters);
   /* Extract the target if necessary */
   ExtractTarget (tempdocument, target);
   /* Add the  base content if necessary */
   if (CE_event == CE_RELATIVE || CE_event == CE_FORM_GET
       || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
     NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
   else
     NormalizeURL (tempdocument, 0, pathname, documentname, NULL);

   /* if it's a file: url, we remove the protocol, as it
      is a local file */

#ifdef _WINDOWS
   usprintf (wTitle, TEXT("%s"), documentname);
#endif /* _WINDOWS */

   /* check the document suffix */
   if (IsMathMLName (documentname))
     docType = docMath;
   else if (IsSVGName (documentname))
     docType = docSVG;
   else if (IsCSSName (documentname))
     docType = docCSS;
   else
     docType = docHTML;
    
   /* we skip the file: prefix if it is present and do other local
    file urls conversions */
   if (!IsW3Path (pathname))
       {
	 /* we take the long way around to get the result
	    of normalizeFile, as the function doesn't allocate
	    memory dynamically (note: this can generate some MAX_LENGTH
	    problems) */
	 if (CE_event == CE_RELATIVE || CE_event == CE_FORM_GET ||
	     CE_event == CE_ANNOT || CE_event == CE_FORM_POST ||
	     CE_event == CE_MAKEBOOK)
	   /* we're following a link, so do all the convertions on
	      the URL */
	   NormalizeFile (pathname, tempfile, AM_CONV_ALL);
	 else
	   NormalizeFile (pathname, tempfile, AM_CONV_NONE);
	 ustrcpy (pathname, tempfile);
	 tempfile[0] = WC_EOS;
       }

   /* check if the user is already browsing the document in another window */
   if (CE_event == CE_FORM_GET || CE_event == CE_FORM_POST)
     {
       newdoc = IsDocumentLoaded (pathname, form_data);
       /* we don't concatenate the new parameters as we give preference
	  to the form data */
     }
   else
     {
       /* concatenate the parameters before making the test */
       if (parameters[0] != WC_EOS)
	 {
	   ustrcat (pathname, TEXT("?"));
	   ustrcat (pathname, parameters);
	 }
       newdoc = IsDocumentLoaded (pathname, NULL);
     }

   if (newdoc != 0)
     /* the document is already loaded */
     {

       if (newdoc == doc)
	 /* it's a move in the same document */
	 {
	   if (history)
	     /* record the current position in the history */
	     AddDocHistory (newdoc, DocumentURLs[newdoc], 
			    DocumentMeta[doc]->initial_url,
			    DocumentMeta[newdoc]->form_data,
			    DocumentMeta[newdoc]->method);
	 }
       else
	 /* following the link to another open window */
	 {
	   /* raise its window */
	   TtaRaiseView (newdoc, 1);
	   /* don't add it to the doc's historic */
	   history = FALSE;
	 }
     }

   if (ok)
     {
       /* Create the context for the callback */
       ctx = TtaGetMemory (sizeof (GETHTMLDocument_context));
       ctx->doc = doc;
       ctx->baseDoc = baseDoc;
       ctx->ok = ok;
       ctx->history = history;
       ctx->target = target;
       ctx->documentname = documentname;
       ctx->initial_url = TtaWCSdup (pathname);
       if (form_data)
          ctx->form_data = TtaWCSdup (form_data);
       else
           ctx->form_data = NULL;
       ctx->method = CE_event;
       ctx->tempdocument = tempdocument;
       ctx->cbf = cbf;
       ctx->ctx_cbf = ctx_cbf;
       ctx->local_link = 0;
     }

   toparse = 0;
   if (ok && newdoc == 0)
     {
       /* document not loaded yet */
       if ((CE_event == CE_RELATIVE || CE_event == CE_FORM_GET ||
	    CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK ||
	    CE_event == CE_ANNOT) &&
	    !IsW3Path (pathname) && !TtaFileExist (pathname))
	 {
	   /* the target document doesn't exist */
	   TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	   ok = FALSE; /* do not continue */
	 }
       else if (CE_event == CE_LOG)
	   /* need to create a new window for the document */
	     newdoc = InitDocView (doc, documentname, docLog, 0, FALSE);
       else if (CE_event == CE_HELP)
	 {
	   /* need to create a new window for the document */
	   newdoc = InitDocView (doc, documentname, docType, 0, TRUE);
	   if (newdoc)
	     {
	       /* help document has to be in read-only mode */
	       TtcSwitchCommands (newdoc, 1); /* no command open */
	       TtaSetToggleItem (newdoc, 1, Views, TShowTextZone, FALSE);
	       TtaSetMenuOff (newdoc, 1, Help_);
	     }
	 }
#ifdef ANNOTATIONS
       else if (CE_event == CE_ANNOT)
	 {
	   if (newdoc == 0)
	     /* need to create a new window for the document */
	     newdoc = InitDocView (doc, documentname, docAnnot, 0, FALSE);
	   /* we're downloading an annotation, fix the accept_header
	      (thru the content_type variable) to application/rdf */
	   content_type = TEXT("application/rdf");
	 }
#endif /* ANNOTATIONS */
       else if (doc == 0)
	 /* In case of initial document, open the view before loading */
	 newdoc = InitDocView (doc, documentname, docType, 0, FALSE);
       else
	 {
	   newdoc = doc;
	   /* stop current transfer for previous document */
	   if (CE_event != CE_MAKEBOOK)
	     StopTransfer (baseDoc, 1);
	   else
	     /* temporary docs to make a book are not in ReadOnly mode */
	     DocumentTypes[newdoc] = docHTML;
	 }

       if (newdoc == 0)
	 /* cannot display the new document */
	 ok = FALSE;
       if (ok)
	 {
	   /* this document is currently in load */
	   W3Loading = newdoc;
	   ActiveTransfer (newdoc);
	   /* set up the transfer mode */
	   mode = AMAYA_ASYNC | AMAYA_FLUSH_REQUEST;

	   if (CE_event == CE_FORM_POST)
	     mode = mode | AMAYA_FORM_POST | AMAYA_NOCACHE;
	   else if (CE_event == CE_MAKEBOOK)
	     mode = AMAYA_ASYNC;

	   if (IsW3Path (pathname))
	     {
	       /* @@ JK: stop concatenating the / systematically.
		if there are no side effects, I'll remove this bit of code 
	       */
#if 0
	       if (CE_event != CE_FORM_POST
		   && !ustrcmp (documentname, TEXT("noname.html")))
		 {
		   slash = ustrlen (pathname);
		   if (slash && pathname[slash - 1] != TEXT('/'))
		     ustrcat (pathname, TEXT("/"));
		 }
#endif
	       css = SearchCSS (0, pathname);
	       if (css == NULL)
		 toparse =  GetObjectWWW (newdoc, pathname, form_data,
					  tempfile, mode, NULL, NULL,
					  (void *) GetHTMLDocument_callback,
					  (void *) ctx, YES, content_type);
	       else
		 {
		   /* it was already loaded, we need to open it */
		   TtaSetStatus (newdoc, 1,
				 TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
		   /* just take a copy of the local temporary file */
		   ustrcpy (tempfile, css->localName);
		   GetHTMLDocument_callback (newdoc, 0, pathname,
					     tempfile, NULL,
					     (void *) ctx);
		   TtaHandlePendingEvents ();
		 }
	     }
	   else
	     {
	       /* wasn't a document off the web, we need to open it */
	       TtaSetStatus (newdoc, 1,
			     TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
			     NULL);
	       GetHTMLDocument_callback (newdoc, 0, pathname, tempfile,
					 NULL, (void *) ctx);
	       TtaHandlePendingEvents ();
	     }
	 }
     }
   else if (ok && newdoc != 0)
     {
       /* following a local link */
       TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
       ctx->local_link = 1;
       GetHTMLDocument_callback (newdoc, 0, pathname, tempfile, NULL, (void *) ctx);
       TtaHandlePendingEvents ();
     }

   if (ok == FALSE)
     /* if the document isn't loaded off the web (because of an error, or 
	because it was already loaded), we invoke the callback function */
     {
       if (ctx->form_data)
	 TtaFreeMemory (ctx->form_data);
       TtaFreeMemory (ctx->initial_url);
       if (ctx)
	 TtaFreeMemory (ctx);
       if (cbf)
	 (*cbf) (newdoc, -1, pathname, tempfile, NULL, ctx_cbf);
       /* Free the memory associated with the context */
       TtaFreeMemory (target);
       TtaFreeMemory (documentname);
       TtaFreeMemory (tempdocument);
     }
   TtaFreeMemory (parameters);
   TtaFreeMemory (tempfile);
   TtaFreeMemory (pathname);
   return (newdoc);

   /*TtaHandlePendingEvents ();*/
}

/*----------------------------------------------------------------------
   UpdateSaveAsButtons
   Maintain consistency between buttons in the Save As dialog box
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	UpdateSaveAsButtons ()
#else
static void	UpdateSaveAsButtons ()
#endif
{
#ifndef _WINDOWS
  int	active;

  if (SaveAsHTML)
    {
      TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsXML);
      TtaSetToggleMenu (BaseDialog + ToggleSave, 2, SaveAsText);
      active = 1;
    }
  else if (SaveAsXML)
    {
      TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
      TtaSetToggleMenu (BaseDialog + ToggleSave, 2, SaveAsText);
      active = 1;
    }
  else
    active = 0;
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 4, NULL, -1, active);
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 5, NULL, -1, active);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   SetFileSuffix
   Set the suffix of the file name used for saving a document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	SetFileSuffix ()
#else
static void	SetFileSuffix ()

#endif
{
  CHAR_T	       suffix[6];
  CHAR_T*          filename;
  int		       i, len;

  if (SavingDocument != 0 && SaveName[0] != WC_EOS)
    {
     if (SaveAsHTML)
        ustrcpy (suffix, TEXT("html"));
     else if (SaveAsXML)
        ustrcpy (suffix, TEXT("xml"));
     else if (SaveAsText)
        ustrcpy (suffix, TEXT("txt"));
     else
        return;

     /* looks for a suffix at the end of the document name */
     len = ustrlen (SaveName);
     for (i = len-1; i > 0 && SaveName[i] != TEXT('.'); i--);
     if (SaveName[i] != TEXT('.'))
       {
         /* there is no suffix */
         i = len;
         SaveName[i++] = TEXT('.');
       }
     else
       {
	 /* there is a suffix */
	 i++;
	 if (ustrncmp (suffix, &SaveName[i], 3) == 0)
	   /* the requested suffix is already here. Do nothing */
	   i = 0;
#ifdef _WINDOWS
	 ustrcpy (DocToOpen, SavePath );
	 if (ustrchr (SavePath, TEXT('/')))
	   ustrcat (DocToOpen, WC_URL_STR);
	 else
	   ustrcat (DocToOpen, WC_DIR_STR);
	 ustrcat (DocToOpen, SaveName);
#endif /* _WINDOWS */
       }
     
     if (i > 0)
       {
	 /* change or append the suffix */
	 ustrcpy (&SaveName[i], suffix);
	 /* display the new filename in the dialog box */
	 filename = TtaAllocString (MAX_LENGTH);
	 ustrcpy (filename, SavePath );
	 if (ustrchr (SavePath, TEXT('/')))
           ustrcat (filename, WC_URL_STR);
	 else
	   ustrcat (filename, WC_DIR_STR);
	 ustrcat (filename, SaveName);
#ifdef _WINDOWS
	 usprintf (DocToOpen, filename);
#else /* _WINDOWS */
	 TtaSetTextForm (BaseDialog + NameSave, filename);
#endif /* _WINDOWS */
	 TtaFreeMemory (filename);
       }
    }
}

/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackDialogue (int ref, int typedata, CHAR_T* data)
#else
void                CallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
CHAR_T*             data;
#endif
{
  CHAR_T*           tempfile;
  CHAR_T*           tempname;
  CHAR_T            sep;
  int               val, i;
  ThotBool          change;

  if (typedata == STRING_DATA && data && ustrchr (data, TEXT('/')))
    sep = WC_URL_SEP;
  else
    sep = WC_DIR_SEP;

   val = (int) data;

   if (ref - BaseDialog >= OptionMenu &&
       ref - BaseDialog <= OptionMenu + MAX_SUBMENUS)
     /* a popup menu corresponding to a SELECT element or a submenu
        corresponding to an OPTGROUP element*/
     {
       ReturnOption = val;
       ReturnOptionMenu = ref - BaseDialog - OptionMenu;
       TtaDestroyDialogue (BaseDialog + OptionMenu);
     }
   else switch (ref - BaseDialog)
     {
     case OpenForm:
       /* *********Load URL or local document********* */
       if (val == 1)
	 /* Confirm */
	 {
	   TtaDestroyDialogue (BaseDialog + OpenForm);
	   TtaDestroyDialogue (BaseDialog + FileBrowserForm);
	   if (LastURLName[0] != WC_EOS)
	     {
	       if (NewFile)
		 InitializeNewDoc (LastURLName, NewDocType);
	       /* load an URL */
		   else if (InNewWindow)
		     GetHTMLDocument (LastURLName, NULL, 0, 0, Loading_method,
				      FALSE, NULL, NULL);
	       else
		 GetHTMLDocument (LastURLName, NULL, CurrentDocument,
				  CurrentDocument, Loading_method, TRUE,
				  NULL, NULL);
	     }
	   else if (DirectoryName[0] != WC_EOS &&
		    DocumentName[0] != WC_EOS)
	     {
	       /* load a local file */
	       tempfile = TtaAllocString (MAX_LENGTH);
	       memset (tempfile, WC_EOS, MAX_LENGTH);
	       ustrcpy (tempfile, DirectoryName);
	       ustrcat (tempfile, WC_DIR_STR);
	       ustrcat (tempfile, DocumentName);
	       if (FileExistTarget (tempfile))
		 {
		   if (InNewWindow)
		     GetHTMLDocument (tempfile, NULL, 0, 0, Loading_method,
				      FALSE, NULL, NULL);
		   else
		     GetHTMLDocument (tempfile, NULL, CurrentDocument,
				      CurrentDocument, Loading_method,
				      TRUE, NULL, NULL);
		 }
	       else if (NewFile)
		 InitializeNewDoc (tempfile, NewDocType);
	       else
		 TtaSetStatus (CurrentDocument, 1,
			       TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			       tempfile);
	       TtaFreeMemory (tempfile);
	     }
	   else
	     {
		   if (DocumentName[0] != EOS)
		     TtaSetStatus (CurrentDocument, 1,
				   TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
				   DocumentName);
		   else if (DirectoryName[0] != EOS)
		     TtaSetStatus (CurrentDocument, 1,
				   TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
				   DirectoryName);
		   else
		     TtaSetStatus (CurrentDocument, 1,
				   TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
				   TEXT(""));
	     }
	   NewFile = FALSE;
	   CurrentDocument = 0;
	 }
       else if (val == 2)
	 {
	   /* Browse button */
#ifndef _WINDOWS
	   WidgetParent = OpenDocBrowser;
	   BrowserForm (CurrentDocument, 1, &LastURLName[0]);
#endif /* !_WINDOWS */
	 }
       else if (NewFile)
	 {
	   /* the command is aborted */
	   CheckAmayaClosed ();
	   NewFile = FALSE;
	 }
       break;

     case URLName:
       RemoveNewLines (data);
       if (IsW3Path (data))
	 {
	   /* save the URL name */
	   ustrcpy (LastURLName, data);
	   DocumentName[0] = WC_EOS;
	 }
       else
	 {
	   LastURLName[0] = WC_EOS;
	   tempfile = TtaAllocString (MAX_LENGTH);
	   change = NormalizeFile (data, tempfile, AM_CONV_NONE);
	   
	   if (TtaCheckDirectory (tempfile))
	     {
	       ustrcpy (DirectoryName, tempfile);
	       DocumentName[0] = WC_EOS;
	     }
	   else
	     TtaExtractName (tempfile, DirectoryName, DocumentName);
	   TtaFreeMemory (tempfile);
	 }       
       break;

     case DirSelect:
#ifdef _WINDOWS
       usprintf (DirectoryName, TEXT("%s"), data);
#else  /* _WINDOWS */
       if (DirectoryName[0] != EOS)
	 {
	   if (!ustrcmp (data, ".."))
	     {
	       /* suppress last directory */
	       tempname = TtaAllocString (MAX_LENGTH);
	       tempfile = TtaAllocString (MAX_LENGTH);
	       ustrcpy (tempname, DirectoryName);
	       TtaExtractName (tempname, DirectoryName, tempfile);
	       TtaFreeMemory (tempfile);
	       TtaFreeMemory (tempname);
	     }
	   else
	     {
	       ustrcat (DirectoryName, DIR_STR);
	       ustrcat (DirectoryName, data);
	     }
	   TtaSetTextForm (BaseDialog + URLName, DirectoryName);
	   TtaListDirectory (DirectoryName, BaseDialog + OpenForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR),
			     BaseDialog + DirSelect, ScanFilter,
			     TtaGetMessage (AMAYA, AM_FILES),
			     BaseDialog + DocSelect);
	   DocumentName[0] = EOS;
	 }
#endif /* _WINDOWS */
       break;
     case DocSelect:
       if (DirectoryName[0] == EOS)
	 /* set path on current directory */
	 ugetcwd (DirectoryName, MAX_LENGTH);
       
       /* Extract suffix from document name */
       ustrcpy (DocumentName, data);
       LastURLName[0] = WC_EOS;
       /* construct the document full name */
       tempfile = TtaAllocString (MAX_LENGTH);
       ustrcpy (tempfile, DirectoryName);
       ustrcat (tempfile, WC_DIR_STR);
       ustrcat (tempfile, DocumentName);
#ifndef _WINDOWS
       TtaSetTextForm (BaseDialog + URLName, tempfile);
#endif /* !_WINDOWS */
       TtaFreeMemory (tempfile);
       break;
     case ConfirmForm:
       /* *********Confirm********* */
       UserAnswer = (val == 1);
       TtaDestroyDialogue (BaseDialog + ConfirmForm);
       break;
     case FilterText:
       /* Filter value */
       if (ustrlen(data) <= NAME_LENGTH)
	 ustrcpy (ScanFilter, data);
#ifndef _WINDOWS
       else
	 TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
#endif /* !_WINDOWS */
       break;
     case FormAnswer:
       /* *********Get an answer********* */
       if (val == 0)
	 {
	   /* no answer */
	   Answer_text[0] = EOS;
	   Answer_name[0] = WC_EOS;
	   Answer_password[0] = WC_EOS;
	   UserAnswer = 0;
	 }
       else
	 UserAnswer = 1;
       TtaDestroyDialogue (BaseDialog + FormAnswer);
       break;
     case AnswerText:
       ustrncpy (Answer_text, data, MAX_LENGTH);
       Answer_text[MAX_LENGTH - 1] = EOS;
       break;
     case NameText:
       ustrncpy (Answer_name, data, NAME_LENGTH);
       Answer_text[NAME_LENGTH - 1] = EOS;
       break;
     case PasswordText:
       i = ustrlen (data);
       if (i < NAME_LENGTH - 1)
	 {
	   if (Lg_password < i)
	     {
	       /* a new char */
	       Answer_password[Lg_password] = data[Lg_password];
	       Display_password[Lg_password] = TEXT('*');
	       Answer_password[++Lg_password] = WC_EOS;
	       Display_password[Lg_password] = EOS;
	     }
	   else if (Lg_password > i)
	     {		/* a valid backspace */
	       
	       Lg_password--;
	       Answer_password[Lg_password] = WC_EOS;
	       Display_password[Lg_password] = EOS;
	     }
	 }
       else
	 Answer_password[NAME_LENGTH - 1] = WC_EOS;
#ifndef _WINDOWS
       if (i > 0)
	 TtaSetTextForm (BaseDialog + PasswordText, Display_password);
#endif /* !_WINDOWS */
       break;
       
       /* *********Save document as********* */
     case ToggleSave:
       /* Output format */
       switch (val)
	 {
	 case 0:	/* "Save as HTML" button */
	   SaveAsHTML = TRUE;
	   SaveAsXML = FALSE;
	   SaveAsText = FALSE;
	   UpdateSaveAsButtons ();
	   SetFileSuffix ();
	   break;
	 case 1:	/* "Save as XML" button */
	   SaveAsHTML = FALSE;
	   SaveAsXML = TRUE;
	   SaveAsText = FALSE;
	   UpdateSaveAsButtons ();
	   SetFileSuffix ();
	   /* Set the document charset */
	   if (TtaGetDocumentCharset (SavingDocument) == UNDEFINED_CHARSET)
	     TtaSetDocumentCharset (SavingDocument, ISO_8859_1);
	   break;
	 case 2:	/* "Save as Text" button */
	   SaveAsText = TRUE;
	   SaveAsHTML = FALSE;
	   SaveAsXML = FALSE;
#ifndef _WINDOWS
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsXML);
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
	   UpdateSaveAsButtons ();
#endif /* _WINDOWS */
	   SetFileSuffix ();
	   break;
	 case 4:	/* "Copy Images" button */
	   CopyImages = !CopyImages;
	   break;
	 case 5:	/* "Transform URLs" button */
	   UpdateURLs = !UpdateURLs;
	   break; 
	 }
       break;
     case SaveForm:
       if (val == 1)
	 /* "Confirm" Button */
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   if (SavingDocument != 0)
	     DoSaveAs ();
	   else if (SavingObject != 0)
	     DoSaveObjectAs ();

	   /* Move the information into LastURLName or DirectoryName */
	   if (IsW3Path (SavePath))
	     {
	       ustrcpy (LastURLName, SavePath);
	       ustrcat (LastURLName, TEXT("/"));
	       ustrcat (LastURLName, SaveName);
	       DirectoryName[0] = WC_EOS;
	     }
	   else
	     {
	       LastURLName[0] = WC_EOS;
	       ustrcpy (DirectoryName, SavePath);
	       ustrcat (DocumentName, SaveName);
	     }
	 }
       else if (val == 2)
	 /* "Clear" button */
	 {
	   if (SavingDocument != 0)
	     {
	       SavePath[0] = WC_EOS;
	       SaveImgsURL[0] = WC_EOS;
	       SaveName[0] = WC_EOS;
#ifndef _WINDOWS
	       TtaSetTextForm (BaseDialog + NameSave, SaveImgsURL);
	       TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
#endif /* !_WINDOWS */
	     }
	 }
       else if (val == 3)
	 /* "Filter" button */
	 {
	   /* reinitialize directories and document lists */
	   if (SavingDocument != 0)
	     TtaListDirectory (SavePath, BaseDialog + SaveForm,
			       TtaGetMessage (LIB, TMSG_DOC_DIR),
			       BaseDialog + DirSave, ScanFilter,
			       TtaGetMessage (AMAYA, AM_FILES),
			       BaseDialog + DocSave);
	 }
       else
	 /* "Cancel" button */
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   if (SavingObject != 0)
	      /* delete temporary file */
	      DeleteTempObjectFile ();
	   SavingDocument = 0;
	   SavingObject = 0;
	 }
       break;
     case NameSave:
       /* Document location */
       tempfile = TtaAllocString (MAX_LENGTH);
       if (!IsW3Path (data))
	 change = NormalizeFile (data, tempfile, AM_CONV_NONE);
       else
	 ustrcpy (tempfile, data);
       
       if (*tempfile && tempfile[ustrlen (tempfile) - 1] == sep)
	 {
	   ustrcpy (SavePath, tempfile);
	   SaveName[0] = WC_EOS;
	 }
       else
	 {
	   /* Extract document name */
	   if (SavingDocument != 0)
	     TtaExtractName (tempfile, SavePath, SaveName);
	   else
	     TtaExtractName (tempfile, SavePath, ObjectName);
	 }
       TtaFreeMemory (tempfile);
       break;
     case ImgDirSave:
       /* Image directory */
       if (!IsW3Path (data))
	 change = NormalizeFile (data, SaveImgsURL, AM_CONV_NONE);
       else
	 ustrcpy (SaveImgsURL, data);
       break;
     case DirSave:
       if (!IsW3Path (SavePath))
	 {
	   /* Document directories */
	   tempfile = TtaAllocString (MAX_LENGTH);
	   if (!ustrcmp (data, TEXT("..")))
	     {
	       /* suppress last directory */
	       tempname = TtaAllocString (MAX_LENGTH);
	       ustrcpy (tempname, SavePath);
	       TtaExtractName (tempname, SavePath, tempfile);
	       TtaFreeMemory (tempname);
	     }
	   else
	     {
	       ustrcat (SavePath, WC_DIR_STR);
	       ustrcat (SavePath, data);
	     }
	   ustrcpy (tempfile, SavePath);
	   ustrcat (tempfile, WC_DIR_STR);
	   if (SavingDocument != 0)
	     ustrcat (tempfile, DocumentName);
	   else
	     ustrcat (tempfile, ObjectName);
#ifndef _WINDOWS
	   TtaSetTextForm (BaseDialog + NameSave, SavePath);
#endif /* !_WINDOWS */
	   TtaListDirectory (SavePath, BaseDialog + SaveForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR),
			     BaseDialog + DirSave,
			     ScanFilter, TtaGetMessage (AMAYA, AM_FILES),
			     BaseDialog + DocSave);
	   TtaFreeMemory (tempfile);
	 }
       break;
     case DocSave:
       /* Files */
       if (SaveName[0] == EOS)
	 /* set path on current directory */
	 ugetcwd (SavePath, MAX_LENGTH);
       
       ustrcpy (SaveName, data);
       /* construct the document full name */
       tempfile = TtaAllocString (MAX_LENGTH);
       ustrcpy (tempfile, SavePath);
       ustrcat (tempfile, WC_DIR_STR);
       ustrcat (tempfile, SaveName);
#ifndef _WINDOWS
       TtaSetTextForm (BaseDialog + NameSave, tempfile);
#endif /* !_WINDOWS */
       TtaFreeMemory (tempfile);
       break;

     case ConfirmSave:
       /* *********SaveConfirm********* */
       UserAnswer = (val == 1);
       TtaDestroyDialogue (BaseDialog + ConfirmSave);
       if (!UserAnswer)
	 {
	   SavingDocument = 0;
	   SavingObject = 0;
	 }
       break;
       
     case AttrHREFForm:
       /* *********HREF Attribute*********** */
       if (val == 1)
	 {
	   /* Confirm button */
	   /* create an attribute HREF for the Link_Anchor */
	   if (AttrHREFvalue[0] != WC_EOS)
	     SetREFattribute (AttrHREFelement, AttrHREFdocument,
			      AttrHREFvalue, NULL);
	   TtaDestroyDialogue (BaseDialog + AttrHREFForm);
	   TtaDestroyDialogue (BaseDialog + FileBrowserForm);
	 }
       else if (val == 2)
	 /* Browse button */
	 {
#ifndef _WINDOWS
	   WidgetParent = HrefAttrBrowser;
           BrowserForm (AttrHREFdocument, 1, &AttrHREFvalue[0]);
#endif /* !_WINDOWS */
	 }
       else 
	 /* Cancel button */
	 if (IsNewAnchor)
	   {
	     LinkAsCSS = FALSE;
	     /* remove the link if it was just created */
	     TtaCancelLastRegisteredSequence (AttrHREFdocument);	   
	     DeleteAnchor (AttrHREFdocument, 1);
	     TtaCancelLastRegisteredSequence (AttrHREFdocument);	   
	   }
       break;

     case AttrHREFText:
       /* save the HREF name */
       RemoveNewLines (data);
       if (IsW3Path (data))
	 {
	   /* save the URL name */
	     ustrcpy (AttrHREFvalue, data);
	   DocumentName[0] = WC_EOS;
	 }
       else
	 {
	   tempfile = TtaAllocString (MAX_LENGTH);
	   change = NormalizeFile (data, tempfile, AM_CONV_NONE);
	   if (TtaCheckDirectory (tempfile))
	     {
	       ustrcpy (DirectoryName, tempfile);
	       DocumentName[0] = WC_EOS;
	     }
	   else
	     TtaExtractName (tempfile, DirectoryName, DocumentName);
	   ustrcpy (AttrHREFvalue, tempfile);
	   TtaFreeMemory (tempfile);
	 }       
       break;

       /* *********File Browser*********** */
     case FileBrowserForm:
       if (val == 1)
	 {
	   /* Confirm button */
#ifndef _WINDOWS
	   /* this code is only valid under Unix. */
	   /* In Windows, we're using a system widget */
	   tempfile = TtaAllocString (MAX_LENGTH);
	   memset (tempfile, WC_EOS, MAX_LENGTH);
	   ustrcpy (tempfile, DirectoryName);
	   ustrcat (tempfile, WC_DIR_STR);
	   ustrcat (tempfile, DocumentName);
	   if (WidgetParent == HrefAttrBrowser)
	     {
	       TtaSetTextForm (BaseDialog + AttrHREFText, tempfile);
	       ustrcpy (AttrHREFvalue, tempfile);
	       TtaFreeMemory (tempfile);
	       CallbackDialogue (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*) 1);
	     }
	   else if (WidgetParent == OpenDocBrowser)
	     {
	       TtaSetTextForm (BaseDialog + URLName, tempfile);
	       TtaFreeMemory (tempfile);
	       CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
	     }
#endif /* !_WINDOWS */
	 }
       else if (val == 2)
	 /* Clear button */
	 {
	   if (WidgetParent == OpenDocBrowser)
	     {
	       LastURLName[0] = WC_EOS;
#ifndef _WINDOWS
	       TtaSetTextForm (BaseDialog + FileBrowserText, LastURLName);
#endif /* !_WINDOWS */
	     }
	   else if (WidgetParent == HrefAttrBrowser)
	     {
	       tempname = TtaAllocString (MAX_LENGTH);
	       tempname[0] = WC_EOS; 	       
#ifndef _WINDOWS
	     TtaSetTextForm (BaseDialog + FileBrowserText, tempname);
#endif /* !_WINDOWS */
	       TtaFreeMemory (tempname);
	     }
	 }
       else if (val == 3)
	 /* Filter button */
	 {
	   /* reinitialize directories and document lists */
	   TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR),
			     BaseDialog + BrowserDirSelect,
			     ScanFilter, TtaGetMessage (AMAYA, AM_FILES),
			     BaseDialog + BrowserDocSelect);
	 }
       else 
	 /* Cancel button */
	 {
	 }
       break;

     case FileBrowserText:
       RemoveNewLines (data);
       if (IsW3Path (data))
	 {
	   DocumentName[0] = WC_EOS;
	 }
       else
	 {
	   tempfile = TtaAllocString (MAX_LENGTH);
	   change = NormalizeFile (data, tempfile, AM_CONV_NONE);
	   if (TtaCheckDirectory (tempfile))
	     {
	       ustrcpy (DirectoryName, tempfile);
	       DocumentName[0] = WC_EOS;
	     }
	   else
	     TtaExtractName (tempfile, DirectoryName, DocumentName);
	   ustrcpy (AttrHREFvalue, tempfile);
	   TtaFreeMemory (tempfile);
	 }       
       break;

       /* *********Browser DirSelect*********** */
     case BrowserDirSelect:
#ifdef _WINDOWS
       usprintf (DirectoryName, TEXT("%s"), data);
#else  /* _WINDOWS */
       if (DirectoryName[0] != EOS)
	 {
	   if (!ustrcmp (data, ".."))
	     {
	       /* suppress last directory */
	       tempname = TtaAllocString (MAX_LENGTH);
	       tempfile = TtaAllocString (MAX_LENGTH);
	       ustrcpy (tempname, DirectoryName);
	       TtaExtractName (tempname, DirectoryName, tempfile);
	       TtaFreeMemory (tempfile);
	       TtaFreeMemory (tempname);
	     }
	   else
	     {
	       ustrcat (DirectoryName, DIR_STR);
	       ustrcat (DirectoryName, data);
	     }
	   TtaSetTextForm (BaseDialog + FileBrowserText, DirectoryName);
	   TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR),
			     BaseDialog + BrowserDirSelect, ScanFilter,
			     TtaGetMessage (AMAYA, AM_FILES),
			     BaseDialog + BrowserDocSelect);
	   DocumentName[0] = EOS;
	 }
#endif /* _WINDOWS */
       break;

       /* *********Browser DocSelect*********** */
     case BrowserDocSelect:
       if (DirectoryName[0] == EOS)
	 /* set path on current directory */
	 ugetcwd (DirectoryName, MAX_LENGTH);

       /* Extract suffix from document name */
       ustrcpy (DocumentName, data);
       if (WidgetParent == OpenDocBrowser)
	 LastURLName[0] = WC_EOS;

       /* construct the document full name */
       tempfile = TtaAllocString (MAX_LENGTH);
       ustrcpy (tempfile, DirectoryName);
       ustrcat (tempfile, WC_DIR_STR);
       ustrcat (tempfile, DocumentName);
#ifndef _WINDOWS
       TtaSetTextForm (BaseDialog + FileBrowserText, tempfile);
#endif /* !_WINDOWS */
       TtaFreeMemory (tempfile);
       break;

       /* *********Browser Filter*********** */
     case FileBrowserFilter:
       /* Filter value */
       if (ustrlen(data) <= NAME_LENGTH)
	 ustrcpy (ScanFilter, data);
#ifndef _WINDOWS
       else
	 TtaSetTextForm (BaseDialog + BrowserFilterText, ScanFilter);
#endif /* !_WINDOWS */
       break;
       
     case TitleForm:
       /* ********Change the document title*********** */
       TtaDestroyDialogue (BaseDialog + TitleForm);
       if (val == 1)
	 SetNewTitle (CurrentDocument);
       break;

     case TitleText:
       /* new document name */
       ustrncpy (Answer_text, data, MAX_LENGTH);
       Answer_text[MAX_LENGTH - 1] = EOS;
       break;

     case ClassForm:
     case ClassSelect:
     case AClassForm:
     case AClassSelect:
       StyleCallbackDialogue (ref, typedata, data);
       break;

     case TableForm:
       /* *********Table Form*********** */
       UserAnswer = (val == 1);
       TtaDestroyDialogue (BaseDialog + TableForm);
       break;
     case TableRows:
       NumberRows = val;
       break;
     case TableCols:
       NumberCols = val;
       break;
     case TableBorder:
       TBorder = val;
       break;
     case MathEntityForm:
       /* ********* MathML entity form ********* */
       if (val == 0)
	   /* no answer */
	   MathMLEntityName[0] = EOS;
       break;
     case MathEntityText:
       ustrncpy (MathMLEntityName, data, MAX_LENGTH);
       MathMLEntityName[MAX_LENGTH - 1] = EOS;
       break;
     case MakeIdMenu:
       switch (val)
	 {
	 case 1:
	   CreateRemoveIDAttribute (IdElemName, IdDoc, TRUE, 
				    (IdApplyToSelection) ? TRUE: FALSE);
#ifndef _WINDOWS
	   /* and show the status */
	   TtaNewLabel (BaseDialog + mIdStatus,
			BaseDialog + MakeIdMenu,
			IdStatus);
#endif /* _WINDOWS */
	   break;
	 case 2:
	   CreateRemoveIDAttribute (IdElemName, IdDoc, FALSE, 
				    (IdApplyToSelection) ? TRUE: FALSE);
#ifndef _WINDOWS
	   /* and show the status */
	   TtaNewLabel (BaseDialog + mIdStatus,
			BaseDialog + MakeIdMenu,
			IdStatus);
#endif /* _WINDOWS */
	   break;
	 }
       break;
     case mElemName:
       ustrncpy (IdElemName, data, MAX_LENGTH);
       IdElemName[MAX_LENGTH - 1] = EOS;
       break;
     case mIdUseSelection:
       IdApplyToSelection = val;
       break;
     }
}

/*----------------------------------------------------------------------
  RestoreOneAmayaDoc restores a saved file
  doc is the suggested doc to be loaded or 0.
  docname is the original name of the document.
  tempdoc is the name of the saved file.
  Return the new recovered document
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int       RestoreOneAmayaDoc (Document doc, CHAR_T* tempdoc, CHAR_T* docname, DocumentType docType)
#else
static int       RestoreOneAmayaDoc (doc, tempdoc, docname, docType)
Document         doc;
CHAR_T*          tempdoc;
CHAR_T*          docname;
DocumentType     docType;
#endif

{
  AHTHeaders          http_headers;
  CHAR_T              content_type[MAX_LENGTH];
  CHAR_T              tempfile[MAX_LENGTH];
  int                 newdoc, len;
  ThotBool            stopped_flag;

  W3Loading = doc;
  BackupDocument = doc;
  TtaExtractName (tempdoc, DirectoryName, DocumentName);
  newdoc = InitDocView (doc, DocumentName, docType, 0, FALSE);
  if (newdoc != 0)
    {
      /* load the saved file */
      W3Loading = newdoc;
      if (IsW3Path (docname))
	{
	  /* it's a remote file */
	  if (docType == docHTML)
	    {
	      ustrcpy (content_type, TEXT("text/html"));
	      http_headers.content_type = content_type;
	    }
	  else
	      http_headers.content_type = NULL;
	  /* we don't know yet how to recover the charset */
	  http_headers.charset = NULL;
	  LoadHTMLDocument (newdoc, docname, NULL, NULL, CE_ABSOLUTE, 
			    tempdoc, DocumentName, &http_headers, FALSE);
	}
      else
	{
	  /* it's a local file */
	  tempfile[0] = EOS;
	  /* load the temporary file */
	  LoadHTMLDocument (newdoc, tempdoc, NULL, NULL, CE_ABSOLUTE,
			    tempfile, DocumentName, NULL, FALSE);
	  /* change its URL */
	  TtaFreeMemory (DocumentURLs[newdoc]);
	  len = ustrlen (docname) + 1;
	  DocumentURLs[newdoc] = TtaAllocString (len);
	  ustrcpy (DocumentURLs[newdoc], docname);
	  DocumentSource[newdoc] = 0;
	  TtaSetTextZone (newdoc, 1, 1, docname);
	  /* change its directory name */
	  TtaSetDocumentDirectory (newdoc, DirectoryName);
	}
      TtaSetDocumentModified (newdoc);
      W3Loading = 0;		/* loading is complete now */
      DocNetworkStatus[newdoc] = AMAYA_NET_ACTIVE;
      stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE);
      if (!stopped_flag)
	{
	  DocNetworkStatus[newdoc] = AMAYA_NET_INACTIVE;
	  /* almost one file is restored */
	  TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
			NULL);
	}
      /* unlink this saved file */
      TtaFileUnlink (tempdoc);
    }
  BackupDocument = 0;
  return (newdoc);
}


/*----------------------------------------------------------------------
  RestoreAmayaDocs checks if Amaya has previously crashed.
  The file Crash.amaya gives the list of saved files
  ----------------------------------------------------------------------*/
static ThotBool       RestoreAmayaDocs ()
{
  FILE              * f;
  int                 docType;
  CHAR_T              tempname[MAX_LENGTH], tempdoc[MAX_LENGTH];
  CHAR_T              docname[MAX_LENGTH];  
  char                tempdocA[MAX_LENGTH];
  char                docnameA[MAX_LENGTH];  
  ThotBool            aDoc;

  /* check if Amaya has crashed */
  usprintf (tempname, TEXT("%s%cCrash.amaya"), TempFileDirectory, WC_DIR_SEP);
  /* no document is opened */
  aDoc = FALSE;
  if (TtaFileExist (tempname))
    {
      InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_RELOAD_FILES));
      if (UserAnswer)
        f = ufopen (tempname, TEXT("r"));
      else
	f = NULL;

      if (f != NULL)
	{
	  InNewWindow = TRUE;
	  tempdoc[0] = WC_EOS;
	  fscanf (f, "%s %s %d\n", tempdocA, docnameA, &docType);
	  iso2wc_strcpy (tempdoc, tempdocA);
	  iso2wc_strcpy (docname, docnameA);
	  while (tempdoc[0] != WC_EOS && TtaFileExist (tempdoc))
	    {
	      if (UserAnswer)
		{
                  if (RestoreOneAmayaDoc (0, tempdoc, docname, (DocumentType) docType))
		    aDoc = TRUE;
		}
	      else
		/* unlink this saved file */
		TtaFileUnlink (tempdoc);
               /*next saved file */
               tempdoc[0] = WC_EOS;
               tempdocA[0] = EOS;
               fscanf (f, "%s %s %d\n", tempdocA, docnameA, &docType);
               iso2wc_strcpy (tempdoc, tempdocA);
               iso2wc_strcpy (docname, docnameA);
	    }
	  InNewWindow = FALSE;	  
	  fclose (f);
	}
      TtaFileUnlink (tempname);
    }
  return (aDoc);
}


/*----------------------------------------------------------------------
  CheckMakeDirectory verifies if a directory name exists. If it doesn't
  exist, it tries to create it. If recusive == TRUE, it tries to create
  all the intermediary directories.
  Returns TRUE if the operation succeeds, FALSE otherwise.
  ----------------------------------------------------------------------*/
ThotBool CheckMakeDirectory (CHAR_T* name, ThotBool recursive)
{
  ThotBool  i;
  CHAR_T* tmp_name;
  CHAR_T* ptr;
  CHAR_T  tmp_char;

  /* protection against bad calls */
  if (!name || *name == WC_EOS)
    return FALSE;

  /* does the directory exist? */
  if (TtaMakeDirectory (name))
    return TRUE;

  /* no, try to create it then */
  i = TtaMakeDirectory (name);

  /* were be able to create the directory (or found it already?) */
  if (!i)
    {
      /* don't do anything else */
      if (!recursive)
	return FALSE;
      
      /* try to create all the missing directories up to name */
      tmp_name = TtaWCSdup (name);
      ptr = tmp_name;
      /* create all the intermediary directories */
      while (*ptr != WC_EOS)
	{
	  if (*ptr != WC_DIR_SEP)
	    ptr++;
	  else
	    {
	      tmp_char = *ptr;
	      *ptr = WC_EOS;
	      i = TtaMakeDirectory (tmp_name);
	      if (!i)
		{
		  TtaFreeMemory (tmp_name);
		  return FALSE;
		}
	      *ptr = tmp_char;
	      ptr++;
	    }
	}
      /* create the last dir */
      i = TtaMakeDirectory (tmp_name);
      TtaFreeMemory (tmp_name);
      if (!i)
	return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
   FreeAmayaStructures cleans up memory ressources.
  ----------------------------------------------------------------------*/
void                FreeAmayaStructures ()
{
  if (LastURLName)
    {
      /* now exit the application */
      TtaFreeMemory (LastURLName);
      LastURLName = NULL;
      TtaFreeMemory (DocumentName);
      TtaFreeMemory (DirectoryName);
      TtaFreeMemory (SavePath);
      TtaFreeMemory (SaveName);
      TtaFreeMemory (ObjectName);
      TtaFreeMemory (SaveImgsURL);
      TtaFreeMemory (SavingFile);
      TtaFreeMemory (AttrHREFvalue);
      TtaFreeMemory (UserCSS);
      FreeHTMLParser ();
#ifdef OLD_XML_PARSER
      FreeXMLParser ();
#else /* OLD_XML_PARSER */
      FreeXmlParserContexts ();
#endif /* OLD_XML_PARSER */
      FreeDocHistory ();
      FreeTransform ();
      QueryClose ();
#ifdef ANNOTATIONS
      ANNOT_Quit ();
#endif /* ANNOTATIONS */
    }
}


/*----------------------------------------------------------------------
  InitAmaya intializes Amaya variables and opent the first document
  window.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitAmaya (NotifyEvent * event)
#else
void                InitAmaya (event)
NotifyEvent        *event;
#endif
{
   CHAR_T             *s;
   CHAR_T             *tempname;
   int                 i;
   ThotBool            restoredDoc;

   if (AmayaInitialized)
      return;
   AmayaInitialized = 1;
   W3Loading = 0;
   BackupDocument = 0;
   /* initialize status */
   SelectionDoc = 0;
   SelectionInPRE = FALSE;
   SelectionInComment = FALSE;
   SelectionInEM = FALSE;
   SelectionInSTRONG = FALSE;
   SelectionInCITE = FALSE;
   SelectionInABBR = FALSE;
   SelectionInACRONYM = FALSE;
   SelectionInINS = FALSE;
   SelectionInDEL = FALSE;
   SelectionInDFN = FALSE;
   SelectionInCODE = FALSE;
   SelectionInVAR = FALSE;
   SelectionInSAMP = FALSE;
   SelectionInKBD = FALSE;
   SelectionInI = FALSE;
   SelectionInB = FALSE;
   SelectionInTT = FALSE;
   SelectionInBIG = FALSE;
   SelectionInSMALL = FALSE;
   HTMLErrorsFound = FALSE;
   XMLErrorsFound = FALSE;
   LinkAsCSS = FALSE; /* we're not linking an external CSS */

   /* initialize icons */
#ifndef _WINDOWS
   stopR = TtaCreatePixmapLogo (stopR_xpm);
   stopN = TtaCreatePixmapLogo (stopN_xpm);
   iconSave = TtaCreatePixmapLogo (save_xpm);
   iconSaveNo = TtaCreatePixmapLogo (saveNo_xpm);
   iconFind = TtaCreatePixmapLogo (find_xpm);
   iconReload = TtaCreatePixmapLogo (Reload_xpm);
   iconHome = TtaCreatePixmapLogo (home_xpm);
   iconI = TtaCreatePixmapLogo (I_xpm);
   iconINo = TtaCreatePixmapLogo (INo_xpm);
   iconB = TtaCreatePixmapLogo (B_xpm);
   iconBNo = TtaCreatePixmapLogo (BNo_xpm);
   iconT = TtaCreatePixmapLogo (T_xpm);
   iconTNo = TtaCreatePixmapLogo (TNo_xpm);
   iconBack = TtaCreatePixmapLogo (Back_xpm);
   iconBackNo = TtaCreatePixmapLogo (BackNo_xpm);
   iconForward = TtaCreatePixmapLogo (Forward_xpm);
   iconForwardNo = TtaCreatePixmapLogo (ForwardNo_xpm);
   iconBrowser = TtaCreatePixmapLogo (Browser_xpm);
   iconEditor = TtaCreatePixmapLogo (Editor_xpm);
   iconH1 = TtaCreatePixmapLogo (H1_xpm);
   iconH1No = TtaCreatePixmapLogo (H1No_xpm);
   iconH2 = TtaCreatePixmapLogo (H2_xpm);
   iconH2No = TtaCreatePixmapLogo (H2No_xpm);
   iconH3 = TtaCreatePixmapLogo (H3_xpm);
   iconH3No = TtaCreatePixmapLogo (H3No_xpm);
   iconPrint = TtaCreatePixmapLogo (Print_xpm);
   iconBullet = TtaCreatePixmapLogo (Bullet_xpm);
   iconBulletNo = TtaCreatePixmapLogo (BulletNo_xpm);
   iconNum = TtaCreatePixmapLogo (Num_xpm);
   iconNumNo = TtaCreatePixmapLogo (NumNo_xpm);
   iconImage = TtaCreatePixmapLogo (Image_xpm);
   iconImageNo = TtaCreatePixmapLogo (ImageNo_xpm);
   iconDL = TtaCreatePixmapLogo (DL_xpm);
   iconDLNo = TtaCreatePixmapLogo (DLNo_xpm);
   iconLink = TtaCreatePixmapLogo (Link_xpm);
   iconLinkNo = TtaCreatePixmapLogo (LinkNo_xpm);
   iconTable = TtaCreatePixmapLogo (Table_xpm);
   iconTableNo = TtaCreatePixmapLogo (TableNo_xpm);
#ifdef AMAYA_PLUGIN
   iconPlugin = TtaCreatePixmapLogo (Plugin_xpm);
#endif /* AMAYA_PLUGIN */
#endif /* !_WINDOWS */
   InitMathML ();
#ifdef GRAPHML
   InitGraphML ();
#endif /* GRAPHML */

   /* init transformation callback */
   TtaSetTransformCallback ((Func) TransformIntoType);
   TargetName = NULL;

   /*
    * Initialize the Amaya user and tmp directories
    */

   s = TtaGetEnvString ("APP_TMPDIR");
   if (!CheckMakeDirectory (s, TRUE))
     /* try to use the default value */
     {
       s = TtaGetDefEnvString ("APP_TMPDIR");
       if (CheckMakeDirectory (s, TRUE))
	 /* make it the current user one */
	 TtaSetEnvString ("APP_TMPDIR", s, TRUE);
       else
	 /* didn't work, so we exit */
	 {
	   usprintf (TempFileDirectory,
		     TEXT("InitAmaya: Couldnt' create directory %s"), s);
#ifdef _WINDOWS
	   MessageBox (NULL, TempFileDirectory, TEXT("Error"), MB_OK);
#else
	   fprintf (stderr, TempFileDirectory);
#endif /* _WINDOWS */
	   exit (1);
	 }
     }

   /* add the temporary directory in document path */
   ustrcpy (TempFileDirectory, s);
   TtaAppendDocumentPath (TempFileDirectory);
 
#ifdef _WINDOWS
   s = TtaGetEnvString ("APP_HOME");
   if (!CheckMakeDirectory (s, TRUE))
     /* didn't work, so we exit */
     {
       usprintf (TempFileDirectory,
		 TEXT("InitAmaya: Couldnt' create directory %s"), s);
       MessageBox (NULL, TempFileDirectory, TEXT("Error"), MB_OK);
       exit (1);
     }
#endif /* _WINDOWS */
   /*
    * Build the User preferences file name:
    * $HOME/.amaya/amaya.css on Unix platforms
    * $HOME\amaya\amaya.css on Windows platforms
    */
   tempname = TtaAllocString (MAX_LENGTH);
   usprintf (tempname, TEXT("%s%c%s.css"), s, WC_DIR_SEP, HTAppName);
   if (TtaFileExist (tempname))
     UserCSS = TtaWCSdup (tempname);
   else
     /* no User preferences */
     UserCSS = NULL;

   /* Create and intialize resources needed for each document */
   /* Style sheets are strored in directory .amaya/0 */
   for (i = 0; i < DocumentTableLength; i++)
     {
       /* initialize document table */
       DocumentURLs[i] = NULL;
       DocumentTypes[i] = docHTML;
       DocumentSource[i] = 0;
       DocumentMeta[i] = NULL;
       ReadOnlyDocument[i] = FALSE;
       /* initialize history */
       InitDocHistory (i);
       /* Create a temporary sub-directory for storing the HTML and
	  image files */
       usprintf (tempname, TEXT("%s%c%d"), TempFileDirectory, WC_DIR_SEP, i);
       TtaMakeDirectory (tempname);
       /* adding the CSS directory */
       if (i == 0)
	 TtaAppendDocumentPath (tempname);
     }

   /* allocate working buffers */
   LastURLName = TtaAllocString (MAX_LENGTH);
   LastURLName[0] = WC_EOS;
   DirectoryName = TtaAllocString (MAX_LENGTH);

   /* set path on current directory */
   ugetcwd (DirectoryName, MAX_LENGTH);
   DocumentName = TtaAllocString (MAX_LENGTH);
   memset (DocumentName, EOS, MAX_LENGTH);
   SavePath = TtaAllocString (MAX_LENGTH);
   SavePath[0] = WC_EOS;
   SaveName = TtaAllocString (MAX_LENGTH);
   SaveName[0] = EOS;
   ObjectName = TtaAllocString (MAX_LENGTH);
   ObjectName[0] = EOS;
   SaveImgsURL = TtaAllocString (MAX_LENGTH);
   SaveImgsURL[0] = EOS;
   ustrcpy (ScanFilter, TEXT(".*htm*"));
   SaveAsHTML = TRUE;
   SaveAsXML = FALSE;
   SaveAsText = FALSE;
   CopyImages = FALSE;
   UpdateURLs = FALSE;
   SavingFile = TtaAllocString (MAX_LENGTH);
   AttrHREFvalue = TtaAllocString (MAX_LENGTH);
   AttrHREFvalue[0] = EOS;

#ifdef WITH_SOCKS
   SOCKSinit ("amaya");
#endif

   /* initialize parser mapping table and HTLib */
   InitMapping ();

   /* Define the backup function */
   TtaSetBackup (BackUpDocs);
   TtaSetApplicationQuit (FreeAmayaStructures);
   TtaSetDocStatusUpdate ((Proc) DocStatusUpdate);
   AMAYA = TtaGetMessageTable (TEXT("amayamsg"), AMAYA_MSG_MAX);
   /* allocate callbacks for amaya */
   BaseDialog = TtaSetCallback (CallbackDialogue, MAX_REF);
   /* init the Picture context */
   InitImage ();
   /* init the Picture context */
   InitPrint ();
   /* init the CSS context */
   InitCSS ();
   /* initialize the structure transformation context */
   InitTransform ();
   /* initialize automaton for the HTML parser */
   InitAutomaton ();
   /* initialize the configuration menu context */
   InitConfMenu ();
#ifdef ANNOTATIONS
   ANNOT_Init ();
#endif /* ANNOTATIONS */
   /* initialize the libwww */
   QueryInit ();

   CurrentDocument = 0;
   DocBook = 0;
   InNewWindow = FALSE;
   TtaFreeMemory (tempname);
   restoredDoc = RestoreAmayaDocs ();
   s = NULL;
   if (appArgc % 2 == 0)
      /* The last argument in the command line is the document to be opened */
      s = appArgv[appArgc - 1];
   else if (restoredDoc)
     /* old documents are restored */
     return;
#ifdef _WINDOWS
   usprintf (LostPicturePath, TEXT("%s\\amaya\\lost.gif"),
	     TtaGetEnvString ("THOTDIR"));              
#endif /* _WINDOWS */
   if (!s)
      /* No argument in the command line, no HOME_PAGE variable. Open the */
      /* default Amaya URL */
     GoToHome (0, 1);
   else if (IsW3Path (s))
     {
       /* it's a remote document */
       ustrcpy (LastURLName, s);
       CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
     }
   else
     {
       NormalizeFile (s, LastURLName, AM_CONV_NONE);
       if (FileExistTarget (LastURLName)) {
         /* check if it is an absolute or a relative name */
#ifdef _WINDOWS
	 if ((LastURLName[0] == WC_DIR_SEP) || (LastURLName[1] == TEXT(':')))
#else  /* !_WINDOWS */
           if (LastURLName[0] == WC_DIR_SEP)
#endif /* !_WINDOWS */
	     /* it is an absolute name */
	     TtaExtractName (LastURLName, DirectoryName, DocumentName);
	   else
	     /* it is a relative name */
	     ustrcpy (DocumentName, LastURLName);
	   /* start with the local document */
	   LastURLName[0] = WC_EOS;
	   CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
	 }
    else
        /* Create a new document */
        NewXHTML (0, 0);
    }
   Loading_method = CE_ABSOLUTE;
}

/*----------------------------------------------------------------------
  ChangeAttrOnRoot
  If the root element of the document does not have an attribute of
  type attrNum, create one.
  If the root has such an attribute, delete it.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ChangeAttrOnRoot (Document document, int attrNum)
#else
void         ChangeAttrOnRoot (document, view)
Document            document;
int                 attrNum;
#endif
{
   Element	    root;
   AttributeType    attrType;
   Attribute	    attr;
   ThotBool	    docModified;

   docModified = TtaIsDocumentModified (document);
   root = TtaGetMainRoot (document);
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = attrNum;
   attr = TtaGetAttribute (root, attrType);
   if (attr != NULL)
      /* the root element has a SectionNumbering attribute. Remove it */
      TtaRemoveAttribute (root, attr, document);
   else
      /* the root element has no SectionNumbering attribute. Create one */
      {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (root, attr, document);
        TtaSetAttributeValue (attr, 1, root, document);
      }
   if (!docModified)
     {
	TtaSetDocumentUnmodified (document);
	/* switch Amaya buttons and menus */
	DocStatusUpdate (document, docModified);
     }
}

/*----------------------------------------------------------------------
  ShowMapAreas
  Execute the "Show Map Areas" command
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowMapAreas (Document document, View view)
#else
void                ShowMapAreas (document, view)
Document            document;
View                view;

#endif
{
#ifdef _WINDOWS
   int frame = GetWindowNumber (document, view);

   if (currentFrame == 0 || currentFrame > 10)
      TtaError (ERR_invalid_parameter);
   else {
        HMENU hmenu = WIN_GetMenu (currentFrame); 
        if (!itemChecked) {
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          itemChecked = TRUE;
       } else {
              hmenu = WIN_GetMenu (currentFrame); 
              CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
              itemChecked = FALSE;
       }
   }
#endif /* _WINDOWS */
   ChangeAttrOnRoot (document, HTML_ATTR_ShowAreas);
}

/*----------------------------------------------------------------------
  MakeIDMenu
  A menu for adding or removing ID attributes in a document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MakeIDMenu (Document doc, View view)
#else
void                MakeIDMenu (doc, view)
Document            doc;
View                view;

#endif
{
#ifndef _WINDOWS
  int i;
  CHAR_T  s[MAX_LENGTH];
#endif /* _WINDOWS */

  /* initialize the global variables */
  IdElemName[0] = EOS;
  IdStatus[0] = EOS;
  IdDoc = doc;

  /* Create the dialogue form */
#ifndef _WINDOWS
  i = 0;
  strcpy (&s[i], TEXT("Add ID"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Remove ID"));
  TtaNewSheet (BaseDialog + MakeIdMenu,
	       TtaGetViewFrame (doc, view),
	       TEXT("ID attributes"),
	       2, s, FALSE, 6, 'L', D_DONE);
  TtaNewTextForm (BaseDialog + mElemName,
		  BaseDialog + MakeIdMenu,
		  TEXT("Enter an element name"),
		  10,
		  1,
		  TRUE);
  /* apply operation in */
  strcpy (s, "TIn the whole document");
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("TWithin selection"));
  TtaNewSubmenu (BaseDialog + mIdUseSelection,
		 BaseDialog + MakeIdMenu,
		 0,
		 TEXT("Apply operation"),
		 2,
		 s,
		 NULL,
		 TRUE);
  /* status label */
  TtaNewLabel (BaseDialog + mIdStatus,
	       BaseDialog + MakeIdMenu,
	       " ");
  /* select the current radio button */
  TtaSetMenuForm (BaseDialog + mIdUseSelection, IdApplyToSelection);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + MakeIdMenu, TRUE);
#else
  CreateMakeIDDlgWindow (TtaGetViewFrame (doc, view));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HelpAmaya (Document document, View view)
#else
void                HelpAmaya (document, view)
Document            document;
View                view;
#endif
{
   CHAR_T                localname[MAX_LENGTH];
#ifdef AMAYA_DEBUG
   Element             el;
   View                structView, tocView;
   FILE               *list;

   /* get the root element */
   el = TtaGetMainRoot (document);
   ustrcpy (localname, TempFileDirectory);
   ustrcat (localname, "/tree.debug");
   list = fopen (localname, "w");
   TtaListAbstractTree (el, list);
   fclose (list);
   ustrcpy (localname, TempFileDirectory);
   ustrcat (localname, "/view.debug");
   list = fopen (localname, "w");
   TtaListView (document, view, list);
   fclose (list);
   ustrcpy (localname, TempFileDirectory);
   ustrcat (localname, "/boxes.debug");
   list = fopen (localname, "w");
   TtaListBoxes (document, view, list);
   fclose (list);
   structView = TtaGetViewFromName (document, "Structure_view");
   if (structView != 0 && TtaIsViewOpened (document, structView))
     {
       ustrcpy (localname, TempFileDirectory);
       ustrcat (localname, "/structview.debug");
       list = fopen (localname, "w");
       TtaListView (document, structView, list);
       fclose (list);
       ustrcpy (localname, TempFileDirectory);
       ustrcat (localname, "/structboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, structView, list);
       fclose (list);
     }
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (tocView != 0 && TtaIsViewOpened (document, tocView))
     {
       ustrcpy (localname, TempFileDirectory);
       ustrcat (localname, "/tocview.debug");
       list = fopen (localname, "w");
       TtaListView (document, tocView, list);
       fclose (list);
       ustrcpy (localname, TempFileDirectory);
       ustrcat (localname, "/tocboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, tocView, list);
       fclose (list);
     }
   /* list now CSS rules */
   ustrcpy (localname, TempFileDirectory);
   ustrcat (localname, "/style.debug");
   list = fopen (localname, "w");
   TtaListStyleSchemas (document, list);
   fclose (list);
#endif /* AMAYA_DEBUG */

#ifndef _WINDOWS
   TtaNewDialogSheet (BaseDialog + AboutForm, TtaGetViewFrame (document, view),
		      HTAppName, 1, TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1,'L');
#endif  /* _WINDOWS */
   ustrcpy (localname, HTAppName);
   ustrcat (localname, TEXT(" - "));
   ustrcat (localname, HTAppVersion);
   ustrcat (localname, TEXT("     "));
   ustrcat (localname, HTAppDate);
#ifndef _WINDOWS
   TtaNewLabel(BaseDialog + Version, BaseDialog + AboutForm, localname);
   TtaNewLabel(BaseDialog + About1, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT1));
   TtaNewLabel(BaseDialog + About2, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT2));
   TtaShowDialogue (BaseDialog + AboutForm, FALSE);
#else  /* _WINDOWS */
   CreateHelpDlgWindow (TtaGetViewFrame (document, view), localname,
			TtaGetMessage(AMAYA, AM_ABOUT1),
			TtaGetMessage(AMAYA, AM_ABOUT2));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HelpAtW3C (Document document, View view)
#else
void                HelpAtW3C (document, view)
Document            document;
View                view;

#endif
{
  CHAR_T    localname[MAX_LENGTH];
  
  ustrcpy (localname, AMAYA_PAGE_DOC);
  ustrcat (localname, TEXT("BinDist.html"));
  document = GetHTMLDocument (localname, NULL, 0, 0, CE_HELP, FALSE, NULL,
			      NULL);
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void DisplayHelp (int doc, int index)
#else /* __STDC__*/
static void DisplayHelp (doc, index)
int         doc;
int         index;
#endif /* __STDC__*/
{
  Document    document;
  CHAR_T      localname[MAX_LENGTH];
  CHAR_T     *s, *lang;

  lang = TtaGetVarLANG ();
  s = TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      /* get the documentation in the current language */
      usprintf (localname, TEXT("%s%cdoc%camaya%c%s.%s"), s, DIR_SEP, DIR_SEP,
		DIR_SEP, Manual[index], lang);

      if (!TtaFileExist (localname))
      /* get the standard english documentation */
	usprintf (localname, TEXT("%s%cdoc%camaya%c%s"), s, DIR_SEP, DIR_SEP,
		  DIR_SEP, Manual[index]);
    }
  document = GetHTMLDocument (localname, NULL, 0, 0, CE_HELP, FALSE, NULL,
			      NULL);
  InitDocHistory (document);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpBrowsing (Document document, View view)
#else /* __STDC__*/
void HelpBrowsing (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, BROWSING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSelecting (Document document, View view)
#else /* __STDC__*/
void HelpSelecting (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, SELECTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSearching (Document document, View view)
#else /* __STDC__*/
void HelpSearching (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, SEARCHING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpViews (Document document, View view)
#else /* __STDC__*/
void HelpViews (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, VIEWS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpCreating (Document document, View view)
#else /* __STDC__*/
void HelpCreating (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, CREATING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpLinks (Document document, View view)
#else /* __STDC__*/
void HelpLinks (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, LINKS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpChanging (Document document, View view)
#else /* __STDC__*/
void HelpChanging (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, CHANGING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpTables (Document document, View view)
#else /* __STDC__*/
void HelpTables (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, TABLES);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpMath (Document document, View view)
#else /* __STDC__*/
void HelpMath (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, MATH);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSVG (Document document, View view)
#else /* __STDC__*/
void HelpSVG (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, SVG);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpImageMaps (Document document, View view)
#else /* __STDC__*/
void HelpImageMaps (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, IMAGEMAPS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpStyleSheets (Document document, View view)
#else /* __STDC__*/
void HelpStyleSheets (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, CSS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpAttributes (Document document, View view)
#else /* __STDC__*/
void HelpAttributes (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, ATTRIBUTES);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSpellChecking (Document document, View view)
#else /* __STDC__*/
void HelpSpellChecking (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, SPELLCHECKING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpPublishing (Document document, View view)
#else /* __STDC__*/
void HelpPublishing (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, PUBLISHING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpPrinting (Document document, View view)
#else /* __STDC__*/
void HelpPrinting (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, PRINTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpNumbering (Document document, View view)
#else /* __STDC__*/
void HelpNumbering (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, NUMBERING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpMakeBook (Document document, View view)
#else /* __STDC__*/
void HelpMakeBook (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, MAKEBOOK);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpAnnotation (Document document, View view)
#else /* __STDC__*/
void HelpAnnotation (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, ANNOTATE);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpConfigure (Document document, View view)
#else /* __STDC__*/
void HelpConfigure (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, CONFIGURE);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpShortCuts (Document document, View view)
#else /* __STDC__*/
void HelpShortCuts (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, SHORTCUTS);
}

/*----------------------------------------------------------------------
Accessibility help page. Added by Charles McCN oct 99
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpAccess (Document document, View view)
#else /* __STDC__*/
void HelpAccess (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (document, ACCESS);
}

/*----------------------------------------------------------------------
  ShowLogFile
  Show error messages generated by the parser.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ShowLogFile (Document doc, View view)
#else  /* __STDC__ */
void ShowLogFile (doc, view)
Document doc; 
View     view;
#endif /* __STDC__ */
{
  CHAR_T    localname[MAX_LENGTH];
  int       newdoc;

  usprintf (localname, TEXT("%s%c%d%cHTML.ERR"), TempFileDirectory, DIR_SEP,
	    doc, DIR_SEP);
  newdoc = GetHTMLDocument (localname, NULL, doc, 0, CE_LOG, FALSE, NULL, NULL);
  /* store the relation with the original document */
  if (newdoc)
    DocumentSource[newdoc] = doc;
}


/*----------------------------------------------------------------------
   CheckAmayaClosed closes the application when there is any more
   opened document
  ----------------------------------------------------------------------*/
void                CheckAmayaClosed ()
{
  int                i;

  /* is it the last loaded document ? */
  i = 1;
  while (i < DocumentTableLength && DocumentURLs[i] == NULL)
    i++;
  
  if (i == DocumentTableLength)
    TtaQuit ();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CloseDocument (Document doc, View view)
#else
void                CloseDocument (doc, view)
Document            doc;
View                view;

#endif
{
  TtcCloseDocument (doc, view);
  if (!W3Loading)
    CheckAmayaClosed ();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AmayaClose (Document document, View view)
#else
void                AmayaClose (document, view)
Document            document;
View                view;

#endif
{
   int              i;
   ThotBool         documentClosed;

   /* invalid current loading */

   W3Loading = 0;

   documentClosed = TRUE;
   /* free each loaded document */
   for (i = 1; i < DocumentTableLength; i++)
      if (DocumentURLs[i] != NULL)
	{
	  TtcCloseDocument (i, 1);
	  documentClosed = (DocumentURLs[i] == NULL);
	  if (!documentClosed)
	    /* the close has been aborted */
	    return;
	}
   TtaQuit ();
}
