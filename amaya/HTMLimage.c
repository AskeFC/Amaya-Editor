/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A
 * These functions concern Image elements.
 *
 * Author: I. Vatton
 *
 */
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "GraphML.h"

#include "init_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "html2thot_f.h"
#include "Xml2thot_f.h"

/*----------------------------------------------------------------------
   AddLoadedImage adds a new image into image descriptor table.	
   Local images are not stored into this table and the function    
   returns no descriptor and the value FALSE.                      
   If the remote image is new, adds a descriptor for this image    
   notes its local name and returns TRUE.                          
   For already loaded remote images the function returns the      
   descriptor entry and the value FALSE.                           
  ----------------------------------------------------------------------*/
ThotBool AddLoadedImage (char *name, char *pathname,
			 Document doc, LoadedImageDesc ** desc)
{
   LoadedImageDesc    *pImage, *previous, *sameImage;
   char               *localname;

   *desc = NULL;
   sameImage = NULL;
   if (pathname == NULL || name == NULL)
      return (FALSE);
   else if (IsHTTPPath (pathname))
     localname = GetLocalPath (doc, pathname); 
   else
      /* it is a local image */
      return (FALSE);		/* nothing to do */

   pImage = ImageURLs;
   previous = NULL;
   while (pImage != NULL)
     {
	if (strcmp (pathname, pImage->originalName) == 0)
	  {
	     /* image already loaded */
	     sameImage = pImage;
	     if (pImage->document == doc)
	       {
		  *desc = pImage;
		  TtaFreeMemory (localname);
		  return (FALSE);
	       }
	     else
	       {
		  /* see the next descriptor */
		  previous = pImage;
		  pImage = pImage->nextImage;
	       }
	  }
	else
	  {
	     /* see the next descriptor */
	     previous = pImage;
	     pImage = pImage->nextImage;
	  }
     }

   /* It is a new loaded image */
   pImage = (LoadedImageDesc *) TtaGetMemory (sizeof (LoadedImageDesc));
   pImage->originalName = TtaGetMemory (strlen (pathname) + 1);
   strcpy (pImage->originalName, pathname);
   pImage->localName = TtaGetMemory (strlen (localname) + 1);
   strcpy (pImage->localName, localname);
   pImage->prevImage = previous;
   if (previous != NULL)
      previous->nextImage = pImage;
   else
      ImageURLs = pImage;
   pImage->nextImage = NULL;
   pImage->document = doc;
   pImage->elImage = NULL;
   pImage->imageType = unknown_type;
   *desc = pImage;
   TtaFreeMemory (localname);
   if (sameImage != NULL)
     {
	/* the image file exist for a different document */
	pImage->status = IMAGE_LOADED;
	TtaFileCopy (sameImage->localName, pImage->localName);
	if (sameImage->content_type)
	  pImage->content_type = TtaStrdup (sameImage->content_type);
	else
	  pImage->content_type = NULL;
	return (FALSE);
     }
   else
     {
	pImage->status = IMAGE_NOT_LOADED;
	pImage->content_type = NULL;
	return (TRUE);
     }
}

/*----------------------------------------------------------------------
   SearchLoadedImage searches the image descriptor of a loaded image using
   its local name.
   The function returns the descriptor entry or NULL.
  ----------------------------------------------------------------------*/
