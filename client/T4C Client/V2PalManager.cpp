#include "pch.h"
#include "Global.h"
#include "V2PalManager.h"
#include <string>
#include "Zlib/zlib.h"
#include "MD5\MD5Checksum.h"

using namespace std;


#define V2DATABASE_PALETTE     "Game Files\\V2ColorI.dpd"

#define V2NMSDATABASE_PALETTE  "Game Files\\V2NMSColorI.dpd"

// Registered at the very start of the constructor so a re-entrant GetInstance()
// (the constructor calls m_plRefPal.LoadPalette -> Sprite::LoadPalette -> GetInstance)
// returns the instance-under-construction instead of deadlocking on the VS2022
// thread-safe static-init guard (/Zc:threadSafeInit, absent in the original VC6/VC8 build).
static CV2PalManager* g_pPalManagerInstance = NULL;

CV2PalManager::CV2PalManager()
{
   g_pPalManagerInstance = this;

   //load V2 data file
   char   strChksumMd5[33];
   char   *pstrMD5Part2 = &strChksumMd5[16];
   ULONG  ulTailleUnpack;
   ULONG  ulTaillePack;
   BYTE   uchChksum;

   FILE *pFilePal = NULL;
   fopen_s(&pFilePal,V2DATABASE_PALETTE,"rb");
   if(!pFilePal)
      return ;
   fread(strChksumMd5    ,1,16,pFilePal);
   fread(&ulTailleUnpack ,4,1,pFilePal);
   fread(&ulTaillePack   ,4,1,pFilePal);
   fread(pstrMD5Part2    ,1,17,pFilePal);

   BYTE *pchData    = new unsigned char[ulTailleUnpack];
   BYTE *pCompressed= new unsigned char[ulTaillePack];
   
   fread(pCompressed,1,ulTaillePack,pFilePal);
   fread(&uchChksum ,1,1,pFilePal);
   fclose(pFilePal);

   BYTE uchVal = CalcChecksumComp2(pCompressed,ulTaillePack);
   if(uchVal != uchChksum)
   {
      delete []pchData;
      delete []pCompressed;
      return;
   }


   if(uncompress(pchData,&ulTailleUnpack,pCompressed,ulTaillePack) != S_OK)
      OutputDebugString("*********** IMP ********  Probleme de load du fichier de palette...\n");

   char *pstrMD5 = CMD5Checksum::GetMD5(pchData,ulTailleUnpack);
   if(strcmp(pstrMD5,strChksumMd5)!= 0)
   {
      delete []pchData;
      delete []pCompressed;
      delete []pstrMD5;
      return;
   }

   UINT i=0;
   for(i=0;i<ulTailleUnpack;i++)
      pchData[i] ^= 0x66;

   delete []pCompressed;
   delete []pstrMD5;

   int iNbrPal = (ulTailleUnpack/sizeof(PalInfo));

   //*****************************Load NMS palette
   int iNbrPalNMS = 0;
   BYTE *pchDataNMS = NULL; 


   
   FILE *pFilePalNMS = NULL;
   fopen_s(&pFilePalNMS,V2NMSDATABASE_PALETTE,"rb");
   if(pFilePalNMS)
   {
      fread(strChksumMd5    ,1,16,pFilePalNMS);
      fread(&ulTailleUnpack ,4,1 ,pFilePalNMS);
      fread(&ulTaillePack   ,4,1 ,pFilePalNMS);
      fread(pstrMD5Part2    ,1,17,pFilePalNMS);

               pchDataNMS   = new unsigned char[ulTailleUnpack];
      BYTE    *pCompressed  = new unsigned char[ulTaillePack];
   
      fread(pCompressed,1,ulTaillePack,pFilePalNMS);
      fread(&uchChksum ,1,1,pFilePalNMS);
      fclose(pFilePalNMS);

      BYTE uchVal = CalcChecksumComp2(pCompressed,ulTaillePack);
      if(uchVal != uchChksum)
      {
         delete []pchDataNMS;
         delete []pCompressed;
         return;
      }


      if(uncompress(pchDataNMS,&ulTailleUnpack,pCompressed,ulTaillePack) != S_OK)
         OutputDebugString("*********** IMP ********  Probleme de load du fichier de palette...\n");

      char *pstrMD5 = CMD5Checksum::GetMD5(pchDataNMS,ulTailleUnpack);
      if(strcmp(pstrMD5,strChksumMd5)!= 0)
      {
         delete []pchDataNMS;
         delete []pCompressed;
         delete []pstrMD5;
         return;
      }

      for(UINT i=0;i<ulTailleUnpack;i++)
         pchDataNMS[i] ^= 0x66;

      delete []pCompressed;
      delete []pstrMD5;

      iNbrPalNMS = (ulTailleUnpack/sizeof(PalInfo));
   }


   m_dwNbrPalette = iNbrPal+iNbrPalNMS;
   m_pV2Palette = new PalInfo[m_dwNbrPalette];

   //Affecte les palette ORI
   int iCnt = 0;
   BYTE *pchDataTmp = pchData;
   for(i=0;i<(UINT)iNbrPal;i++)
   {
      pchDataTmp = pchData+(i*sizeof(PalInfo));
      memcpy(&m_pV2Palette[iCnt],pchDataTmp,sizeof(PalInfo));
      if (m_pV2Palette[iCnt].lpszID[strlen(m_pV2Palette[iCnt].lpszID)-1] == 'P')
			m_pV2Palette[iCnt].lpszID[strlen(m_pV2Palette[iCnt].lpszID)-1] = 0;
      iCnt++;
   }

   //Affecte les palette NMS
   pchDataTmp = pchDataNMS;
   for(i=0;i<(UINT)iNbrPalNMS;i++)
   {
      pchDataTmp = pchDataNMS+(i*sizeof(PalInfo));
      memcpy(&m_pV2Palette[iCnt],pchDataTmp,sizeof(PalInfo));
      if (m_pV2Palette[iCnt].lpszID[strlen(m_pV2Palette[iCnt].lpszID)-1] == 'P')
			m_pV2Palette[iCnt].lpszID[strlen(m_pV2Palette[iCnt].lpszID)-1] = 0;
      iCnt++;
   }

   delete []pchData;
   if(pchDataNMS)
      delete []pchDataNMS;
  

     
   m_plRefPal.LoadPalette("Bright1");
   

}

