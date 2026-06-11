// TFCObject.cpp: implementation of the TFCObject class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "TFCObject.h"
#include "Icon3D.h"
#include "Global.h"
#include "NewFont.h"
#include "SaveGame.h"


extern CSaveGame g_SaveGame;

const int MaxOverheadLines = 3;

extern short TalkToOffset;
extern unsigned long TalkToID;

bool isemptyObj(const char *s) {
   int l = strlen(s);
   for (int i = 0; i < l; i++)
      if (s[i] != ' ')
         return false;
   return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFCObject::TFCObject() 
{
    m_pName = NULL;
	 GuildName = NULL;
    m_DisplayName = NULL;
	 m_DisplayGuildName = NULL;
    m_TalkingText = NULL;
    m_TalkingBox = NULL;
    allowOutOfBound = false;
    ushTAGID = 0;
    ushSmileID = 0;
    ushSmileIDCnt = 0;

    wLevel = 0;
    InitializeCriticalSection(&csLockDamage);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
TFCObject::~TFCObject()
{
    Deplacement *D;
    
    while ((D = MovingQueue.Retreive())) {
        delete D;
    }
     
    if (m_pName)
        delete []m_pName;
	 m_pName = NULL;
	 if (GuildName)
		delete GuildName;
    GuildName = NULL;

    if(m_DisplayName)
      delete m_DisplayName;  
    m_DisplayName = NULL;

    if(m_DisplayGuildName)
	   delete m_DisplayGuildName;
    m_DisplayGuildName = NULL;

    
    onAttack[0].Release(FALSE);
    onAttack[1].Release(FALSE);
    onAttack[2].Release(FALSE);
    onAttack[3].Release(FALSE);
    onAttack[4].Release(FALSE);
    onAttacked[0].Release(FALSE);
    onAttacked[1].Release(FALSE);
    onAttacked[2].Release(FALSE);
    onKilled[0].Release(FALSE);
    onKilled[1].Release(FALSE); 

    EnterCriticalSection(&csLockDamage);
    unsigned int i;
    for( i = 0; i < m_vDamageText.size(); i++ )
    {
       delete m_vDamageText[i]->DamageText;
       delete m_vDamageText[i];
    }

    m_vDamageText.clear();
    LeaveCriticalSection(&csLockDamage);

    if(m_TalkingBox)
       delete m_TalkingBox;
    m_TalkingBox = NULL;

    if(m_TalkingText)
       delete m_TalkingText;
    m_TalkingText = NULL;
    
    DeleteCriticalSection(&csLockDamage);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TFCObject::operator <( const TFCObject *&x ){
    if( OC + OY > x->OC + x->OY ){
        return true;
    }
    if( OX > x->OX ){
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::InitTalkText( COLORREF textColor )
{
    if(m_TalkingText)
       delete m_TalkingText;
    m_TalkingText = FontManager::GetInstance()->GetTalkFont();
    
    const int MaxOverheadTextWidth = 300;
    const int OverheadLineSpacing = 0;
    
    m_TalkingText->EnableOutline();
    m_TalkingText->EnableCenterJustification();
    m_TalkingText->SetText( ".", textColor, MaxOverheadTextWidth, OverheadLineSpacing, false, true, false );
    m_TalkingText->ResetText("");
}

void TFCObject::ProcessTextRep(char *pstrTxt,char *pstrMod)
{
   std::vector<std::string> vRep;
   std::vector<std::string> vRepMod;
   if (pstrTxt && pstrTxt[0] && !isemptyObj(pstrTxt)) 
   {
      char strProcess[2048];
      sprintf_s(strProcess,2048,"%s",pstrTxt);

      char strRepR[1000];
      char strRepK[1000];
      char strRepRep[1000];

      int iPHEnd = 0;
      int iSR = 0;
      int iSK = 0;
      int iSRep = 0;
      bool bStartR = false;
      bool bStartK = false;
      for(UINT i=0;i<strlen(strProcess);i++)
      {
         if(!bStartR && strProcess[i] == '@')
         {
            //Start la Reponse
            if(iPHEnd == 0)
               iPHEnd = i-1;
            iSR = 0;
            iSRep = 0;
            bStartR = true;
         }
         else if(bStartR && strProcess[i] == '@')
         {
            //stop the reponse
            strRepR[iSR] = 0x00;
            strRepRep[iSRep] = 0x00;
            vRep.push_back(strRepR);
            vRepMod.push_back(strRepRep);

            bStartR = false;
         }
         else if(bStartR)
         {
            strRepRep[iSRep++] = strProcess[i];
            if(!bStartK && strProcess[i] == '"')
            {
               //Start la key
               iSK = 0;
               bStartK = true;
            }
            else if(bStartK && strProcess[i] == '"')
            {
               //STOP la key
               strRepK[iSK] = 0x00;
               bStartK = false;
            }
            else if(bStartK)
            {
               strRepR[iSR++] = strProcess[i];
               strRepK[iSK++] = strProcess[i];
            }
            else
            {
               strRepR[iSR++] = strProcess[i];
            }
         }
      }

      if(iPHEnd >0)
      {
         strProcess[iPHEnd] = 0x00;
         char strTmpR[2048];
         char strTmp2[2048];
         sprintf_s(strTmpR,"%s",strProcess);

         for( UINT k=0;k<vRep.size();k++)
         {
            sprintf_s(strTmp2,2048," <>> %s",vRepMod[k].c_str());
            strcat_s(strTmpR,2048,strTmp2);
         }
         if(pstrMod)
            sprintf_s(pstrMod,2048,"%s",strTmpR);
      }
      else
         sprintf_s(pstrMod,2048,"%s",pstrTxt);
   }
   else
   {
      sprintf_s(pstrMod,2048,"%s",pstrTxt);
   }
}




//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::SetTalkText( const char *txt, COLORREF textColor ,BOOL bHyperLink, DWORD dwID,char *pstrMod)
{
    if(ushSmileIDCnt >0)
       ushSmileIDCnt = 0; //stop smile

    if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
    {
       if( m_TalkingText == NULL ){
          InitTalkText( textColor );
       }

       //Doit Ajouter un process text pour reformatter en cas de new option avec des @@
       char strTextMod[2048];
       strTextMod[0] = 0x00;
       ProcessTextRep((char*)txt,strTextMod);
       if(pstrMod)
          sprintf_s(pstrMod,2048,"%s",strTextMod);


       m_TalkingText->SetMaxLines( MaxOverheadLines, 0 );
       bool forceRedo = false;
       if( _stricmp( strTextMod, m_TalkingText->GetText() ) == 0 ){
          forceRedo = true;
       }
       m_TalkingText->ResetText( strTextMod );

       if( forceRedo ){
          m_TalkingText->Redo();
       }

       if( ID == TalkToID ){
          TalkToOffset = m_TalkingText->GetTruncatedTextLineOffset();
       }
    }
    else
    {
       if(!m_TalkingBox)
          m_TalkingBox = new CDisplayTextBox();

       if(m_TalkingBox)
       {
          m_TalkingBox->SetText((char*)txt,textColor,bHyperLink,dwID,pstrMod);
       }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::SetTalkOffset( DWORD talkOffset )
{
   if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
   {
      if( !IsTalkTextDrawn() )
         return;
      m_TalkingText->SetMaxLines( MaxOverheadLines, talkOffset );
      m_TalkingText->Redo();

      if( ID == TalkToID )
         TalkToOffset = m_TalkingText->GetTruncatedTextLineOffset();
   }

}

//////////////////////////////////////////////////////////////////////////////////////////
bool TFCObject::IsTalkTextDrawn()
{
   
   if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
   {
      if( m_TalkingText == NULL )
         return false;
      if( m_TalkingText->IsEmpty() )
         return false;

   }
   else
   {
      if( m_TalkingBox == NULL )
         return false;
      if( m_TalkingBox->IsEmpty() )
         return false;
   }
   return true;
}

bool TFCObject::ProcessHyperLink(DWORD dwTalkPNJID,int imX, int imY, char *pstrLink, DWORD &dwID)
{
   if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
   {
      if( m_TalkingText == NULL )
         return false;
      if( m_TalkingText->IsEmpty() )
         return false;
   }
   else
   {
      if( m_TalkingBox == NULL )
         return false;
      if( m_TalkingBox->IsEmpty() )
         return false;
      return m_TalkingBox->ProcessHyperLink(dwTalkPNJID,imX,imY,pstrLink,dwID);
   }
   return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::DrawTalkText( int x, int y ){
    if( !IsTalkTextDrawn() )
    {
        return;
    }    

    if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
    {
       x -= m_TalkingText->GetWidthTO() / 2;
       y -= m_TalkingText->GetHeight() / 2;
       m_TalkingText->Draw( x, y, NULL );
    }
    else
    {
       x -= m_TalkingBox->GetWidthTO() / 2;
       y -= m_TalkingBox->GetHeight() ;
       m_TalkingBox->Draw( x, y, NULL );
    }
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::StopTalkText()
{
   if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
   {
      if(m_TalkingText)
         delete m_TalkingText;
      m_TalkingText = NULL;
   }
   else
   {
      if(m_TalkingBox)
         m_TalkingBox->SetText(NULL,0,FALSE,0,NULL);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
LPCTSTR TFCObject::GetTalkText()
{
   if( !IsTalkTextDrawn() )
      return "";

   if(g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
   {
      return m_TalkingBox->GetText();
   }
   else
   {
      return m_TalkingText->GetText();
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::InitNameText()
{
    if(m_DisplayName)
       delete m_DisplayName;
    m_DisplayName = NULL;

    m_DisplayName = FontManager::GetInstance()->GetTalkFont();
    
    const int MaxOverheadTextWidth = 300;
    const int OverheadLineSpacing = 0;
    
    m_DisplayName->EnableOutline();
    m_DisplayName->EnableCenterJustification();
    m_DisplayName->SetText( ".", NameColor, MaxOverheadTextWidth, OverheadLineSpacing, false, true, false );
    m_DisplayName->ResetText("");
}

void TFCObject::InitGuildNameText()
{
   if(m_DisplayGuildName)
	   delete m_DisplayGuildName;
   m_DisplayGuildName = NULL;

	m_DisplayGuildName = FontManager::GetInstance()->GetTalkFont();//BLBLBL
    
    const int MaxOverheadTextWidth = 300;
    const int OverheadLineSpacing = 0;
    
    m_DisplayGuildName->EnableOutline();
    m_DisplayGuildName->EnableCenterJustification();
    m_DisplayGuildName->SetText( ".", GuildColor , MaxOverheadTextWidth, OverheadLineSpacing, false, true, false );
    m_DisplayGuildName->ResetText("");
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::DisplayName()
{
   if( m_DisplayName == NULL )
      InitNameText();
   m_DisplayName->ResetText( m_pName );

   if( m_DisplayGuildName == NULL )
      InitGuildNameText();
   m_DisplayGuildName->ResetText( GuildName );
}
//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::DrawName( int x, int y )
{
   if( !IsNameDrawn() )
      return;

   y+=6;
   int yTexte = y-m_DisplayName->GetHeight() ;
    
   if( lstrlen( m_DisplayGuildName->GetText() ) > 0 )
   {
      yTexte -=m_DisplayGuildName->GetHeight();
      m_DisplayGuildName->Draw( x-m_DisplayGuildName->GetWidthTO() / 2, y-m_DisplayGuildName->GetHeight(), NULL );
   }
   m_DisplayName->Draw( x-m_DisplayName->GetWidthTO() / 2, yTexte, NULL );    
   
   /*
   if( lstrlen( m_DisplayGuildName->GetText() ) > 0 )
   {
      y -= m_DisplayGuildName->GetHeight() / 2;
      m_DisplayGuildName->Draw( x-m_DisplayGuildName->GetWidth() / 2, y, NULL );
      y -= (m_DisplayGuildName->GetHeight()/2+6);
   }
    

   //x -= m_DisplayName->GetWidth() / 2;
   y -= m_DisplayName->GetHeight() / 2;	
   m_DisplayName->Draw( x-m_DisplayName->GetWidth() / 2, y, NULL );    
   */

} 

//////////////////////////////////////////////////////////////////////////////////////////
bool TFCObject::IsNameDrawn()
{
    if( m_DisplayName == NULL || m_DisplayGuildName == NULL ){
        return false;
    }
    if( m_DisplayName->IsEmpty() ){
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::StopNameDisplay()
{
   if(m_DisplayName)
      delete m_DisplayName;
   if(m_DisplayGuildName)
	   delete m_DisplayGuildName;
   m_DisplayName = NULL;
	m_DisplayGuildName = NULL;
}



//NMNMNM degats au dessus de la tete
//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::DisplayDamage(char *pstrDamage, DWORD dwTemp, DWORD dwColor)
{

   
   sDamage *pDamage = new sDamage;
   
   pDamage->DamageText = FontManager::GetInstance()->GetTalkFont();
   //pDamage->DamageText->EnableOutline();
   pDamage->DamageText->EnableCenterJustification();
   pDamage->DamageText->SetText( pstrDamage, dwColor, 100, 0, false, true, false );
   pDamage->iOffY      = 0;
   pDamage->DamageTemp = dwTemp;
   
   EnterCriticalSection(&csLockDamage);
   if(m_vDamageText.size() >0)
   {
      //patch pour compenser les degats trop vites...
      if(m_vDamageText[m_vDamageText.size()-1]->iOffY < 14)
      {
         int iNeedOff = 14 - m_vDamageText[m_vDamageText.size()-1]->iOffY;
         for(unsigned int i=0;i<m_vDamageText.size();i++)
            m_vDamageText[i]->iOffY += iNeedOff;
      }
   }
   m_vDamageText.push_back(pDamage);
   if(m_vDamageText.size()> 8)
      m_vDamageText.erase(m_vDamageText.begin());

   LeaveCriticalSection(&csLockDamage);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::DrawDamage( int x, int y )
{
    if( !IsDamageDrawn() )
        return;

    EnterCriticalSection(&csLockDamage);
    int xRef = x;
    int yRef = y;
    
    unsigned int i;
    for( i = 0; i < m_vDamageText.size(); i++ )
    {
       xRef = x;
       yRef = y;

       xRef -= m_vDamageText[i]->DamageText->GetWidthTO() / 2;
       yRef -= m_vDamageText[i]->DamageText->GetHeight() / 2;
       yRef -= m_vDamageText[i]->iOffY;
       m_vDamageText[i]->iOffY+=1;

       m_vDamageText[i]->DamageText->Draw( xRef, yRef, NULL );    
    }
    LeaveCriticalSection(&csLockDamage);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TFCObject::IsDamageDrawn()
{
   if(m_vDamageText.size() >0)
      return true;
    
    return false;
}

bool TFCObject::ValidDamageDrawn(DWORD dwTime)
{

    EnterCriticalSection(&csLockDamage);
    std::vector< sDamage * >::iterator i;
    for( i = m_vDamageText.begin(); i != m_vDamageText.end(); i++ )
    {
        if ((dwTime - (*i)->DamageTemp) > 2500)
        {
            sDamage *pTmp = *i;
            delete pTmp->DamageText;
            delete pTmp;
            m_vDamageText.erase( i );
            LeaveCriticalSection(&csLockDamage);
            return true;
        }
    }
    LeaveCriticalSection(&csLockDamage);
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TFCObject::StopDamageDisplay()
{
    EnterCriticalSection(&csLockDamage);
    unsigned int i;
    for( i = 0; i < m_vDamageText.size(); i++ )
    {
       delete m_vDamageText[i]->DamageText;
       delete m_vDamageText[i];
    }
    
    m_vDamageText.clear();
    LeaveCriticalSection(&csLockDamage);
}