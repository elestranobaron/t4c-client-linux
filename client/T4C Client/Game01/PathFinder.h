// PathFinder.h: interface for the CPathFinder class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Cases.h"

class CPathFinder  
{
public:
   CPathFinder(RECT rc);
	virtual ~CPathFinder();

   int GetCase(int i, int j);
	void SetCase(int i, int j, int c);

   void VideListe(CCases ** liste);

   void RemplirColonne(int i,int j1,int j2);
   void RemplirLigne(int j,int i1,int i2);
  
	void RemplirRectangle(int i1,int j1,int i2,int j2);

   void ChercheChemin(int Xs,int Ys,int Xf,int Yf,BOOL diagonale);
   void ConstruireChemin(int Xs,int Ys,int Xf,int Yf,BOOL diagonale);
   void AnalyseVoisin(CCases * LaCase,int Xv,int Yv);
   
   void RemplirLabyrinthe();
	
    

	void DessinePoint(int i,int j,bool quel,HDC hDC);
   void DessineGob(int i,int j,bool oui,HDC hDC);
	void DessineLabyrinthe(HDC hDC);

   UINT GetIdBitmap(CSprite* sp,CPacman* pPacman,bool bQuel,bool bFin,int iCptFin, int iSignal);
	
	
	
	

public:
   bool m_ChangeGob;
   int m_NbrCasesExaminees;
   int * m_pTerrain;
   int * m_pLesX;
   int * m_pLesY;
   CCases * m_pChemin;
   CCases * m_pPasDeChemin;
	CCases * m_pLaCaseSuivante;


protected:
   HPEN   m_NPen    ;
   HPEN   m_BPen    ;
   HPEN   m_GPen    ;
   HPEN   m_Crayon  ;

   HBRUSH m_BBlack  ;
   HBRUSH m_BGray   ;
   HBRUSH m_BJaune  ;
   HBRUSH m_BOrange ;
   HBRUSH m_BBrosse ;

   RECT   m_Rc;
   int    m_idX;
   int    m_idY;
   int    m_iXd;
   int    m_iYd;
   int    m_iCaseSize;
};

