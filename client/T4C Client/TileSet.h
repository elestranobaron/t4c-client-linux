// **************************************************************************
// ***                                                                    ***
//      File Name: TileSet.H
//      Project:   The Fourth Coming
//      Creation:  May 30th, 1997
//      Author:    Benoit Thomas (TH), Tim Sprenger (TS)
// ***                                                                    ***
// **************************************************************************
// ***                                                                    ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         05-30-1997      1.0     TH          Initial development.
//         01-08-2018      1.1     TS          Added XaradonObj for new Objects 
//		  Fonctions:
//			  Provide TileSet Basic.
//
// ***                                                                    ***
//	**************************************************************************
// *** Copyright (c) 1996-1997 Vritual Dreams (r). All rights reserved.   ***
 
#ifndef __Virtual_Dreams_TileSet_092819053097
#define __Virtual_Dreams_TileSet_092819053097
 
#include "Sprite.h"
#include "Random.h" 
#include "V2Sprite.h"
#include "cs.h"
#include "Icon3D.h"
#include "TFCLandDef.h"


//#define DONE 4
//#define MOVX 8
//#define MOVY 4


//#define DONE 4
//#define MOVX 8
//#define MOVY 4

extern int g_DONE;
extern int g_MOVX;
extern int g_MOVY;

//#define DONE 8
//#define MOVX 4
//#define MOVY 2
//#define FPS  32

#define SKILLS 0
#define STATS 1
#define OBJECTS 2
#define EQUIPMENTS 3
#define TRAIN_SKILL 4

enum TileSetError { NOT_DEFINED_VIEW, FILE_NOT_FOUND };

template <class type>
class TestArray {
   private:
       type *Array;
       DWORD MaxAddr;

   public:
      TestArray(void) {
         Array = NULL;
      }

      ~TestArray(void) {
         if (Array)
            delete Array;
      }
      
      void Alloc(DWORD alloc) {
         Array = new type [alloc];
         MaxAddr = alloc;
      }
      
      type &operator [] (DWORD addr) {
         if (addr > MaxAddr) {
            exit(0);
         }
         return Array[addr];
      }

};

class TileSetException {
	public:
		TileSetException(TileSetError);
};

class GraphicStructure {
   public:
      int iCorner, iBorder, iWall, iDarkWall, 
          iExcp1, iDarkExcp1,
          iExcp2, iDarkExcp2,
          iExcp3, iDarkExcp3,
          iExcp4, iDarkExcp4,
          iExcp5, iDarkExcp5;
      int iG;
      int iE;
      int WallSet;
      char *Name;
      void Load(void);
      void Draw(int, int, int, RECT *, RECT *, RECT *, LPDIRECTDRAWSURFACE7, int);
};

const int MAXCUSTOMCORNER    = 10;
const int MAXCUSTOMBORDER    = 10;
const int MAXCUSTOMWALL      = 10;
const int MAXCUSTOMDARKWALL  = 10;
const int MAXCUSTOMEXCP1     = 10;
const int MAXCUSTOMDARKEXCP1 = 10;
const int MAXCUSTOMEXCP2     = 10;
const int MAXCUSTOMDARKEXCP2 = 10;
const int MAXCUSTOMEXCP3     = 10;
const int MAXCUSTOMDARKEXCP3 = 10;
const int MAXCUSTOMEXCP4     = 10;
const int MAXCUSTOMDARKEXCP4 = 10;
const int MAXCUSTOMEXCP5     = 10;
const int MAXCUSTOMDARKEXCP5 = 10;

class TileSet {
	private:
      bool StaticSave;
      bool StaticState;
   public:
      BYTE FadeLevel;
	  CriticalSection csLock;
	  bool SetFading;
	  bool RealFading;
		char *XPString, *MPString, *HPString;
	private:
	
		RandomTable<215> Rnd;
		
		int Middle;

		int a, b;
		
