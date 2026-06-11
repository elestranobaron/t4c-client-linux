// VisualObjectList.h: interface for the VisualObjectList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALOBJECTLIST_H__003154E2_FA13_11D0_BEAA_444553540000__INCLUDED_)
#define AFX_VISUALOBJECTLIST_H__003154E2_FA13_11D0_BEAA_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TemplateList.h"
#include "Icon3D.h"
#include "V2Sprite.h"
#include "TFCObject.h"
#include "Apparence.h"
#include "cs.h"
#include "BasicSprite.h"
#include "GameMusic.h"
#include "LightMap.h"
#include "Tileset.h"
#include "Mouse.h"


#define VOL_PLAYER 1
#define VOL_NPC 2
#define VOL_MONSTER 3
#define VOL_OBJECT 4
#define VOL_MINIONS 5

#define VOL_CANNOTTALK 0x0
#define VOL_CANTALK 0x1
#define VOL_ISPLAYER 0x2
#define VOL_ISMINIONS 0x3



class BACK {
public:
	int nbObjects;
	int Type;
	int ID;
	int Def;
	int Time;
	char Name[100];
};
 
#define LOCK_OBJECTS(s) Objects.Lock(s);

//NMNMNM
//#define StandardSprite(obj) (obj ? &Objects.VObject[obj] : &Objects.VObject[274])
#define StandardSprite(obj) (obj ? GetVSFObject(obj) : GetVSFObject(274)])
#define UNLOCK_OBJECTS(s) Objects.Unlock(s);

#define NBBASICSPRITE 350

struct FXType {
   DWORD Time;
   T3VSBSound Sound;
	BOOL Debug;
};




class VisualObjectList : public TemplateList<TFCObject> 
{

protected:
   CV2Sprite       *pVObject;
   CV2Sprite       *pVObjectAttack;
   //NM:NM://CV2Sprite       pVObject[__OBJGROUP_LASTGROUP+__OBJANIM_LASTGROUP];

   CV2Sprite       m_v2SpriteLifeS_V3[50];
   CV2Sprite       m_v2SpriteLifeSM_V3[50];
   CV2Sprite       m_v2SpriteLifeB_V3[50];
   CV2Sprite       m_v2SpriteLifeBM_V3[50];


   CV2Sprite       m_v2SpritePShadow;
   CV2Sprite       m_v2SpriteNMSTAG[NMS_TAG_MAX_TAG];
   CV2Sprite       m_v2SpriteSmiles[NMS_SMILES_MAX];
   CV2Sprite       m_NMCombatMode1[12];
   CV2Sprite       m_NMCombatMode2[12];
   CV2Sprite       m_NMCombatMode3[12];
   int             m_iFrameCombatMode1;
   int             m_iFrameCombatMode2;
   int             m_iFrameCombatMode3;
   int             m_iSubFrameCombatMode1;
   int             m_iSubFrameCombatMode2;
   int             m_iSubFrameCombatMode3;

public:
 
	LightMap lmPlayerLightR;
   LightMap lmChandelleLight;
   LightMap lmOtherPlayerLight;
   LightMap lmTorchLight;
   LightMap lmTorchLanterne1;
   LightMap lmTorchPoutre;
   LightMap lmTorchChandelle2;
   LightMap lmCerberusPod;
   LightMap lmCerberusPodAnim;
   LightMap lmLanterneWood01;
   LightMap lmLanterneWood02;
      
   
   CV2Sprite*       GetVSFObject(int dwIndex);
   CV2Sprite*       GetVSFObjectAttack(int dwIndex);
   
	BasicSprite *VObject3D[NBBASICSPRITE];
	bool      Object3DLoad[NBBASICSPRITE];
   WORD      Object3DSound[NBBASICSPRITE][12];
	bool      DoUnlock;
	TemplateObject<class TFCObject> * SavePtr;
   ListStatus SaveQuery;
   TemplateList <struct FXType> SoundFXList;
   CriticalSection csLock;
   BOOL NoMove;

   short SDID;
   typedef struct _SDObject
   {
      short SDObjX;
      short SDObjY;
      short SDObjW;
      short SDObjType;
      short SDObjID;
      char  SDObjLight;
   }SDObject;
 