CV2PalManager::~CV2PalManager()
{
    delete []m_pV2Palette;
}

LPBYTE CV2PalManager::GetPal(const char *lpszID, int PalNb) 
{

	UINT iLen = 0;//NM
	LPBYTE lpbBestPal = m_plRefPal.lpSpritePal;
   //if(m_plRefPal.lpSpritePal != NULL)
   //   return lpbBestPal;

	char Temp[100];
	for (int i = m_dwNbrPalette-1; i >= 0; i--) 
	{
		if (PalNb == 1) 
		{
			if (!_strnicmp(m_pV2Palette[i].lpszID, lpszID, strlen(m_pV2Palette[i].lpszID)) && iLen < strlen(m_pV2Palette[i].lpszID))  {
				lpbBestPal = (LPBYTE)(m_pV2Palette[i]).lpSpritePal;
				iLen = strlen(m_pV2Palette[i].lpszID);
			}
		} 
		else 
		{
			if( PalNb < 10 ) 
			{
				if (m_pV2Palette[i].lpszID[strlen(m_pV2Palette[i].lpszID)-1] == PalNb + '0')  
				{
					strcpy_s(Temp,100, m_pV2Palette[i].lpszID);
					Temp[strlen(Temp)-1] = 0;
					if (!_strnicmp(Temp, lpszID, strlen(Temp)) && iLen < strlen(Temp))  
					{
						lpbBestPal = (LPBYTE)(m_pV2Palette[i]).lpSpritePal;
						iLen = strlen(Temp);
					}
				}
			} 
			else 
			{
				string strTemp(m_pV2Palette[i].lpszID);
				if (atoi(strTemp.substr(strlen(strTemp.c_str())-2, 2).c_str()) == PalNb)  
				{
					strcpy_s(Temp,100, m_pV2Palette[i].lpszID);
					Temp[strlen(Temp)-2] = 0;
					if (!_strnicmp(Temp, lpszID, strlen(Temp)) && iLen < strlen(Temp))  
					{
						lpbBestPal = (LPBYTE)(m_pV2Palette[i]).lpSpritePal;
						iLen = strlen(Temp);
					}
				}
			}
		}
	}
	return lpbBestPal;
}

LPBYTE CV2PalManager::GetPalIndex(const char *lpszID, int PalNb) 
{
   LPBYTE pPalTmp = GetPal(lpszID,PalNb);
   LPBYTE lpSpritePal = new BYTE [768];
   memcpy(lpSpritePal,pPalTmp,768);
   return lpSpritePal;
}


CV2PalManager* CV2PalManager::GetInstance(void) 
{
   // Return the under-construction instance during the constructor's re-entrant call,
   // before the magic-static guard below would otherwise self-deadlock.
   if (g_pPalManagerInstance)
      return g_pPalManagerInstance;
   static CV2PalManager pmInstance;
   return &pmInstance;
}

unsigned char  CV2PalManager::CalcChecksumComp2(unsigned char	*pData, unsigned long dwNbrData)
{
   if(!pData || dwNbrData <=0)
      return 0x00; // pas de donnes a calculer
   
   unsigned char chkSum;
   unsigned char sumByte = 0x00;
   // on additionne tous les BYTES sauf le checksum...
   for(DWORD i=0;i<dwNbrData-1;i++)
   {
      sumByte += pData[i];
   }

   chkSum = 256 - (sumByte);

   return chkSum;
}