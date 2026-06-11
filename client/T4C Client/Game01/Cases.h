// Cases.h: interface for the CCases class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>

#define EN_DEHORS       -2
#define ERREUR          -1
#define COULEUR1         0
#define COULEUR2         1
#define COULEUR3         2
#define COULEUR4         3
#define COULEUR5         4
#define COULEUR6         5
#define COULEUR7         6
#define COULEUR8         7
#define COULEUR9         8
#define COULEUR10        9
#define COULEUR11        10
#define COULEUR12        11
#define CASE_ANALYSEE    12
#define CASE_ARRIVEE     13
#define CASE_DEPART      14
#define CASE_OBSTACLE    15
#define CASE_VIDE        16
#define COIN_GAUCHE_HAUT 17
#define COIN_GAUCHE_BAS  18
#define COIN_DROIT_HAUT  19
#define COIN_DROIT_BAS   20
#define MUR_VERTICAL     21
#define MUR_HORIZONTAL   22
#define MUR_PORTE        23
#define GAUCHE           24
#define DROITE           25
#define BAS              26
#define HAUT             27
#define PACMAN           28
#define PAC_GOB          29
#define PAC_POINT        30
#define FANTOME_ROUGE    31
#define FANTOME_VERT     32
#define FANTOME_BLEU     33
#define FANTOME_CHOIX    34
#define FORT             35
#define FAIBLE           36
#define BONUS1           37
#define BONUS2           38
#define BONUS3           39
#define BONUS4           40
#define BONUS5           41
#define BONUS6           42



extern int MAXX;
extern int MAXY;
extern int necessaire;


class CCases  
{
public:
	CCases();
	virtual ~CCases();

public :
	int m_x;
   int m_y;
   int m_Cout;
	CCases * m_pParent;
	CCases * m_pPrecedant;
	CCases * m_pSuivant;
};



class CSprite
{
public :
   CSprite();
   virtual ~CSprite();
   virtual UINT Dessine(int x,int y,bool oui,bool reelle_position,RECT rc,HDC hDC,UINT Idb,int &iXPos,int &iYPos){return 0;};
   int GetPosX();
   int GetPosY();
   void SetPosX(int i);
   void SetPosY(int j);
   int GetLastX();
   int GetLastY();

public:
   UINT LastBitmapTrue;
   UINT LastBitmapFalse;
   int Identifiant;
   int Etape;
   bool Atteint;
   int Direction;
   bool Etat;
   bool premiere_fois;
	bool Vivant;

   HPEN   m_NPen    ;
   HPEN   m_BPen    ;
   
   HBRUSH m_BBlack  ;
   HBRUSH m_BJaune  ;

   HINSTANCE m_hInstance;

protected:
	int PosX;
	int PosY;
	int LastX;
	int LastY;

};

class CPacman : public CSprite
{
public :
   CPacman(int Id = PACMAN);
   virtual ~CPacman() {};

   virtual UINT Dessine(int x,int y,bool oui,bool reelle_position,RECT rc,HDC hDC,UINT Idb,int &iXPos,int &iYPos);

   int Puissance;
protected :
};


class CFantome : public CSprite
{
public :
   CFantome(int=0);
	virtual ~CFantome() {};
	bool Atteignable();


	int dureepoursuite;
	long int sensibilite;
	int TargetX;
	int TargetY;
	bool candeplace;
	
	virtual UINT Dessine(int x,int y,bool oui,bool reelle_position,RECT rc,HDC hDC,UINT Idb,int &iXPos,int &iYPos);
};





