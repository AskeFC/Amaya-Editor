/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint
 *
 */
 
#define THOT_EXPORT extern
#include "amaya.h"

#include "HTMLhistory_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"

#define DOC_HISTORY_SIZE 32

/* a record in an history */
typedef struct _HistElement
{
     STRING HistUrl;        /* document URL */
     STRING HistInitialUrl; /* document URL */
     STRING form_data;      /* data associated with forms */
     int    method;         /* method used to request this URL */
     int    HistPosition;	/* volume preceding the first element to be
                               made visible in the main window */
     int    HistDistance;	/* distance from the  top of the window to the
                               top of this element (% of the window height) */
} HistElement;

/* the history of a window */
typedef HistElement   anHistory[DOC_HISTORY_SIZE];


/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _GotoHistory_context
{
  Document             doc;
  int                  prevnext;
  ThotBool	       last;
  ThotBool             next_doc_loaded;
  STRING               initial_url;
} GotoHistory_context;

/* the history of all windows */
static anHistory    DocHistory[DocumentTableLength];
/* current position in the history of each window */
static int          DocHistoryIndex[DocumentTableLength];

/*----------------------------------------------------------------------
   InitDocHistory
   Reset history for document doc
  ----------------------------------------------------------------------*/
void                InitDocHistory (Document doc)
{
   DocHistoryIndex[doc] = -1;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                FreeDocHistory ()
{
  int               doc, i;

  for (doc = 0; doc < DocumentTableLength; doc++)
    {
      if (DocHistoryIndex[doc] > 0)
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	  {
	    if (DocHistory[doc][i].HistUrl != NULL)
	      TtaFreeMemory (DocHistory[doc][i].HistUrl);
	    if (DocHistory[doc][i].form_data != NULL)
	      TtaFreeMemory (DocHistory[doc][i].form_data);
	  }
    }
}
/*----------------------------------------------------------------------
  ElementAtPosition
  Returns the element that is at position pos in document doc.
  ----------------------------------------------------------------------*/
Element       	ElementAtPosition (Document doc, int pos)
{
   Element	el, result, child, next;
   int		sum, vol;
   ThotBool	stop;

   sum = 0;
   result = NULL;
   el = TtaGetMainRoot (doc);
   while (el != NULL && !result)
      {
      if (sum >= pos)
	result = el;
      else
	{
        child = TtaGetFirstChild (el);
	if (child == NULL)
	   result = el;
	else
	   {
	   el = child;
	   stop = FALSE;
	   do
	     {
	     vol = TtaGetElementVolume (el);
	     if (sum + vol <= pos)
	        {
	next = el;
	        TtaNextSibling (&next);
		if (next == NULL)
		  stop = TRUE;
		else
		  {
		  el = next;
	          sum += vol;
		  }
	        }
	     else
	        stop = TRUE;
	     }
           while (el != NULL && !stop);
	  }
        }
      }
   if (result)
      /* return the first leaf in the element found */
      {
      do
	{
	child = TtaGetFirstChild (result);
	if (child != NULL)
	   result = child;
	}
      while (child != NULL);
      }
   return result;
}

/*----------------------------------------------------------------------
  RelativePosition
  Returns the position of the first visible element in the main view of
  document doc.
  ----------------------------------------------------------------------*/
int             RelativePosition (Document doc, int *distance)
{
   int		sum;
   Element	el, sibling, ancestor;

   sum = 0;
   el = TtaGetFirstElementShown (doc, 1, distance);
   ancestor = el;
   while (ancestor != NULL)
      {
      sibling = ancestor;
      do
	 {
         TtaPreviousSibling (&sibling);
	 if (sibling != NULL)
	    sum += TtaGetElementVolume (sibling);
	 }
      while (sibling != NULL);
      ancestor = TtaGetParent (ancestor);
      }
   return sum;
}

/*----------------------------------------------------------------------
  IsNextDocLoaded
  A IsDocumentLoaded frontend which returns TRUE if a given URL is already
  being displayed in another window. 
  ----------------------------------------------------------------------*/
static ThotBool IsNextDocLoaded (const Document baseDoc, const STRING url,
				 const STRING form_data, const ClickEvent CE_event)
{
  STRING              tempdocument;
  STRING              target;
  STRING              documentname;
  STRING              parameters;
  STRING              pathname;
  ThotBool loaded;

  if (url == (STRING) NULL)
    return FALSE;

  tempdocument = TtaGetMemory (MAX_LENGTH);
  target = TtaGetMemory (MAX_LENGTH);
  documentname = TtaGetMemory (MAX_LENGTH);
  parameters = TtaGetMemory (MAX_LENGTH);
  pathname = TtaGetMemory (MAX_LENGTH);

  strcpy (tempdocument, url);
  ExtractParameters (tempdocument, parameters);
  /* Extract the target if necessary */
  ExtractTarget (tempdocument, target);

  /* Add the  base content if necessary */
  if (CE_event == CE_RELATIVE || CE_event == CE_FORM_GET
      || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
    NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
  else
    NormalizeURL (tempdocument, 0, pathname, documentname, NULL);

   /* check if the user is already browsing the document in another window */
   if (CE_event == CE_FORM_GET || CE_event == CE_FORM_POST)
     {
       loaded = IsDocumentLoaded (pathname, form_data);
       /* we don't concatenate the new parameters as we give preference
	  to the form data */
     }
   else
     {
       /* concatenate the parameters before making the test */
       if (parameters[0] != EOS)
	 {
	   strcat (pathname, "?");
	   strcat (pathname, parameters);
	 }
       loaded = IsDocumentLoaded (pathname, NULL);
     }

  TtaFreeMemory (pathname);
  TtaFreeMemory (tempdocument);
  TtaFreeMemory (target);
  TtaFreeMemory (documentname);
  TtaFreeMemory (parameters);

  return (loaded != 0);
}

/*----------------------------------------------------------------------
   GotoPreviousHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoPreviousHTML_callback (int newdoc, int status, STRING urlName,
				STRING outputfile, AHTHeaders *http_headers,
				void * context)
{
  Document             doc;
  Element	       el;
  GotoHistory_context *ctx = (GotoHistory_context *) context;
  int                  prev;

  if (ctx == NULL)
    return;

  prev = ctx->prevnext;
  doc = ctx->doc;
  if (doc == newdoc && DocHistoryIndex[doc] == prev)
    {
      /* we are still working on the same document */
      /* show the document at the position stored in the history */
      el = ElementAtPosition (doc, DocHistory[doc][prev].HistPosition);
      TtaShowElement (doc, 1, el, DocHistory[doc][prev].HistDistance);

      /* set the Forward button on if it was the last document in the history */
      if (ctx->last)
	SetArrowButton (doc, FALSE, TRUE);
    }
  if (!DocumentMeta[doc]->initial_url)
    DocumentMeta[doc]->initial_url = ctx->initial_url;
  else
    TtaFreeMemory (ctx->initial_url);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
   GotoPreviousHTML
   This function is called when the user presses the Previous button
  ----------------------------------------------------------------------*/
void  GotoPreviousHTML (Document doc, View view)
{
   GotoHistory_context *ctx;
   STRING              url = NULL;
   STRING              initial_url = NULL;
   STRING              form_data = NULL;
   int                 prev, i;
   int                 method;
   ThotBool	       last = FALSE;
   ThotBool            hist = FALSE;
   ThotBool            same_form_data;
   ThotBool            next_doc_loaded = FALSE;

   if (doc < 0 || doc >= DocumentTableLength)
      return;
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;

   /* previous document in history */
   prev = DocHistoryIndex[doc];
   if (prev ==  0)
      prev = DOC_HISTORY_SIZE - 1;
   else
      prev--;
 
   /* nothing to do if there is no previous document */
   if (DocHistory[doc][prev].HistUrl == NULL)
      return;

   /* get the previous document information*/
   url = DocHistory[doc][prev].HistUrl;
   initial_url = DocHistory[doc][prev].HistInitialUrl;
   form_data = DocHistory[doc][prev].form_data;
   method = DocHistory[doc][prev].method;

   /* is it the same form_data? */
   if (!form_data && (!DocumentMeta[doc]  || !DocumentMeta[doc]->form_data))
     same_form_data = TRUE;
   else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	    && (!strcmp (form_data, DocumentMeta[doc]->form_data)))
     same_form_data = TRUE;
   else
     same_form_data = FALSE;

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (DocumentURLs[doc] != NULL &&
       (strcmp(DocumentURLs[doc], url) || !same_form_data))
     {
       /* is the next document already loaded? */
       next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
       if (!next_doc_loaded && !CanReplaceCurrentDocument (doc, view))
	 return;
     }


   if (!next_doc_loaded)
     {
       /* the current document must be put in the history if it's the last
	  one */
       if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
	 {
	   if (!IsW3Path (DocumentURLs[doc]) && !TtaFileExist (DocumentURLs[doc]))
	     {
	       /* cannot store the current document in the history */
	       last = FALSE;
	       hist = FALSE;
	     }
	   else
	     {
	       last = TRUE;
	       hist = TRUE;
	     }
	 }
       else
	 {
	   i = DocHistoryIndex[doc];
	   i++;
	   i %= DOC_HISTORY_SIZE;
	   if (DocHistory[doc][i].HistUrl == NULL)
	     last = TRUE;
	 }

   /* set the Back button off if there is no previous document in history */
       i = prev;
       if (i ==  0)
	 i = DOC_HISTORY_SIZE - 1;
       else
	 i--;
       if (DocHistory[doc][i].HistUrl == NULL)
	 /* there is no previous document, set the Back button OFF */
	 SetArrowButton (doc, TRUE, FALSE);
     }

   /* save the context */
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = prev;
   ctx->last = last;
   ctx->doc = doc;
   ctx->next_doc_loaded = next_doc_loaded;
   ctx->initial_url = TtaStrdup (initial_url);

   /* 
   ** load (or jump to) the previous document 
   */
   if (!next_doc_loaded)
     {
     if (hist)
       /* record the current position in the history */
       AddDocHistory (doc, DocumentURLs[doc], DocumentMeta[doc]->initial_url,
		      DocumentMeta[doc]->form_data, DocumentMeta[doc]->method);
     
     DocHistoryIndex[doc] = prev;
     }

   /* is it the current document ? */     
   if (DocumentURLs[doc] && !strcmp (url, DocumentURLs[doc]) && same_form_data)
     {
       /* it's just a move in the same document */
       GotoPreviousHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
     }
   else
     {
       StopTransfer (doc, 1);
       (void) GetHTMLDocument (url, form_data, doc, doc, method, FALSE, (void *) GotoPreviousHTML_callback, (void *) ctx);
     }
}

