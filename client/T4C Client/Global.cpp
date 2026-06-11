// Global.cpp: implementation of the Global class.
//
//////////////////////////////////////////////////////////////////////
#include "Global.h"
#include "App.h"
#include <stdio.h>
#include <shlobj.h>
#include "TileSet.h"
#include "TFCPlayer.h"
#include "..\Zlib/zlib.h"

#include "SaveGame.h"
extern CSaveGame g_SaveGame; 
extern TFCPlayer Player;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define RGBNM(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
 

#define ZONE_MAP_X   3072
#define ZONE_MAP_Y   3072
#define ZONE_VIEW_X  3072//512
#define ZONE_VIEW_Y  3072//512
#define ZONE_VIEW_X2 (ZONE_VIEW_X/2)
#define ZONE_VIEW_Y2 (ZONE_VIEW_Y/2)

const DWORD g_GUI_FILE_VER = 0x00001000;

unsigned char ZONE_key[]={ 0xE1 ,0xC4 ,0x1B ,0xC0 ,0xC5 ,0x0D ,0x58 ,0xB7,
                           0x01 ,0xCE ,0x33 ,0xD3 ,0xA9 ,0x3F ,0xBA ,0x99 };

HANDLE g_SaveGUIThreadHandle = NULL;
BOOL   g_bSaveGUIChanged     = FALSE;



Global g_Global;

Global::Global()
{

   m_crBtnEText = RGBNM(223, 157, 0);
   m_crBtnDText = RGBNM(150, 90, 0);

   m_dwAlphaValue       = 200;
   m_dwLastScrollStatus = 0;

   m_ZoneMapInfo[0].bEnable = false;
   m_ZoneMapInfo[1].bEnable = false;
   m_ZoneMapInfo[2].bEnable = false;
   m_ZoneMapInfo[3].bEnable = false;
   m_ZoneMapInfo[4].bEnable = false;
   m_ZoneMapInfo[5].bEnable = false;
   m_ZoneMapInfo[6].bEnable = false;
   m_ZoneMapInfo[7].bEnable = false;
   m_dwCurMapsZone = -1;
   m_dwCurPosZone  = -1;
   m_dwChangeX     = -1;
   m_dwChangeY     = -1;
   //m_dwLoadZoneXS  = -1;
   //m_dwLoadZoneYS  = -1;
   m_bCanDisplayChange  = true;
   m_bDisplayChangeZone = false;
   m_bForceDisplayZone  = false;

   for(int n=0;n<NBR_ZONE_MAP;n++)
   {
      for(int i=0;i<256;i++)
      {
         m_ZoneInfo[n][i].strZoneName[0] = 0;
         m_ZoneInfo[n][i].bEnable = false;
      }
   }

   m_bConfigRead = false;

   ReadClientConfig();
   
   m_pchZoneMaps = new unsigned char[ZONE_MAP_X*ZONE_MAP_Y];

   


   m_pItemInfo = new sItemInfo;
   m_pItemInfo->Name       = NULL;
   m_pItemInfo->vBoust.clear();
   m_pItemInfo->bSet       = false;

   m_dwLastWorldX = 0;
   m_dwLastWorldY = 0;



   m_iDialoglastXPos             = -1;
   m_iDialoglastYPos             = -1;
   m_iDialogChatlastXPos         = -1;
   m_iDialogChatlastYPos         = -1;
   m_iDialogGMlastXPos           = -1;
   m_iDialogGMlastYPos           = -1;
   m_iDialogArene1WlastXPos      = -1;
   m_iDialogArene1WlastYPos      = -1;
   m_iDialogArene1PlastXPos      = -1;
   m_iDialogArene1PlastYPos      = -1;
   m_iDialogVollastXPos          = -1;
   m_iDialogVollastYPos          = -1;
   m_iDialogAHlastXPos           = -1;
   m_iDialogAHlastYPos           = -1;
   m_iDialogIFlastXPos           = -1;
   m_iDialogIFlastYPos           = -1;
   m_iDialogChestNewlastXPos     = -1;
   m_iDialogChestNewlastYPos     = -1;
   m_iDialogIPVPlastXPos         = -1;
   m_iDialogIPVPlastYPos         = -1;
                                 
   m_iDialoglastMainBarXPos      = -1;
   m_iDialoglastMainBarYPos      = -1;
   m_iDialoglastTMIXPos          = -1;
   m_iDialoglastTMIYPos          = -1;
   m_iDialoglastLifeXPos         = -1;
   m_iDialoglastLifeYPos         = -1;
   m_iDialoglastGroupOSXPos      = -1;
   m_iDialoglastGroupOSYPos      = -1;
   m_iDialoglastChatLogXPos      = -1;
   m_iDialoglastChatLogYPos      = -1;
   m_iDialoglastChatLogH         = -1;
   m_iDialoglastChatLogW         = -1;
   m_iDialoglastChatLogPinned    = -1;
   m_iLastDialogOpenned          = -1;
   m_iDialoglastTargetXPos       = -1;
   m_iDialoglastTargetYPos       = -1;
   m_iDialoglastMP3XPos          = -1;
   m_iDialoglastMP3YPos          = -1;
   m_iDialoglastArenePointXPos   = -1;
   m_iDialoglastArenePointYPos   = -1;
   m_iDialoglastStatusXPos       = -1;
   m_iDialoglastStatusYPos       = -1;
   m_iDialoglastStatusPinned     = -1;
   m_iDialoglastStatusNbrEffect  = -1;
   m_iDialogChestlastXPos        = -1;
   m_iDialogChestlastYPos        = -1;
   

   m_bDraggingDialog          = false;
   
   //LoadGUI();


   if(!g_SaveGUIThreadHandle)
   {
      unsigned int threadId = 0;
      g_SaveGUIThreadHandle = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, SaveGUIThread, NULL, 0, &threadId ));
   }

   InitializeCriticalSection(&csInternal);
}

Global::~Global()
{
   if(m_pchZoneMaps)
      delete []m_pchZoneMaps;
   m_pchZoneMaps = NULL;

   if(m_pItemInfo->Name)
      delete []m_pItemInfo->Name;
   m_pItemInfo->Name = NULL;
   m_pItemInfo->vBoust.clear();
   m_pItemInfo->bSet = false;
 
   delete m_pItemInfo;
   WriteClientConfig();
   DeleteCriticalSection(&csInternal);
}

void Global::InitZoneFile()
{
   ClientConfig *pConfig = g_Global.GetClientConfig();
   sprintf_s(m_strPath,512,"%s",pConfig->strPath);
   sprintf_s(m_ZoneFile,512,"%s%s",m_strPath,"\\Game Files\\Zone_Map.dat");
   sprintf_s(m_ZoneFileC,512,"%s\\%s%s",m_strPath,Player.ServerName,"\\Game Files\\Zone_Map.dat");


   //Load le fichier de zonemap....  ZoneMapInfo.zfn et affecte les bonne zone...
   char strZoneFileName[1024];
   char strZoneFileNameC[1024];
   sprintf_s(strZoneFileName,1024,"%s%s",m_strPath,"\\Game Files\\ZoneMapInfo.zfn");
   sprintf_s(strZoneFileNameC,1024,"%s\\%s%s",m_strPath,Player.ServerName,"\\Game Files\\ZoneMapInfo.zfn");
   FILE *pf = NULL;
   fopen_s(&pf,strZoneFileNameC,"rt");
   if(pf == NULL)
      fopen_s(&pf,strZoneFileName,"rt");
   if(pf)
   {
      char *pstrRead;
      char strLine[1024];
      do
      {
         pstrRead = fgets(strLine,1024,pf);
         if(pstrRead)
         {
            if(pstrRead[strlen(pstrRead)-1] == '\n')
               pstrRead[strlen(pstrRead)-1] = NULL; //delete \n

            for(unsigned int i=0;i<strlen(pstrRead);i++)
            {
               pstrRead[i] ^= ZONE_key[i%16];
            }


            int iW,iIdx;

            iW  = atoi(pstrRead);pstrRead+=4;
            iIdx= atoi(pstrRead);pstrRead+=4;

            if(iW>=0   && iW <=7      &&
               iIdx>=0 && iIdx <= 255 && 
               strlen(pstrRead) < 256     )
            {
               sprintf_s(m_ZoneInfo[iW][iIdx].strZoneName,256,"%s",pstrRead);
               m_ZoneInfo[iW][iIdx].bEnable = true;
               m_ZoneMapInfo[iW].bEnable = true;
            }
         }
      }while(pstrRead);

      fclose(pf);
   }
}

