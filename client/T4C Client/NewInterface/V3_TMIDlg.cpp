// V3_TMIDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_TMIDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

extern CSaveGame g_SaveGame;


#define NBR_TMI_MAP 8
#define GPS_MAP_X   3072
#define GPS_MAP_Y   3072
#define GPS_VIEW_X  3072//512
#define GPS_VIEW_Y  3072//512
#define GPS_VIEW_X2 (GPS_VIEW_X/2)
#define GPS_VIEW_Y2 (GPS_VIEW_Y/2)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_TMIDlg::V3_TMIDlg():
Drag_Button(dragEvent)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastTMIXPos();
   m_iYPos = g_Global.GetDialogLastTMIYPos();
   
 
   m_Background.SetSpriteId("V3_TMIBack",true,"V3_TMIBackMask");
   m_BackMap   .SetSpriteId( "V3_TMIMap" );
   m_PlayerPos .SetSpriteId( "V3_PlayerPos" );

   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_BackMap );
   AddChild( &m_Background); 
   AddChild( &m_stPosition);
   AddChild( &Drag_Button);
   
  
   


   //Set TMI Map File name
   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPath[512];
   sprintf_s(strPath,512,"%s",pConfig->strPath);
   sprintf_s(m_TMIFile,512,"%s%s",strPath,"\\Game Files\\TMI_Map.dat");
   sprintf_s(m_TMIFileC,512,"%s\\%s%s",strPath,Player.ServerName,"\\Game Files\\TMI_Map.dat");

   //Setup TMI Buffers
   m_pTMIData  = new WORD[g_TMIDlgMAPWH*g_TMIDlgMAPWH];
   m_pTMIMap   = new BYTE[GPS_VIEW_X*GPS_VIEW_Y];
   m_pTMIPal   = new BYTE[256*3];
   memset(m_pTMIMap,0x00,GPS_VIEW_X*GPS_VIEW_Y);
   memset(m_pTMIPal,0x00,256*3);


   m_dwCurrentLoadedWorld = -1;
   SetupViewObject();

   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_TMIDlg::~V3_TMIDlg()
{
   if(m_pTMIMap)
      delete m_pTMIMap;
   if(m_pTMIPal)
      delete m_pTMIPal;
   if(m_pTMIData)
      delete []m_pTMIData;
   
   m_pTMIMap = NULL;
   m_pTMIPal = NULL;
   m_pTMIData = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_TMIDlg *V3_TMIDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_TMIDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_TMIDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   GetInstance()->m_iXPos = g_Global.GetDialogLastTMIXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastTMIYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();
   return 0;
}

void V3_TMIDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_TMIDlgW, iPosY+g_TMIDlgH ) );
   m_Background.SetPos( FramePos( iPosX, iPosY, iPosX+g_TMIDlgW, iPosY+g_TMIDlgH ) );
   Drag_Button .SetPos( FramePos( iPosX, iPosY, iPosX+g_TMIDlgW, iPosY+g_TMIDlgH ) );
   m_BackMap   .SetPos( FramePos( iPosX+26, iPosY+27, iPosX+26+g_TMIDlgMAPWH, iPosY+27+g_TMIDlgMAPWH ) );
   m_stPosition.SetPos( FramePos( iPosX+33, iPosY+131,iPosX+113, iPosY+144) );
   m_PlayerPos .SetPos( FramePos( iPosX+72, iPosY+74,iPosX+72+4, iPosY+74+4) );
   
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_TMIDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(!g_SaveGame.GetOptionsParam()->bDisplayRadar)
   {
      Hide();
      return;
   }

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );

      int iNewPosX = m_iXPos + (mousePos.x-m_iXPosSD);
      int iNewPosY = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(iNewPosX < 0)
         iNewPosX = 0;

      if(iNewPosY < 0)
         iNewPosY = 0;
      
      if(iNewPosX+g_TMIDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_TMIDlgW;
      if(iNewPosY+g_TMIDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_TMIDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   DrawTMI();
   GameUI::Draw( vsfFX );
   if(g_SaveGame.GetOptionsParam()->bDisplayYouTMI)
      m_PlayerPos.Draw(vsfFX);
}


void V3_TMIDlg::Hide()
{ 
   BoxUI::Hide();
}

void V3_TMIDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogLastTMIXPos(),g_Global.GetDialogLastTMIYPos());
   BoxUI::Show();
}




