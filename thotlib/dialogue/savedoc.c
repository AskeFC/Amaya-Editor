/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "tree.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"
#include "application.h"
#include "document.h"
#include "fileaccess.h"
#include "thotdir.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"

#include "actions_f.h"
#include "applicationapi_f.h"
#include "browser_f.h"
#include "callback_f.h"
#include "config_f.h"
#include "createdoc_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "fileaccess_f.h"
#include "font_f.h"
#include "opendoc_f.h"
#include "platform_f.h"
#include "translation_f.h"
#include "viewapi_f.h"
#include "writedoc_f.h"

extern int          CurrentDialog;
char                DefaultFileSuffix[5];        
static PathBuffer   SaveDirectoryName;
static PathBuffer   SaveFileName;
static PathBuffer   TraductionSchemaName;
static ThotBool     SaveDocWithCopy;
static ThotBool     SaveDocWithMove;
static PtrDocument  DocumentToSave;
static int          PivotEntryNum;

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  UnsetEntryMenu
  displays as non active the "ent" entry of the menu referenced by "ref".
  ----------------------------------------------------------------------*/
void UnsetEntryMenu (int ref, int ent)
{
   char                fontname[100];
   char                text[20];

   if (TtWDepth > 1)
      TtaRedrawMenuEntry (ref, ent, NULL, InactiveB_Color, 0);
   else
     {
	FontIdentifier ('L', 'T', 2, 11, 1, text, fontname);
	TtaRedrawMenuEntry (ref, ent, fontname, -1, 0);
     }
}

/*----------------------------------------------------------------------
  CallbackSaveDocMenu
  callback handler for the Save File menu.
  ----------------------------------------------------------------------*/