   std::vector< SDObject > m_vSDObject;


   TFCObject MainObject;
   BOOL      m_bAskForSkinChange;
   UINT      m_dwNewSkin;
   
	
	signed   char  Direction2;
	int            Set;
	short          nbUnit;
	unsigned char  ViewWidth, ViewHeight;
	unsigned char  RangeWidth, RangeHeight;

	unsigned long LastAttempt;
   unsigned long LastAttemptIDAll;


   short m_animObjNbr;
   short m_animObjID;

   typedef struct _sanimObj
   {
      short SDObjX;
      short SDObjY;
      short SDObjW;
      short SDObjType;
      short SDObjID;
      char  SDObjLight;
   }sanimObj;
   sanimObj *m_psAnimObj;

   typedef struct _sAnimOverlap
   {
      UINT  uiID;
      UINT  uiIDSprite;
      short ObjX;
      short ObjY;
      RECT  rcClip;
   }sAnimOverlap;

   std::vector< sAnimOverlap > m_sAnimOverlap;



   LPV2SPRITEFX m_pVsfFX;
   LPRECT       m_pClip;


public:
   void SDAnimClear(void);
   void SDAnimAddObject(short,short,short,char,short);
   void ClearAnimID(UINT id);

   void AddOverlapAnim(UINT,UINT,short,short,RECT);
   int  FoundAnimListID(UINT dwID);
   int  FoundAnimListIDInClip(UINT dwID,std::vector<int>& rIDs);
   void CreateTorch();

public:
	VisualObjectList();
	virtual ~VisualObjectList();
   void FirstInitObjects();
    
    TFCObject *GetMainObject();
    int GetWhirpoorFromFloor(int iX,int iY);
	//void Add(unsigned long  ID, unsigned short Type, unsigned short AbsX, unsigned short AbsY, signed char Light, unsigned char Friend, unsigned long AttachID, char pHp, char CombatMode, char chHiddenInv2, char defaultDirection = 2, bool allowOutOfBound = false, long spellEffectID = 0, long spellChildID = 0 );
   void Add(unsigned long  ID, 
            unsigned short Type, 
            unsigned short AbsX, 
            unsigned short AbsY, 
            signed char Light, 
            unsigned char Friend, 
            unsigned long AttachID, 
            char pHp, 
            char CombatMode,
            char chHiddenInv2,
            char defaultDirection/* = 2*/, 
            bool allowOutOfBound/* = false*/, 
            long spellEffectID/* = 0*/, 
            long spellChildID/* = 0 */);


   void CreateVsfFX();
   void DeleteVsfFX();
   void ClearGrid();
   