LoadedImageDesc    *SearchLoadedImage (char *localpath, Document doc)
{
  LoadedImageDesc    *pImage;
  
  if (localpath == NULL)
    return (NULL);
  else
    {
      pImage = ImageURLs;
      while (pImage != NULL)
	{
	  if (strcmp (localpath, pImage->localName) == 0 && 
	      ((doc == 0) || (pImage->document == doc)))
	    /* image found */
	    return (pImage);
	  else
	    /* see the next descriptor */
	    pImage = pImage->nextImage;
	}
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   SearchLoadedDocImage searches the image descriptor of a loaded image 
   using the docImage document id and the URL (there's only one such image
   per document).
   The function returns the descriptor entry or NULL.
  ----------------------------------------------------------------------*/
LoadedImageDesc    *SearchLoadedDocImage (Document doc, char *url)
{
  LoadedImageDesc    *pImage;
  char               *ptr;

  pImage = ImageURLs;
  while (pImage != NULL)
    {
      if (pImage->document == doc)
	{
	  ptr = pImage->originalName + sizeof ("internal:") - 1;
	  if (!strcmp (ptr, url))
	    break;
	}
      pImage = pImage->nextImage;
    }
  return (pImage);
}


/*----------------------------------------------------------------------
   SetAreaCoords computes the coords attribute value from x, y,       
   width and height of the box.                           
  ----------------------------------------------------------------------*/
void SetAreaCoords (Document document, Element element, int attrNum)
{
   ElementType         elType;
   Element             child, map;
   AttributeType       attrType;
   Attribute           attrCoords, attrX, attrY;
   Attribute           attrW, attrH, attrShape;
   char               *text, *buffer;
   int                 x1, y1, x2, y2;
   int                 w, h;
   int                 length, shape, i;

   /* Is it an AREA element */
   elType = TtaGetElementType (element);
   if (elType.ElTypeNum != HTML_EL_AREA)
      return;
   /* get size of the map */
   map = TtaGetParent (element);
   TtaGiveBoxSize (map, document, 1, UnPixel, &w, &h);
   /* Search the coords attribute */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_coords;
   attrCoords = TtaGetAttribute (element, attrType);
   if (attrCoords == NULL)
      return;

   /* Search the shape attribute */
   attrType.AttrTypeNum = HTML_ATTR_shape;
   attrShape = TtaGetAttribute (element, attrType);
   if (attrShape == NULL)
      return;
   shape = TtaGetAttributeValue (attrShape);
   /* prepare the coords string */
   length = 2000;
   text = TtaGetMemory (length);
   if (shape == HTML_ATTR_shape_VAL_rectangle || shape == HTML_ATTR_shape_VAL_circle)
     {
	/* Search the x_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_x_coord;
	attrX = TtaGetAttribute (element, attrType);
	if (attrX == NULL)
	   return;
	/* Search the y_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_y_coord;
	attrY = TtaGetAttribute (element, attrType);
	if (attrY == NULL)
	   return;
	/* Search the width attribute */
	attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
	attrW = TtaGetAttribute (element, attrType);
	if (attrW == NULL)
	   return;
	/* Search the height attribute */
	attrType.AttrTypeNum = HTML_ATTR_height_;
	attrH = TtaGetAttribute (element, attrType);
	if (attrH == NULL)
	   return;

	x1 = TtaGetAttributeValue (attrX);
	if (x1 < 0)
	  {
	    /* out of left side */
	    x1 = 0;
	    TtaSetAttributeValue (attrX, x1, element, document);	    
	  }
	y1 = TtaGetAttributeValue (attrY);
	if (y1 < 0)
	  {
	    /* out of top side */
	    y1 = 0;
	    TtaSetAttributeValue (attrY, y1, element, document);	    
	  }
	x2 = TtaGetAttributeValue (attrW);
	if (x1 + x2 > w)
	  {
	    /* out of right side */
	    if (x1 > w - 4)
	      {
		if (x2 < w)
		  x1 = w - x2;
		else
		  {
		    x1 = 0;
		    x2 = w;
		    TtaSetAttributeValue (attrW, x2, element, document);
		  }
		TtaSetAttributeValue (attrX, x1, element, document);	    
	      }
	    else
	      {
		x2 = w - x1;
		TtaSetAttributeValue (attrW, x2, element, document);
	      }	    
	  }
	y2 = TtaGetAttributeValue (attrH);
	if (y1 + y2 > h)
	  {
	    /* out of bottom side */
	    if (y1 > h - 4)
	      {
		if (y2 < h)
		  y1 = h - y2;
		else
		  {
		    y1 = 0;
		    y2 = h;
		    TtaSetAttributeValue (attrH, y2, element, document);
		  }
		TtaSetAttributeValue (attrY, y1, element, document);	    
	      }
	    else
	      {
		y2 = h - y1;
		TtaSetAttributeValue (attrH, y2, element, document);
	      }    
	  }
	if (shape == HTML_ATTR_shape_VAL_rectangle)
	   sprintf (text, "%d,%d,%d,%d", x1, y1, x1 + x2, y1 + y2);
	else
	  {
	     /* to make a circle, height and width have to be equal */
	     if ((attrNum == 0 && x2 > y2) ||
		 attrNum == HTML_ATTR_height_)
	       {
		 /* we need to update the width */
		 w = y2;
		 h = w / 2;
		 TtaSetAttributeValue (attrW, w, element, document);
	       }
	     else if ((attrNum == 0 && x2 < y2) ||
		      attrNum == HTML_ATTR_IntWidthPxl)
	       {
		 /* we need to update the height */
		 w = x2;
		 h = w / 2;
		 TtaSetAttributeValue (attrH, w, element, document);
	       }
	     else
	       if (x2 > y2)
		 h = y2 / 2;
	       else
		 h = x2 / 2;
	     sprintf (text, "%d,%d,%d", x1 + h, y1 + h, h);
	  }
     }
   else if (shape == HTML_ATTR_shape_VAL_polygon)
     {
	child = TtaGetFirstChild (element);
	length = TtaGetPolylineLength (child);
	/* get points */
	i = 1;
	buffer = TtaGetMemory (100);
	text[0] = EOS;
	while (i <= length)
	  {
	     TtaGivePolylinePoint (child, i, UnPixel, &x1, &y1);
	     sprintf (buffer, "%d,%d", x1, y1);
	     strcat (text, buffer);
	     if (i < length)
	       strcat (text, ",");
	     i++;
	  }
	TtaFreeMemory (buffer);
     }
   TtaSetAttributeText (attrCoords, text, element, document);
   TtaFreeMemory (text);
}


/*----------------------------------------------------------------------
  UpdateImageMap sets or updates Ref_IMG MAP attributes for the current
  image.
  If there is a map, updates all mapareas.
  oldWidth is -1 or the old image width.
  oldHeight is -1 or the old image height.
  ----------------------------------------------------------------------*/
void UpdateImageMap (Element image, Document doc, int oldWidth, int oldHeight)
{
   AttributeType       attrType;
   Attribute           attr;
   Element             el, child;
   Element             prev, next, parent;
   char               *text;
   int                 shape, w, h, length;
   int                 deltax, deltay, val;
   DisplayMode         dispMode;
   ThotBool            newMap;

   dispMode = TtaGetDisplayMode (doc);
   /* Search the USEMAP attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_USEMAP;
   attr = TtaGetAttribute (image, attrType);
   if (attr != NULL)
     {
	/* Search the MAP element associated with IMAGE element */
	length = TtaGetTextAttributeLength (attr);
	length++;
	text = TtaGetMemory (length);
	TtaGiveTextAttributeValue (attr, text, &length);
	if (text[0] == '#')
	   el = SearchNAMEattribute (doc, &text[1], NULL);
	else
	   el = NULL;
	TtaFreeMemory (text);
	if (el == NULL)
	   return;

	/* ask Thot to stop displaying changes made in the document */
        if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (doc, DeferredDisplay);

	/* Update MAP attribute */
	attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	attr = TtaGetAttribute (el, attrType);
	parent = NULL;
	prev = el;
	newMap = (attr == NULL);
	if (newMap)
	  {
	    /* change the type of the map */
	    TtaPreviousSibling (&prev);
	    if (prev == NULL)
	      {
		next = el;
		TtaNextSibling (&next);
		if (next == NULL)
		  parent = TtaGetParent (el);
	      }
	    
	    TtaRemoveTree (el, doc);
	    ChangeElementType (el, HTML_EL_MAP);
	    /* create the attribute */
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (el, attr, doc);
	    /* now reinsert the element new map */
	    if (prev != NULL)
	      TtaInsertSibling (el, prev, FALSE, doc);
	    else if (next != NULL)
	      TtaInsertSibling (el, next, TRUE, doc);
	    else
	      TtaInsertFirstChild (&el, parent, doc);
	  }
	TtaSetAttributeReference (attr, el, doc, image, doc);

	/* Update AREAs attribute */
	el = TtaGetFirstChild (el);
	TtaGiveBoxSize (image, doc, 1, UnPixel, &w, &h);
	while (el != NULL)
	  {
	     /* Search the shape attribute */
	     attrType.AttrTypeNum = HTML_ATTR_shape;
	     attr = TtaGetAttribute (el, attrType);
	     if (attr != NULL)
	       {
		  shape = TtaGetAttributeValue (attr);
		  if (shape == HTML_ATTR_shape_VAL_polygon)
		    {
		       attrType.AttrTypeNum = HTML_ATTR_AreaRef_IMG;
		       attr = TtaGetAttribute (el, attrType);
		       if (attr == NULL)
			 {
			    /* create it */
			    attr = TtaNewAttribute (attrType);
			    TtaAttachAttribute (el, attr, doc);
			 }
		       TtaSetAttributeReference (attr, el, doc, image, doc);
		       /* do we need to initialize the polyline limits */
		       if (oldWidth == -1 && oldHeight == -1)
			 {
                            child = TtaGetFirstChild (el);
                            TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);
			 }
		    }
		  else if (oldWidth != -1 || oldHeight != -1)
		    {
		      deltax = deltay = 0;
		      /* move and resize the current map area */
		      if (oldWidth != -1 && w != 0)
			{
			  deltax = (w - oldWidth) * 100 / oldWidth;
			  if (deltax != 0)
			    {
			      /* Search the x_coord attribute */
			      attrType.AttrTypeNum = HTML_ATTR_x_coord;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltax / 100);
			      TtaSetAttributeValue (attr, val, el, doc);    
			      /* Search the width attribute */
			      attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltax / 100);
			      TtaSetAttributeValue (attr, val, el, doc);
			    }
			}
		      if (oldHeight != -1 && h != 0)
			{
			  deltay = (h - oldHeight) * 100 / oldHeight;
			  if (deltay != 0)
			    {
			      /* Search the y_coord attribute */
			      attrType.AttrTypeNum = HTML_ATTR_y_coord;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltay / 100);
			      TtaSetAttributeValue (attr, val, el, doc);
			      /* Search the height attribute */
			      attrType.AttrTypeNum = HTML_ATTR_height_;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltay / 100);
			      TtaSetAttributeValue (attr, val, el, doc);
			    }
			}

		      /* update area coords */
		      if (deltax && deltay)
			/* both width and height */
			SetAreaCoords (doc, el, 0);
		      else if (deltax)
			/* only width */
			SetAreaCoords (doc, el, HTML_ATTR_IntWidthPxl);
		      else
			/* only height */
			SetAreaCoords (doc, el, HTML_ATTR_height_);
		    }
	       }
	     TtaNextSibling (&el);
	  }
     }

   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  DisplayImage
  ----------------------------------------------------------------------*/
