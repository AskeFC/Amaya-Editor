/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_H 
#define AMAYA_H

/* Thot interface */
#include "thot_gui.h"
#include "thot_sys.h"
#include "appstruct.h"
#include "application.h"
#include "attribute.h"
#include "browser.h"
#include "content.h"
#include "dialog.h"
#include "fileaccess.h"
#include "interface.h"
#include "libmsg.h"
#include "message.h"
#include "presentation.h"
#include "selection.h"
#include "reference.h"
#include "tree.h"
#include "view.h"
#include "uconvert.h"
#include "undo.h"
/* Included headerfiles */
#include "EDITOR.h"
#include "HTML.h"
#include "MathML.h"
#include "GraphML.h"
#include "TextFile.h"
#include "amayamsg.h"
#include "parser.h"

#define NAME_LENGTH     32

#define HTAppName     "amaya"
#define HTAppVersion  "V4.4"
#define HTAppDate     "25 April 2001"

#define URL_STR       "/"
#define URL_SEP       '/'

/* Number of views used in Amaya */
#define AMAYA_MAX_VIEW_DOC  7

/* the type of character convertion we want to make on local URLs */
typedef enum _ConvertionType
{
  AM_CONV_NONE = 0,
  AM_CONV_URL_SEP = 0x1, /* converts URL_SEPs into DIR_SEPs */
  AM_CONV_PERCENT = 0x2, /* converts %xx into the equiv. char */
  AM_CONV_ALL = 0xFFFF   /* do everything */
}
ConvertionType;

/* the HTTP header name we want to make visible to the application */
typedef enum _AHTHeaderName
  {
    AM_HTTP_CONTENT_TYPE = 0,
    AM_HTTP_CHARSET = 1,
    AM_HTTP_CONTENT_LENGTH = 2,
    AM_HTTP_REASON = 3
  } 
AHTHeaderName;

/* the structure used for exchanging HTTP header info between the net library
   and amaya */
typedef struct _AHTHeaders
  {
    char *content_type;
    char *charset;
    char *content_length;
    char *reason;
  }
AHTHeaders;

/* The structures used for request callbacks */
typedef void   TIcbf (Document doc, int status, char *urlName,
		      char *outputfile, const AHTHeaders *http_headers,
		      const char *data_block, int data_block_size,
		      void *context);

typedef void  TTcbf (Document doc, int status, char *urlName,
                     char *outputfile, const AHTHeaders *http_headers,
                     void *context);

/* How are Network accesses provided ? */
#include "libwww.h"

/* The different events to open a new document */
typedef enum _ClickEvent {
  CE_ABSOLUTE, CE_RELATIVE, CE_FORM_POST, CE_FORM_GET,
  CE_HELP, CE_MAKEBOOK, CE_LOG , CE_TEMPLATE, CE_INIT,
  CE_CSS, CE_ANNOT
} ClickEvent;

#define NO               0
#define YES              1

/* dialogues */
#define URLForm          1
#define OpenForm         2
#define URLName          3
#define LocalName        4
#define DirSelect        5
#define DocSelect        6
#define StopCommand      7
#define SaveForm         8
#define DirSave          9
#define DocSave         10
#define ToggleSave      11
#define NameSave        12
#define ImgDirSave      13
#define Label1          14
#define Label2          15
#define Label3          16
#define Label4          17
#define ConfirmForm     18
#define ConfirmText     19
#define AttrHREFForm    20
#define AttrHREFText    21
#define FormAnswer      22
#define RealmText       23
#define AnswerText      24
#define NameText        25
#define PasswordText    26
#define FilterText      27
#define ClassForm       28
#define ClassSelect     29
#define AClassForm      30
#define AClassSelect    31
#define ConfirmSave     32
#define ConfirmSaveList 33
#define BrowserDirSelect   34
#define BrowserDocSelect   35
#define BrowserFilterText  36
#define HREFLocalName   37
#define MakeIdMenu      38
#define mElemName       39
#define mAddId          40
#define mRemoveId       41
#define mIdUseSelection 42
#define mIdStatus       43
#define FileBrowserForm 44
#define FileBrowserText 45
#define FileBrowserFilter 46
#define FileBrowserLocalName 47
#define MimeTypeForm    48
#define MimeTypeSel     49
#define MimeTypeFormL1  50
#define MimeTypeFormL2  51
#define MimeTypeSaveL   52
#define MimeTypeSave    53
#define CharsetSaveL    54
#define CharsetSave     55
#define CharsetForm     56
#define CharsetSel      57
#define Label5          58
#define Label6          59
#define MimeFormStatus  60
#define SaveFormStatus  61
#define OptionMenu	62