		char  TileWidth;
		char  TileHeight; 
		char  ViewWidth;
		char  ViewHeight;
		char  SizeWidth;
		char  SizeHeight;
   public:
      short VirtualWidth;
		short VirtualHeight;
   private:
		short WorldWidth;
		short WorldHeight;
	
      TestArray<unsigned short> CompiledView1;
      TestArray<unsigned short> CompiledView2;
      TestArray<unsigned short> CompiledViewA;
		unsigned short *VirtualView1;
      unsigned short *VirtualView2;
      unsigned short *VirtualViewA;
      unsigned short *VirtualViewM;
      unsigned short *VirtualViewL;
      unsigned short *VirtualViewX;

		VOID (* flpLoadSprite[LASTONE])(VOID);
		GraphicStructure *flpLoadSprite2[LASTONE];
		GraphicStructure *flpDeletePtr[LASTONE];
      bool              m_bflpLoadSprite[LASTONE];
		int iNbGSPtr;

		char WorldFileName1[8][256];
      char WorldFileName2[8][256];
      char WorldFileNameA[8][256];
      char WorldFileNameM[8][256];
      char WorldFileNameL[8][256];
      char WorldFileNameX[8][256];
      char WorldFileNameC1[8][256];
      char WorldFileNameC2[8][256];
      char WorldFileNameCA[8][256];
      char WorldFileNameCM[8][256];
      char WorldFileNameCL[8][256];
      char WorldFileNameCX[8][256];
   public:
		char NumberofWorld;
   private:
		char CurrentWorld;

      int    m_dwFogX[15];
      int    m_dwFogY[15];
      float  m_fFoyY[15];
      WORD   m_wFogStatus;
      WORD   m_wFogAlphaVal;
      ULONG  m_dwFogCntGlobal;

      WORD   m_wRainStatus;
      WORD   m_wRainAlphaVal;
      ULONG  m_dwRainCntGlobal;

      WORD   m_wSnowStatus;
      WORD   m_wSnowAlphaVal;
      ULONG  m_dwSnowCntGlobal;

	public:
		LPDIRECTDRAWSURFACE7 lpPreLightSurface;
      //LPDIRECTDRAWSURFACE7 lpPreLightSurfaceBackup;
      LPDIRECTDRAWSURFACE7 lpPreLightSurfaceEffect2;
		LPDIRECTDRAWSURFACE7 lpSmoothSurface2;
		LPDIRECTDRAWSURFACE7 lpSmoothTemp;
	private:
		static LPDIRECTDRAWSURFACE7 lpFractalSurface;

		RECT ScreenDest;
	public:
		RECT SmoothSource, SmoothSource2;
	private:
		RECT FractalSource[1];
		RECT SurfaceDest;
		RECT SurfaceSource;
		RECT SourceRect[4];
		RECT DestRect[4];

	public:
		int Done, Done2;
	private:
	public:

      WORD *m_pSmoothTileTmpR;
      WORD *m_pSmoothTileTmpG;
      WORD *m_pSmoothTileTmpB;

		int MovX,    MovY, MovX2, MovY2;
		int SepX,    SepY, SepX2, SepY2;
		int OldSepX, OldSepY;
		int xPos,    yPos, wPos; 

      #define T_EARTH_X  32 // 11
      #define T_EARTH_Y  32 // 10
      #define T_NGRASS_X 32 // 13
      #define T_NGRASS_Y 32 // 15
	   #define T_DGRASS_X 32 // 11
      #define T_DGRASS_Y 32 // 10

      
      #define T_WATER1_X 32 // 10
      #define T_WATER1_Y 32 // 9
	   #define T_WATER2_X 32 // 10
      #define T_WATER2_Y 32 // 9
      #define T_MOUNT_X  32 // 10
      #define T_MOUNT_Y  32 // 9


