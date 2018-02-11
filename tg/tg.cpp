/* ******************************************************************** **
** @@ TG
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Directory HTML Table Generator
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include <stdafx.h>

#include <stdio.h>
#include <time.h>

#include "..\shared\file_find.h"
#include "..\shared\text.h"
#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

const char* ICONS[] =
{
   "none.png",
   "default.png",
   "dos16.png",
   "dos32.png",
   "win16.png",
   "win32.png",
   "mac.png",
   "x.png"
};

enum  ICON_TYPE
{
   IT_NONE,
   IT_DEFAULT, 
   IT_DOS16, 
   IT_DOS32, 
   IT_WIN16, 
   IT_WIN32, 
   IT_MAC, 
   IT_X
};

struct ListEntry
{
   ICON_TYPE      _Icon;
   DWORD          _dwFileSizeLo;
   FILETIME       _FileTime;
   char           _pszShortName[MAX_PATH + 1];
   char           _pszFullName [MAX_PATH + 1];
   char           _pszVers     [MAX_PATH + 1];
   char           _pszInfo     [(MAX_PATH << 4) + 1];
};

#define  INITIAL_SIZE         (256)
#define  INITIAL_DELTA        (32)

#define  TAG_SIZE             (5)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD   dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

static SortedVector*    _pList = NULL;

static char             _pszPrefix[MAX_PATH + 1];

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ CompareNamesShort()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int CompareNamesShort(const void** const pKey1,const void** const pKey2)
{
   ListEntry**     p1 = (ListEntry**)pKey1;
   ListEntry**     p2 = (ListEntry**)pKey2;
      
   return stricmp((*p1)->_pszShortName,(*p2)->_pszShortName);
}

/* ******************************************************************** **
** @@ CompareNamesFull()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int CompareNamesFull(const void** const pKey1,const void** const pKey2)
{
   ListEntry**     p1 = (ListEntry**)pKey1;
   ListEntry**     p2 = (ListEntry**)pKey2;
      
   // Reverse order sort
   return -stricmp((*p1)->_pszFullName,(*p2)->_pszFullName);
}

/* ******************************************************************** **
** @@ GetIconType()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static ICON_TYPE GetIconType(const char* const pszText)
{
   if (!stricmp(pszText,"DEFAULT"))
   {
      return IT_DEFAULT;
   }
   else if (!stricmp(pszText,"DOS16"))
   {
      return IT_DOS16;
   }
   else if (!stricmp(pszText,"DOS32"))
   {
      return IT_DOS32;
   }
   else if (!stricmp(pszText,"WIN16"))
   {
      return IT_WIN16;
   }
   else if (!stricmp(pszText,"WIN32"))
   {
      return IT_WIN32;
   }
   else if (!stricmp(pszText,"MAC"))
   {
      return IT_MAC;
   }
   else if (!stricmp(pszText,"X"))
   {
      return IT_X;
   }
   else
   {
      return IT_NONE;
   }
}

/* ******************************************************************** **
** @@ Text_2_Html()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Text_2_Html(const char* const pInBuf,char* pOutBuf)
{
   int      iSize = strlen(pInBuf);

   int   iPos = 0;

   for (int ii = 0; ii < iSize; ++ii)
   {
      switch ((BYTE)pInBuf[ii])
      {
         case 0x22:
         {
            strcpy(&pOutBuf[iPos],"&quot;");
            iPos += 6;
            break;
         }
         case 0x26:
         {
            strcpy(&pOutBuf[iPos],"&amp;");
            iPos += 5;
            break;
         }
         case 0x3C:
         {
            strcpy(&pOutBuf[iPos],"&lt;");
            iPos += 4;
            break;
         }
         case 0x3E:
         {
            strcpy(&pOutBuf[iPos],"&gt;");
            iPos += 4;
            break;
         }
         case 0xA0:
         {
            strcpy(&pOutBuf[iPos],"&nbsp;");
            iPos += 6;
            break;
         }
         case 0xA1:
         {
            strcpy(&pOutBuf[iPos],"&iexcl;");
            iPos += 7;
            break;
         }
         case 0xA2:
         {
            strcpy(&pOutBuf[iPos],"&cent;");
            iPos += 6;
            break;
         }
         case 0xA3:
         {
            strcpy(&pOutBuf[iPos],"&pound;");
            iPos += 7;
            break;
         }
         case 0xA4:
         {
            strcpy(&pOutBuf[iPos],"&curren;");
            iPos += 8;
            break;
         }
         case 0xA5:
         {
            strcpy(&pOutBuf[iPos],"&yen;");
            iPos += 5;
            break;
         }
         case 0xA6:
         {
            strcpy(&pOutBuf[iPos],"&brvbar;");
            iPos += 8;
            break;
         }
         case 0xA7:
         {
            strcpy(&pOutBuf[iPos],"&sect;");
            iPos += 6;
            break;
         }
         case 0xA8:
         {
            strcpy(&pOutBuf[iPos],"&uml;");
            iPos += 5;
            break;
         }
         case 0xA9:
         {
            strcpy(&pOutBuf[iPos],"&copy;");
            iPos += 6;
            break;
         }
         case 0xAA:
         {
            strcpy(&pOutBuf[iPos],"&ordf;");
            iPos += 6;
            break;
         }
         case 0xAB:
         {
            strcpy(&pOutBuf[iPos],"&laquo;");
            iPos += 7;
            break;
         }
         case 0xAC:
         {
            strcpy(&pOutBuf[iPos],"&not;");
            iPos += 5;
            break;
         }
         case 0xAD:
         {
            strcpy(&pOutBuf[iPos],"&shy;");
            iPos += 5;
            break;
         }
         case 0xAE:
         {
            strcpy(&pOutBuf[iPos],"&reg;");
            iPos += 5;
            break;
         }
         case 0xAF:
         {
            strcpy(&pOutBuf[iPos],"&macr;");
            iPos += 6;
            break;
         }
         case 0xB0:
         {
            strcpy(&pOutBuf[iPos],"&deg;");
            iPos += 5;
            break;
         }
         case 0xB1:
         {
            strcpy(&pOutBuf[iPos],"&plusmn;");
            iPos += 8;
            break;
         }
         case 0xB2:
         {
            strcpy(&pOutBuf[iPos],"&sup2;");
            iPos += 6;
            break;
         }
         case 0xB3:
         {
            strcpy(&pOutBuf[iPos],"&sup3;");
            iPos += 6;
            break;
         }
         case 0xB4:
         {
            strcpy(&pOutBuf[iPos],"&acute;");
            iPos += 7;
            break;
         }
         case 0xB5:
         {
            strcpy(&pOutBuf[iPos],"&micro;");
            iPos += 7;
            break;
         }
         case 0xB6:
         {
            strcpy(&pOutBuf[iPos],"&para;");
            iPos += 6;
            break;
         }
         case 0xB7:
         {
            strcpy(&pOutBuf[iPos],"&middot;");
            iPos += 8;
            break;
         }
         case 0xB8:
         {
            strcpy(&pOutBuf[iPos],"&cedil;");
            iPos += 7;
            break;
         }
         case 0xB9:
         {
            strcpy(&pOutBuf[iPos],"&sup1;");
            iPos += 6;
            break;
         }
         case 0xBA:
         {
            strcpy(&pOutBuf[iPos],"&ordm;");
            iPos += 6;
            break;
         }
         case 0xBB:
         {
            strcpy(&pOutBuf[iPos],"&raquo;");
            iPos += 7;
            break;
         }
         case 0xBC:
         {
            strcpy(&pOutBuf[iPos],"&frac14;");
            iPos += 8;
            break;
         }
         case 0xBD:
         {
            strcpy(&pOutBuf[iPos],"&frac12;");
            iPos += 8;
            break;
         }
         case 0xBE:
         {
            strcpy(&pOutBuf[iPos],"&frac34;");
            iPos += 8;
            break;
         }
         case 0xBF:
         {
            strcpy(&pOutBuf[iPos],"&iquest;");
            iPos += 8;
            break;
         }
         default:
         {
            pOutBuf[iPos++] = pInBuf[ii];
            break;
         }
      }
   }

   pOutBuf[iPos] = 0;   // ASCIIZ
}

/* ******************************************************************** **
** @@ ParseInfoFile()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ParseInfoFile(ListEntry* pEntry,const char* const pszName)
{
   char     pszFile[MAX_PATH + 1];

   memset(pszFile,0,sizeof(pszFile));

   if (strlen(_pszPrefix))
   {
      strcpy(pszFile,_pszPrefix);
      strcat(pszFile,"\\");
   }

   strcat(pszFile,pszName);

   FILE*    pIn = fopen(pszFile,"rt"); 
   
   if (!pIn)  
   {
      // Error !
      return;
   }    

   char     pszLine[MAX_PATH + 1];

   while (fgets(pszLine,MAX_PATH,pIn))
   {
      pszLine[MAX_PATH] = 0; // ASCIIZ

      DWORD    dwEOL = strcspn((char*)pszLine,"\r\n");

      pszLine[dwEOL] = 0;  // Remove EOL chars

      if (!strlen(pszLine) || (*pszLine == ';'))
      {
         // Empty string or comment line
         continue;
      }

      char     pszNaked[MAX_PATH + 1];

      memset(pszNaked,0,sizeof(pszNaked));

      Unspace(pszLine,pszNaked);

      if (!strnicmp(pszNaked,"ICON=",TAG_SIZE))
      {
         pEntry->_Icon = GetIconType(pszNaked + TAG_SIZE);
      }
      else if (!strnicmp(pszNaked,"NAME=",TAG_SIZE))
      {
         Text_2_Html(pszNaked + TAG_SIZE,pEntry->_pszFullName);
      }
      else if (!strnicmp(pszNaked,"VERS=",TAG_SIZE))
      {
         Text_2_Html(pszNaked + TAG_SIZE,pEntry->_pszVers);
      } 
      else if (!strnicmp(pszNaked,"INFO=",TAG_SIZE))
      {
         Text_2_Html(pszNaked + TAG_SIZE,pEntry->_pszInfo);
      }
      else
      {
         // Some Junk may be inserted
         // Do nothing !
      }
   }
}

/* ******************************************************************** **
** @@ ForEach()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ForEach(const WIN32_FIND_DATA& w32fd)
{
   char*    pDot = strrchr(w32fd.cFileName,'.');

   if (!pDot)
   {
      // Error !
      // No Extension
      return;
   }

   bool     bInfoFile = (stricmp(pDot + 1,"i") == 0);

   // Check exists
   ListEntry      Dummy;

   memset(&Dummy,0,sizeof(ListEntry));

   strcpy(Dummy._pszShortName,w32fd.cFileName);

   if (bInfoFile)
   {
      // Remove Info Extension
      char*    pExtDot = strrchr(Dummy._pszShortName,'.');

      if (!pExtDot)
      {
         // Error !
         return;
      }

      *pExtDot = 0;  // Cut Here
   }
   
   bool     bNewRecord = false;

   ListEntry*     pEntry = (ListEntry*)_pList->Search(&Dummy);

   if (!pEntry)
   {
      // Create NEW !
      pEntry = new ListEntry;

      if (!pEntry)
      {
         return;
      }

      memset(pEntry,0,sizeof(ListEntry));

      bNewRecord = true;
   }

   if (!bInfoFile)
   {
      // Ordinal file
      strcpy(pEntry->_pszShortName,w32fd.cFileName);

      pEntry->_dwFileSizeLo = w32fd.nFileSizeLow;
      pEntry->_FileTime     = w32fd.ftCreationTime;
   }
   else
   {
      ParseInfoFile(pEntry,w32fd.cFileName);
   }

   if (bNewRecord)
   {
      if (_pList->Insert(pEntry) == -1)
      {
         delete pEntry;
         pEntry = NULL;
      }
   }
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ShowHelp()
{
   const char  pszCopyright[] = "-*-   TG 1.0   *   (c) Gazlan, 2011   -*-";
   const char  pszDescript [] = "Directory HTML Table Generator";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: tg.exe [FullPath]\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{
   if (argc > 2)
   {
      ShowHelp();
      return 0;
   }

   memset(_pszPrefix,0,sizeof(_pszPrefix));

   FindFile*   pFF = new FindFile;

   if (!pFF)
   {
      // Error !
      return -1;
   }

   if (argc == 2)
   {
      if ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h")))
      {
         ShowHelp();
         return 0;
      }

      pFF->SetPrefix(argv[1]);
   }

   strcpy(_pszPrefix,pFF->_pszPrefix);

   pFF->SetMask("*.*");

   _pList = new SortedVector;

   if (!_pList)
   {
      // Error !
      delete pFF;
      pFF = NULL;
      return -1;
   }

   _pList->Resize(INITIAL_SIZE);
   _pList->Delta(INITIAL_DELTA);
   _pList->SetSorter(CompareNamesShort);

   while (pFF->Fetch())
   {
      if ((pFF->_w32fd.dwFileAttributes | FILE_ATTRIBUTE_NORMAL) && !(pFF->_w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         ForEach(pFF->_w32fd);
      }
   }

   delete pFF;
   pFF = NULL;

   _pList->SetSorter(CompareNamesFull);
   _pList->QSort();

   int iCnt = _pList->Count();

   FILE*    pOut = fopen("Index.html","wt");

   fprintf(pOut,"<html>\n");
   fprintf(pOut,"<head>\n");
   fprintf(pOut,"<meta name=\"generator\" content=\"Directory HTML Table Generator 1.0\">\n");
   fprintf(pOut,"<meta http-equiv=\"content-type\" content=\"text/html; charset=windows-1251\">\n");
   fprintf(pOut,"</head>\n");
   fprintf(pOut,"<body>\n");
   fprintf(pOut,"<table border=0 cellpadding=5 cellspacing=5 width=90% align=\"left\">\n");

   for (int ii = iCnt - 1; ii >= 0; --ii)
   {
      ListEntry*     pEntry = (ListEntry*)_pList->At(ii);

      fprintf(pOut,"<tr>\n");
      fprintf(pOut,"   <td align=\"center\"><a href=\"%s\"><img src=\"../design/icons/%s\" width=32 height=32 border=0></a></td>\n",pEntry->_pszShortName,ICONS[pEntry->_Icon]);
      fprintf(pOut,"   <td align=\"left\"><a href=\"%s\"><strong>%s</strong></a></td>\n",pEntry->_pszShortName,pEntry->_pszShortName);
      fprintf(pOut,"   <td align=\"right\"><strong>%s</strong></td>\n",*pEntry->_pszVers  ?  pEntry->_pszVers  :  "&nbsp;");
      fprintf(pOut,"   <td align=\"right\"><em>%12d</em></td>\n",pEntry->_dwFileSizeLo);
//      fprintf(pOut,"   <td>%08X</td>\n",pEntry->_FileTime);
      fprintf(pOut,"   <td align=\"left\"><small><strong>%s</strong><br>%s</small></td>\n",pEntry->_pszFullName,*pEntry->_pszInfo  ?  pEntry->_pszInfo  :  "&nbsp;");

      fprintf(pOut,"</tr>\n");

      delete pEntry;
      pEntry = NULL;

      _pList->RemoveAt(ii);
   }

   fprintf(pOut,"</table>\n");
   fprintf(pOut,"<!-- Created by Directory HTML Table Generator 1.0 * (c) gazlan@yandex.ru 2011 //-->\n");
   fprintf(pOut,"</body>\n");
   fprintf(pOut,"</html>\n");

   delete _pList;
   _pList = NULL;

   fclose(pOut);
   pOut = NULL;

   return 0;
}