/* MAX_SUBMENUS references reserved for submenus of Option menu */
/* Do not insert new entries here */
#define MAX_SUBMENUS    400
#define About1		450
#define About2		451
#define About3		452
#define Version		453
#define AboutForm      	454
#define TableForm       455
#define TableRows       456
#define TableCols       457
#define TableBorder     458
#define MathEntityForm  459
#define MathEntityText  460
#define TitleForm       461
#define TitleText       462
#define DocInfoForm           463
#define DocInfoMimeTypeTitle  464
#define DocInfoMimeType       465
#define DocInfoCharsetTitle   466
#define DocInfoCharset        467
#define DocInfoContentTitle   468
#define DocInfoContent        469
#define DocInfoLocationTitle  470
#define DocInfoLocation       471
#define DocInfoTitle1         472
#define DocInfoTitle2         473
#define DocInfoContent1       474
#define DocInfoContent2       475
#define MAX_REF         476

#define FormPrint        1
#define PrintOptions     2
#define PaperFormat      3
#define PrintSupport     4
#define PPrinterName     5
#define PaperOrientation 6
#define PPagesPerSheet   7
#define PRINT_MAX_REF	 8

#define ImageURL	1
#define ImageLabel	2
#define ImageLabel2	3
#define ImageLabel3	4
#define ImageLabel4	5
#define ImageDir	6
#define ImageSel	7
#define ImageFilter     8
#define FormImage	9
#define RepeatImage    10
#define FormBackground 11
#define ImageAlt       12
#define FormAlt        13
#define IMAGE_MAX_REF  14

#define CSSForm         0
#define CSSSelect       1
#define MAX_CSS_REF     2

#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

#ifdef GRAPHML
#define FormGraph 0
#define MenuGraph 1
#define MAX_GRAPH 2
#endif /* GRAPHML */

/* The possible GET/POST/PUT request modes */
/*synchronous request*/
#define AMAYA_SYNC	1	/*0x000001 */  
/*synchronous request with incremental callbacks */
#define AMAYA_ISYNC	2	/*0x000010 */  
/*asynchronous request */
#define AMAYA_ASYNC	4	/*0x000100 */
/*asynchronous request with incremental callbacks */
#define AMAYA_IASYNC	8	/*0x001000 */
/* send the form using the POST HTTP method */
#define AMAYA_FORM_POST 16	/*0x010000 */
/* send the form using the GET HTTP method */
#define AMAYA_FORM_GET  32	/*0x100000 */
/* bypass caching */
#define AMAYA_NOCACHE	64
/* don't follow redirections */
#define AMAYA_NOREDIR	128
/* Prevents a stop race condition in ASYNC transfers */
#define AMAYA_ASYNC_SAFE_STOP  256
#define AMAYA_LOAD_CSS   512
#define AMAYA_FLUSH_REQUEST 1024
#define AMAYA_USE_PRECONDITIONS 2048
#define AMAYA_LOAD_IMAGE 4096
/* post an annonation */
#define AMAYA_FILE_POST 8192
/* delete an annotation */
#define AMAYA_DELETE    16384
#define AMAYA_MBOOK_IMAGE 32768
/*
 * Flags to indicate the action to take when the network options
 * are modified
 */