void Global::SetDisplaySize(int dwW, int dwH)
{
   m_dwDisplayW     = dwW;
   m_dwDisplayH     = dwH;
   m_dwDisplayWDiv2 = m_dwDisplayW/2;
   m_dwDisplayHDiv2 = m_dwDisplayH/2;

   m_dwNbrTileX     = m_dwDisplayW/32;
   m_dwNbrTileY     = m_dwDisplayH/16;

   if(m_dwNbrTileY%2)
      m_dwNbrTileYDiv2 = m_dwNbrTileY/2+1;
   else
      m_dwNbrTileYDiv2 = m_dwNbrTileY/2;

   if(m_dwNbrTileX%2)
      m_dwNbrTileXDiv2 = m_dwNbrTileX/2+1;
   else
      m_dwNbrTileXDiv2 = m_dwNbrTileX/2;

   

   



   m_dwScreenW      = m_dwNbrTileX*32;
   m_dwScreenH      = m_dwNbrTileY*16;
   m_dwScreenWDiv2  = m_dwScreenW/2;
   m_dwScreenHDiv2  = m_dwScreenH/2;
}


void Global::LoadZoneMapWorld(int dwWorld, int dwX, int dwY)
{
   if(dwWorld <0 ||  dwWorld >NBR_ZONE_MAP-1)
      dwWorld = 0;

   if(dwWorld == m_dwCurMapsZone)
      return;
   m_dwCurMapsZone = dwWorld;
   
   /*
   int dwX1 = dwX;
   int dwY1 = dwY;

   if(dwX1 >= ZONE_MAP_X)
      dwX1 = ZONE_MAP_X-1;

   if(dwY1 >= ZONE_MAP_Y)
      dwY1 = ZONE_MAP_Y-1;

   if( (dwWorld == m_dwCurMapsZone) && 
       (dwX >= m_dwLoadZoneXS && dwX1 <m_dwLoadZoneXS+ZONE_VIEW_X && dwY >= m_dwLoadZoneYS && dwY1 <m_dwLoadZoneYS+ZONE_VIEW_Y ))
      return;
   
   //if(dwWorld == m_dwCurMapsZone)
   //   return;

   m_dwCurMapsZone = dwWorld;
   m_dwLoadZoneXS  = dwX-ZONE_VIEW_X2;
   m_dwLoadZoneYS  = dwY-ZONE_VIEW_Y2;

   if(m_dwLoadZoneXS <0)
      m_dwLoadZoneXS = 0;
   else if(m_dwLoadZoneXS > ZONE_MAP_X-ZONE_VIEW_X)
      m_dwLoadZoneXS = ZONE_MAP_X-ZONE_VIEW_X;

   if(m_dwLoadZoneYS <0)
      m_dwLoadZoneYS = 0;
   else if(m_dwLoadZoneYS > ZONE_MAP_Y-ZONE_VIEW_Y)
      m_dwLoadZoneYS = ZONE_MAP_Y-ZONE_VIEW_Y;
   */

   FILE *pf1 = NULL;
   fopen_s(&pf1,m_ZoneFileC,"rb");
   if(pf1 == NULL)
      fopen_s(&pf1,m_ZoneFile,"rb");

   if(!pf1)
   {
      memset(m_pchZoneMaps,0xFF,ZONE_VIEW_X*ZONE_VIEW_Y);
   }
   else
   {

      ULONG dwTailleOri;
      ULONG dwTailleComp[NBR_ZONE_MAP];
      ULONG dwFilePos[NBR_ZONE_MAP];

      fread(&dwTailleOri,4,1,pf1);
      fread(&dwTailleComp[0],4,1,pf1);
      fread(&dwTailleComp[1],4,1,pf1);
      fread(&dwTailleComp[2],4,1,pf1);
      fread(&dwTailleComp[3],4,1,pf1);
      fread(&dwTailleComp[4],4,1,pf1);
      fread(&dwTailleComp[5],4,1,pf1);
      fread(&dwTailleComp[6],4,1,pf1);
      fread(&dwTailleComp[7],4,1,pf1);
      fread(&dwFilePos[0],4,1,pf1);
      fread(&dwFilePos[1],4,1,pf1);
      fread(&dwFilePos[2],4,1,pf1);
      fread(&dwFilePos[3],4,1,pf1);
      fread(&dwFilePos[4],4,1,pf1);
      fread(&dwFilePos[5],4,1,pf1);
      fread(&dwFilePos[6],4,1,pf1);
      fread(&dwFilePos[7],4,1,pf1);

      fseek( pf1, dwFilePos[dwWorld], SEEK_SET);

      BYTE *pFileDataTmpC = NULL;
      pFileDataTmpC= new BYTE[dwTailleComp[dwWorld]];
      fread(pFileDataTmpC,1,dwTailleComp[dwWorld],pf1);
      fclose(pf1);

      BYTE *pFileDataTmp = NULL;
      pFileDataTmp = new BYTE[dwTailleOri];

      //BYTE *pTMIPal = new BYTE[256*3];
      if(uncompress((BYTE*)pFileDataTmp,&dwTailleOri,(BYTE*)pFileDataTmpC,dwTailleComp[dwWorld]) != Z_OK)
      {
         memset(m_pchZoneMaps,0xFF,ZONE_MAP_X*ZONE_MAP_Y);
      }
      else
      {
         BYTE *WPosPal = pFileDataTmp+(ZONE_MAP_X*ZONE_MAP_Y);

         memcpy(m_pchZoneMaps,pFileDataTmp,ZONE_MAP_X*ZONE_MAP_Y);
         //memcpy(pTMIPal,WPosPal,256*3);
      }

      
      //delete []pTMIPal;
      //pTMIPal = NULL;
      delete []pFileDataTmp;
      pFileDataTmp = NULL;
      delete []pFileDataTmpC;
      pFileDataTmpC = NULL;

      /*
      BYTE *pFileDataTmp = NULL;
      pFileDataTmp = new BYTE[dwTailleOri];

      BYTE *pRTMapD = new BYTE[ZONE_MAP_X*ZONE_MAP_Y];
      
      if(uncompress((BYTE*)pFileDataTmp,&dwTailleOri,(BYTE*)pFileDataTmpC,dwTailleComp[dwWorld]) != Z_OK)
      {
         memset(pRTMapD,0x00,ZONE_MAP_X*ZONE_MAP_Y);
      }

      BYTE *WPosMap = pFileDataTmp;
      memcpy(pRTMapD,WPosMap,ZONE_MAP_X*ZONE_MAP_Y);

      // maintenant on doit extraire la zone qui nous interresse...
      for(int y=0;y<ZONE_VIEW_Y;y++)
      {
         if((y+m_dwLoadZoneYS) < ZONE_MAP_Y)
         {
            int dwOffset = ((y+m_dwLoadZoneYS)*ZONE_MAP_X);
            dwOffset+=m_dwLoadZoneXS;
            BYTE *pS = pRTMapD  +dwOffset;
            BYTE *pD = m_pchZoneMaps +(y*ZONE_VIEW_X);

            memcpy(pD,pS,ZONE_VIEW_X);
         }
         else
         {
            BYTE *pD = m_pchZoneMaps +(y*ZONE_VIEW_X);
            memset(pD,0x00,ZONE_VIEW_X);
         }
      }

      //SaveCurrentImage8(m_pchZoneMaps,ZONE_VIEW_X, ZONE_VIEW_Y,"C:\\!!!OHOH.bmp",FALSE);

      delete []pRTMapD;
      pRTMapD = NULL;
      delete []pFileDataTmp;
      pFileDataTmp = NULL;
      delete []pFileDataTmpC;
      pFileDataTmpC = NULL;
      */
   }




}

int Global::GetDisplayZone()
{
   if(m_bDisplayChangeZone || m_bForceDisplayZone)
   {
      m_bDisplayChangeZone = false;
      m_bForceDisplayZone  = false;
      return m_dwCurPosZone;
   }
   else
      return -1;
}
char* Global::GetDisplayZoneName()
{
   return m_ZoneInfo[m_dwCurMapsZone][m_dwCurPosZone].strZoneName;
}