      #define T_FOREST_X 32 // 11
      #define T_FOREST_Y 32 // 11
      #define T_SAND_X   32 // 2
      #define T_SAND_Y   32 // 2


      #define T_ROCK_X 16 // 3
      #define T_ROCK_Y 16 // 3
      #define T_OLDROCK_X 4 // 2
      #define T_OLDROCK_Y 4 // 2
      #define T_SNOW_X 32//16 
      #define T_SNOW_Y 32//16 
      #define T_EARTHI_X 16 
      #define T_EARTHI_Y 16 
      #define T_CAVERNG_X 16 
      #define T_CAVERNG_Y 16 
      #define T_ICE_X 16 
      #define T_ICE_Y 16 

      #define T_2WOODEN_X 8 
      #define T_2WOODEN_Y 8 

      #define T_3WOODEN_X 8 
      #define T_3WOODEN_Y 8 
      
      #define T_TUILE1_X 16 
      #define T_TUILE1_Y 16 
      #define T_TUILE2_X 16 
      #define T_TUILE2_Y 16 

      #define T_STUILE1_X 8 
      #define T_STUILE1_Y 8 
      #define T_STUILE2_X 8 
      #define T_STUILE2_Y 8 

      #define T_ROCK2_X 16 
      #define T_ROCK2_Y 16 

      #define T_AWATER01_X 16 
      #define T_AWATER01_Y 16 

      //#define T_ALAVA01_N 63
      //#define T_ALAVA01_X 16 
      //#define T_ALAVA01_Y 16 

      #define T_CENDRE_X 32 
      #define T_CENDRE_Y 32 

      #define T_NLAVA_X 32 
      #define T_NLAVA_Y 32 

      #define T_NMS_FX 32 
      #define T_NMS_FY 32 

      #define T_NMS_FRPX 16
      #define T_NMS_FRPY 16 



      //CV2Sprite NMSCendre[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSDesert[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSEarth[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSEarthForest[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSGrass[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSOcean01[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSOcean02[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSOcean03[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSPave01[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSSand[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSSandRoad[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSSnow[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSSnowRoad[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSPave02[T_NMS_FY][T_NMS_FX];
      //CV2Sprite NMSRiverNo01[T_NMS_FY][T_NMS_FX];
      CV2Sprite NMSDungeonGrass01[T_NMS_FY][T_NMS_FX];
      CV2Sprite NMSSky[T_NMS_FY][T_NMS_FX];
      CV2Sprite NMSFog[T_NMS_FY][T_NMS_FX];