#define AMAYA_CACHE_RESTART 1
#define AMAYA_PROXY_RESTART 2
#define AMAYA_LANNEG_RESTART 4
#define AMAYA_SAFEPUT_RESTART 8

/*
 * Flags to indicate the status of the network requests associated
 * to a document.
 */

#define AMAYA_NET_INACTIVE 1
#define AMAYA_NET_ERROR    2
#define AMAYA_NET_ACTIVE   4

/*
 * Flags for HTParse, specifying which parts of the URL are needed
 */
#define AMAYA_PARSE_ACCESS      16  /* Access scheme, e.g. "HTTP" */
#define AMAYA_PARSE_HOST        8   /* Host name, e.g. "www.w3.org" */
#define AMAYA_PARSE_PATH        4   /* URL Path, e.g. "pub/WWW/TheProject.html" */
#define AMAYA_PARSE_ANCHOR      2   /* Fragment identifier, e.g. "news" */
#define AMAYA_PARSE_PUNCTUATION 1   /* Include delimiters, e.g, "/" and ":" */
#define AMAYA_PARSE_ALL         31  /* All the parts */

THOT_EXPORT int        AMAYA;     /* Index of amaya message table */
THOT_EXPORT int        appArgc;
THOT_EXPORT char     **appArgv;
THOT_EXPORT char       TempFileDirectory[MAX_LENGTH];
THOT_EXPORT char       Answer_text[MAX_LENGTH];
THOT_EXPORT char       Answer_name[NAME_LENGTH];
THOT_EXPORT char       Answer_password[NAME_LENGTH];
THOT_EXPORT char       Display_password[NAME_LENGTH];
THOT_EXPORT char       ScanFilter[NAME_LENGTH]; /* to scan directories    */
THOT_EXPORT char       MathMLEntityName[MAX_LENGTH]; /* entity name typed by the user for a MathML expression */
THOT_EXPORT char       IdElemName[MAX_LENGTH]; /* element name typed by the user from the MakeID menu */
THOT_EXPORT char       IdStatus[50]; /* element name typed by the user from the MakeID menu */
THOT_EXPORT char       ImgFilter[NAME_LENGTH];
THOT_EXPORT char      *LastURLName;	/* last URL requested               */
THOT_EXPORT char      *DirectoryName;	/* local path of the document       */
THOT_EXPORT char      *DocumentName;	/* document name                    */
THOT_EXPORT char      *SavePath;	/* saving path                      */
THOT_EXPORT char      *SaveName;	/* saving name of the document      */
THOT_EXPORT char      *ObjectName;	/* document name                    */
THOT_EXPORT char      *SaveImgsURL;	/* where to save remote Images      */
THOT_EXPORT char      *TargetName;
THOT_EXPORT char      *SavingFile;	/* complete path or URL of the document */
THOT_EXPORT char      *SavedDocumentURL;/* URL of the document that contained
					   the elements that are now in the
					   Cut and Paste buffer */
THOT_EXPORT char      UserMimeType[MAX_LENGTH];
                                        /* Used to pass the user's MIME type 
					   choice when doing a Save As of a
					   local object to a server */
THOT_EXPORT char      UserCharset[MAX_LENGTH];
                                        /* Used to pass the user's charset
					   choice when doing a Save As of a
					   local object to a server */