/*----------------------------------------------------------------------
   GotoNextHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoNextHTML_callback (int newdoc, int status, STRING urlName,
			    STRING outputfile, AHTHeaders *http_headers,
			    void * context)
{
  Element	       el;
  Document             doc;
  GotoHistory_context     *ctx = (GotoHistory_context *) context;
  int                  next;
  
  /* retrieve the context */

  if (ctx == NULL)
    return;

  next = ctx->prevnext;
  doc = ctx->doc;
  if (doc == newdoc && DocHistoryIndex[doc] == next)
    {
      /* we are still working on the same document */
      /* show the document at the position stored in the history */
      el = ElementAtPosition (doc, DocHistory[doc][next].HistPosition);
      TtaShowElement (doc, 1, el, DocHistory[doc][next].HistDistance);
    }
  if (!DocumentMeta[doc]->initial_url)
    DocumentMeta[doc]->initial_url = ctx->initial_url;
  else
    TtaFreeMemory (ctx->initial_url);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
   GotoNextHTML
   This function is called when the user presses the Next button
  ----------------------------------------------------------------------*/
void                GotoNextHTML (Document doc, View view)
{
   GotoHistory_context  *ctx;
   STRING        url = NULL;
   STRING        initial_url = NULL;
   STRING        form_data = NULL;
   int           method;
   int		 next, i;
   ThotBool      same_form_data;
   ThotBool      next_doc_loaded = FALSE;

   if (doc < 0 || doc >= DocumentTableLength)
      return;
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;

   /* next entry in history */
   next = DocHistoryIndex[doc] + 1;
   next %= DOC_HISTORY_SIZE;

   /* nothing to do if there is no next document */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
      return;
   if (DocHistory[doc][next].HistUrl == NULL)
      return;

   /* Get the next document information */
   url = DocHistory[doc][next].HistUrl;
   initial_url = DocHistory[doc][next].HistInitialUrl;
   form_data = DocHistory[doc][next].form_data;
   method = DocHistory[doc][next].method;

   /* is the form_data the same? */
   if (!form_data && (!DocumentMeta[doc] || !DocumentMeta[doc]->form_data))
     same_form_data = TRUE;
   else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	    && (!strcmp (form_data, DocumentMeta[doc]->form_data)))
     same_form_data = TRUE;
   else
     same_form_data = FALSE;

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (DocumentURLs[doc] != NULL &&
       (strcmp (DocumentURLs[doc], DocHistory[doc][next].HistUrl) ||
	!same_form_data))
     {
       /* is the next document already loaded? */
       next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
       if (!CanReplaceCurrentDocument (doc, view))
	 return;
     }

   if (!next_doc_loaded)
     {
       /* set the Back button on if it's off */
       i = DocHistoryIndex[doc];
       if (i ==  0)
	 i = DOC_HISTORY_SIZE - 1;
       else
	 i--;
       if (DocHistory[doc][i].HistUrl == NULL)
	 /* there is no document before the current one. The Back button is
	    normally OFF */
	 /* set the Back button ON */
	 SetArrowButton (doc, TRUE, TRUE);

       /* set the Forward button off if the next document is the last one
	  in the history */
       i = next;
       i++;
       i %= DOC_HISTORY_SIZE;
       if (DocHistory[doc][i].HistUrl == NULL)
	 SetArrowButton (doc, FALSE, FALSE);
     }
   
   /*
   ** load the next document
   */
   if (!next_doc_loaded)
     DocHistoryIndex[doc] = next;

   /* save the context */
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = next;
   ctx->doc = doc;
   ctx->next_doc_loaded = next_doc_loaded;
   ctx->initial_url = TtaStrdup (initial_url);

   /* is it the current document ? */
   if (DocumentURLs[doc] && !strcmp (url, DocumentURLs[doc]) && same_form_data)
     /* it's just a move in the same document */
     GotoNextHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
   else
     {
       StopTransfer (doc, 1);
       (void) GetHTMLDocument (url, form_data, doc, doc, method, FALSE, (void *) GotoNextHTML_callback, (void *) ctx);
     }
}

