// PathFinder.cpp: implementation of the CPathFinder class.
//
//////////////////////////////////////////////////////////////////////

#include "PathFinder.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPathFinder::CPathFinder(RECT rc)
{
   m_Rc.left   = rc.left;  
   m_Rc.top    = rc.top;   
   m_Rc.right  = rc.right; 
   m_Rc.bottom = rc.bottom;

   int Xcase,Ycase;

   Xcase = (rc.right-rc.left) /(MAXX);
	Ycase = (rc.bottom-rc.top)/ ( MAXY);

	if(Xcase>Ycase) 
      Xcase=Ycase;
	else 
      Ycase=Xcase;
   
   m_iCaseSize = Xcase;

	m_idX = Xcase/2;
	m_idY = Ycase/2;

	m_iXd =m_Rc.left+((m_Rc.right -m_Rc.left-(2*(MAXX-1)+2)*m_idX)/2);
	m_iYd =m_Rc.top +((m_Rc.bottom-m_Rc.top -(2*(MAXY-1)+2)*m_idY)/2);




   m_ChangeGob=false;
   m_NbrCasesExaminees=0;
   m_pTerrain = new int[MAXX*MAXY];
   m_pLesX    = new int[4];
   m_pLesY    = new int[4];

   //Set toutes les case a des points a ramasser...
   for(int i=0;i<MAXX;i++)
   {
      for(int j=0;j<MAXY;j++)
      {
         SetCase(i,j,PAC_POINT);
      }
   }
      
   for(int k=0;k<4;k++)
   {
      m_pLesX[k]=EN_DEHORS;
      m_pLesY[k]=EN_DEHORS;
   };
   RemplirLabyrinthe();
   
   m_pChemin         = NULL;
   m_pPasDeChemin    = NULL;
   m_pLaCaseSuivante = NULL;

   m_NPen    = CreatePen(PS_NULL ,1,RGB(  0,  0,  0));
   m_BPen    = CreatePen(PS_SOLID,1,RGB(  0,  0,  0));
   m_GPen    = CreatePen(PS_SOLID,3,RGB(  0,255,  0));
   m_Crayon  = CreatePen(PS_SOLID,3,RGB(  0,  0,255));
   
   m_BBlack  = CreateSolidBrush(RGB(  0,  0,  0));
   m_BGray   = CreateSolidBrush(RGB(128,128,128));
   m_BJaune  = CreateSolidBrush(RGB(255,255,  0));
   m_BOrange = CreateSolidBrush(RGB(255,128,  0));
   m_BBrosse = CreateSolidBrush(RGB( 32, 70,105));

   
  
}

CPathFinder::~CPathFinder()
{
   delete []m_pLesX;
   delete []m_pLesY;
   delete []m_pTerrain;

   VideListe(&m_pChemin);
   VideListe(&m_pPasDeChemin);


   DeleteObject(m_NPen);
   DeleteObject(m_BPen);
   DeleteObject(m_GPen);
   DeleteObject(m_Crayon);

   DeleteObject(m_BBlack );
   DeleteObject(m_BGray  );
   DeleteObject(m_BJaune );
   DeleteObject(m_BOrange);
   DeleteObject(m_BBrosse);
}


void CPathFinder::SetCase(int i, int j, int c)
{
   if((i>=0) && (i<MAXX) && (j>=0) && (j<MAXY)) 
      m_pTerrain[MAXY*i+j] = c;
}

int CPathFinder::GetCase(int i, int j)
{
   if((i>=0) && (i<MAXX) && (j>=0) && (j<MAXY)) 
      return m_pTerrain[MAXY*i+j];
   else 
      return ERREUR;
}

void CPathFinder::VideListe(CCases ** liste)
{
   CCases *tmp;
   while(*liste!=NULL)
   {
      tmp= *liste;
      *liste=(*liste)->m_pSuivant;
      delete tmp;
   };
}

void CPathFinder::RemplirColonne(int i, int j1, int j2)
{
   for(int j=j1;j<=j2;j++)
      SetCase(i,j,MUR_VERTICAL);
}