void CallbackSaveDocMenu (int ref, int typedata, char *txt)
{
  PathBuffer          ptTranslatedName;
  PathBuffer          BufDir;
  char               *URL_DIR_SEP;
  int                 i, nbitem;
  int                 val;

  if (typedata == STRING_DATA && txt && strchr (txt, '/'))
    URL_DIR_SEP = '/';
  else 
    URL_DIR_SEP = DIR_SEP;

  val = (int) txt;
  switch (ref)
    {
    case NumZoneDocNameTooSave:
      /* zone de saisie du nom du document a creer */
      if (TtaCheckDirectory (txt) &&
	  txt[strlen (txt) - 1] != URL_DIR_SEP)
	{
	  strcpy (SaveDirectoryName, txt);
	  SaveFileName[0] = EOS;
	}
      else
	{
	  /* conserve le nom du document a sauver */
	  TtaExtractName (txt, SaveDirectoryName, BufDir);
	  /* Remove le suffixe du nom de fichier */
	  i = strlen (BufDir) - 4;
	  if (!strcmp (&BufDir[i], DefaultFileSuffix))
	    BufDir[i] = EOS;
	  else
	    i += 4;
	  if (i >= MAX_NAME_LENGTH - 1)
	    {
	      i = MAX_NAME_LENGTH - 1; /* Longueur du nom limitee */
	      BufDir[i] = EOS;
	      strcpy (ptTranslatedName, SaveDirectoryName);
	      strcat (ptTranslatedName, DIR_STR);
	      strcat (ptTranslatedName, BufDir);
	      if (TraductionSchemaName[0] == EOS)
		strcat (ptTranslatedName, DefaultFileSuffix);
	      /* reinitialise la zone du nom de document */
	      TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
	    }
	  strcpy (SaveFileName, BufDir);
	}

      if (TtaCheckDirectory (SaveDirectoryName))
	{
	  /* Est-ce un nouveau directory qui contient des documents*/
	  if (!TtaIsInDocumentPath (SaveDirectoryName) &&
	      TtaIsSuffixFileIn (SaveDirectoryName, DefaultFileSuffix))
	    {
	      /* il faut ajouter le directory au path */
	      i = strlen (DocumentPath);
	      if (i + strlen (SaveDirectoryName) + 2 < MAX_PATH)
		{
		  strcat (DocumentPath, PATH_STR);
		  strcat (DocumentPath, SaveDirectoryName);
		  BuildPathDocBuffer (BufDir, EOS, &nbitem);
		  TtaNewSelector (NumZoneDirDocToSave, NumFormSaveAs,
				  TtaGetMessage (LIB, TMSG_DOC_DIR),
				  nbitem, BufDir, 6, NULL, FALSE, TRUE);
		}
	    }
	}
      break;

    case NumZoneDirDocToSave:
      /* zone de saisie du directory ou le document doit etre cree */
      strcpy (SaveDirectoryName, txt);
      strcpy (ptTranslatedName, SaveDirectoryName);
      strcat (ptTranslatedName, DIR_STR);
      strcat (ptTranslatedName, SaveFileName);
      if (TraductionSchemaName[0] == EOS)
	strcat (ptTranslatedName, DefaultFileSuffix);
      /* reinitialise la zone du nom de document */
      TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
      break;

    case NumMenuFormatDocToSave:
      /* sous-menu pour le choix du format de sauvegarde */
      strcpy (ptTranslatedName, SaveDirectoryName);
      strcat (ptTranslatedName, DIR_STR);
      strcat (ptTranslatedName, SaveFileName);
      if (val == 0)
	{
	  /* premiere entree du menu format: format Thot */
	  TraductionSchemaName[0] = EOS;
	  strcat (ptTranslatedName, DefaultFileSuffix);
	  TtaRedrawMenuEntry (NumMenuCopyOrRename, 0, NULL, -1, 1);
	  TtaRedrawMenuEntry (NumMenuCopyOrRename, 1, NULL, -1, 1);
	}
      else if (val == PivotEntryNum)
	{
	  strcpy (TraductionSchemaName, "_ThotOther_");
	  UnsetEntryMenu (NumMenuCopyOrRename, 0);
	  UnsetEntryMenu (NumMenuCopyOrRename, 1);
	}
      else
	{
	  ConfigGetExportSchemaName (val, TraductionSchemaName);
	  UnsetEntryMenu (NumMenuCopyOrRename, 0);
	  UnsetEntryMenu (NumMenuCopyOrRename, 1);
	}
      /* reinitialise la zone du nom de document */
      TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
      break;

    case NumMenuCopyOrRename:
      /* sous-menu copier/renommer un document */
      if (val == 0)
	/* c'est un copy */
	{
	  SaveDocWithCopy = TRUE;
	  SaveDocWithMove = FALSE;
	}
      else
	/* c'est un move */
	{
	  SaveDocWithCopy = FALSE;
	  SaveDocWithMove = TRUE;
	}
      break;

    case NumFormSaveAs:
      /* le formulaire Sauver Comme */
      /* fait disparaitre la feuille de dialogue */
      if (val == 1)
	{
	  /* c'est une confirmation */
	  TtaUnmapDialogue (NumFormSaveAs);
	  CurrentDialog = NumFormSaveAs;
	  if (DocumentToSave != NULL &&
	      DocumentToSave->DocSSchema != NULL)
	    {
	      /* le document a sauver n'a pas ete ferme' entre temps */
	      if (!TtaCheckDirectory (SaveDirectoryName))
		{
		  /* le repertoire est invalide : affiche un message et
		     detruit les dialogues */
		  TtaDisplayMessage (CONFIRM,
				     TtaGetMessage (LIB, TMSG_MISSING_DIR),
				     SaveDirectoryName);
		  if (ThotLocalActions[T_confirmcreate] != NULL)
		    (*ThotLocalActions[T_confirmcreate])
		      (0, 1, (char *) 1);
		}
	      else if (!strcmp (SaveDirectoryName, DocumentToSave->DocDirectory) &&
		       !strcmp (SaveFileName, DocumentToSave->DocDName) &&
		       TraductionSchemaName[0] == WC_EOS)
		{
		  /* traite la confirmation */
		  if (ThotLocalActions[T_confirmcreate] != NULL)
		    (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm,
							  1, (char *) 1);
		}
	      else
		{
		  strcpy (ptTranslatedName, SaveDirectoryName);
		  strcat (ptTranslatedName, WC_DIR_STR);
		  strcat (ptTranslatedName, SaveFileName);
		  if (TraductionSchemaName[0] == WC_EOS)
		    strcat (ptTranslatedName, DefaultFileSuffix);
		  if (TtaFileExist (ptTranslatedName))
		    {
		      /* demande confirmation */
		      sprintf (BufDir, TtaGetMessage(LIB, TMSG_FILE_EXIST),
			       ptTranslatedName);
		      TtaNewLabel (NumLabelConfirm, NumFormConfirm, BufDir);
		      TtaShowDialogue (NumFormConfirm, FALSE);
		    }
		  else if (ThotLocalActions[T_confirmcreate] != NULL)
		    /* traite la confirmation */
		    (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm,
							  1, (char *) 1);
		}
	    }
	}
      else if (ThotLocalActions[T_confirmcreate] != NULL)
	(*ThotLocalActions[T_confirmcreate]) (0, 1, (char *) 1);
      break;
    }
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   BuildSaveDocMenu
   does the File Save.
  ----------------------------------------------------------------------*/
void BuildSaveDocMenu ()
{
  PathBuffer          outputFileName;
  NotifyDialog        notifyDoc;
  int                 i;

  /* Name du fichier a sauver */
  strcpy (outputFileName, SaveDirectoryName);
  strcat (outputFileName, DIR_STR);
  strcat (outputFileName, SaveFileName);
  if (TraductionSchemaName[0] == EOS)
    /* sauver en format Thot */
    {
      if (ThotLocalActions[T_setwritedirectory] != NULL &&
	  ThotLocalActions[T_writedocument] != NULL)
	{
	  if (DocumentToSave->DocPivotVersion == -1)
	    {
	      (*ThotLocalActions [T_setwritedirectory]) (DocumentToSave,
							 SaveFileName, 
							 SaveDirectoryName, 
							  SaveDocWithCopy, 
							SaveDocWithMove);
	       (*ThotLocalActions[T_writedocument]) (DocumentToSave, 0);
	     }
	   else
	     {
	       strcat (outputFileName, ".PIV"); 
	       (void) StoreDocument (DocumentToSave,
				     SaveFileName, SaveDirectoryName,
				     SaveDocWithCopy, SaveDocWithMove);
	     }
	 }
     }
   else if (!strcmp (TraductionSchemaName, "_ThotOther_"))
     {
       if (DocumentToSave->DocPivotVersion == -1)
         {
	   strcat (outputFileName, ".PIV"); 
	   (void) StoreDocument (DocumentToSave,
				 SaveFileName, SaveDirectoryName,
				 SaveDocWithCopy, SaveDocWithMove);
	 }
     }
   else
      /* exporter le document */
     {
	/* envoie le message DocExport.Pre a l'application */
	notifyDoc.event = TteDocExport;
	notifyDoc.document = (Document) IdentDocument (DocumentToSave);
	notifyDoc.view = 0;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	   /* l'application accepte que Thot exporte le document */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_EXPORTING),
				DocumentToSave->DocDName);
	     FindCompleteName (SaveFileName, "", SaveDirectoryName,
			       outputFileName, &i);
	     ExportDocument (DocumentToSave, outputFileName,
			     TraductionSchemaName, FALSE);
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB,TMSG_LIB_DOC_WRITTEN),
				outputFileName);
	     /* envoie le message DocExport.Post a l'application */
	     notifyDoc.event = TteDocExport;
	     notifyDoc.document = (Document) IdentDocument (DocumentToSave);
	     notifyDoc.view = 0;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
  SaveDocAs creates, initializes and activates the "Save As" form for
  document pDoc.
  ----------------------------------------------------------------------*/