THOT_EXPORT int        Lg_password;
THOT_EXPORT int        BaseDialog;
THOT_EXPORT int        BasePrint;
THOT_EXPORT int        BaseImage;
THOT_EXPORT int	       MathsDialogue;
#ifdef GRAPHML
THOT_EXPORT int	       GraphDialogue;
#endif /* GRAPHML */
THOT_EXPORT int	       BaseCSS;
THOT_EXPORT int        ReturnOption;
THOT_EXPORT int        NumberRows;
THOT_EXPORT int        NumberCols;
THOT_EXPORT int        TBorder;
THOT_EXPORT int        ReturnOptionMenu;
THOT_EXPORT int        IdDoc;
THOT_EXPORT Document   CurrentDocument;
THOT_EXPORT Document   SavingDocument;
THOT_EXPORT Document   SavingObject;
THOT_EXPORT Document   AttrHREFdocument;
THOT_EXPORT Document   DocBook;
THOT_EXPORT Document   IncludedDocument;
THOT_EXPORT Element    AttrHREFelement;
THOT_EXPORT char      *AttrHREFvalue;
THOT_EXPORT Document   SelectionDoc;
THOT_EXPORT ThotBool  IsNewAnchor;
THOT_EXPORT ThotBool  UseLastTarget;
THOT_EXPORT ThotBool  LinkAsCSS;
THOT_EXPORT ThotBool   SaveAsHTML;
THOT_EXPORT ThotBool   SaveAsXML;
THOT_EXPORT ThotBool   SaveAsText;
THOT_EXPORT ThotBool   CopyImages;	/* should we copy images in Save As */
THOT_EXPORT ThotBool   UpdateURLs;	/* should we update URLs in Save As */
THOT_EXPORT ThotBool   UserAnswer;
THOT_EXPORT ThotBool   InNewWindow;
THOT_EXPORT ThotBool   SelectionInPRE;
THOT_EXPORT ThotBool   SelectionInComment;
THOT_EXPORT ThotBool   SelectionInEM;
THOT_EXPORT ThotBool   SelectionInSTRONG;
THOT_EXPORT ThotBool   SelectionInCITE;
THOT_EXPORT ThotBool   SelectionInABBR;
THOT_EXPORT ThotBool   SelectionInACRONYM;
THOT_EXPORT ThotBool   SelectionInINS;
THOT_EXPORT ThotBool   SelectionInDEL;
THOT_EXPORT ThotBool   SelectionInDFN;
THOT_EXPORT ThotBool   SelectionInCODE;
THOT_EXPORT ThotBool   SelectionInVAR;
THOT_EXPORT ThotBool   SelectionInSAMP;
THOT_EXPORT ThotBool   SelectionInKBD;
THOT_EXPORT ThotBool   SelectionInI;
THOT_EXPORT ThotBool   SelectionInB;
THOT_EXPORT ThotBool   SelectionInTT;
THOT_EXPORT ThotBool   SelectionInBIG;
THOT_EXPORT ThotBool   SelectionInSMALL;
THOT_EXPORT ThotBool   SelectionInSub;
THOT_EXPORT ThotBool   SelectionInSup;
THOT_EXPORT ThotBool   SelectionInQuote;
THOT_EXPORT ThotBool   SelectionInBDO;
THOT_EXPORT ThotBool   HTMLErrorsFound;
THOT_EXPORT ThotBool   XMLErrorsFound;
THOT_EXPORT ThotBool   CSSErrorsFound;
THOT_EXPORT ThotBool   XMLErrorsFoundInProfile;
THOT_EXPORT ThotBool   XMLNotWellFormed;
THOT_EXPORT ThotBool   XMLCharacterNotSupported;
THOT_EXPORT ThotBool   XMLUnknownEncoding;

THOT_EXPORT ThotBool   IdApplyToSelection; /* used in the Make ID menu */
THOT_EXPORT FILE      *ErrFile;
typedef enum
{
  docHTML,
  docText,
  docImage,
  docCSS,
  docSource,
  docAnnot,
  docLog,
  docSVG,
  docMath,
  docXml
} DocumentType;


/* a record for data associated with a request */
typedef struct _DocumentMetaDataElement
{
  char    *initial_url;      /* if the server returns a different URL name
				after a redirection, we store here the one
				that the browser first asked */
  char    *form_data;        /* form data associated with a URL */
  char    *content_type;     /* content type returned by the server */
  char    *charset;          /* charset returned by the server */
  char    *content_length;   /* content length returned by the server */
  ClickEvent method;           /* method used to send this data */
  ThotBool   put_default_name; /* URL name was concatenated with DEFAULT_NAME*/
  ThotBool   xmlformat;        /* the document should be exported in xml format */
  
#ifdef ANNOTATIONS
  Document   source_doc;       /* if the document is an annotation,
				  this variable giveso the annoted document
				  docid */
#endif /* ANNOTATIONS */
} DocumentMetaDataElement;