void Global::ValidMapZonePosition(int dwPosX, int dwPosY)
{
   // non actif ou changement deja en cours...
   if(!m_ZoneMapInfo[m_dwCurMapsZone].bEnable || m_bDisplayChangeZone)
      return;

   // on valide si ya eu un deplacement de aumoins 5 case en x et en y, si la personne reste
   // a 5 case de la zone... ben on affiche pas elle est pas aller assez loin...
   if(m_dwChangeX != -1 && m_dwChangeY != -1 && !m_bCanDisplayChange)
   {
      if((abs(dwPosX -m_dwChangeX) >5) && (abs(dwPosY -m_dwChangeY) >5))
         m_bCanDisplayChange = true;
   }

   // on affiche pas changement don pas la peine aller plus loin...
   if(!m_bCanDisplayChange)
      return;

  
   // 
   if(m_pchZoneMaps[(dwPosY*3072)+dwPosX] != m_dwCurPosZone)
   {
      m_dwCurPosZone = m_pchZoneMaps[(dwPosY*3072)+dwPosX];
      m_dwChangeX    = dwPosX;
      m_dwChangeY    = dwPosY;
      if(m_ZoneInfo[m_dwCurMapsZone][m_dwCurPosZone].bEnable)
      {
         m_bCanDisplayChange  = false;
         m_bDisplayChangeZone = true;
      }
   }

}

void Global::ForceDisplayZone(int dwPosX, int dwPosY)
{
   if(!m_ZoneMapInfo[m_dwCurMapsZone].bEnable)
      return;
   m_dwCurPosZone = m_pchZoneMaps[(dwPosY*3072)+dwPosX];
   //m_dwCurPosZone = m_pchZoneMaps[((dwPosY-m_dwLoadZoneYS)*ZONE_VIEW_X)+(dwPosX-m_dwLoadZoneXS)];
   m_dwChangeX    = dwPosX;
   m_dwChangeY    = dwPosY;
   if(m_ZoneInfo[m_dwCurMapsZone][m_dwCurPosZone].bEnable)
   {
      
      m_bCanDisplayChange  = false;
      m_bDisplayChangeZone = true;
   }
}