static void SaveDocAs (PtrDocument pDoc)
{
   int                 nbitem;
   char                BufMenu[MAX_TXT_LEN];
   char                BufMenuB[MAX_TXT_LEN];   
   char                BufDir[MAX_PATH];
   char               *src;
   char               *dest;
   int                 i, k, l, Indx,entry;

   if (pDoc != NULL)
      if (!pDoc->DocReadOnly)
	 if (pDoc->DocSSchema != NULL)
	   {
	      DocumentToSave = pDoc;
#ifndef _WINDOWS
	      /* cree le formaulaire Sauver comme */
	      TtaNewSheet (NumFormSaveAs, 0, TtaGetMessage (LIB, TMSG_SAVE_AS), 1, TtaGetMessage (LIB, TMSG_SAVE), TRUE, 3, 'L', D_CANCEL);
#endif /* !_WINDOWS */

	      /* cree et */
	      /* initialise le selecteur sur aucune entree */
	      BuildPathDocBuffer (BufDir, EOS, &nbitem);
#ifndef _WINDOWS
	      TtaNewSelector (NumZoneDirDocToSave, NumFormSaveAs,
			      TtaGetMessage (LIB, TMSG_DOC_DIR), nbitem,
			      BufDir, 6, NULL, FALSE, TRUE);
#endif /* _WINDOWS */
	      entry = SearchStringInBuffer(BufDir,pDoc->DocDirectory,nbitem);
#ifndef _WINDOWS
	      TtaSetSelector (NumZoneDirDocToSave, entry, "");
#endif /* !_WINDOWS */
	      /* initialise le titre du formulaire Sauver Comme */
	      strcpy (SaveFileName, pDoc->DocDName);
	      strcpy (SaveDirectoryName, pDoc->DocDirectory);
	      /* compose le menu des formats de sauvegarde applicables */
	      /* a ce document, d'apres sa classe */
	      nbitem = ConfigMakeMenuExport(pDoc->DocSSchema->SsName, BufMenu);
	      /* met le format Thot en tete */
	      BufMenuB[0] = 'B';
	      strcpy (&BufMenuB[1], TtaGetMessage (LIB, TMSG_THOT_APP));
	      l = strlen (TtaGetMessage (LIB, TMSG_THOT_APP)) + 2;
	      /* ajoute 'B' au debut de chaque entree */
	      dest = &BufMenuB[l];
	      src = &BufMenu[0];
	      for (k = 1; k <= nbitem; k++)
		{
		   strcpy (dest, "B");
		   dest++;
		   l = strlen (src);
		   strcpy (dest, src);
		   dest += l + 1;
		   src += l + 1;
		}
	      nbitem++;
	      if (pDoc->DocPivotVersion == -1)
		{
		  strcpy (dest, "B");
		  dest++;
		  strcpy (dest, "Pivot");
		  PivotEntryNum = nbitem;
		  nbitem++;
		}
	      TtaNewSubmenu (NumMenuFormatDocToSave, NumFormSaveAs, 0,
			     TtaGetMessage (LIB, TMSG_DOC_FORMAT), nbitem,
			     BufMenuB, NULL, TRUE);
	      TtaSetMenuForm (NumMenuFormatDocToSave, 0);
	      /* sous-menu copier/renommer un document */
	      Indx = 0;
	      sprintf (&BufMenu[Indx], "B%s",
			TtaGetMessage (LIB, TMSG_COPY));
	      Indx += strlen (&BufMenu[Indx]) + 1;
	      sprintf (&BufMenu[Indx], "B%s",
			TtaGetMessage (LIB, TMSG_RENAME));
	      TtaNewSubmenu (NumMenuCopyOrRename, NumFormSaveAs, 0,
		   TtaGetMessage (LIB, TMSG_SAVE), 2, BufMenu, NULL, FALSE);
	      TtaSetMenuForm (NumMenuCopyOrRename, 0);
	      /* initialise le  nom de document propose */
	      strcpy (BufMenu, TtaGetMessage (LIB, TMSG_SAVE));
	      strcat (BufMenu, " ");
	      strcat (BufMenu, pDoc->DocDName);
	      TtaChangeFormTitle (NumFormSaveAs, BufMenu);
	      strcpy (BufMenu, SaveDirectoryName);
	      strcat (BufMenu, WC_DIR_STR);
	      strcat (BufMenu, SaveFileName);
	      if (pDoc->DocPivotVersion == -1)
             strcat (BufMenu, ".xml");
	      else
               strcat (BufMenu, ".PIV");
	      /* nom de document propose' */
#ifndef _WINDOWS
	      TtaNewTextForm (NumZoneDocNameTooSave, NumFormSaveAs,
			      TtaGetMessage (LIB, TMSG_DOCUMENT_NAME), 50, 1,
			      TRUE);
#endif /* !_WINDOWS */
	      TtaSetTextForm (NumZoneDocNameTooSave, BufMenu);
	      /* premiere entree du menu format: format Thot */
	      TraductionSchemaName[0] = WC_EOS;
	      SaveDocWithCopy = TRUE;
	      SaveDocWithMove = FALSE;
	      /* Formulaire Confirmation creation */
	      strcpy (BufMenu, TtaGetMessage (LIB, TMSG_SAVE_AS));
	      i = strlen (BufMenu) + 1;
	      strcpy (&BufMenu[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
#ifndef _WINDOWS
	      TtaNewDialogSheet (NumFormConfirm,  0, NULL, 2, BufMenu, FALSE,
				 1, 'L');
#endif /* !_WINDOWS */

	      /* affiche le formulaire */
	      TtaShowDialogue (NumFormSaveAs, FALSE);
	   }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  TtcSaveDocumentAs
  standard handler for a SaveDocumentAs action.
  ----------------------------------------------------------------------*/
void TtcSaveDocumentAs (Document document, View view)
{
   PtrDocument         pDoc;
   int                 frame;

   if (document != 0)
     {
       pDoc = LoadedDocument[document - 1];
       if (pDoc != NULL)
	 /* il y a un document pour cette entree de la table */
	 {
	   if (ThotLocalActions[T_savedoc] == NULL)
	     {
	       TteConnectAction (T_savedoc, (Proc) BuildSaveDocMenu);
	       TteConnectAction (T_confirmcreate, (Proc) CallbackConfirmMenu);
	       TteConnectAction (T_rsavedoc, (Proc) CallbackSaveDocMenu);
	       TteConnectAction (T_buildpathdocbuffer, 
				 (Proc) BuildPathDocBuffer);
	     }
	   frame = GetWindowNumber (document, view);
	   if (ThotLocalActions[T_updateparagraph] != NULL)
	     (*ThotLocalActions[T_updateparagraph]) (ViewFrameTable[frame - 1].FrAbstractBox, frame);
	   SaveDocAs (pDoc);
	 }
     }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  TtcSaveDocument
  standard handler for a SaveDocument action.
  ----------------------------------------------------------------------*/
void TtcSaveDocument (Document document, View view)
{
  PtrDocument         pDoc;
  int                 frame;
  
  if (document != 0)
    {
      pDoc = LoadedDocument[document - 1];
      if (pDoc != NULL)
	/* il y a un document pour cette entree de la table */
	{
	  frame = GetWindowNumber (document, view);
	  if (ThotLocalActions[T_updateparagraph] != NULL)
	    (*ThotLocalActions[T_updateparagraph]) (ViewFrameTable[frame - 1].FrAbstractBox, frame);
	  SetDocumentModified (pDoc, !(* (Func)ThotLocalActions[T_writedocument]) (pDoc, 4), 0);
	}
    }
}