      CV2Sprite NMSFRP_NWood01[T_NMS_FRPX][T_NMS_FRPY];
      CV2Sprite NMSFRP_NWood02[T_NMS_FRPX][T_NMS_FRPY];
      CV2Sprite NMSFRP_NWood03[T_NMS_FRPX][T_NMS_FRPY];
      CV2Sprite NMSFRP_NWood04[T_NMS_FRPX][T_NMS_FRPY];
      //NMS FLOOR REPLACEMENT NEW
      CV2Sprite NMSFRP_RockTown0[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_RockTown1[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_RockTown2[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_RockTown3[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveSolidO[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveMiSolidO[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveLiquidO[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveSolid[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveMiSolid[T_NMS_FRPX][T_NMS_FRPY],
                NMSFRP_LaveLiquid[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Grass2Flower[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Grass2Low[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Grass2High[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Grass2LowD[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Grass2HighD[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Earth2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Earth2Forest[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Beach2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_MountainHigh[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_MountainLow[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Ocean2High[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Ocean2Low[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Ocean2Border[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_River2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Snow2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Snow2Foot[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Earth2Frozen[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Desert2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Ocean2Frozen[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_MountainGravel[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN01[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN02[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN03[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN04[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN05[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN06[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN07[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_WoodFloorN08[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileBlackN1[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileBlackN2[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileBlackN3[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileBlueN1[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileBlueN2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileBlueN3[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileGreenN1[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileGreenN2[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileGreenN3[T_NMS_FRPX][T_NMS_FRPY],  
                NMRPF_TuileRedN1[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileRedN2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileRedN3[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileSandN1[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileSandN2[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_TuileSandN3[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon01br[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon01gr[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon01brb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon01grb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon02br[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon02gr[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon02brb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon02grb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon03br[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon03gr[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon03brb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon03grb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon04br[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon04gr[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon04brb[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Dungeon04grb[T_NMS_FRPX][T_NMS_FRPY],
                NewCavern[T_NMS_FRPX][T_NMS_FRPY],
                NMRPF_Swomp[6][T_NMS_FRPX][T_NMS_FRPY];




      CV2Sprite NMSFalaise01[3],
                NMSFalaise01M[3],
                NMSFalaise01MH[3],
                NMSFalaise01MUP[2],
                NMSFalaise01MUPF[2],
                NMSFalaise01MUPL[2],
                NMSFalaise01MUPLF[2];
      CV2Sprite NMSFalaise02[29];
      CV2Sprite NMSRiverBorder01[21];
      CV2Sprite NMSRiverBorder02[21];
      CV2Sprite NMSRiverBorder01Mask[8];
      CV2Sprite NMSDecorsPont01[4];
      CV2Sprite NMSDecorsLPont01[4];
      CV2Sprite NMSDecorsDPont01[4];
      CV2Sprite NMSDecorsWPont01[1];
      CV2Sprite NMSDecorsLPont02[4];
      CV2Sprite NMSDecorsMont[10];
      CV2Sprite NMSDecorArbre[28];
      CV2Sprite NMSDecorArbuste[12];
      CV2Sprite NMSDecorCloture01[4];
      CV2Sprite NMSDecorRoche[5];
      CV2Sprite NMSDecorPuits[1];
      CV2Sprite NMSDecorMisc[3];
      CV2Sprite NMSCerberusPod[1];
      CV2Sprite NMSCerberusPodBrooken[3];
      CV2Sprite NMSSkeletonCorpseFloor[7];
      CV2Sprite NMSHighGrass02C1[12];
      CV2Sprite NMSHighGrass02C2[12];
      CV2Sprite NMSBleC1[13];
      CV2Sprite NMSMonsterDeath[10];
      CV2Sprite NMSCerberusDeath[2];
      CV2Sprite NMSNewCrypt01[2]; //left and right
      CV2Sprite NMSRiverWall01[33];
      CV2Sprite NMSR_FalaiseRiver[12];
      CV2Sprite NMSR_FalaiseRiverM[12];
      CV2Sprite NMSR_QuaiR[4];
      CV2Sprite NMSR_QuaiL[4];
      CV2Sprite NMSR_QuaiEsc[4];

      //CV2Sprite NMSRiverWall01Mask[33];

	   //CV2Sprite  NewMountainRock[T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewWater1      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewWater2      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewEarth       [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewDesert      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewGrass       [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewForest      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewDGrass      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewRock2       [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewSnow        [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewEarthI      [T_NMS_FRPY][T_NMS_FRPX],
      //CV2Sprite  NewIceFloor    [T_NMS_FRPY][T_NMS_FRPX];
      

      CV2Sprite  NewNMSBar01,NewNMSBar02;

     //OBSOLETE
         //RivWat[6]
	  
	  CV2Sprite   Rock[7][7],RockFloor2[T_ROCK2_Y][T_ROCK2_X], Floor[6],Floor2[T_2WOODEN_Y][T_2WOODEN_X],Floor3[T_3WOODEN_Y][T_3WOODEN_Y], GraRoc2[4], GraRoc[4], Tree[43], 
				  CavernG[T_CAVERNG_Y][T_CAVERNG_X],
				  TuileTmps1 [T_TUILE1_Y] [T_TUILE1_X] ,TuileTmps2 [T_TUILE2_Y] [T_TUILE2_X],
				  STuileTmps1[T_STUILE1_Y][T_STUILE1_X],STuileTmps2[T_STUILE2_Y][T_STUILE2_X],
				  Lava[10], Ice[10], Object[259], Grave[10], FlipDungeonRockWall[5],Moulin[2],FountainBlood,XaradonObj[12],XaradonNewTrees[21],XaradonNewBuissons[11],XaradonDeadThings[60],
              FirepitA[4],
				  SapinNoel,SapinNoelA,tAnimGRass[3],CerberusPodAnimatedFixe,
				  WhiteWall[31], Haie[7], Cavern[14][14], Rampart[7], 
				  CavernWall[44],GCavernWall[44], BrickWall[31], DungeonRockWall[42], 
				  SRockFloor[4],SRockFloor2[4],SRockFloor3[4],SRockFloor4[4],SRockFloor5[4],SRockFloor6[4], Mountain[6], DungeonFloor[5][5], DungeonWall[28],
				  DrakeSkeleton[4],Boat01[14],NMSDecors[3],NMSHighGrass[6],
				  EscalierObject[6],
				  DungeonWallSpec[5], FlipDungeonWall[5], FlipDungeonWallSpec[5], DungeonLight[5][5],
				  FlipDungeonLight[5][5], DungeonWallSpec2[5], FlipDungeonWallSpec2[5], 
				  DungeonWallSpec3[5], FlipDungeonWallSpec3[5], DungeonOver1[10][10], DungeonOver2[15][15],
				  DungeonRockFloor[6][7], DungeonRockWallSpec[5], FlipDungeonRockWallSpec[5],
				  CastleWall[18],V2CastleWall[28],V2CastleWallS[21],V2CastleRampart[11], FlipCastleWall[5], DungeonRockWall2[42], FlipDungeonRockWall2[5],V2CastleWall2[29], 
				  NMSRockWall01[39],NMSBanc01[2],NMDungeon01[38],
				  NMSCollonne01[9],NMSDechuStatut01[2],NMSHotelAxeBlood01[2],NMSBibli01[4],NMSAlchemieDecors[22],
				  NStucoWall[45],NMurPoutre[7],NTourGarde[4],
				  NBrickWl01[56],
				  DungeonRockWall3[42], FlipDungeonRockWall3[5], DungeonRockWall4[42], /*GrassBreak2[7][5],*/
				  FlipDungeonRockWall4[5], DungeonRockWall2Spec[5], FlipDungeonRockWall2Spec[5],
				  DungeonRockWall3Spec[5], FlipDungeonRockWall3Spec[5], DungeonRockWall4Spec[5], 
				  FlipDungeonRockWall4Spec[5],  OldRock[T_OLDROCK_Y][T_OLDROCK_X], Troll[5], RuinedBrickWall[31],
				  RuinedWhiteWall[43],WoodWall[19],NBrickWall[17],NBrickWallS[21],NBrickWallS2[17],NBrickWallS3[17],NBrickWallS4[17],NBrickWallS5[17],NBrickWallS6[17],
				  NBrickWallRS3[5],NBrickWallRS4[5],NBrickWallRS5[5],NBrickWallRS6[5],
				  *CustomCorner[MAXCUSTOMCORNER], *CustomWall[MAXCUSTOMWALL], *CustomDarkWall[MAXCUSTOMDARKWALL],
				  *CustomExcp1[MAXCUSTOMEXCP1],  *CustomDarkExcp1[MAXCUSTOMDARKEXCP1], *CustomBorder[MAXCUSTOMBORDER],
				  *CustomExcp2[MAXCUSTOMEXCP2],  *CustomDarkExcp2[MAXCUSTOMDARKEXCP2],
				  *CustomExcp3[MAXCUSTOMEXCP3],  *CustomDarkExcp3[MAXCUSTOMDARKEXCP3],
				  *CustomExcp4[MAXCUSTOMEXCP4],  *CustomDarkExcp4[MAXCUSTOMDARKEXCP4],
				  *CustomExcp5[MAXCUSTOMEXCP5],  *CustomDarkExcp5[MAXCUSTOMDARKEXCP5];

     CV2Sprite NTempleGR[41];
     CV2Sprite NTempleBE[41];
     CV2Sprite NTempleDGR[41];
     CV2Sprite NTempleDBL[50];
     CV2Sprite NWhW[35];
     CV2Sprite NBrW[35];
     CV2Sprite NWhWb[18];
     CV2Sprite NBrWb[18];
     CV2Sprite NWD01h[31];
     CV2Sprite NGG01[46];
     CV2Sprite NSH01h[29];
     CV2Sprite NSH02h[29];
     CV2Sprite NSH03h[29];
     CV2Sprite NSH04h[29];
     CV2Sprite NSH05h[29];
     CV2Sprite NSH01hBr[14];
     CV2Sprite NSH02hBr[14];
     CV2Sprite NSH03hBr[14];
     CV2Sprite NSH04hBr[14];
     CV2Sprite NCastle[86];
     CV2Sprite NCim01[19];
     CV2Sprite NCWood01[19];
     CV2Sprite NCWood02[19];
     CV2Sprite NCrypt01[146];
     CV2Sprite NCrypt02[146];
     CV2Sprite NDungeon01[103];
	 CV2Sprite TemplGRfix[9];
	 CV2Sprite TemplBEfix[9];
	 CV2Sprite TemplDGRfix[9];
	 CV2Sprite Crypt2fix[15];
   
     
     CV2Sprite  WoodLanterne01[3];
     CV2Sprite  WoodLanterne01F[3];
	  CV2Sprite  WoodLanterne02[3];
	  CV2Sprite  WoodLanterne02F[3];
     CV2Sprite  IronLanterne01[3];
     CV2Sprite  IronLanterne02[3];
     CV2Sprite  IronLanterne03[3];


     CV2Sprite NNTreeForest[48];
     CV2Sprite NNTreeForestD[24];
     CV2Sprite NNTreeForestS[24];

     CV2Sprite NNTreePalmier[40];
     CV2Sprite NNTreeSpecial[21];

     CV2Sprite NNTreeSapin[19];
     CV2Sprite NNTreeSapinS[6];

     CV2Sprite NNArbuste[19];
     CV2Sprite NNPlantForest[23];

     CV2Sprite NewNMSBarOK[4];
     CV2Sprite HFloorBlee[12];
     CV2Sprite HFloorLGrass[12];
     CV2Sprite HFloorHGrass[12];
     CV2Sprite HFloorLDGrass[12];
     CV2Sprite HFloorHDGrass[12];

     CV2Sprite HFloorLFlower01[8];
     CV2Sprite HFloorLFlower02[8];
     CV2Sprite HFloorLFlower03[8];
     CV2Sprite HFloorLFlower04[8];
     CV2Sprite HFloorLFlower05[8];
     CV2Sprite HFloorHFlower01[8];
     CV2Sprite HFloorHFlower02[8];
     CV2Sprite HFloorHFlower03[8];
     CV2Sprite HFloorHFlower04[8];
     CV2Sprite HFloorHFlower05[8];
     CV2Sprite NCactusSerie01[6];
     CV2Sprite NCactusSerie02[6];
     

     
	  
      //////////////////////
      // NightMare --- Add  2005-03-17
      // Add Weather CV2Sprite Sprite Attribute
      CV2Sprite m_VSFWeatherRainA01;
      CV2Sprite m_VSFWeatherRainAM01;
      CV2Sprite m_VSFWeatherRainA02;
      CV2Sprite m_VSFWeatherRainAM02;
      CV2Sprite m_VSFWeatherRainA03;
      CV2Sprite m_VSFWeatherRainAM03;
      CV2Sprite m_VSFWeatherRain;
      CV2Sprite m_VSFWeatherLightning[2];

      CV2Sprite m_VSFWeatherSemiTransFog;

      CV2Sprite m_VSFWeatherSnow1;
      CV2Sprite m_VSFWeatherSnow2;
      CV2Sprite m_VSFBlackTile;
      CV2Sprite ArealinkS;
      CV2Sprite ArealinkD;

      //END NightMare
      //////////////////////

	  // Dialsoft new floors
		 

         
		int iCustomCorner, iCustomWall, iCustomDarkWall, iCustomBorder, 
          iCustomExcp1, iCustomDarkExcp1,
          iCustomExcp2, iCustomDarkExcp2,
          iCustomExcp3, iCustomDarkExcp3,
          iCustomExcp4, iCustomDarkExcp4,
          iCustomExcp5, iCustomDarkExcp5;
      Sprite Border[6];
		Sprite Pp1, Pp2;
      Sprite m_SpriteSmooth;


      int m_iLastXLoad;
      int m_iLastYLoad;
      int m_iLastXLoadDX;
      int m_iLastYLoadDY;

		bool DrawTileSet (int xScreen, int yScreen, int, RECT *Clip, RECT *OutBound, RECT *Wrap, bool Chk, LPDIRECTDRAWSURFACE7 lpSurface, bool Overlap, BOOL ParamDither,UINT uiID=0,UINT uiAnimCount=0);
      bool DrawTileSetA(int xScreen, int yScreen, USHORT, RECT *Clip, RECT *OutBound, RECT *Wrap, bool Chk, LPDIRECTDRAWSURFACE7 lpSurface, bool Overlap, BOOL ParamDither,int iX,int iY, int iW,UINT uiID=0,UINT uiAnimCount=0);
		void LoadVirtualMap(int, int, int);
		void ReloadMap(void);
		void CompileView(int);
      int  GetPosView1(int iX,int iY);
      int  GetPosViewM(int iX,int iY);
      int  GetPosViewL(int iX,int iY);
      int  GetPosViewX(int iX,int iY);
   protected :
      void ValidFogMoving();

	public:
		Sprite Bug[1];				 
		
		TileSet(void);
		~TileSet(void);

		void SetTileDimension   (int, int);
		void SetViewDimension   (int, int);
		void SetVirtualDimension(int, int);
		void SetWorldDimension  (int, int);
		void SetWorldFileName   (char *,char *);
		void SetNumberofWorld   (char);

		void SetPosition        (int, int, int, BOOL = TRUE,BOOL ForceName = TRUE);
		int  MoveToPosition     (int, int);

		void UpdateView (bool Draw, bool, int, int,bool);
		void CreateView (void);
      void CreateSmooth(void);


		void Resize(int x, int y);

		void DrawOverlapFct(int x, int y, int XC, int YC, int xPos, int yPos, Sprite *Over, int ID, INFO *Sp,UINT uiID);
      void DecorDrawOverlap(int x, int y, int XC, int YC, int xPos, int yPos, LPDIRECTDRAWSURFACE7 lpBlitSurface, int ID, INFO *Sp,UINT uiID,RECT *pRc);
      void DecorDrawOverlapID(int x, int y, int XC, int YC, int xPos, int yPos, LPDIRECTDRAWSURFACE7 lpBlitSurface, int ID, INFO *Sp,UINT uiID,RECT *pRc);
		void Switch(void);
      void Switch2(void);

		void Lock(int);
		void Unlock(int);

		void GetHeight(int, int, int *, int *, signed char *);

      void ChangeStatic(void);
      void Save(void);
      void Restore(void);

      void Register(int, char *, int, int);
      void Mathieu(void);
      void ReleaseSurface();

      CV2Sprite *GetSpriteID(int iID);
};

extern TileSet World;

#endif