int Global::LoadImageDataToBmpFile(char * lpszFileName,unsigned char *pData) 
{
   void   *lpBits; 
   HANDLE hSection = NULL;
   unsigned long  dwOffset = 0;
   
   FILE *file=NULL;
   fopen_s(&file, lpszFileName, "rb");
   if( file ==NULL )
      return NULL;
   
   BITMAPFILEHEADER bmfHeader;
   long nFileLen;
   
	fseek(file,0,SEEK_END);
	nFileLen = ftell(file);
	fseek(file,0,SEEK_SET);
   
   // Read file header
   if (fread((LPSTR)&bmfHeader,1, sizeof(bmfHeader),file) != sizeof(bmfHeader))
      return NULL;
   
   // File type should be 'BM'
   if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
   {
      fclose(file);
      return NULL;
   }
   
   BITMAPINFO *pbmInfo;
   pbmInfo = (BITMAPINFO *)::GlobalAlloc(GMEM_FIXED, 
      sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );
   if (pbmInfo == NULL)
   {
      fclose(file);
      return NULL;
   }
   
   // Read the BITMAPINFO
   fread( pbmInfo, 1,sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256,file );
   
   BITMAPINFO &bmInfo = *pbmInfo ;
   
   HBITMAP hBmp = CreateDIBSection( NULL, pbmInfo, DIB_RGB_COLORS, &lpBits, 
						hSection, dwOffset );
   
   LPBYTE  lpDIBBits;              // Pointer to DIB bits
   int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
   1 << bmInfo.bmiHeader.biBitCount;
   
   if( bmInfo.bmiHeader.biBitCount > 8 )
      lpDIBBits = (LPBYTE)((LPDWORD)(bmInfo.bmiColors + 
      bmInfo.bmiHeader.biClrUsed) + 
      ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
   else
      lpDIBBits = (LPBYTE)(bmInfo.bmiColors + nColors);
   
   if(bmInfo.bmiHeader.biWidth != 3072 || bmInfo.bmiHeader.biHeight != 3072 || bmInfo.bmiHeader.biBitCount != 8)
   {
      ::GlobalFree(pbmInfo);
      //delete []pMapTmp;
      DeleteObject(hBmp);
      fclose(file);
      return 0;
   }
   
   int nOffset = sizeof(BITMAPFILEHEADER) + (lpDIBBits - (LPBYTE)pbmInfo);
   fseek(file,nOffset,SEEK_SET);
   fread((LPSTR)lpBits, 1,nFileLen - nOffset,file);
   
 
   memcpy(pData,lpBits,nFileLen - nOffset);
   ::GlobalFree(pbmInfo);
   //delete []pMapTmp;
   DeleteObject(hBmp);
   fclose(file);
   return 0;
} 

HRESULT Global::LoadImageDataToBmpFile2(LPCTSTR lpszFileName,BYTE **pData,BYTE **pPal,int *pdwWidth,int *pdwHeight, int *pdwImageBitsPixel) 
{
   LPVOID lpBits; 
   HANDLE hSection = NULL;
   DWORD  dwOffset = 0;

   FILE *file = NULL;
   fopen_s(&file,lpszFileName, "rb");
   if( file ==NULL )
      return NULL;

   BITMAPFILEHEADER bmfHeader;
   long nFileLen;

   fseek(file,0,SEEK_END);
   nFileLen = ftell(file);
   fseek(file,0,SEEK_SET);

   // Read file header
   if (fread((LPSTR)&bmfHeader,1, sizeof(bmfHeader),file) != sizeof(bmfHeader))
      return NULL;

   // File type should be 'BM'
   if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
   {
      fclose(file);
      return NULL;
   }

   BITMAPINFO *pbmInfo;
   pbmInfo = (BITMAPINFO *)::GlobalAlloc(GMEM_FIXED, 
      sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );
   if (pbmInfo == NULL)
   {
      fclose(file);
      return NULL;
   }

   // Read the BITMAPINFO
   fread( pbmInfo, 1,sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256,file );

   BITMAPINFO &bmInfo = *pbmInfo ;

   HBITMAP hBmp = CreateDIBSection( NULL, pbmInfo, DIB_RGB_COLORS, &lpBits, 
      hSection, dwOffset );

   LPBYTE  lpDIBBits;              // Pointer to DIB bits
   int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
      1 << bmInfo.bmiHeader.biBitCount;

   if( bmInfo.bmiHeader.biBitCount > 8 )
      lpDIBBits = (LPBYTE)((LPDWORD)(bmInfo.bmiColors + 
      bmInfo.bmiHeader.biClrUsed) + 
      ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
   else
      lpDIBBits = (LPBYTE)(bmInfo.bmiColors + nColors);

   *pdwWidth          = bmInfo.bmiHeader.biWidth;
   *pdwHeight         = bmInfo.bmiHeader.biHeight;
   *pdwImageBitsPixel = (bmInfo.bmiHeader.biBitCount);

   int nOffset = (int)(sizeof(BITMAPFILEHEADER) + (lpDIBBits - (LPBYTE)pbmInfo));
   fseek(file,nOffset,SEEK_SET);
   fread((LPSTR)lpBits, 1,nFileLen - nOffset,file);

   if((*pPal))
      delete [](*pPal);
   (*pPal) = NULL;

   if(bmInfo.bmiHeader.biBitCount == 8)
   {
      (*pPal) = new BYTE[256*3];

      int dwCnt = 0;
      for(int i=0;i<256;i++)
      {
         (*pPal)[dwCnt++] = pbmInfo->bmiColors[i].rgbRed;
         (*pPal)[dwCnt++] = pbmInfo->bmiColors[i].rgbGreen;
         (*pPal)[dwCnt++] = pbmInfo->bmiColors[i].rgbBlue;
      }
   }

   if((*pData))
      delete [](*pData);
   (*pData) = new BYTE[nFileLen - nOffset];

   memcpy((*pData),lpBits,nFileLen - nOffset);

   ::GlobalFree(pbmInfo);
   DeleteObject(hBmp);
   fclose(file);
   return TRUE;
} 





void Global::Lock(void) 
{
	EnterCriticalSection(&csInternal);
}

void Global::Unlock(void) 
{
	LeaveCriticalSection(&csInternal);
}

void Global::ResetItemInfo()
{
   EnterCriticalSection(&csInternal);
   if(m_pItemInfo->Name)
      delete []m_pItemInfo->Name;
   m_pItemInfo->Name = NULL;
   m_pItemInfo->vBoust.clear();
   m_pItemInfo->bSet = false;
   LeaveCriticalSection(&csInternal);
}

unsigned long Global::GetPLFactionMask(unsigned long ulVal)
{
   switch(ulVal)
   {
      case  0: return PL_FACTION_NONE;
      case  1: return PL_FACTION_01;
      case  2: return PL_FACTION_02;
      case  3: return PL_FACTION_03;
      case  4: return PL_FACTION_04;
      case  5: return PL_FACTION_05;
      case  6: return PL_FACTION_06;
      case  7: return PL_FACTION_07;
      case  8: return PL_FACTION_08;
      case  9: return PL_FACTION_09;
      case 10: return PL_FACTION_10;
      case 11: return PL_FACTION_11;
      case 12: return PL_FACTION_12;
      case 13: return PL_FACTION_13;
      case 14: return PL_FACTION_14;
      case 15: return PL_FACTION_15;
      case 16: return PL_FACTION_16;
      case 17: return PL_FACTION_17;
      case 18: return PL_FACTION_18;
      case 19: return PL_FACTION_19;
      case 20: return PL_FACTION_20;
      case 21: return PL_FACTION_21;
      case 22: return PL_FACTION_22;
      case 23: return PL_FACTION_23;
      case 24: return PL_FACTION_24;
      case 25: return PL_FACTION_25;
      case 26: return PL_FACTION_26;
      case 27: return PL_FACTION_27;
      case 28: return PL_FACTION_28;
      case 29: return PL_FACTION_29;
      case 30: return PL_FACTION_30;
      case 31: return PL_FACTION_31;
   }
   return PL_FACTION_NONE;
}

void Global::SetItemInfoID(ULONG ulID)
{
   EnterCriticalSection(&csInternal);
   m_pItemInfo->ulID = ulID;
   LeaveCriticalSection(&csInternal);
}
void Global::SetItemInfoInventory(bool bInventory)
{
   EnterCriticalSection(&csInternal);
   m_pItemInfo->bInventory = bInventory;
   LeaveCriticalSection(&csInternal);
}

int Global::SaveCurrentImage8(BYTE *pshImage,BYTE *pPal,int dwWidth, int dwHeight,char *pstrName,BOOL bFlipV)
{
   int dwBufSize = dwWidth*dwHeight;

   BYTE *pchImageByte = new BYTE[dwWidth*dwHeight];

   if(bFlipV)
   {
      int dwPosDest = 0;

      for(int y=dwHeight-1;y>=0;y--)
      {
         for(int x=0;x<dwWidth;x++)
         {
            pchImageByte[(y*dwWidth)+x] = (BYTE) pshImage[(dwPosDest*dwWidth)+x];
         }
         dwPosDest++;
      }
   }
   else
      memcpy(pchImageByte,pshImage,dwWidth*dwHeight);


   HANDLE hfile = CreateFile(pstrName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,NULL,NULL);
   if(hfile != INVALID_HANDLE_VALUE)
   {
      BITMAPFILEHEADER bfh;
      memset(&bfh,0,sizeof(bfh));
      bfh.bfType    = 'MB';
      bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +(sizeof(RGBQUAD)*256);
      bfh.bfSize    = bfh.bfOffBits + (1*dwWidth*dwHeight);

      DWORD dwWritten = 0;
      WriteFile(hfile,&bfh,sizeof(bfh),&dwWritten,NULL);
      
      BITMAPINFOHEADER  bih;
      
      memset(&bih,0 , sizeof(bih));
      bih.biSize          = sizeof(bih);
      bih.biWidth         = dwWidth;
      bih.biHeight        = dwHeight;
      bih.biPlanes        = 1;
      bih.biCompression   = BI_RGB;
      bih.biSizeImage     = dwBufSize;
      bih.biBitCount      = 8;
      bih.biClrUsed       = 0;
      bih.biClrImportant  = 0;

      dwWritten = 0;
      WriteFile(hfile,&bih,sizeof(bih),&dwWritten,NULL);

      ////////////////////////////////////////////
      // ecriture palette                
      unsigned char PalTmp[1024];
      int cntTmp = 0;
      int dwCnt2 = 0;

      if(pPal)
      {
         for(int j=0;j<1024;j+=4)
         { 
            PalTmp[j+2] = pPal[dwCnt2++];
            PalTmp[j+1] = pPal[dwCnt2++];
            PalTmp[j+0] = pPal[dwCnt2++];
            PalTmp[j+3] = 0;
            /*
            PalTmp[j]   = (unsigned char)cntTmp;
            PalTmp[j+1] = (unsigned char)cntTmp;
            PalTmp[j+2] = (unsigned char)cntTmp;
            PalTmp[j+3] = (unsigned char)0;
            cntTmp++;
            */
         }
      }
      else
      {
         for(int j=0;j<1024;j+=4)
         { 
            PalTmp[j]   = (unsigned char)cntTmp;
            PalTmp[j+1] = (unsigned char)cntTmp;
            PalTmp[j+2] = (unsigned char)cntTmp;
            PalTmp[j+3] = (unsigned char)0;
            cntTmp++;
         }
      }
      
      dwWritten = 0;
      WriteFile(hfile,&PalTmp,sizeof(PalTmp),&dwWritten,NULL);
      ////////////////////////////////////////////
      
      dwWritten = 0;
      WriteFile(hfile,pchImageByte,dwBufSize,&dwWritten,NULL);
      
      CloseHandle(hfile);
   }
   if(pchImageByte)
   {
      delete []pchImageByte;
      pchImageByte = NULL;
   }

   return 0;
}

ClientConfig *Global::GetClientConfig()
{
   ReadClientConfig();
   return &m_ClientConfig;
}

ClientConfig *Global::ReadClientConfig()
{
   if(m_bConfigRead)
      return &m_ClientConfig;

   //set le path de l<application
   char Path[MAX_PATH];
   GetModuleFileName(NULL, Path, MAX_PATH);
   int len = strlen(Path) - 1;
   while (len && Path[len] != '\\')
      len--;
   Path[len] = 0;
   sprintf_s(m_ClientConfig.strPath ,512      ,"%s",Path);
   
   //set le path de save...
   if(SHGetSpecialFolderPath(NULL,m_ClientConfig.strSavePath,CSIDL_PERSONAL   ,FALSE) != 1)
      GetTempPath(MAX_PATH,m_ClientConfig.strSavePath);
   strcat_s(m_ClientConfig.strSavePath,1024,"\\Dialsoft");
   CreateDirectory(m_ClientConfig.strSavePath,NULL);
   strcat_s(m_ClientConfig.strSavePath,1024,"\\T4CDEV");
   CreateDirectory(m_ClientConfig.strSavePath,NULL);


   //create all save sub directory
   //Logs
   char strTmpLogs[1024];
   sprintf_s(strTmpLogs,1024,"%s\\Logs",m_ClientConfig.strSavePath);
   CreateDirectory( strTmpLogs, NULL );
   //Savegame
   char strTmpSaveGame[1024];
   sprintf_s(strTmpSaveGame,1024,"%s\\SaveGame",m_ClientConfig.strSavePath);
   CreateDirectory( strTmpSaveGame, NULL );
   //ScreenShot
   char strTmpScreenShot[1024];
   sprintf_s(strTmpScreenShot,1024,"%s\\ScreenShot",m_ClientConfig.strSavePath);
   CreateDirectory( strTmpScreenShot, NULL );
   //Sounds
   char strTmpSounds[1024];
   sprintf_s(strTmpSounds,1024,"%s\\Sounds",m_ClientConfig.strSavePath);
   CreateDirectory( strTmpSounds, NULL );


   char strConfig[1024];
   sprintf_s(strConfig,1024,"%s\\T4C.ini",m_ClientConfig.strSavePath);

   m_iniT4C.SetIniFileName(strConfig);
   
   if(m_iniT4C.CheckSection("SETTINGS"))
   {
      sprintf_s(m_ClientConfig.strAccountIP,50,"%s",m_iniT4C.GetINIValue("SETTINGS","ACCOUNTIP",""));
      sprintf_s(m_ClientConfig.strAccountSvrName,128,"%s",m_iniT4C.GetINIValue("SETTINGS","ACCOUNTSVRNAME",""));
      
      m_ClientConfig.dwAfkStatus    = m_iniT4C.GetINIValueInt("SETTINGS","AFKSTATUS",0);
      sprintf_s(m_ClientConfig.strAfkMessage,2048,"%s",m_iniT4C.GetINIValue("SETTINGS","AFKMESSAGE",""));
      m_ClientConfig.dwNMShowFPS    = m_iniT4C.GetINIValueInt("SETTINGS","SHOWFPSOVERLAY",0);
      m_ClientConfig.FirstTimeAddon = m_iniT4C.GetINIValueInt("SETTINGS","FIRSTTIMECONNECT",1);
   }
   else
   {
      m_iniT4C.AddSection("SETTINGS");
      m_iniT4C.AddKey("SETTINGS","ACCOUNTIP");
      m_iniT4C.AddKey("SETTINGS","ACCOUNTSVRNAME");
      m_iniT4C.AddKey("SETTINGS","AFKSTATUS");
      m_iniT4C.AddKey("SETTINGS","AFKMESSAGE");
      m_iniT4C.AddKey("SETTINGS","SHOWFPSOVERLAY");
      m_iniT4C.AddKey("SETTINGS","FIRSTTIMECONNECT");
      

      m_ClientConfig.strAccountIP  [0] = 0x00;
      m_ClientConfig.strAccountSvrName[0] = 0x00;
      m_ClientConfig.strAfkMessage [0] = 0x00;
      m_ClientConfig.dwAfkStatus      = 0;
      m_ClientConfig.dwNMShowFPS      = 0;
      m_ClientConfig.FirstTimeAddon   = 1;


      m_iniT4C.WriteINIValue("SETTINGS","ACCOUNTIP"          ,m_ClientConfig.strAccountIP);
      m_iniT4C.WriteINIValue("SETTINGS","ACCOUNTSVRNAME"     ,m_ClientConfig.strAccountSvrName);
      m_iniT4C.WriteINIValueInt("SETTINGS","AFKSTATUS"       ,m_ClientConfig.dwAfkStatus);
      m_iniT4C.WriteINIValue("SETTINGS","AFKMESSAGE"         ,m_ClientConfig.strAfkMessage);
      m_iniT4C.WriteINIValueInt("SETTINGS","SHOWFPSOVERLAY"  ,m_ClientConfig.dwNMShowFPS);
      m_iniT4C.WriteINIValueInt("SETTINGS","FIRSTTIMECONNECT"  ,m_ClientConfig.FirstTimeAddon);
   }

   m_bConfigRead = true;
   return &m_ClientConfig;
}

void Global::WriteClientConfig()
{
   m_iniT4C.WriteINIValue("SETTINGS","ACCOUNTIP"           ,m_ClientConfig.strAccountIP);
   m_iniT4C.WriteINIValue("SETTINGS","ACCOUNTSVRNAME"      ,m_ClientConfig.strAccountSvrName);
   m_iniT4C.WriteINIValueInt("SETTINGS","AFKSTATUS"        ,m_ClientConfig.dwAfkStatus);
   m_iniT4C.WriteINIValue("SETTINGS","AFKMESSAGE"          ,m_ClientConfig.strAfkMessage);
   m_iniT4C.WriteINIValueInt("SETTINGS","SHOWFPSOVERLAY"   ,m_ClientConfig.dwNMShowFPS);
   m_iniT4C.WriteINIValueInt("SETTINGS","FIRSTTIMECONNECT" ,m_ClientConfig.FirstTimeAddon);
}


int Global::GetDialogLastXPos()
{
   if(m_iDialoglastXPos == -1)
   {
      m_iDialoglastXPos =  (g_Global.GetScreenW()-g_OptionDlgW)/2;
   }

   return m_iDialoglastXPos;
}

int Global::GetDialogLastYPos()
{
   if(m_iDialoglastYPos == -1)
   {
      m_iDialoglastYPos =  ((g_Global.GetScreenH()-g_OptionDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialoglastYPos <0)
      m_iDialoglastYPos =0;

   return m_iDialoglastYPos;
}


void Global::SetDialogLastXYPos(int X, int Y)
{
   m_iDialoglastXPos = X;
   m_iDialoglastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogChestLastXPos()
{
   if(m_iDialogChestlastXPos == -1)
   {
      m_iDialogChestlastXPos =  (g_Global.GetScreenW()-g_ChestDlgW)/2;
   }

   return m_iDialogChestlastXPos;
}

int Global::GetDialogChestLastYPos()
{
   if(m_iDialogChestlastYPos == -1)
   {
      m_iDialogChestlastYPos =  ((g_Global.GetScreenH()-g_ChestDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogChestlastYPos <0)
      m_iDialogChestlastYPos =0;

   return m_iDialogChestlastYPos;
}


void Global::SetDialogChestLastXYPos(int X, int Y)
{
   m_iDialogChestlastXPos = X;
   m_iDialogChestlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogChatLastXPos()
{
   if(m_iDialogChatlastXPos == -1)
   {
      m_iDialogChatlastXPos =  (g_Global.GetScreenW()-g_ChatDlgW)/2;
   }
   
   return m_iDialogChatlastXPos;
}

int Global::GetDialogChatLastYPos() 
{
   if(m_iDialogChatlastYPos == -1)
   {
      m_iDialogChatlastYPos =  ((g_Global.GetScreenH()-g_ChatDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogChatlastYPos <0)
      m_iDialogChatlastYPos =0;
   
   return m_iDialogChatlastYPos;
}


void Global::SetDialogChatLastXYPos(int X, int Y)
{
   m_iDialogChatlastXPos = X;
   m_iDialogChatlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}



int Global::GetDialogLastGMXPos()
{
   if(m_iDialogGMlastXPos == -1)
   {
      m_iDialogGMlastXPos =  (g_Global.GetScreenW()-g_GMDlgW)/2;
   }
   
   return m_iDialogGMlastXPos;
}

int Global::GetDialogLastGMYPos() 
{
   if(m_iDialogGMlastYPos == -1)
   {
      m_iDialogGMlastYPos =  50;
   }
   if(m_iDialogGMlastYPos <0)
      m_iDialogGMlastYPos =0;
   
   return m_iDialogGMlastYPos;
}


void Global::SetDialogLastGMXYPos(int X, int Y)
{
   m_iDialogGMlastXPos = X;
   m_iDialogGMlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogLastArene1WXPos()
{
   if(m_iDialogArene1WlastXPos == -1)
   {
      m_iDialogArene1WlastXPos =  (g_Global.GetScreenW()-g_Arene1WDlgW)/2;
   }

   return m_iDialogArene1WlastXPos;
}

int Global::GetDialogLastArene1WYPos() 
{
   if(m_iDialogArene1WlastYPos == -1)
   {
      m_iDialogArene1WlastYPos =  50;
   }
   if(m_iDialogArene1WlastYPos <0)
      m_iDialogArene1WlastYPos =0;

   return m_iDialogArene1WlastYPos;
}


void Global::SetDialogLastArene1WXYPos(int X, int Y)
{
   m_iDialogArene1WlastXPos = X;
   m_iDialogArene1WlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastArene1PXPos()
{
   if(m_iDialogArene1PlastXPos == -1)
   {
      m_iDialogArene1PlastXPos =  (g_Global.GetScreenW()-g_Arene1PDlgW)/2;
   }

   return m_iDialogArene1PlastXPos;
}

int Global::GetDialogLastArene1PYPos() 
{
   if(m_iDialogArene1PlastYPos == -1)
   {
      m_iDialogArene1PlastYPos =  50;
   }
   if(m_iDialogArene1PlastYPos <0)
      m_iDialogArene1PlastYPos =0;

   return m_iDialogArene1PlastYPos;
}


void Global::SetDialogLastArene1PXYPos(int X, int Y)
{
   m_iDialogArene1PlastXPos = X;
   m_iDialogArene1PlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastVolXPos()
{
   if(m_iDialogVollastXPos == -1)
   {
      m_iDialogVollastXPos =  (g_Global.GetScreenW()-g_VolDlgW)/2;
   }
   
   return m_iDialogVollastXPos;
}

int Global::GetDialogLastVolYPos() 
{
   if(m_iDialogVollastYPos == -1)
   {
      m_iDialogVollastYPos =  ((g_Global.GetScreenH()-g_VolDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogVollastYPos <0)
      m_iDialogVollastYPos =0;
   
   return m_iDialogVollastYPos;
}


void Global::SetDialogLastVolXYPos(int X, int Y)
{
   m_iDialogVollastXPos = X;
   m_iDialogVollastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastAHXPos()
{
   if(m_iDialogAHlastXPos == -1)
   {
      m_iDialogAHlastXPos =  (g_Global.GetScreenW()-g_AHDlgW)/2;
   }
   
   return m_iDialogAHlastXPos;
}

int Global::GetDialogLastAHYPos() 
{
   if(m_iDialogAHlastYPos == -1)
   {
      m_iDialogAHlastYPos  =  ((g_Global.GetScreenH()-g_AHDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogAHlastYPos  <0)
      m_iDialogAHlastYPos  =0;
   
   return m_iDialogAHlastYPos ;
}


void Global::SetDialogLastAHXYPos(int X, int Y)
{
   m_iDialogAHlastXPos = X;
   m_iDialogAHlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastChestNewXPos()
{
   if(m_iDialogChestNewlastXPos == -1)
   {
      m_iDialogChestNewlastXPos =  (g_Global.GetScreenW()-g_ChestNewDlgW)/2;
   }

   return m_iDialogChestNewlastXPos;
}

int Global::GetDialogLastChestNewYPos() 
{
   if(m_iDialogChestNewlastYPos == -1)
   {
      m_iDialogChestNewlastYPos  =  ((g_Global.GetScreenH()-g_ChestNewDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogChestNewlastYPos  <0)
      m_iDialogChestNewlastYPos  =0;

   return m_iDialogChestNewlastYPos ;
}


void Global::SetDialogLastChestNewXYPos(int X, int Y)
{
   m_iDialogChestNewlastXPos = X;
   m_iDialogChestNewlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}



int Global::GetDialogLastIPVPXPos()
{
   if(m_iDialogIPVPlastXPos == -1)
   {
      m_iDialogIPVPlastXPos =  (g_Global.GetScreenW()-g_IPVPDlgW)/2;
   }

   return m_iDialogIPVPlastXPos;
}

int Global::GetDialogLastIPVPYPos() 
{
   if(m_iDialogIPVPlastYPos == -1)
   {
      m_iDialogIPVPlastYPos  =  ((g_Global.GetScreenH()-g_IPVPDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogIPVPlastYPos  <0)
      m_iDialogIPVPlastYPos  =0;

   return m_iDialogIPVPlastYPos ;
}


void Global::SetDialogLastIPVPXYPos(int X, int Y)
{
   m_iDialogIPVPlastXPos = X;
   m_iDialogIPVPlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}








int Global::GetDialogLastIFXPos()
{
   if(m_iDialogIFlastXPos == -1)
   {
      m_iDialogIFlastXPos =  (g_Global.GetScreenW()-g_IFDlgW)/2;
   }
   
   return m_iDialogIFlastXPos;
}

int Global::GetDialogLastIFYPos() 
{
   if(m_iDialogIFlastYPos == -1)
   {
      m_iDialogIFlastYPos  =  ((g_Global.GetScreenH()-g_IFDlgH)/2)-(g_MainBarDlgH/2);
   }
   if(m_iDialogIFlastYPos  <0)
      m_iDialogIFlastYPos  =0;
   
   return m_iDialogIFlastYPos ;
}


void Global::SetDialogLastIFXYPos(int X, int Y)
{
   m_iDialogIFlastXPos = X;
   m_iDialogIFlastYPos = Y;

   g_bSaveGUIChanged = TRUE;
}




int Global::GetDialogLastMainBarXPos()
{
   if(m_iDialoglastMainBarXPos == -1)
   {
      m_iDialoglastMainBarXPos =  (g_Global.GetScreenW()-g_MainBarDlgW)/2;
   }
   
   return m_iDialoglastMainBarXPos;
}

int Global::GetDialogLastMainBarYPos()
{
   if(m_iDialoglastMainBarYPos == -1)
   {
      m_iDialoglastMainBarYPos =  (g_Global.GetScreenH()-g_MainBarDlgH);
   }
   if(m_iDialoglastMainBarYPos <0)
      m_iDialoglastMainBarYPos =0;
   int iVal = m_iDialoglastMainBarYPos;
   
   if(g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      iVal+=47;
   return iVal;
}

void Global::SetDialogLastMainBarXYPos(int X, int Y)
{
   int iYVal = Y;
   if(g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      iYVal-=47;
   m_iDialoglastMainBarXPos = X;
   m_iDialoglastMainBarYPos = iYVal;

   g_bSaveGUIChanged = TRUE;
}



int Global::GetDialogLastTMIXPos()
{
   if(m_iDialoglastTMIXPos == -1)
   {
      m_iDialoglastTMIXPos =  (g_Global.GetScreenW()-g_TMIDlgW);
   }
   
   return m_iDialoglastTMIXPos;
}

int Global::GetDialogLastTMIYPos()
{
   if(m_iDialoglastTMIYPos == -1)
   {
      m_iDialoglastTMIYPos =  (g_Global.GetScreenH()-g_TMIDlgH);
   }
   if(m_iDialoglastTMIYPos <0)
      m_iDialoglastTMIYPos =0;
   
   return m_iDialoglastTMIYPos;
}

void Global::SetDialogLastTMIXYPos(int X, int Y)
{
   m_iDialoglastTMIXPos = X;
   m_iDialoglastTMIYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastLifeXPos()
{
   if(m_iDialoglastLifeXPos == -1)
   {
      m_iDialoglastLifeXPos =  (g_Global.GetScreenW()-g_LifeDlgW);
   }
   
   return m_iDialoglastLifeXPos;
}

int Global::GetDialogLastLifeYPos()
{
   if(m_iDialoglastLifeYPos == -1)
   {
      m_iDialoglastLifeYPos =  0;
   }
   if(m_iDialoglastLifeYPos <0)
      m_iDialoglastLifeYPos =0;
   
   return m_iDialoglastLifeYPos;
}

void Global::SetDialogLastLifeXYPos(int X, int Y)
{
   m_iDialoglastLifeXPos = X;
   m_iDialoglastLifeYPos = Y;

   g_bSaveGUIChanged = TRUE;
}



int Global::GetDialogLastStatusXPos()
{
   if(m_iDialoglastStatusXPos == -1)
   {
      m_iDialoglastStatusXPos =  0;
   }
   
   return m_iDialoglastStatusXPos;
}

int Global::GetDialogLastStatusYPos()
{
   if(m_iDialoglastStatusYPos == -1)
   {
      m_iDialoglastStatusYPos =  80;
   }
   if(m_iDialoglastStatusYPos <0)
      m_iDialoglastStatusYPos =0;
   
   return m_iDialoglastStatusYPos;
}

void Global::SetDialogLastStatusXYPos(int X, int Y)
{
   m_iDialoglastStatusXPos = X;
   m_iDialoglastStatusYPos = Y;
   
   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogLastStatusP()
{
   if(m_iDialoglastStatusPinned == -1)
   {
      m_iDialoglastStatusPinned =  1;
   }
   
   return m_iDialoglastStatusPinned;
}

void Global::SetDialogLastStatusP(int P)
{
   m_iDialoglastStatusPinned = P;
   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogLastStatusNbrEffect()
{
   if(m_iDialoglastStatusNbrEffect == -1)
   {
      m_iDialoglastStatusNbrEffect =  11;
   }
   
   return m_iDialoglastStatusNbrEffect;
}

void Global::SetDialogLastStatusNbrEffect(int iNbr)
{
   m_iDialoglastStatusNbrEffect = iNbr;
   g_bSaveGUIChanged = TRUE;
}




int Global::GetDialogLastTargetXPos()
{
   if(m_iDialoglastTargetXPos == -1)
   {
      m_iDialoglastTargetXPos =  0;
   }
   
   return m_iDialoglastTargetXPos;
}

int Global::GetDialogLastTargetYPos()
{
   if(m_iDialoglastTargetYPos == -1)
   {
      m_iDialoglastTargetYPos =  0;
   }
   if(m_iDialoglastTargetYPos <0)
      m_iDialoglastTargetYPos =0;
   
   return m_iDialoglastTargetYPos;
}

void Global::SetDialogLastTargetXYPos(int X, int Y)
{
   m_iDialoglastTargetXPos = X;
   m_iDialoglastTargetYPos = Y;
   
   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogLastMP3XPos()
{
   if(m_iDialoglastMP3XPos == -1)
   {
      m_iDialoglastMP3XPos =  (g_Global.GetScreenW()-g_MP3DlgW)/2;
   }
   
   return m_iDialoglastMP3XPos;
}

int Global::GetDialogLastMP3YPos()
{
   if(m_iDialoglastMP3YPos == -1)
   {
      m_iDialoglastMP3YPos =  0;
   }
   if(m_iDialoglastMP3YPos <0)
      m_iDialoglastMP3YPos =0;
   
   return m_iDialoglastMP3YPos;
}

void Global::SetDialogLastMP3XYPos(int X, int Y)
{
   m_iDialoglastMP3XPos = X;
   m_iDialoglastMP3YPos = Y;
   
   g_bSaveGUIChanged = TRUE;
}

int Global::GetDialogLastArenePointXPos()
{
   if(m_iDialoglastArenePointXPos == -1)
   {
      m_iDialoglastArenePointXPos =  (g_Global.GetScreenW()-g_ArenePointDlgW)/2;
   }

   return m_iDialoglastArenePointXPos;
}

int Global::GetDialogLastArenePointYPos()
{
   if(m_iDialoglastArenePointYPos == -1)
   {
      m_iDialoglastArenePointYPos =  0;
   }
   if(m_iDialoglastArenePointYPos <0)
      m_iDialoglastArenePointYPos =0;

   return m_iDialoglastArenePointYPos;
}

void Global::SetDialogLastArenePointXYPos(int X, int Y)
{
   m_iDialoglastArenePointXPos = X;
   m_iDialoglastArenePointYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogGroupOSXPos()
{
   if(m_iDialoglastGroupOSXPos == -1)
   {
      m_iDialoglastGroupOSXPos =  (g_Global.GetScreenW()-g_GroupOSDlgW);
   }
   
   return m_iDialoglastGroupOSXPos;
}

int Global::GetDialogGroupOSYPos()
{
   if(m_iDialoglastGroupOSYPos == -1)
   {
      m_iDialoglastGroupOSYPos =  115;
   }
   if(m_iDialoglastGroupOSYPos <0)
      m_iDialoglastGroupOSYPos =0;
   
   return m_iDialoglastGroupOSYPos;
}

void Global::SetDialogGroupOSXYPos(int X, int Y)
{
   m_iDialoglastGroupOSXPos = X;
   m_iDialoglastGroupOSYPos = Y;

   g_bSaveGUIChanged = TRUE;
}


int Global::GetDialogLastChatLogXPos()
{
   if(m_iDialoglastChatLogXPos == -1)
   {
      m_iDialoglastChatLogXPos =  (g_Global.GetScreenW()-g_ChatLogDlgDefW)/2;
   }
   
   return m_iDialoglastChatLogXPos;
} 

int Global::GetDialogLastChatLogYPos()
{
   if(m_iDialoglastChatLogYPos == -1)
   {
      m_iDialoglastChatLogYPos =  (g_Global.GetScreenH()-g_ChatLogDlgDefH-(g_MainBarDlgH-13));
   }
   if(m_iDialoglastChatLogYPos <0)
      m_iDialoglastChatLogYPos =0;
   
   return m_iDialoglastChatLogYPos;
}

int Global::GetDialogLastChatLogH()
{
   if(m_iDialoglastChatLogH == -1)
   {
      m_iDialoglastChatLogH =  g_ChatLogDlgDefH;
   }

   return m_iDialoglastChatLogH;
}

int Global::GetDialogLastChatLogW()
{
   if(m_iDialoglastChatLogW == -1)
   {
      m_iDialoglastChatLogW =  g_ChatLogDlgDefW;
   }
   
   return m_iDialoglastChatLogW;
}

int Global::GetDialogLastChatLogP()
{
   if(m_iDialoglastChatLogPinned == -1)
   {
      m_iDialoglastChatLogPinned =  1;
   }
   
   return m_iDialoglastChatLogPinned;
}





void Global::SetDialogLastChatLogXYPos(int X, int Y, int W, int H)
{
   m_iDialoglastChatLogXPos = X;
   m_iDialoglastChatLogYPos = Y;
   m_iDialoglastChatLogW    = W;
   m_iDialoglastChatLogH    = H;

   g_bSaveGUIChanged = TRUE;
}

void Global::SetDialogLastChatLogP(int P)
{
   m_iDialoglastChatLogPinned = P;

   g_bSaveGUIChanged = TRUE;
}


void Global::SetLastDialogOpen(int iVal)
{
   m_iLastDialogOpenned = iVal;
   if(m_iLastDialogOpenned <0 || m_iLastDialogOpenned >7)
      m_iLastDialogOpenned = 0;
      
}

int  Global::GetLastDialogOpen()
{
   if(m_iLastDialogOpenned == -1)
      m_iLastDialogOpenned = 0;

   return m_iLastDialogOpenned;
}

void Global::SetDraggingDialog(bool bVal)
{
   m_bDraggingDialog =bVal;
}

bool Global::GetDraggingDialog()
{
   return m_bDraggingDialog;
}


void Global::ResetAllGUIPositionAndSize()
{
   m_iDialoglastXPos             = -1;
   m_iDialoglastYPos             = -1;
   m_iDialogChatlastXPos         = -1;
   m_iDialogChatlastYPos         = -1;
   m_iDialogGMlastXPos           = -1;
   m_iDialogGMlastYPos           = -1;
   m_iDialogArene1WlastXPos      = -1;
   m_iDialogArene1WlastYPos      = -1;
   m_iDialogArene1PlastXPos      = -1;
   m_iDialogArene1PlastYPos      = -1;
   m_iDialogVollastXPos          = -1;
   m_iDialogVollastYPos          = -1;
   m_iDialogAHlastXPos           = -1;
   m_iDialogAHlastYPos           = -1;
   m_iDialogIFlastXPos           = -1;
   m_iDialogIFlastYPos           = -1;
   m_iDialogChestNewlastXPos     = -1;
   m_iDialogChestNewlastYPos     = -1;
   m_iDialogIPVPlastXPos         = -1;
   m_iDialogIPVPlastYPos         = -1;
                                 
   m_iDialoglastMainBarXPos      = -1;
   m_iDialoglastMainBarYPos      = -1;
   m_iDialoglastTMIXPos          = -1;
   m_iDialoglastTMIYPos          = -1;
   m_iDialoglastLifeXPos         = -1;
   m_iDialoglastLifeYPos         = -1;
   m_iDialoglastGroupOSXPos      = -1;
   m_iDialoglastGroupOSYPos      = -1;
   m_iDialoglastChatLogXPos      = -1;
   m_iDialoglastChatLogYPos      = -1;
   m_iDialoglastChatLogH         = -1;
   m_iDialoglastChatLogW         = -1;
   m_iDialoglastChatLogPinned    = -1;
   m_iDialoglastTargetXPos       = -1;
   m_iDialoglastTargetYPos       = -1;
   m_iDialoglastMP3XPos          = -1;
   m_iDialoglastMP3YPos          = -1;
   m_iDialoglastArenePointXPos   = -1;
   m_iDialoglastArenePointYPos   = -1;
   m_iDialoglastStatusXPos       = -1;
   m_iDialoglastStatusYPos       = -1;
   m_iDialoglastStatusPinned     = -1;
   m_iDialoglastStatusNbrEffect  = -1;
   m_iDialogChestlastXPos        = -1;
   m_iDialogChestlastYPos        = -1;

   
   
   m_iLastDialogOpenned          = -1;

   g_bSaveGUIChanged = TRUE;
}


void Global::SaveGUI()
{
   char strSaveGameDir[MAX_PATH];
   char strGUIFile[MAX_PATH];


   ClientConfig *pConfig = GetClientConfig();
   sprintf_s(strSaveGameDir,MAX_PATH,"%s\\",pConfig->strSavePath);
   sprintf_s(strGUIFile,MAX_PATH,"%sGUI.dat",strSaveGameDir);


   FILE *pf = NULL;
   fopen_s(&pf,strGUIFile,"wb"); //oki
   if(pf)
   {
      //ecrit le header du file
      fwrite(&g_GUI_FILE_VER,sizeof(DWORD),1,pf);

      fwrite(&m_iDialoglastXPos         ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastYPos         ,sizeof(int),1,pf);
      fwrite(&m_iDialogChatlastXPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialogChatlastYPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialogGMlastXPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogGMlastYPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogVollastXPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialogVollastYPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialogAHlastXPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogAHlastYPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogIFlastXPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogIFlastYPos       ,sizeof(int),1,pf);

      fwrite(&m_iDialoglastMainBarXPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastMainBarYPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastTMIXPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastTMIYPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastLifeXPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastLifeYPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastGroupOSXPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastGroupOSYPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastChatLogXPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastChatLogYPos  ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastChatLogH     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastChatLogW     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastChatLogPinned,sizeof(int),1,pf);
      fwrite(&m_iDialoglastTargetXPos   ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastTargetYPos   ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastMP3XPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastMP3YPos      ,sizeof(int),1,pf);

      int iLastW = g_Global.GetScreenW();
      int iLastH = g_Global.GetScreenH();
      fwrite(&iLastW     ,sizeof(int),1,pf);
      fwrite(&iLastH     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastStatusXPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastStatusYPos     ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastStatusPinned   ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastStatusNbrEffect,sizeof(int),1,pf);
      fwrite(&m_iDialogChestlastXPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialogChestlastYPos      ,sizeof(int),1,pf);
      fwrite(&m_iDialogChestNewlastXPos   ,sizeof(int),1,pf);
      fwrite(&m_iDialogChestNewlastYPos   ,sizeof(int),1,pf);
      fwrite(&m_iDialogArene1WlastXPos    ,sizeof(int),1,pf);
      fwrite(&m_iDialogArene1WlastYPos    ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastArenePointXPos ,sizeof(int),1,pf);
      fwrite(&m_iDialoglastArenePointYPos ,sizeof(int),1,pf);
      fwrite(&m_iDialogArene1PlastXPos    ,sizeof(int),1,pf);
      fwrite(&m_iDialogArene1PlastYPos    ,sizeof(int),1,pf);
      fwrite(&m_iDialogIPVPlastXPos       ,sizeof(int),1,pf);
      fwrite(&m_iDialogIPVPlastYPos       ,sizeof(int),1,pf);
      
      
      

      fclose(pf);
   }
}

void Global::LoadGUI()
{
   char strSaveGameDir[MAX_PATH];
   char strGUIFile[MAX_PATH];
   
   ClientConfig *pConfig = GetClientConfig();
   sprintf_s(strSaveGameDir,MAX_PATH,"%s\\",pConfig->strSavePath);
   sprintf_s(strGUIFile,MAX_PATH,"%sGUI.dat",strSaveGameDir);
   
   
   FILE *pf = NULL;
   fopen_s(&pf,strGUIFile,"rb");
   if(pf)
   {

      //ecrit le header du file
      DWORD dwVer;
      fread(&dwVer,sizeof(DWORD),1,pf);
      if(dwVer == g_GUI_FILE_VER)
      {
         if(fread(&m_iDialoglastXPos        ,sizeof(int),1,pf)!= 1)
            m_iDialoglastXPos = -1;
         if(fread(&m_iDialoglastYPos        ,sizeof(int),1,pf)!= 1)
            m_iDialoglastYPos = -1;
         if(fread(&m_iDialogChatlastXPos    ,sizeof(int),1,pf)!= 1)
            m_iDialogChatlastXPos = -1;
         if(fread(&m_iDialogChatlastYPos    ,sizeof(int),1,pf)!= 1)
            m_iDialogChatlastYPos = -1;
         if(fread(&m_iDialogGMlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogGMlastXPos = -1;
         if(fread(&m_iDialogGMlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialoglastXPos = -1;
         if(fread(&m_iDialogVollastXPos     ,sizeof(int),1,pf)!= 1)
            m_iDialogVollastXPos = -1;
         if(fread(&m_iDialogVollastYPos     ,sizeof(int),1,pf)!= 1)
            m_iDialogVollastYPos = -1;
         if(fread(&m_iDialogAHlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogAHlastXPos = -1;
         if(fread(&m_iDialogAHlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogAHlastYPos = -1;
         if(fread(&m_iDialogIFlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogIFlastXPos = -1;
         if(fread(&m_iDialogIFlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogIFlastYPos = -1;
         
         if(fread(&m_iDialoglastMainBarXPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastMainBarXPos = -1;
         if(fread(&m_iDialoglastMainBarYPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastMainBarYPos = -1;
         if(fread(&m_iDialoglastTMIXPos     ,sizeof(int),1,pf)!= 1)
            m_iDialoglastTMIXPos = -1;
         if(fread(&m_iDialoglastTMIYPos     ,sizeof(int),1,pf)!= 1)
            m_iDialoglastTMIYPos = -1;
         if(fread(&m_iDialoglastLifeXPos    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastLifeXPos = -1;
         if(fread(&m_iDialoglastLifeYPos    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastLifeYPos = -1;
         if(fread(&m_iDialoglastGroupOSXPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastGroupOSXPos = -1;
         if(fread(&m_iDialoglastGroupOSYPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastGroupOSYPos = -1;
         if(fread(&m_iDialoglastChatLogXPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastChatLogXPos = -1;
         if(fread(&m_iDialoglastChatLogYPos ,sizeof(int),1,pf)!= 1)
            m_iDialoglastChatLogYPos = -1;
         if(fread(&m_iDialoglastChatLogH    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastChatLogH = -1;
         if(fread(&m_iDialoglastChatLogW    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastChatLogW = -1;
         
         if(fread(&m_iDialoglastChatLogPinned,sizeof(int),1,pf)!= 1)
            m_iDialoglastChatLogPinned = -1;

         if(fread(&m_iDialoglastTargetXPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastTargetXPos = -1;
         if(fread(&m_iDialoglastTargetYPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastTargetYPos = -1;

         if(fread(&m_iDialoglastMP3XPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastMP3XPos = -1;
         if(fread(&m_iDialoglastMP3YPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastMP3YPos = -1;

         int iLastW;
         int iLastH;
         if(fread(&iLastW,sizeof(int),1,pf)!= 1)
            iLastW = -1;
         if(fread(&iLastH,sizeof(int),1,pf)!= 1)
            iLastH = -1;

         if(fread(&m_iDialoglastStatusXPos    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastStatusXPos = -1;
         if(fread(&m_iDialoglastStatusYPos    ,sizeof(int),1,pf)!= 1)
            m_iDialoglastStatusYPos = -1;
         if(fread(&m_iDialoglastStatusPinned,sizeof(int),1,pf)!= 1)
            m_iDialoglastStatusPinned = -1;
         if(fread(&m_iDialoglastStatusNbrEffect,sizeof(int),1,pf)!= 1)
            m_iDialoglastStatusNbrEffect = -1;

         if(fread(&m_iDialogChestlastXPos    ,sizeof(int),1,pf)!= 1)
            m_iDialogChestlastXPos = -1;
         if(fread(&m_iDialogChestlastYPos    ,sizeof(int),1,pf)!= 1)
            m_iDialogChestlastYPos = -1;

         if(fread(&m_iDialogChestNewlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogChestNewlastXPos = -1;
         if(fread(&m_iDialogChestNewlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogChestNewlastYPos = -1;

         if(fread(&m_iDialogArene1WlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogArene1WlastXPos = -1;
         if(fread(&m_iDialogArene1WlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogArene1WlastYPos = -1;

         if(fread(&m_iDialoglastArenePointXPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastArenePointXPos = -1;
         if(fread(&m_iDialoglastArenePointYPos,sizeof(int),1,pf)!= 1)
            m_iDialoglastArenePointYPos = -1;

         if(fread(&m_iDialogArene1PlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogArene1PlastXPos = -1;
         if(fread(&m_iDialogArene1PlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogArene1PlastYPos = -1;

         if(fread(&m_iDialogIPVPlastXPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogIPVPlastXPos = -1;
         if(fread(&m_iDialogIPVPlastYPos      ,sizeof(int),1,pf)!= 1)
            m_iDialogIPVPlastYPos = -1;

         

         if(iLastW != g_Global.GetScreenW() || iLastH != g_Global.GetScreenH())
         {
            ResetAllGUIPositionAndSize();
         }


         
      }
      fclose(pf);
   }

}

//////////////////////////////////////////////////////////////////////////////////////////
UINT WINAPI Global::SaveGUIThread(void *nothing)
{
   while( !g_boQuitApp )
   {
      Sleep( 500 );
      if(!g_boQuitApp && g_bSaveGUIChanged)
      {
         g_bSaveGUIChanged = FALSE;

         g_Global.SaveGUI();

         //Save la pos des fenetre...
         
      }
   }
   
   return 0;
}