void CPathFinder::RemplirLigne(int j, int i1, int i2)
{
   for(int i=i1;i<=i2;i++)
      SetCase(i,j,MUR_HORIZONTAL);
}

void CPathFinder::RemplirRectangle(int i1, int j1, int i2, int j2)
{
   //NMNMNM
   int i,j;
   
   for(i=i1;i<=i2;i++)
   {
      for(j=j1;j<=j2;j++)
      {
         SetCase(i,j,CASE_VIDE);
      }
   }
      
   for(i=i1;i<=i2;i++)
   {
      SetCase(i,j1,MUR_HORIZONTAL);
      SetCase(i,j2,MUR_HORIZONTAL);
   };
   
   for(j=j1;j<=j2;j++)
   {
      SetCase(i1,j,MUR_VERTICAL);
      SetCase(i2,j,MUR_VERTICAL);
   };
   
   SetCase(i1,j1,COIN_GAUCHE_HAUT);
   SetCase(i2,j2,COIN_DROIT_BAS);
   SetCase(i1,j2,COIN_GAUCHE_BAS);
   SetCase(i2,j1,COIN_DROIT_HAUT);
}


void CPathFinder::ChercheChemin(int Xs,int Ys,int Xf,int Yf,BOOL diagonale)
{
   CCases * tmp;
   CCases * Courant;
   
   m_NbrCasesExaminees=0;
   
   VideListe(&m_pChemin);
   VideListe(&m_pPasDeChemin);
   m_pChemin = new CCases();
   
   m_pChemin->m_x=Xs;
   m_pChemin->m_y=Ys;
   
   while(m_pChemin!=NULL)
   {
      Courant = m_pChemin;
      tmp     = m_pChemin;
      while(tmp!=NULL)
      {
         if(tmp->m_Cout<Courant->m_Cout) Courant=tmp;
         tmp=tmp->m_pSuivant;
      };
      
      if(Courant->m_x==Xf&&Courant->m_y==Yf) return;
      
      if(Courant->m_pPrecedant==NULL) 
         m_pChemin = m_pChemin->m_pSuivant;
      else 
         (Courant->m_pPrecedant)->m_pSuivant=Courant->m_pSuivant;
      
      if((Courant->m_x==17)&&(Courant->m_y==0))
         AnalyseVoisin(Courant,Courant->m_x,25);
      
      if((Courant->m_x==17)&&(Courant->m_y==25))
         AnalyseVoisin(Courant,Courant->m_x,0);
      
      AnalyseVoisin(Courant,Courant->m_x,Courant->m_y-1);
      AnalyseVoisin(Courant,Courant->m_x+1,Courant->m_y);
      AnalyseVoisin(Courant,Courant->m_x,Courant->m_y+1);
      AnalyseVoisin(Courant,Courant->m_x-1,Courant->m_y);
      
      if(diagonale)
      {
         AnalyseVoisin(Courant,Courant->m_x-1,Courant->m_y-1);
         AnalyseVoisin(Courant,Courant->m_x-1,Courant->m_y+1);
         AnalyseVoisin(Courant,Courant->m_x+1,Courant->m_y-1);
         AnalyseVoisin(Courant,Courant->m_x+1,Courant->m_y+1);
      };
      
      Courant->m_pSuivant = m_pPasDeChemin;
      m_pPasDeChemin = Courant;
   }
};

void CPathFinder::ConstruireChemin(int Xs, int Ys, int Xf, int Yf, BOOL diagonale)
{
   ChercheChemin(Xs,Ys,Xf,Yf,diagonale);
   
   CCases *tmp;

   tmp = m_pChemin;
   if(tmp!=NULL)
   {
      while((tmp->m_pParent->m_x!=Xs)||(tmp->m_pParent->m_y!=Ys))
      {
         tmp=tmp->m_pParent;
         if (tmp==NULL) 
         {
            m_pLaCaseSuivante = tmp;
            return;
         };
      };
   };
   
   m_pLaCaseSuivante = tmp;
}