/*----------------------------------------------------------------------
   AddDocHistory
   Add a new URL in the history associated with the window of document doc.
  ----------------------------------------------------------------------*/
void AddDocHistory (Document doc, STRING url, STRING initial_url,
		    STRING form_data, ClickEvent method)
{
   int                 i, position, distance;

   if (!url || *url == EOS)
      return;
   /* avoid storing POST forms */
   if (method == CE_FORM_POST)
     return;
   /* don't register a new document not saved */
   if (!IsW3Path (url) && !TtaFileExist (url))
     return;
   else if (method == CE_RELATIVE)
     /* All registered URLs are absolute */
     method = CE_ABSOLUTE;

   /* initialize the history if it has not been done yet */
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
     {
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	  {
	    DocHistory[doc][i].HistUrl = NULL;
	    DocHistory[doc][i].form_data = NULL;
	  }
	DocHistoryIndex[doc] = 0;
     }

   /* set the Back button on if necessary */
   i = DocHistoryIndex[doc];
   if (i ==  0)
      i = DOC_HISTORY_SIZE - 1;
   else
      i--;
   if (DocHistory[doc][i].HistUrl == NULL)
      /* there is no document before in the history */
      /* The Back button is normally OFF set it ON */
      SetArrowButton (doc, TRUE, TRUE);
     
   /* store the URL and the associated form data */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);
   
   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = TtaStrdup (url);
   DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl = TtaStrdup (initial_url);
   DocHistory[doc][DocHistoryIndex[doc]].form_data = TtaStrdup (form_data);
   DocHistory[doc][DocHistoryIndex[doc]].method = method;

   position = RelativePosition (doc, &distance);
   DocHistory[doc][DocHistoryIndex[doc]].HistDistance = distance;
   DocHistory[doc][DocHistoryIndex[doc]].HistPosition = position;

   DocHistoryIndex[doc]++;
   DocHistoryIndex[doc] %= DOC_HISTORY_SIZE; 

   /* delete the next entry in the history */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl);

   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);

   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].form_data = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].method = CE_ABSOLUTE;

   /* set the Forward button off */
   SetArrowButton (doc, FALSE, FALSE);
}




