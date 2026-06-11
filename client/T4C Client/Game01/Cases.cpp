// Cases.cpp: implementation of the CCases class.
//
//////////////////////////////////////////////////////////////////////
#include "Cases.h"
#include "..\Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int MAXX=32;
int MAXY=26;
int necessaire=0;

CCases::CCases()
{
	m_x=EN_DEHORS;
	m_y=EN_DEHORS;
	m_pParent=NULL;
	m_pPrecedant=NULL;
	m_pSuivant=NULL;
	m_Cout=0;
}

CCases::~CCases()
{

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSprite::CSprite()
{
	Atteint         = false;
   Vivant          = true;
   Etat            = true;
	premiere_fois   = true;
	Direction       = HAUT;
	LastX           = EN_DEHORS;
	LastY           = EN_DEHORS;
	PosX            = EN_DEHORS;
	PosY            = EN_DEHORS;
	Identifiant     = EN_DEHORS;
	LastBitmapTrue  = 0;
   LastBitmapFalse = 0;

   m_NPen    = CreatePen(PS_NULL ,1,RGB(  0,  0,  0));
   m_BPen    = CreatePen(PS_SOLID,1,RGB(  0,  0,  0));

   m_BBlack  = CreateSolidBrush(RGB(  0,  0,  0));
   m_BJaune  = CreateSolidBrush(RGB(255,255,  0));

   m_hInstance = GetModuleHandle("T4C Client.exe");
   if(m_hInstance == 0x00)
      m_hInstance = GetModuleHandle("T4C.exe");

};

CSprite::~CSprite()
{
   DeleteObject(m_NPen);
   DeleteObject(m_BPen);
   
   DeleteObject(m_BBlack );
   DeleteObject(m_BJaune );
}

int CSprite::GetPosX()
{
	return PosX;
};

int CSprite::GetPosY()
{
	return PosY;
};

void CSprite::SetPosX(int i)
{
	PosX=i;
};

void CSprite::SetPosY(int j)
{
	PosY=j;
};

int CSprite::GetLastX()
{
	return LastX;
};

int CSprite::GetLastY()
{
	return LastY;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacman::CPacman(int Id)
{
   Identifiant= Id;
   Etat       = true;
   Puissance  = FAIBLE;
   Direction  = HAUT;
   PosX       = 7;
   PosY       = 15;
   Vivant     = true;
};

UINT CPacman::Dessine(int x,int y,bool oui,bool reelle_position,RECT rc,HDC hDC,UINT Idb,int &iXPos,int &iYPos)
{	
   if(oui&&Etat) 
      LastBitmapTrue=Idb;
   if(oui&&(!Etat)) 
      LastBitmapFalse=Idb;
   
   int Xcase,Ycase,Xd,Yd,dX,dY;
   
   UINT TheIdb = Idb;
   
   if(!oui) 
   {
      if(Etat) 
         TheIdb = LastBitmapFalse;
      else 
         TheIdb = LastBitmapTrue;
   };
   
   premiere_fois=false;
   

   Xcase = (rc.right-rc.left) /(MAXX);
   Ycase = (rc.bottom-rc.top)/ ( MAXY);
   
   if(Xcase>Ycase) 
      Xcase=Ycase;
   else 
      Ycase=Xcase;
   
  
   dX = Xcase/2;
   dY = Ycase/2;
   
   Xd =rc.left+((rc.right -rc.left-(2*(MAXX-1)+2)*dX)/2);
	Yd =rc.top +((rc.bottom-rc.top -(2*(MAXY-1)+2)*dY)/2);

 
   int xx=x,yy=y;
   
   if(reelle_position)
   {
      LastX = Xd+(2*PosX+1)*dX;
      LastY = Yd+(2*PosY+1)*dY;
      
      xx=LastX;
      yy=LastY;
   };
   
   LastX=xx;
   LastY=yy;
   
   int t=dX/5;
   
   xx=xx+1;


   iXPos   = xx-2*(dX-t);
   iYPos   = yy-2*(dY-t);
   
   if(oui) 
      Etat=!Etat;

   return TheIdb;
};












CFantome::CFantome(int i)
{
	Etape=0;
	Atteint=false;
	dureepoursuite=0;
	candeplace=true;
	sensibilite=0;
	switch(i)
	{
	   case 0:
		   PosX=15;
		   PosY=11;
		   Identifiant=FANTOME_CHOIX;
		break;
	   case 1:
		   PosX=15;
		   PosY=14;
		   Identifiant=FANTOME_VERT;
		break;
	   case 2:
		   PosX=18;
		   PosY=11;
		   Identifiant=FANTOME_BLEU;
		break;
	   case 3:
		   PosX=18;
		   PosY=14;
		   Identifiant=FANTOME_ROUGE;
		break;
	};
};

UINT CFantome::Dessine(int x,int y,bool oui,bool reelle_position,RECT rc,HDC hDC,UINT Idb,int &iXPos,int &iYPos)
{	

   if(oui&&Etat) 
      LastBitmapTrue=Idb;
   if(oui&&(!Etat)) 
      LastBitmapFalse=Idb;
   
   int Xcase,Ycase,Xd,Yd,dX,dY;
   
   UINT TheIdb = Idb;
   
   if(!oui) 
   {
      if(Etat) 
         TheIdb = LastBitmapFalse;
      else 
         TheIdb = LastBitmapTrue;
   };
   
   premiere_fois=false;
   
   
   Xcase = (rc.right-rc.left) /(MAXX);
   Ycase = (rc.bottom-rc.top)/ ( MAXY);
   
   if(Xcase>Ycase) 
      Xcase=Ycase;
   else 
      Ycase=Xcase;
   
   
   dX = Xcase/2;
   dY = Ycase/2;
   
   Xd =rc.left+((rc.right -rc.left-(2*(MAXX-1)+2)*dX)/2);
	Yd =rc.top +((rc.bottom-rc.top -(2*(MAXY-1)+2)*dY)/2);


	int xx=x,yy=y;

	if(reelle_position)
	{
		LastX = Xd+(2*PosX+1)*dX;
		LastY = Yd+(2*PosY+1)*dY;

		xx=LastX;
		yy=LastY;
	};

	LastX=xx;
	LastY=yy;


	int t=dX/5;

	xx=xx+1;


   iXPos   = xx-2*(dX-t);
   iYPos   = yy-2*(dY-t);

	if(oui) 
      Etat=!Etat;

   return TheIdb;
};

bool CFantome::Atteignable()
{
	if(PosX>19) return true;
	if(PosX<14) return true;
	if(PosY>15) return true;
	if(PosY<10) return true;

	if(TargetX!=0)
	{
		if(TargetX>19) return true;
		if(TargetX<14) return true;
		if(TargetY>15) return true;
		if(TargetY<10) return true;
	};
	return false;
}