void CPathFinder::AnalyseVoisin(CCases * LaCase,int Xv,int Yv)
{
   if(Xv<0 || Xv>=MAXX || Yv<0 || Yv>=MAXY) 
      return;
   
   m_NbrCasesExaminees++;
   
   int i,j;
   i=Xv;
   j=Yv;
   
   if(GetCase(i,j)==COIN_GAUCHE_HAUT) return ;
   if(GetCase(i,j)==COIN_GAUCHE_BAS)  return ;
   if(GetCase(i,j)==COIN_DROIT_HAUT)  return ;
   if(GetCase(i,j)==COIN_DROIT_BAS)   return ;
   if(GetCase(i,j)==MUR_VERTICAL)     return ;
   if(GetCase(i,j)==MUR_HORIZONTAL)   return ;
   
   for(int k=0;k<4;k++)
   {
      if( (m_pLesX[k]==i) && (m_pLesY[k]==j) ) return;
   };
   
   CCases * Voisin;
   
   Voisin = new CCases();
   Voisin->m_x=Xv;
   Voisin->m_y=Yv;
   Voisin->m_pParent=LaCase;
   Voisin->m_Cout=LaCase->m_Cout+1;
   
   CCases * tmp;
   bool  fin;
   
   tmp = m_pChemin;
   fin = false;
   while(tmp!=NULL && fin==false)
   {
      if(tmp->m_x==Voisin->m_x&&tmp->m_y==Voisin->m_y)
      {
         fin=true;
         if(tmp->m_Cout<=Voisin->m_Cout)
         {
            delete Voisin;
            return;
         }
         else
         {
            if(tmp->m_pPrecedant==NULL) 
               m_pChemin=m_pChemin->m_pSuivant;
            else 
               (tmp->m_pPrecedant)->m_pSuivant=tmp->m_pSuivant;
            delete tmp;
         };
      };
      tmp=tmp->m_pSuivant;
   };
   if(fin==false)
   {
      tmp = m_pPasDeChemin;
      while(tmp!=NULL&&fin==false)
      {
         if(tmp->m_x==Voisin->m_x&&tmp->m_y==Voisin->m_y)
         {
            fin=true;
            if(tmp->m_Cout<=Voisin->m_Cout)
            {
               delete Voisin;
               return;
            }
            else
            {
               if(tmp->m_pPrecedant==NULL) 
                  m_pPasDeChemin = m_pPasDeChemin->m_pSuivant;
               else 
                  (tmp->m_pPrecedant)->m_pSuivant=tmp->m_pSuivant;
               delete tmp;
            };
         };
         tmp=tmp->m_pSuivant;
      };
   };
   
   if(fin==false)
   {
      tmp = m_pChemin;
      if(tmp==NULL) 
         m_pChemin = Voisin;
      else
      {
         while(tmp->m_pSuivant!=NULL) 
            tmp=tmp->m_pSuivant;
         tmp->m_pSuivant=Voisin;
      };
   };
}

