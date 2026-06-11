#ifndef __BASIC_SPRITE
#define __BASIC_SPRITE

#include "TFCObject.h"

class BasicSprite {
   public:
		int MaxWidth;
		int MaxHeight;
		int MaxxCor;
		int MaxyCor;
		int fTransparent;
		int TransColor;
      signed char StAtt, StMov;
      DWORD LastAccess;

		char Type, Faces, FacesA, FacesC, FacesR, FacesStA, FacesStM;

   public:
      virtual INFO *GetCurrentVSF(int Direction, int Frame, BYTE Stand) { throw; return NULL; };
		virtual void LoadSprite3D(int, int, int, int, int, char *, int, int, int, int = 1) { throw; };
		virtual void LoadSprite3D2(int, int, int, int, int, char *, int, int, int, int = 1) {throw; };
		virtual void LoadSprite3D(int, int, int, int, int, int, char *, int, int, int, int = 1) { throw; };
	   virtual void LoadSprite3D(int, int, int, char *, int, int, int = 1) { throw; };
      virtual void LoadSpriteC(int, int, int, char *, int = 1) { throw; };
      virtual int  GetTextCorrection(void) { throw; return 0; };
		virtual void DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL) { throw; };
      virtual void DrawSprite3DA(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL) { throw; };
      virtual void DrawSprite3DR(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL) {  throw; };
		virtual void DrawCorpse(int, int X, int Y, RECT *Clip, RECT *OutBound, RECT *Wrap, LPDIRECTDRAWSURFACE7 Surface, signed char Format, int AbsX, int AbsY, TFCObject *,Sprite3D *pMask = NULL) {  throw; };
      virtual void LoadBodyPart(void) {  throw; };
      virtual void LoadSpell(int, char *, int = 1) {  throw; };
      virtual void LoadSpellA(int, char *,char *, int = 1) { throw; };
      virtual BOOL Release(DWORD) {  throw; return FALSE; }; 
      virtual void Release(void) {  throw; };
      virtual void LoadSprite3D(int, char *, int = 1) { throw; };
      virtual void SetPuppet(TFCObject *) {  throw; };
};

#endif