#define DocumentTableLength MAX_DOCUMENTS
/* URL of each loaded document */
THOT_EXPORT char                    *DocumentURLs[DocumentTableLength];
/* Any formdata associated with a URL */
THOT_EXPORT DocumentMetaDataElement *DocumentMeta[DocumentTableLength];
/* Type of document */
THOT_EXPORT DocumentType             DocumentTypes[DocumentTableLength];
/* Document is in read only mode */
THOT_EXPORT ThotBool                 ReadOnlyDocument[DocumentTableLength];
/* identifier of the document displaying the source code */
THOT_EXPORT Document                 DocumentSource[DocumentTableLength];
/* The whole document is loaded when the corresponding entry in FilesLoading is 0 */
THOT_EXPORT int                      FilesLoading[DocumentTableLength];
/* Status (error, success) of the download of the objects of a document */
THOT_EXPORT int                      DocNetworkStatus[DocumentTableLength];
/* Status of the XHTML document (basic, strict, transitional) */
THOT_EXPORT int                      ParsingLevel[DocumentTableLength];

THOT_EXPORT Document                 W3Loading; /* the document being loaded */
THOT_EXPORT Document                 BackupDocument; /* the current backup */

/* button indexes */
THOT_EXPORT int iStop;
THOT_EXPORT int iBack;
THOT_EXPORT int iForward;
THOT_EXPORT int iReload;
THOT_EXPORT int iHome;
THOT_EXPORT int iEditor;
THOT_EXPORT int iSave;
THOT_EXPORT int iPrint;
THOT_EXPORT int iFind;
THOT_EXPORT int iI;
THOT_EXPORT int iB;
THOT_EXPORT int iT;
THOT_EXPORT int iImage;
THOT_EXPORT int iH1;
THOT_EXPORT int iH2;
THOT_EXPORT int iH3;
THOT_EXPORT int iBullet;
THOT_EXPORT int iNum;
THOT_EXPORT int iDL;
THOT_EXPORT int iLink;
THOT_EXPORT int iTable;

#define IMAGE_NOT_LOADED        0
#define IMAGE_LOCAL		1
#define IMAGE_LOADED		2
#define IMAGE_MODIFIED		3


typedef void (*LoadedImageCallback)(Document doc, Element el, char *file, void *extra);
typedef struct _ElemImage
  {
     Element             currentElement;/* first element using this image */
     struct _ElemImage  *nextElement;
     LoadedImageCallback callback;	/* Callback for non-standard handling */
     void		*extra;		/* any extra info for the CallBack */
  }
ElemImage;

typedef struct _LoadedImageDesc
  {
     char               *originalName;  /* complete URL of the image                */
     char               *localName;     /* local name (without path) of the image   */
     char               *content_type;  /* the MIME type as sent by the server      */
     struct _LoadedImageDesc *prevImage;/* double linked list                       */
     struct _LoadedImageDesc *nextImage;/* easier to unchain                        */
     Document            document;	/* document concerned                       */
     struct _ElemImage  *elImage;	/* first element using this image           */
     int                 imageType;     /* the type of the image                    */
     int                 status;	/* the status of the image loading          */
  }
LoadedImageDesc;

/* the structure used for storing the context of the 
   FetchAndDisplayImages_callback function */
typedef struct _FetchImage_context {
  char               *base_url;
  LoadedImageDesc    *desc;
} FetchImage_context;


THOT_EXPORT LoadedImageDesc *ImageURLs;
THOT_EXPORT LoadedImageDesc *ImageLocal;

/* The default Amaya HOME pages (page shown at boot time */
#define AMAYA_PAGE  "AmayaPage.html"
#define AMAYA_PAGE_DOC  "http://www.w3.org/Amaya/User/"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024	/* Max. length of strings */
#endif  /* MAX_TXT_LEN */

#endif /* AMAYA_H */