void CPathFinder::RemplirLabyrinthe()
{
	int i,j;

	SetCase(9,1,PAC_GOB);
	SetCase(25,1,PAC_GOB);
	SetCase(9,24,PAC_GOB);
	SetCase(25,24,PAC_GOB);
   

	for(j=0;j<=4;j++)
   {
		for(i=13;i<=15;i++)
		{
			SetCase(i,j,CASE_VIDE);
		};
   }

	for(j=0;j<=4;j++)
   {
		for(i=19;i<=21;i++)
		{
			SetCase(i,j,CASE_VIDE);
		};
   }


	SetCase(6,0,CASE_VIDE);

	SetCase(6,1,CASE_VIDE);

	RemplirRectangle(14,10,19,15);

	SetCase(19,12,MUR_PORTE);

	RemplirRectangle(8,7,9,10);

	RemplirRectangle(11,7,16,8);

	RemplirRectangle(18,7,25,8);
	RemplirRectangle(21,8,22,10);
	SetCase(21,8,COIN_DROIT_HAUT);
	SetCase(22,8,COIN_GAUCHE_HAUT);

	RemplirRectangle(24,2,25,5);

	RemplirRectangle(27,2,29,5);

	RemplirRectangle(27,7,29,10);

	RemplirRectangle(5,4,10,5);
	RemplirRectangle(9,2,10,5);
	SetCase(9,4,COIN_DROIT_BAS);
	SetCase(9,5,MUR_HORIZONTAL);

	RemplirRectangle(2,2,3,10);
	RemplirRectangle(3,7,6,8);
	SetCase(3,7,COIN_GAUCHE_BAS);
	SetCase(3,8,COIN_GAUCHE_HAUT);

	RemplirRectangle(2,12,5,13);
	RemplirRectangle(5,10,6,15);
	SetCase(5,12,COIN_DROIT_BAS);
	SetCase(5,13,COIN_DROIT_HAUT);

	RemplirRectangle(8,12,11,13);
	RemplirRectangle(11,10,12,15);
	SetCase(11,12,COIN_DROIT_BAS);
	SetCase(11,13,COIN_DROIT_HAUT);

	RemplirRectangle(21,12,24,13);
	RemplirRectangle(24,10,25,15);
	SetCase(24,12,COIN_DROIT_BAS);
	SetCase(24,13,COIN_DROIT_HAUT);


	RemplirColonne(0,0,12);
	RemplirLigne(0,0,5);
	RemplirColonne(5,0,2);
	RemplirLigne(2,5,7);
	RemplirColonne(7,0,2);
	RemplirLigne(0,7,12);
	RemplirColonne(12,0,5);
	RemplirLigne(5,12,16);
	RemplirColonne(16,0,5);
	RemplirColonne(18,0,5);
	RemplirLigne(5,18,22);
	RemplirColonne(22,0,5);
	RemplirLigne(0,22,31);
	RemplirColonne(31,0,12);
	RemplirLigne(12,27,31);

	SetCase(0,0,COIN_GAUCHE_HAUT);
	SetCase(5,0,COIN_DROIT_HAUT);
	SetCase(5,2,COIN_GAUCHE_BAS);
	SetCase(7,2,COIN_DROIT_BAS);
	SetCase(7,0,COIN_GAUCHE_HAUT);

	SetCase(12,0,COIN_DROIT_HAUT);
	SetCase(12,5,COIN_GAUCHE_BAS);
	SetCase(16,5,COIN_DROIT_BAS);
	SetCase(18,5,COIN_GAUCHE_BAS);
	SetCase(22,5,COIN_DROIT_BAS);

	SetCase(22,0,COIN_GAUCHE_HAUT);
	SetCase(31,0,COIN_DROIT_HAUT);
	SetCase(31,12,COIN_DROIT_BAS);
	SetCase(27,12,COIN_GAUCHE_HAUT);
  

	for(i=0;i<MAXX;i++)
   {
		for(j=0;j<=12;j++)
		{
			SetCase(i,MAXY-j-1,GetCase(i,j));
			
			switch(GetCase(i,j))
			{
			case COIN_GAUCHE_HAUT:
				SetCase(i,MAXY-j-1,COIN_GAUCHE_BAS);
				break;
			case COIN_GAUCHE_BAS:
				SetCase(i,MAXY-j-1,COIN_GAUCHE_HAUT);
				break;
			case COIN_DROIT_HAUT:
				SetCase(i,MAXY-j-1,COIN_DROIT_BAS);
				break;
			case COIN_DROIT_BAS:
				SetCase(i,MAXY-j-1,COIN_DROIT_HAUT);
				break;
			};

		};
   }
}