void DisplayImage (Document doc, Element el, char *imageName, char *mime_type)
{
  ElementType         elType;
  int                 modified, i;
  ThotBool            is_svg;
  ThotBool            xmlDec, withDoctype, isXML;
  DocumentType        thotType;
  int                 parsingLevel;
  CHARSET             charset;
  char                charsetname[MAX_LENGTH];

  modified = TtaIsDocumentModified (doc);
  elType = TtaGetElementType (el);
  if ((elType.ElTypeNum == HTML_EL_PICTURE_UNIT) ||
      ((elType.ElTypeNum == HTML_EL_Object) &&
      (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML")  == 0)))
    {
      /** for the moment, the above function won't identify SVG images.
	  So, we do the job here.
	  This block should at some time be integrated with the above one */
      if (TtaGetPictureType (el) == unknown_type)
	{
	  is_svg = FALSE;
	  if (mime_type)
	    {
	      if (!strcmp (mime_type, "image/svg"))
		is_svg = TRUE;
	    }
	  else
	    {
	      /* try the file's extension */
	      for (i = strlen (imageName); i > 0 && imageName[i] != '.'; i--);
	      if (imageName[i] == '.' && !strcmp (&imageName[i+1], "svg"))
		is_svg = TRUE;
	      else /* try sniffing */
		{
		  CheckDocHeader (imageName, &xmlDec, &withDoctype, &isXML,
				  &parsingLevel, &charset, charsetname, &thotType);
		  if (isXML && thotType == docSVG)
		    is_svg = TRUE;
		}
	    }
	}
      else
	is_svg = FALSE;

      if (is_svg)
	{
	  TtaSetPictureType (el, "image/svg");
	  /* parse the SVG file and include the parsed tree at the
	     position of the image element */
	  ParseXmlSubTree (NULL, imageName, el, FALSE,
				doc, TtaGetDefaultLanguage(), "SVG");
	}
      else
	{
	  /* svg images don't use Image Maps */
	  /* display the content of a picture element */
	  TtaSetPictureContent (el, imageName, SPACE, doc, mime_type);
	  UpdateImageMap (el, doc, -1, -1);
	}
    }
  else
    {
      /* create a background image for the element */
      /* set the value */
#ifndef _WINDOWS
      fprintf(stderr,"Background image !\n");
#endif /* _WINDOWS */
    }
  
  /* if the document was not modified before this update reset it unmodified */
  if (!modified)
    {
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, modified);
    }
  /* the image is loaded */
  TtaSetStatus (doc, 1, " ", NULL);
}

/*----------------------------------------------------------------------
   HandleImageLoaded is the callback procedure when the image is loaded	
   		from the web.						
  ----------------------------------------------------------------------*/
static void HandleImageLoaded (int doc, int status, char *urlName,
			       char *outputfile,
			       AHTHeaders *http_headers,
			       void * context)
{
   FetchImage_context *FetchImage_ctx;
   LoadedImageDesc    *desc;
   char               *tempfile;
   char               *base_url;
   char               *ptr;
   ElemImage          *ctxEl, *ctxPrev;
   ElementType         elType;

   /* restore the context */
   desc = NULL;
   FetchImage_ctx = (FetchImage_context *) context;
   if (FetchImage_ctx) 
     {
       desc = FetchImage_ctx->desc;   
       base_url = FetchImage_ctx->base_url;
       TtaFreeMemory (FetchImage_ctx);
       
       /* check if this request wasn't aborted */
       if (strcmp (base_url, DocumentURLs[doc]))
	 {
	   /* it's not the same url, so let's just return */
	   TtaFreeMemory (base_url);
	   /* @@ remove desc, close file? */
	   return;
	 }
       else
	 TtaFreeMemory (base_url);
     }

   if (DocumentURLs[doc] != NULL)
     {
	/* the image could not be loaded */
	if ((status != 200) && (status != 0))
	   return;
	tempfile = TtaGetMemory (MAX_LENGTH);
	/* rename the local file of the image */
	strcpy (tempfile, desc->localName);
	
	/* If this is an image document, point to the correct files */
	if (DocumentTypes[doc] == docImage)
	  {
	    ptr = strrchr (tempfile, '.');
	    if (ptr) 
	      {
		ptr++;
		strcpy (ptr, "html");
	      }
	    else
	      strcat (tempfile, ".html");
	    TtaFreeMemory (desc->localName);
	    desc->localName = TtaStrdup (tempfile);

	  }
	else
	  {
	    TtaFileUnlink (tempfile);	
#ifndef _WINDOWS
	    rename (outputfile, tempfile);
#else /* _WINDOWS */
	    if (rename (outputfile, tempfile) != 0)
	      sprintf (tempfile, "%s", outputfile); 
#endif /* _WINDOWS */
	  }

	/* save pathname */
	/* That could make confusion if the image is redirected:
	   the registered name is not the original name
	   TtaFreeMemory (desc->originalName);
	   pathname = urlName;
	   desc->originalName = TtaGetMemory (strlen (pathname) + 1);
	   strcpy (desc->originalName, pathname);
	*/
	desc->status = IMAGE_LOADED;
	/* display for each elements in the list */
	/* get the mime type if the image was downloaded from the net */
	ptr = HTTP_headers (http_headers, AM_HTTP_CONTENT_TYPE); 
	/* memorize the mime type (in case we want to save the file later on) */
	if (ptr)
	  desc->content_type = TtaStrdup (ptr);
 	ctxEl = desc->elImage;
	desc->elImage = NULL;
	while (ctxEl != NULL)
	  {
	    elType = TtaGetElementType (ctxEl->currentElement);

	    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	      DisplayImage (doc, ctxEl->currentElement, tempfile, ptr);
	    else if (ctxEl->callback != NULL)
	      ctxEl->callback(doc, ctxEl->currentElement, tempfile, ctxEl->extra);
	    /* get image type */
	    if (desc->imageType == unknown_type)
	      desc->imageType = TtaGetPictureType (ctxEl->currentElement);

	    ctxPrev = ctxEl;
	    ctxEl = ctxEl->nextElement;
	    TtaFreeMemory ( ctxPrev);
	  }
	TtaFreeMemory (tempfile);
     }
}

/*----------------------------------------------------------------------
   libWWWImageLoaded is the libWWW callback procedure when the image
                is loaded from the web.
  ----------------------------------------------------------------------*/
static void     libWWWImageLoaded (int doc, int status, char *urlName,
				   char *outputfile, AHTHeaders *http_headers,
				   void * context)
{
  FetchImage_context *FetchImage_ctx;

  if (DocumentURLs[doc] != NULL)
    {
      /* an image of the document is now loaded */
      /* update the stop button status */
      ResetStop (doc);

      /* the image could not be loaded */
      if (status != 0)
	{
	  /* erase the context */
	  FetchImage_ctx = (FetchImage_context *) context;
	  if (FetchImage_ctx) 
	    {
	      if (FetchImage_ctx->base_url)
		TtaFreeMemory (FetchImage_ctx->base_url);
	      /* should we also erase ->desc or update it somehow? */
	      TtaFreeMemory (FetchImage_ctx);
	    }
	  return;
	}

      /* rename the local file of the image */
      HandleImageLoaded (doc, status, urlName, outputfile, http_headers,
			 context);
     }
}

/*----------------------------------------------------------------------
   GetActiveImageInfo returns the URL information if the current      
   element is an image map and NULL if it is not.          
   The non-null returned string has the form "?X,Y"        
  ----------------------------------------------------------------------*/
char *GetActiveImageInfo (Document document, Element element)
{
   char               *ptr;
   int                 X, Y;

   ptr = NULL;
   if (element != NULL)
     {
       /* initialize X and Y. The user may click in any view. If it's not */
       /* the formatted view (view 1), TtaGiveSelectPosition does not */
       /* change variables X and Y. */
       X = Y = 0;
       /* Get the coordinates of the mouse within the image */
       TtaGiveSelectPosition (document, element, 1, &X, &Y);
       if (X < 0)
	 X = 0;
       if (Y < 0)
	 Y = 0;
       /* create the search string to be appended to the URL */
       ptr = TtaGetMemory (27);
       sprintf (ptr, "?%d,%d", X, Y);
     }
   return ptr;
}

/*----------------------------------------------------------------------
   FetchImage loads an image from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
void                FetchImage (Document doc, Element el, char *URL, int flags, LoadedImageCallback callback, void *extra)
{
  ElemImage          *ctxEl;
  ElementType         elType;
  Element             elAttr;
  AttributeType       attrType;
  Attribute           attr;
  LoadedImageDesc    *desc;
  char               *imageName;
  char                pathname[MAX_LENGTH];
  char                tempfile[MAX_LENGTH];
  int                 length, i, newflags;
  ThotBool            update;
  ThotBool            newImage;
  FetchImage_context *FetchImage_ctx;

  pathname[0] = EOS;
  tempfile[0] = EOS;
  imageName = NULL;
  attr = NULL;
  FetchImage_ctx = NULL;

  if (el != NULL && DocumentURLs[doc] != NULL)
    {
      if (URL == NULL)
	{
	  /* prepare the attribute to be searched */
	  elType = TtaGetElementType (el);
	  attrType.AttrSSchema = elType.ElSSchema;
	  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "GraphML"))
	    /* it's not a SVG element, it's then a HTML img element */
	    {
	    attrType.AttrTypeNum = HTML_ATTR_SRC;
	    elAttr = el;
	    }
          else
	    {
            attrType.AttrTypeNum = GraphML_ATTR_xlink_href;
	    elAttr = TtaGetParent (el);
	    }
	  attr = TtaGetAttribute (elAttr, attrType);
	  if (attr != NULL)
	    /* an element with an attribute SRC or xlink:href has been found */
	    {
	      /* get the attribute value */
	      length = TtaGetTextAttributeLength (attr);
	      if (length > 0)
		{
		  /* allocate some memory: length of name + 6 cars for noname */
		  imageName = TtaGetMemory (length + 7);
		  TtaGiveTextAttributeValue (attr, imageName, &length);
		}
	    }
	}
      else
	imageName = URL;

      if (imageName)
	{
	  update = TRUE;
	  /* add BASE to image name, if necessary */
	  NormalizeURL (imageName, doc, pathname, imageName, NULL);
	  /* if it's not a remote URL, make any necessary file: conversions */
	  if (!IsW3Path (pathname))
	  {
	    NormalizeFile (pathname, tempfile, AM_CONV_ALL);
	    strcpy (pathname, tempfile);
	    tempfile[0] = EOS;
	  }
	  /* is the image already loaded ? */
	  newImage = AddLoadedImage (imageName, pathname, doc, &desc);
	  if (newImage)
	    {
	      /* the current element has to be updated when the image 
		 will be loaded */
	      ctxEl = (ElemImage *) TtaGetMemory (sizeof (ElemImage));
	      desc->elImage = ctxEl;
	      ctxEl->currentElement = el;
	      ctxEl->nextElement = NULL;
	      ctxEl->callback = callback;
	      ctxEl->extra = extra;
	      update = FALSE;	/* the image is not loaded yet */
	      /* store the context before downloading the images */
	      FetchImage_ctx = TtaGetMemory (sizeof (FetchImage_context));
	      FetchImage_ctx->desc = desc;
	      FetchImage_ctx->base_url =  TtaStrdup (DocumentURLs[doc]);

	      UpdateTransfer(doc);
	      if (flags & AMAYA_MBOOK_IMAGE)
		newflags = flags | AMAYA_SYNC;
	      else
		newflags = flags | AMAYA_ASYNC;

	      i = GetObjectWWW (doc, pathname, NULL, tempfile,
	                        newflags, NULL, NULL,
				(void *) libWWWImageLoaded,
				(void *) FetchImage_ctx, NO, NULL);
	      if (i != -1) 
		desc->status = IMAGE_LOADED;
	      else
		{
		  update = TRUE;
		  desc->status = IMAGE_NOT_LOADED;
		}
	    }

	  /* display the image within the document */
	  if (update)
	    {
	      if (desc == NULL)
		{
		/* it is a local image */
		if (callback)
		  {
		    if (!strncmp(pathname, "file:/", 6))
		      callback(doc, el, &pathname[6], extra);
		    else
		      callback(doc, el, &pathname[0], extra);
		  }
		else
		  DisplayImage (doc, el, pathname, NULL);
		}
	      else
		if (TtaFileExist (desc->localName))
		  {
		    /* remote image, but already here */
		    if (callback)
		      callback (doc, el, desc->localName, extra);
		    else
		      DisplayImage (doc, el, desc->localName, NULL);
		    /* get image type */
		    desc->imageType = TtaGetPictureType (el);
		  }
		else
		  {
		    /* chain this new element as waiting for this image */
		    ctxEl = desc->elImage;
		    if (ctxEl != NULL && ctxEl->currentElement != el)
		      {
			/* concerned elements are different */
			while (ctxEl->nextElement != NULL)
			  ctxEl = ctxEl->nextElement;
			ctxEl->nextElement = (ElemImage *) TtaGetMemory (sizeof (ElemImage));
			ctxEl = ctxEl->nextElement;
			ctxEl->callback = callback;
			ctxEl->extra = extra;
			ctxEl->currentElement = el;
			ctxEl->nextElement = NULL;
		      }
		  }
	    }
	}
      
      if (attr != NULL && imageName)
	TtaFreeMemory (imageName);
    }
  TtaHandlePendingEvents ();
}