void V3_TMIDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;

   
   if(!m_bDragDialog)
   {
      //si premier start drag, on prend la position de depart du drag
      //pour en calculer le deltat X et Y du deplacement de la fenetre...
      {
         
         m_bDragDialog = true;
         m_iXPosSD = mousePos.x;
         m_iYPosSD = mousePos.y;

         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_Button);
         g_Global.SetDraggingDialog(true);
      }
   }
}

void V3_TMIDlg::StopDragDlg()
{
   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      m_iXPos = m_iXPos + (mousePos.x-m_iXPosSD);
      m_iYPos = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(m_iXPos < 0)
         m_iXPos = 0;
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      if(m_iXPos+g_TMIDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_TMIDlgW;
      if(m_iYPos+g_TMIDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_TMIDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastTMIXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_TMIDlg::DragEvent::LeftClicked( void )
{
   V3_TMIDlg *me = static_cast< V3_TMIDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_TMIDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_TMIDlg *me = static_cast< V3_TMIDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}













void V3_TMIDlg::SetupViewObject()
{
   int i=0;
   for(i=0;i<__OBJGROUP_LASTGROUP;i++)
      m_bShowThisObjType[i] = FALSE;
   
   //Armor and items...
   for(i=1;i<=14;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=18;i<=103;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=107;i<=129;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=141;i<=142;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=163;i<=189;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=200;i<=213;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=235;i<=289;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=294;i<=329;i++)
      m_bShowThisObjType[i] = TRUE;
   
   for(i=421;i<=455;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=458;i<=644;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=646;i<=768;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=782;i<=790;i++)
      m_bShowThisObjType[i] = TRUE;
   for(i=869;i<=1066;i++)
      m_bShowThisObjType[i] = TRUE;

   for(i=1078;i<=1589;i++)
      m_bShowThisObjType[i] = TRUE;
   
   // ajoute les objets pouvant etre afficher...
}


void V3_TMIDlg::LoadTmiWorld(int iWorld,int dwX,int dwY)
{
   if(iWorld <0 ||  iWorld >NBR_TMI_MAP-1)
      iWorld = 0;

   if(iWorld == m_dwCurrentLoadedWorld)
      return;
   m_dwCurrentLoadedWorld = iWorld;

   
   FILE *pf1 = NULL;
   fopen_s(&pf1,m_TMIFileC,"rb");
   if(pf1 == NULL)
      fopen_s(&pf1,m_TMIFile,"rb");
   if(!pf1)
   {
      memset(m_pTMIMap,0x00,GPS_VIEW_X*GPS_VIEW_Y);
      memset(m_pTMIPal,0x00,256*3);
   }
   else
   {

      ULONG dwTailleOri;
      ULONG dwTailleComp[NBR_TMI_MAP];
      ULONG dwFilePos[NBR_TMI_MAP];

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

      fseek( pf1, dwFilePos[iWorld], SEEK_SET);

      BYTE *pFileDataTmpC = NULL;
      pFileDataTmpC= new BYTE[dwTailleComp[iWorld]];
      fread(pFileDataTmpC,1,dwTailleComp[iWorld],pf1);
      fclose(pf1);

      BYTE *pFileDataTmp = NULL;
      pFileDataTmp = new BYTE[dwTailleOri];

      if(uncompress((BYTE*)pFileDataTmp,&dwTailleOri,(BYTE*)pFileDataTmpC,dwTailleComp[iWorld]) != Z_OK)
      {
         memset(m_pTMIMap,0x00,GPS_MAP_X*GPS_MAP_Y);
         memset(m_pTMIPal,0x00,256*3);
      }
      else
      {
         BYTE *WPosMap = pFileDataTmp;
         BYTE *WPosPal = pFileDataTmp+(GPS_MAP_X*GPS_MAP_Y);
         memcpy(m_pTMIMap,WPosMap,GPS_MAP_X*GPS_MAP_Y);
         memcpy(m_pTMIPal,WPosPal,256*3);
      }

      //char strTmp[512];
      //sprintf_s(strTmp,512,"c:\\!!__GPSWorld%d.bmp",iWorld);
      //SaveCurrentImage8(m_pTMIMap,m_pTMIPal,GPS_MAP_X, GPS_MAP_Y,strTmp,false);

      delete []pFileDataTmp;
      pFileDataTmp = NULL;
      delete []pFileDataTmpC;
      pFileDataTmpC = NULL;

   }
   
}


void V3_TMIDlg::DrawTMI( void )
{
   CV2Sprite *pVSFInfo = m_BackMap.GetVSF();
   LPDIRECTDRAWSURFACE7 lpSpriteSurface = pVSFInfo->m_lpV2SpriteInfo->Data.lpDDSurface;
   if(lpSpriteSurface && CreateTMIMap(m_pTMIData,Player.xPos,Player.yPos,Player.World))
   {
      DDCOLORKEY ddColorKey;
      lpSpriteSurface->GetColorKey(DDCKEY_SRCBLT, &ddColorKey); 
      DDSURFACEDESC2 ddsd;
      memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
      ddsd.dwSize = sizeof(DDSURFACEDESC2);
      lpSpriteSurface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
      USHORT *pSource = (USHORT *)ddsd.lpSurface;
      if(!pSource)
      {
         lpSpriteSurface->Unlock(NULL);
         return;
      }
      WORD   *pTmiMap = m_pTMIData;


      int x, y;
      for (y = 0; y < pVSFInfo->m_lpV2SpriteInfo->Header.dwHeight; y++) 
      {

         for (x = 0; x < pVSFInfo->m_lpV2SpriteInfo->Header.dwWidth; x++) 
         {
            if (pSource[x] != ddColorKey.dwColorSpaceLowValue)
            {
               pSource[x] = pTmiMap[x];
            }
         }

         pSource += ddsd.lPitch>>1;
         pTmiMap+=g_TMIDlgMAPWH;
      }
      lpSpriteSurface->Unlock(NULL);
   }

   char strPos[30];
   sprintf_s(strPos,30,"%d,%d,%d",Player.xPos,Player.yPos,Player.World);
   m_stPosition.SetText(strPos,g_DefColorD,g_Global.GetScreenW(),0,false,2);
}

BOOL V3_TMIDlg::CreateTMIMap(WORD* pMap,USHORT xPos,USHORT yPos,USHORT World)
{
   int xSd = 0;
   int xEd = g_TMIDlgMAPWH;
   int ySd = 0;
   int yEd = g_TMIDlgMAPWH;

   int xSs    = xPos-g_TMIDlgMAPWHDiv2;
   int ySs    = yPos-g_TMIDlgMAPWHDiv2;
   int ySsOri = ySs;
   
   if(xPos -g_TMIDlgMAPWHDiv2 < 0)
   {
      xSd = g_TMIDlgMAPWHDiv2-xPos;
      xSs += xSd;
   }
   if(yPos -g_TMIDlgMAPWHDiv2 < 0)
   {
      ySd = g_TMIDlgMAPWHDiv2-yPos;
      ySs += ySd;
   }
   
   if(xPos +g_TMIDlgMAPWHDiv2 >= GPS_MAP_X)
      xEd = GPS_MAP_X-xPos+g_TMIDlgMAPWHDiv2;
   if(yPos +g_TMIDlgMAPWHDiv2 >= GPS_MAP_Y)
      yEd = GPS_MAP_Y-yPos+g_TMIDlgMAPWHDiv2;
   
   LoadTmiWorld(World,xSs,ySs);
   
   BYTE *pTMI  = m_pTMIMap;
   BYTE *pTMIP = m_pTMIPal;
   
   if(!pTMI)
      return TRUE; // ok, but draw no draw map...


   WORD   wColor = 0;
   WORD   wRed,wGreen,wBlue,wPal;
   BYTE   bRDown = lpDXDirectDraw->bRDown;
   BYTE   bGDown = lpDXDirectDraw->bGDown;
   BYTE   bBDown = lpDXDirectDraw->bBDown;
   BYTE   bRUp = lpDXDirectDraw->bRUp;
   BYTE   bGUp = lpDXDirectDraw->bGUp;
   BYTE   bBUp = lpDXDirectDraw->bBUp;

   CV2Sprite *pVSFInfo = m_BackMap.GetVSF();
   LPDIRECTDRAWSURFACE7 lpSpriteSurface = pVSFInfo->m_lpV2SpriteInfo->Data.lpDDSurface;
   DDCOLORKEY ddColorKey;
   lpSpriteSurface->GetColorKey(DDCKEY_SRCBLT, &ddColorKey); 

   int dwOffset = 0;

   for(int y=ySd ; y<yEd ; y++,ySs++)
   {
      BYTE *pS = pTMI + ((ySs*GPS_VIEW_X)+xSs);
      WORD *pD = pMap + y*g_TMIDlgMAPWH;
      int dwCnt = xSs;

      dwOffset = xSd;

      for(int x=xSd ; x<xEd ; x++)
      {
         wPal = pS[x-dwOffset]*3;
         wRed   = (pTMIP[wPal])  >>bRDown;
         wGreen = (pTMIP[wPal+1])>>bGDown;
         wBlue  = (pTMIP[wPal+2])>>bBDown;

         wColor = (wRed<<bRUp)+(wGreen<<bGUp)+(wBlue<<bBUp);
         pD[x] = wColor;
      }
   }



   //Object->Friendly == 1  // Autre Joueurs...
   //Object->Friendly == 2  // Monstres
   //Object->Friendly == 3  // PNJ
   //Object->Friendly == 4  // portes / Chest... 
   //Object->Friendly == 5  // Items au sol...
   //Object->Friendly == 6  // Pancarte...

   // Scann la liste des object pour en afficher sur la map TMI...
   Objects.Lock(2266);

   Objects.ToHead();
   TFCObject *Object;
   while(Objects.QueryNext()) 
   {
      Object = Objects.GetObject();

     
      if(Object->AbsX >  xSs && Object->AbsX < xSs+g_TMIDlgMAPWH-3 && 
         Object->AbsY >  ySsOri && Object->AbsY < ySsOri+g_TMIDlgMAPWH-3     )
      {
         int y = (Object->AbsY-ySsOri);
         int x = (Object->AbsX-xSs);

         x+=dwOffset;


         WORD *pD = pMap + y*g_TMIDlgMAPWH;

         
         BOOL bDraw = FALSE;
         if(Object->Type <10000)
         {
            if(Object->Type <__OBJGROUP_LASTGROUP && m_bShowThisObjType[Object->Type])
            {
               bDraw = TRUE;
               wRed   = (0)  >>bRDown;
               wGreen = (255)>>bGDown;
               wBlue  = (0)>>bBDown;
            }
         } 
         else if(Object->Type >10000 && Object->Friendly == 1)
         {
            //bDraw = TRUE;
            //wRed   = (255)  >>bRDown;
            //wGreen = (0)>>bGDown;
            //wBlue  = (0)>>bBDown;
         }
         else
         {
            bDraw = TRUE; 
            wRed   = (0)   >>bRDown;
            wGreen = (0)   >>bGDown;
            wBlue  = (255) >>bBDown;
         }

         if(bDraw)
         {
            wColor = (wRed<<bRUp)+(wGreen<<bGUp)+(wBlue<<bBUp);
            pD[x]   = wColor;
            pD[x+1] = wColor;
            pD[x+2] = wColor;
            y++;
            pD = pMap + y*g_TMIDlgMAPWH;
            pD[x]   = wColor;
            pD[x+1] = wColor;
            pD[x+2] = wColor;
            y++;
            pD = pMap + y*g_TMIDlgMAPWH;
            pD[x]   = wColor;
            pD[x+1] = wColor;
            pD[x+2] = wColor;
         }
      }
   }
   Objects.Unlock(2266);

   return TRUE;
}