void CPathFinder::DessinePoint(int i,int j,bool quel,HDC hDC)
{
   int Xcenter,Ycenter;
   RECT rc2;
   
   
   Xcenter = m_iXd+(2*i+1)*m_idX;
   Ycenter = m_iYd+(2*j+1)*m_idY;
   
   int t=m_iCaseSize/7;
   
   rc2.left   = Xcenter-t;
   rc2.right  = Ycenter-t;
   rc2.top    = Xcenter+t;
   rc2.bottom = Ycenter+t;
   

   HANDLE hOldBrush = SelectObject(hDC,m_BBlack);
   HANDLE hOldPen   = SelectObject(hDC,m_BPen);

   Rectangle(hDC,Xcenter-t,Ycenter-t,Xcenter+t,Ycenter+t);
     
   if((GetCase(i,j)!=PAC_POINT)||(!quel)) 
   {
      SelectObject(hDC,hOldBrush);
      SelectObject(hDC,hOldPen);
      return;
   };
   
   SelectObject(hDC,m_BJaune);
   Rectangle(hDC,Xcenter-t,Ycenter-t,Xcenter+t,Ycenter+t);
      
   SelectObject(hDC,hOldBrush);
   SelectObject(hDC,hOldPen);
}


void CPathFinder::DessineGob(int i,int j,bool oui,HDC hDC)
{
   int Xcenter,Ycenter;
   
   Xcenter = m_iXd+(2*i+1)*m_idX;
   Ycenter = m_iYd+(2*j+1)*m_idY;
   
   int t=m_idX/2;
   int tt=0;
   
   HANDLE hOldBrush = SelectObject(hDC,m_BBlack);
   HANDLE hOldPen   = SelectObject(hDC,m_BPen);
   
   Ellipse(hDC,Xcenter-2*(m_idX-t),Ycenter-2*(m_idY-t),Xcenter+2*(m_idX-t),Ycenter+2*(m_idY-t));
   
   if(!oui) 
   {
      SelectObject(hDC,hOldBrush);
      SelectObject(hDC,hOldPen);
      return;
   };
   
   SelectObject(hDC,m_BJaune);
   
   if(m_ChangeGob)
      Ellipse(hDC,Xcenter-m_idX+tt,Ycenter-m_idY+tt,Xcenter+m_idX-tt,Ycenter+m_idY-tt);
   else
      Ellipse(hDC,Xcenter-2*(m_idX-t),Ycenter-2*(m_idY-t),Xcenter+2*(m_idX-t),Ycenter+2*(m_idY-t));
   
   
   SelectObject(hDC,hOldBrush);
   SelectObject(hDC,hOldPen);
}



void CPathFinder::DessineLabyrinthe(HDC hDC)
{
	int i,j;
   int Xcenter,Ycenter;


   HANDLE hOldBrush = SelectObject(hDC,m_BBlack);
   HANDLE hOldPen   = SelectObject(hDC,m_BPen);


   Rectangle(hDC,m_Rc.left,m_Rc.top,m_Rc.right,m_Rc.bottom);


   SelectObject(hDC,m_BBrosse);
   SelectObject(hDC,m_Crayon);

	for(i=0;i<MAXX;i++)
   {
		for(j=0;j<MAXY;j++)
		{
	
			Xcenter = m_iXd+(2*i+1)*m_idX;
			Ycenter = m_iYd+(2*j+1)*m_idY;
			
			SelectObject(hDC,m_BBrosse);
			SelectObject(hDC,m_Crayon);
			switch(GetCase(i,j))
			{
			   case COIN_GAUCHE_HAUT :
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter+m_idX,Ycenter);
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter,Ycenter+m_idY);
				break;
			   case COIN_GAUCHE_BAS :
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter+m_idX,Ycenter);
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter,Ycenter-m_idY);
				break;
			   case COIN_DROIT_HAUT :
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter-m_idX,Ycenter);
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter,Ycenter+m_idY);
				break;
			   case COIN_DROIT_BAS :
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter-m_idX,Ycenter);
				   MoveToEx(hDC,Xcenter,Ycenter,NULL);
				   LineTo(hDC,Xcenter,Ycenter-m_idY);
				break;
			   case MUR_VERTICAL :
				   MoveToEx(hDC,Xcenter,Ycenter-m_idY,NULL);
				   LineTo(hDC,Xcenter,Ycenter+m_idY);
				break;
			   case MUR_HORIZONTAL :
				   MoveToEx(hDC,Xcenter-m_idX,Ycenter,NULL);
				   LineTo(hDC,Xcenter+m_idX,Ycenter);
				break;
			   case MUR_PORTE :
				   SelectObject(hDC,m_GPen);
				   MoveToEx(hDC,Xcenter,Ycenter-m_idY,NULL);
				   LineTo(hDC,Xcenter,Ycenter+m_idY);
				break;
			   case CASE_VIDE :
				break;
			};
			SelectObject(hDC,m_BBrosse);
			SelectObject(hDC,m_GPen);
			MoveToEx(hDC,m_iXd+(2*19+1)*m_idX,m_iYd+(2*11+1)*m_idY,NULL);
			LineTo(hDC,m_iXd+(2*19+1)*m_idX,m_iYd+(2*14+1)*m_idY);
		};
   }

   SelectObject(hDC,hOldBrush);
   SelectObject(hDC,hOldPen);
}