/*----------------------------------------------------------------------
   FetchAndDisplayImages   fetch and display all images referred   
   by document doc. The flags may indicate extra transfer parameters,
   for example bypassing the cache.
   elSubTree indicates we ara paring an external SVG image
   Returns TRUE if the the transfer succeeds without being stopped;
   Otherwise, returns FALSE.
  ----------------------------------------------------------------------*/
ThotBool FetchAndDisplayImages (Document doc, int flags, Element elSubTree)
{
   AttributeType       attrType;
   Attribute           attr, attrFound;
   ElementType         elType;
   Element             el, elFound, pic, elNext;
   char               *currentURL, *imageURI;
   int                 length;
   ThotBool            stopped_flag;

   /* JK: verify if StopTransfer was previously called */
   if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
     {
       /* transfer interrupted */
       TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);
       DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
       return FALSE;
     }
   else if (DocumentTypes[doc] == docText ||
	    DocumentTypes[doc] == docCSS)
     return FALSE;

   /* register the current URL */
   currentURL = TtaStrdup (DocumentURLs[doc]);

   /* We are currently fetching images for this document */
   /* during this time LoadImage has not to stop transfer */
   /* prepare the attribute to be searched */
   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
   if (attrType.AttrSSchema)
     /* there are some HTML elements in this documents. 
	Get all 'img' or 'object' elements */
     {
       /* search all elements having an attribute SRC */
       attrType.AttrTypeNum = HTML_ATTR_SRC;
       /* Start from the root element */
       if (elSubTree == NULL)
	 {
	   el = TtaGetMainRoot (doc);
	   TtaSearchAttribute (attrType, SearchForward,
			       el, &elFound, &attr);
	 }
       else
	 TtaSearchAttribute (attrType, SearchInTree,
			     elSubTree, &elFound, &attr);
       el = elFound;
       do
	 {
	   TtaHandlePendingEvents ();
	   /* verify if StopTransfer was called */
	   if (DocumentURLs[doc] == NULL ||
	       strcmp (currentURL, DocumentURLs[doc]))
	     /* the document has been removed */
	     break;
	   
	   if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
	     break;

	   /* FetchImage increments FilesLoading[doc] for
	      each new get request */
	   if (el != NULL)
	     {
	       /* search the next element having an attribute SRC */
	       elNext = el;
	       if (elSubTree == NULL)
		 TtaSearchAttribute (attrType, SearchForward,
				     elNext, &elFound, &attr);
	       if (elSubTree != NULL && elFound != NULL &&
		   !TtaIsAncestor (elFound, elSubTree))
		 elFound = NULL;
	       FetchImage (doc, el, NULL, flags, NULL, NULL);
	       el = elFound;
	     }
	 }
       while (el);
     }

   /* Now, load all SVG images */
   /* prepare the attribute to be searched */
   attrType.AttrSSchema = TtaGetSSchema ("GraphML", doc);
   if (attrType.AttrSSchema)
     {
       attrType.AttrTypeNum = GraphML_ATTR_xlink_href;
       /* Search the next element having an attribute xlink_href */
       /* Start from the root element */
       if (elSubTree == NULL)
	 {
	   el = TtaGetMainRoot (doc);
	   TtaSearchAttribute (attrType, SearchForward,
			       el, &elFound, &attrFound);
	 }
       else
	 TtaSearchAttribute (attrType, SearchInTree,
			     elSubTree, &elFound, &attrFound);
       attr = attrFound;
       el = elFound;
       do
	 {
	   TtaHandlePendingEvents ();
	   /* verify if StopTransfer was called */
	   if (DocumentURLs[doc] == NULL ||
	       strcmp (currentURL, DocumentURLs[doc]))
	     /* the document has been removed */
	     break;
	   
	   if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
	     break;
	   
	   /* FetchImage increments FilesLoading[doc] for each new get request */
	   if (el != NULL)
	     {
	       /* search the next element having an attribute xlink_href */
	       TtaSearchAttribute (attrType, SearchForward,
				   el, &elFound, &attrFound);
	       if (elSubTree != NULL && elFound != NULL &&
		   !TtaIsAncestor (elFound, elSubTree))
		 elFound = NULL;

	       /* get the PICTURE_UNIT element within the image element */
	       elType = TtaGetElementType (el);
	       elType.ElTypeNum = GraphML_EL_PICTURE_UNIT;
	       pic = TtaSearchTypedElement (elType, SearchInTree, el);
	       if (pic)
		 {
		   /* get the attribute value */
		   length = TtaGetTextAttributeLength (attr);
		   if (length > 0)
		     {
		       /* allocate some memory */
		       imageURI = TtaGetMemory (length + 7);
		       TtaGiveTextAttributeValue (attr, imageURI, &length);
		       FetchImage (doc, pic, imageURI, flags, NULL, NULL);
		       TtaFreeMemory (imageURI);
		     }
		 }
	       el = elFound;
	       attr = attrFound;
	     }
	 }
       while (el);
     }
   
   if (W3Loading != doc)
       stopped_flag = FALSE;
   else
     stopped_flag = TRUE;

   /* Images fetching is now finished */
   TtaFreeMemory (currentURL);

   return (stopped_flag);
}
