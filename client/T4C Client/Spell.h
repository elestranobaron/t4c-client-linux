#ifndef __SPELL_H
#define __SPELL_H

#include "V2Sprite.h"
#include "Icon3D.h"
#include "BasicSprite.h"

class Spell : public BasicSprite {
   private:
      CV2Sprite *SpellAnim;
      CV2Sprite *SpellAnimA;
      BOOL bAlreadyLoad;

   public:  
      Spell(void);
      ~Spell(void);
      
      void LoadSpell(int, char *, int = 1);
      void LoadSpellA(int, char *,char *, int = 1);
      void DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL);

      INFO *GetCurrentVSF(int Direction, int Frame, BYTE Stand) { LastAccess = timeGetTime(); INFO *x = new INFO; ZeroMemory(x, sizeof(INFO)); return x; };
      int  GetTextCorrection(void) { LastAccess = timeGetTime(); return -20; };
      void Release(void);
      BOOL Release(DWORD);
};


#endif