   void MovePlayer(signed char X, signed char Y,bool bForceOverlay = false);
	void DrawObject    (int XCor, int YCor, bool Draw, LPDIRECTDRAWSURFACE7, bool Flag, bool, bool,bool);
   void DrawObjectSelect(int XCor, int YCor, bool Draw, LPDIRECTDRAWSURFACE7 lpBlitSurface, bool Flag, bool bDrawText, bool bNewInt,bool);
   void DrawObjectTAG(int XCor, int YCor, bool Draw, LPDIRECTDRAWSURFACE7 lpBlitSurface, bool Flag, bool bDrawText, bool bNewInt);
   void DrawObjectAnim(int XCor, int YCor, bool Draw, LPDIRECTDRAWSURFACE7, bool Flag, bool, bool,bool); 
   void DrawObjectAnimOverLapID(int ID,bool bOffset,int XCor, int YCor,LPDIRECTDRAWSURFACE7) ;
	void Create(void);
	unsigned short Found(signed char X, signed char Y);
	unsigned long FoundID2(signed char X, signed char Y);
	void SetView(int ViewWidth, int ViewHeight);
	void SetRange(int ViewWidth, int ViewHeight);
	void MoveObject(unsigned long, 
                   signed long, 
                   signed long, 
                   BOOL, 
                   unsigned long, 
                   char,
                   char,
                   char, 
                   unsigned short, 
                   signed char, 
                   unsigned char, 
                   bool,
                   bool,DWORD);
	void Delete(unsigned long ID);
   void DeleteType(unsigned long ID);
	int  ChangeType(unsigned long ID, unsigned short Type,unsigned long ulDead);
	int  ChangePlayerType(unsigned short Type);
   void IsNeedChangeSkin();
	int  Modify(signed char X, signed char Y);
	void Text(unsigned long, char *, unsigned int, unsigned char, char *);
	void Anim(unsigned long, char);
	int  RealPos(signed char X, signed char Y, signed char *NX, signed char *NY);
	void SetGrid(TFCObject *);
   void SetGridOld(TFCObject *);
   void SetGridNew(TFCObject *);
	void Sort(void);
	void SetSet(BOOL);
	BOOL Found(signed char *xDif, signed char *yDif, unsigned long FollowID);
	BOOL FoundValidID(signed char *xDif, signed char *yDif, unsigned long FollowID);
	int  SetDirection(unsigned long, unsigned long, bool = false);
	void LoadObject(int);
	void Release(void);
	void CreatePlayer(void);
	void SetTotalGrid(void);
	void MoveObject2(Deplacement *, BOOL = FALSE);
	void SetExtended(signed char X, signed char Y);
	int  ChkText(void);
	void SetAttack(unsigned long, unsigned long, char, char, char, DWORD forcedDirection = 0 );
	void SetOffset(unsigned long, short);
	void SetName(unsigned long, char *, BOOL DisplayNow, DWORD color = CL_RED, short shLevel = 0);
	void SetGuildName(unsigned long, char *, DWORD color);
	T3VSBSound *GetSound(unsigned long);
	void Identify(unsigned long ID);
   void IdentifyAll();
   void SetDamageTxt(unsigned long ID, char* pstrDamage,DWORD dwColor);
	void NoIdentify(unsigned long ID);
//	void Definition(BACK *, int, int);
	void OrderObject(void);
   void DeleteAll(void);
   void DrawOverlap(int XCor, int YCor, bool Draw, LPDIRECTDRAWSURFACE7, bool Flag, LPRECT Clip);
   void SavePointer(void);
   void RestorePointer(void);
   void AttackMode(bool = false);
   void UpdateAttackMode(bool = false);
   void SetGridMode(int);
   void PlAttack(DWORD, char);
   BOOL FoundID(unsigned long, bool = false);
   BOOL FoundChild(unsigned long);
   Sprite Overlap;
   void Lock(int);
   void Unlock(int);
   void AddSDObject(void);
   void AddSDAnimObject(void);
   unsigned long FoundType(short);
   void UpdateLight(BOOL = FALSE, int = 100);
   void Dither(void);
   void SetMonsterStats(int, TFCObject *);
   void ReleaseAll(void);
   void SetMenuPuppet(TFCObject *Object, WORD BODY, WORD FEET, WORD GLOVES, WORD HELM, 
                                 WORD LEGS, WORD W_RIGHT, WORD W_LEFT, WORD CAPE, WORD HAIR);
   void SetPuppet(int);
	void SetPuppet(DWORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD,WORD);
	void SetUnit(DWORD, short, char, char, char,char,char,bool,DWORD);
   void SetSmile(DWORD, short);
	void SetHP(DWORD, char);
	BOOL CheckDead(DWORD);
   BOOL CheckDeadDirect(TFCObject* pObj);
	BOOL CheckDelete(DWORD);
	BOOL SetMissing(DWORD, BOOL);
	WORD GetType(DWORD ID);
   char *GetName(DWORD ID);
   void SDClear(void);
   void SDAddObject(short,short,short,char,short);
   void KillObject(DWORD);
   void DrawPuppet(int xPos, int yPos,int dwDirection);
   void DrawPuppetClip(int xPos, int yPos,int dwDirection, RECT *pClip,TFCObject *pObject);
   void SetEvil(unsigned long);
   TFCObject* GetObjectByID(unsigned long ID);
};

extern VisualObjectList Objects;

#endif // !defined(AFX_VISUALOBJECTLIST_H__003154E2_FA13_11D0_BEAA_444553540000__INCLUDED_)