UINT CPathFinder::GetIdBitmap(CSprite* sp,CPacman* pPacman,bool bQuel,bool bFin,int iCptFin, int iSignal)
{
   UINT id = 3;

	static int cpt1=0 , cpt2=0 , cpt3=0;

	if(sp->Identifiant==PACMAN)
	{
		if(bFin)
		{
			if (iCptFin==1) 
            return 2;
			if (iCptFin==2) 
            return 1;
			if (iCptFin==5) 
            return 3;
			return 23;
		};
		if(sp->Etat)
		{
			switch(sp->Direction)
			{
			   case HAUT:
				   id = 1;
				break;
			   case BAS :
				   id = 4;
				break;
			   case DROITE :
				   id = 6;
				break;
			   case GAUCHE :
				   id = 8;
				break;
			};
		}
		else
		{
			switch(sp->Direction)
			{
			   case HAUT:
				   id = 2;
				break;
			   case BAS :
				   id = 5;
				break;
			   case DROITE :
				   id = 7;
				break;
			   case GAUCHE :
				   id = 9;
				break;
			};
		};

		return id;
	};

	if(!sp->Vivant)
	{
		if((sp->Etape==0)||(sp->Etape==2)) 
		{
			id = 18;
			cpt1++;
			if((sp->Etat)&&(bQuel)) 
			{
				sp->Etape++;
				cpt1=0;
			};
		};

		if(sp->Etape==1) 
		{
			id = 19;
			cpt2++;
			if((sp->Etat)&&(bQuel)) 
			{
				sp->Etape++;
				cpt2=0;
			};
		};

		if(sp->Etape==3)
		{
			id = 22;
			cpt3++;
			if((sp->Etat)&&(bQuel)) 
			{
				sp->Etape=0;
				cpt3=0;
			};
		};

		if(!sp->Etat) 
         id = 3;

		return id;
	};


	if((pPacman->Puissance==FAIBLE) || (iSignal==1) || (!sp->Atteint) )
	{
		if(bQuel==/*true*/bQuel)
		{
			if(sp->Identifiant==FANTOME_ROUGE)
			{
				if(sp->Etat) 
               id = 10;
				else 
               id = 11;
			};
			if(sp->Identifiant==FANTOME_BLEU)
			{
				if(sp->Etat) 
               id = 12;
				else 
               id = 13;
			};
			if(sp->Identifiant==FANTOME_VERT)
			{
				if(sp->Etat) 
               id = 14;
				else 
               id = 15;
			};
			if(sp->Identifiant==FANTOME_CHOIX)
			{
				if(sp->Etat) 
               id = 16;
				else 
               id = 17;
			};
		};
	}
	else
	{
		if(iSignal==0)
		{
			if(sp->Etat) 
            id = 20;
			else 
            id = 21;
		};
	};


	return id;
}