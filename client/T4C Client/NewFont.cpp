#pragma warning( disable : 4291 )

#include "pch.h"
#include "NewFont.h"
#include "IDirectXDraw.h"
#include "FormatText.h"
#include "Packet.h"
#include "Global.h"
#include "SaveGame.h"
extern CSaveGame g_SaveGame;



//SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, 0, &nSmooth, FALSE);
  
int CALLBACK FontManagerCallBack
//**************************************************************************************//
// Load all the Current System Font.
// 
(
 LPLOGFONT lplf,    // Logical Description of a Font.
 LPTEXTMETRIC lptm, // Physical Description of a Font.
 DWORD FontType,    // FontType.
 LPARAM lParam      // User-Defined Parameter.
 )
 // Return: int, 1 Continue enumeration, 0 Stop enumeration 
 //**************************************************************************************//
{
    FontList *fl = new FontList;
    
    memcpy(fl->lplfLogical, lplf, sizeof(LOGFONT));
    fl->lpszFontName = new TCHAR [strlen(lplf->lfFaceName)+1];
    strcpy_s(fl->lpszFontName,strlen(lplf->lfFaceName)+1, lplf->lfFaceName);
    
    FontManager::GetInstance()->AddToTail(fl);
    return 1; 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

FontManager::FontManager(void) {
    // Constructor
    lpvFont = NULL;
}

FontManager::~FontManager(void) {
    // Destructor
    Release();
}

void FontManager::Release(void) {   
    ToHead();
    while(QueryNext()) 
    {
        FontList *flObject;
        flObject = GetObject();
        RemoveObject();
        delete flObject;
    }

    if (lpvFont) 
    {
       delete [] lpvFont;
       lpvFont = NULL;
    }

    
}

FontManager *FontManager::GetInstance(void) {
    // Instance
    static FontManager Instance;
    return &Instance;
}

void FontManager::Initialize(void) {
    // Initialize Font Manager.
    HDC hdc;
    
    DXDGetDC(&hdc, 5);
    
    EnumFonts(hdc, NULL, (FONTENUMPROC) FontManagerCallBack, NULL);

	 std::string strFont = "T4C BeaulieuxV2";
	 //std::string strFont = "courrier";
    
    lpvFont = new FontObject [8];

    CreateFont( "Verdana"        , &lpvFont[0], 14) ;  // Help text defaut
    CreateFont( "Verdana"        , &lpvFont[1], 12);  // statext text SMALLEST font
    CreateFont( (char*)strFont.c_str()  , &lpvFont[2], 22 ); // Talk text
    CreateFont( (char*)strFont.c_str()  , &lpvFont[3], 21 ); // text de chat quand chat log Absent
    CreateFont( (char*)strFont.c_str()  , &lpvFont[4], 22,false,true); //GUI HeaderFont
	 CreateFont( "Verdana"        , &lpvFont[5], 13);  // statext text SMALLEST font
    CreateFont( (char*)strFont.c_str()  , &lpvFont[6], 16 ); // Talk text
    CreateFont( "Verdana"        , &lpvFont[7], 13);  // statext text SMALLEST font
   
    DXDReleaseDC(hdc, 5);
}

void FontManager::CreateFont(LPTSTR lpszFontName, FontObject *foReturn, double Pc,bool bBold,bool bHQ) 
{
    FontList *flObject;
    bool Found = false;
    
    ToHead();
    while(QueryNext()) 
    {
        flObject = GetObject();
        if (!strcmp(flObject->lpszFontName, lpszFontName)) 
        {
            foReturn->Create(flObject->lplfLogical, Pc,bBold,bHQ);
            Found = true;
            SetQueryState(STOP_QUERY);
        }
    }
    
    if (!Found) 
    {
        ToHead();
        while(QueryNext()) 
        {
            flObject = GetObject();
            if (!strcmp(flObject->lpszFontName, "System")) 
            {
                foReturn->Create(flObject->lplfLogical, Pc);
                Found = true;
                SetQueryState(STOP_QUERY);
            }
        }
    }
}

TextObject *FontManager::GetSmallFont(LPCTSTR szDefaultText) 
{
    TextObject *toReturn = new TextObject;
    
    toReturn->foFont = &lpvFont[0];
    
    if( szDefaultText )
    {
        toReturn->SetText( const_cast< char * >( szDefaultText ) );
    }
    
    return toReturn;
};

TextObject *FontManager::GetSmallestFont()
{
   TextObject *toReturn = new TextObject;
   
   
   toReturn->foFont = &lpvFont[1];
   
   return toReturn;
}

TextObject *FontManager::GetMiddleFont()
{
	TextObject *toReturn = new TextObject;


	toReturn->foFont = &lpvFont[5];

	return toReturn;
}

TextObject *FontManager::GetMiddleFontT4C()
{
   TextObject *toReturn = new TextObject;


   toReturn->foFont = &lpvFont[6];

   return toReturn;
}

FontObject *FontManager::GetDisplayTextFont()
{   
   return &lpvFont[7];
}



TextObject *FontManager::GetTalkFont()
{
    TextObject *toReturn = new TextObject;
    toReturn->foFont = &lpvFont[2];
    return toReturn;
}

TextObject *FontManager::GetSysMsgFont()
{
   TextObject *toReturn = new TextObject;
   toReturn->foFont = &lpvFont[3];
   return toReturn;
}

HFONT FontManager::GetGUIHFontHandle()
{
   return lpvFont[4].GetHFONT();
}








/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

FontObject::FontObject(void) {
    // Constructor
}

FontObject::~FontObject(void) {
    // Destructor
}

void FontObject::Create(LPLOGFONT lplf, double Pc,bool bBold,bool bHQ) 
{
    LPLOGFONT lplfLogical = new LOGFONT;
    memcpy(lplfLogical, lplf, sizeof(LOGFONT));
    
    
    lplfLogical->lfWidth  = 0;
    lplfLogical->lfHeight = (LONG)Pc;

    if(bHQ)
      lplfLogical->lfQuality = ANTIALIASED_QUALITY;    
    else
      lplfLogical->lfQuality = NONANTIALIASED_QUALITY;    

    if(bBold)
       lplfLogical->lfWeight = FW_BOLD;    
    else
       lplfLogical->lfWeight = FW_NORMAL;    

    
    
    hf = CreateFontIndirect(lplfLogical);

    delete lplfLogical;
}
 
double FontObject::GetFHeight(HDC hdc) 
{
   SIZE sz; 
   sz.cx = 0; 
   sz.cy = 0;
   HANDLE hOld = SelectObject(hdc, hf);
   GetTextExtentPoint32(hdc, "X", 1, &sz); 
   SelectObject(hdc,hOld);
   return sz.cy;
}

double FontObject::GetFWidth(HDC hdc, LPCTSTR s) 
{
   SIZE sz;
   sz.cx = 0;
   sz.cy = 0;
   HANDLE hOld = SelectObject(hdc, hf);
   GetTextExtentPoint32(hdc, s, lstrlen(s), &sz); 
   SelectObject(hdc,hOld);
   return sz.cx;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

TextObject::TextObject(void) :
m_TruncatedTextLineOffset( 0 ),
bRelease( false ),
allowNewLine( true ),
dwFlag( TO_DEFAULT ),
indentNewLine( false ),
m_LineSpacing( 0 ),
m_MaxWidth( g_Global.GetScreenW()),
m_BlackBackground( true ),
m_MaxLines( INFINITE ),
m_StartLineOffset( 0 ),
m_Outline(false),
m_CenterJustify(false)
{
   m_Text.clear();
}

TextObject::~TextObject(void) {
    // Destructor
    TextSprite.Release();
}

bool isempty(const char *s) {
    int l = strlen(s);
    for (int i = 0; i < l; i++)
        if (s[i] != ' ')
            return false;
        return true;
}

void TextObject::SetFlag(DWORD dwF) {
    dwFlag = dwF;
}

void TextObject::Format( int maxX, int maxY ){
    bRelease = false;
    dwUse = timeGetTime();
    
    m_MaxWidth = maxX;
    
    if( maxY != -1 ){
        m_LineSpacing = maxY;
    }
    
    if( !m_Text.empty() && !isempty(m_Text.c_str()) ){
        Redo();
    }
}

void TextObject::SetText(LPCTSTR s, DWORD color, int maxWidth, int lineSpacing, bool blackBackground, bool iallowNewLine, bool indentNewLineP ) {
    // Variable
    // If the provided string is not empty, and the string or color has changed.
    if (s && s[0] && !isempty(s) && ( strcmp( s, m_Text.c_str() ) != 0 || color != oldColor ) ) {
        
        bRelease = false;
        dwUse = timeGetTime();
        oldColor = color;
        allowNewLine = iallowNewLine;
        
        m_MaxWidth = maxWidth;
        m_LineSpacing = lineSpacing;
        m_BlackBackground = blackBackground;
        indentNewLine = indentNewLineP;
        
        // Set the String.
        m_Text = s;
        
        // Format the String for Multiple Line
        Redo(); 

    } else if( s ){
        m_Text = s;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextObject::ResetText
//////////////////////////////////////////////////////////////////////////////////////////
//  Only changes the text without any onther settings changes
// 
(
 LPCTSTR text // The text.
 )
 //////////////////////////////////////////////////////////////////////////////////////////
{
    SetText( text, oldColor, m_MaxWidth, m_LineSpacing, m_BlackBackground );
}

void TextObject::Release(void) 
{
    bRelease = true;
    TextSprite.Release();
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextObject::TextColor
//////////////////////////////////////////////////////////////////////////////////////////
(
 HDC hdc,
 const char *s,
 int xPos,
 int yPos,
 DWORD &CurrentColor,
 DWORD &NewColor,
 bool skipColorCoding,
 bool transparentBackground
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    char Text[1000];
    char *ptr;
    UINT j;
    bool bDraw = false;   
    
    SelectObject(hdc, foFont->hf);
    
    if( transparentBackground ){
        SetBkMode(hdc, TRANSPARENT);
    }else{
        SetBkMode(hdc, OPAQUE);
    
        if (!m_BlackBackground) {
            SetBkColor(hdc, CL_YELLOW_DARK);
        } else {
            SetBkColor(hdc, CL_BLACK);
        }
    }
    SetTextColor(hdc, CurrentColor);
    
    ptr = Text;
    strcpy_s(Text,1000, s);
    DWORD colorSwitch = 0;
    
    for (j = 0; j < strlen(s); j++) {
        switch (Text[j]) {
        case '{':
            if (!(dwFlag & TO_DRAW_QUOTES)) {
                Text[j] = 0;
                if( !skipColorCoding ){
                    NewColor = CL_GRAY_LIGHT_MID;
                }
                bDraw = true;
            }
            break;
        case '}':
            if (!(dwFlag & TO_DRAW_QUOTES)) {
                Text[j] = 0;
                if( !skipColorCoding ){
                    NewColor = oldColor;
                }
                bDraw = true;
            }
            break;
        case '\"':
            if (!(dwFlag & TO_DRAW_QUOTES)) {
                colorSwitch++;
                Text[j] = 0;
                if( !skipColorCoding ){
                    if (colorSwitch & 1){
                        NewColor = CL_WHITE;
                    }else{
                        NewColor = oldColor;
                    }
                }
                bDraw = true;
            }
            break;
        } 
        if (bDraw || j == (strlen(s)-1)) {
            bDraw = false;
            char szTemp[2048];
            sprintf_s(szTemp,2048, "|%s|", ptr);
            SetTextColor(hdc, CurrentColor);
            ::TextOut(hdc, xPos, yPos, ptr, strlen(ptr));
            
            if( !skipColorCoding ){
                CurrentColor = NewColor;
            }
            
            xPos = xPos+ (int)foFont->GetFWidth(hdc, ptr);
            ptr = &Text[j+1];
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
const char *GetMoreLinesToken(){    
    // Add "More" character.
    static char tmp[4];
    _snprintf_s( tmp, 4, "\"%c\"", 254 );
    return tmp;
    
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextObject::Redo(void) {
   // If the provided string is not empty, and the string or color has changed.
   HDC hdc;
   int iW = 0, iH = 0;
   int i;
   
   bRelease = false;
   
   static const char *MoreLinesToken = GetMoreLinesToken();
   
   // Format the String for Multiple Line
   if (m_MaxWidth != 0) 
   {
      FormatText ft;
      if( !allowNewLine )
      {
         ft.SetSystem( FALSE );
      }
      ft.SetText(m_Text.c_str());
      ft.Format(m_MaxWidth, foFont, indentNewLine);
      
      // Create the DD Surface
      
      DXDGetDC(&hdc, 6);
      iH = (int)foFont->GetFHeight(hdc);
      iW = (int)foFont->GetFWidth(hdc, ft.GetText(0));
      
      iNbLine = ft.GetnbLine();
      
      // If the start offset is beyond the text's farthest line.
      int startOffset = m_StartLineOffset;
      if( startOffset >= iNbLine )
      {
         DXDReleaseDC(hdc, 6);
         
         Release();
         m_Text = "";
         return;
      }         
      
      bool moreLines = false;
      if( (DWORD)(iNbLine - startOffset) > m_MaxLines )
      {
         iNbLine = startOffset + m_MaxLines;
         moreLines = true;
      }
      
      for (i = startOffset; i < iNbLine; i++)  
      {
         int iT = (int)foFont->GetFWidth(hdc, ft.GetText(i));
         if (iW < iT)
            iW = iT;
      }
      
      m_TruncatedTextLineOffset = iNbLine;
      
      int spriteHeight = (iH + m_LineSpacing) * (iNbLine - startOffset);
      
      const int BorderOutlineWidth = 2;
      if( m_Outline )
      {
         iW += BorderOutlineWidth;
         spriteHeight += BorderOutlineWidth;
         if( moreLines )
         {
            iW = iW+ (int)foFont->GetFWidth( hdc, MoreLinesToken );
         }
      }        
      DXDReleaseDC(hdc, 6);
      
      
      TextSprite.Release(); 
      
      if(iW < 0)
         iW = m_MaxWidth+50;
      
      TextSprite.Create(iW, spriteHeight, !m_BlackBackground, !m_BlackBackground ? ((lpDXDirectDraw->wGMask >> 1) & lpDXDirectDraw->wGMask) + ((lpDXDirectDraw->wRMask >> 1) & lpDXDirectDraw->wRMask) : 0);
      
      // Draw the Text
      if(TextSprite.GetSurface())
      {
         TextSprite.GetSpriteDC(&hdc);

         DWORD CurrentColor = oldColor;
         DWORD NewColor = oldColor;

         if( m_Outline )
         {
            DWORD Black = CL_BLACK;

            for (i = startOffset; i < iNbLine; i++) 
            {
               int xPos = 0;
               if( m_CenterJustify )
               {
                  xPos = (int)((iW / 2) - (foFont->GetFWidth( hdc, ft.GetText(i) ) / 2));
               }

               string text( ft.GetText(i) );
               if( moreLines && i + 1 >= iNbLine )
               {
                  text += MoreLinesToken;
               }

               int yPos = (i - startOffset) * (iH + m_LineSpacing);
               TextColor(hdc, text.c_str(), xPos + 0, yPos, Black, Black, true, false);
               TextColor(hdc, text.c_str(), xPos + 2, yPos + 2, Black, Black, true, true);
               TextColor(hdc, text.c_str(), xPos + 1, yPos + 1, CurrentColor, NewColor, false, true);
            }             
         }
         else
         {
            for (i = startOffset; i < iNbLine; i++) {
               TextColor(hdc, ft.GetText(i), 0, i*(iH + m_LineSpacing), CurrentColor, NewColor, false, false);
            }
         }
         TextSprite.ReleaseSpriteDC(hdc);
      }
      
   } 
   else 
   {
      DXDGetDC(&hdc, 7);
      iH = (int)foFont->GetFHeight(hdc);
      iW = (int)foFont->GetFWidth(hdc, m_Text.c_str());
      DXDReleaseDC(hdc, 7);
      TextSprite.Release();
      TextSprite.Create(iW, iH, !m_BlackBackground, !m_BlackBackground ? ((lpDXDirectDraw->wGMask >> 1) & lpDXDirectDraw->wGMask) + ((lpDXDirectDraw->wRMask >> 1) & lpDXDirectDraw->wRMask) : 0);
      if(TextSprite.GetSurface())
      {
         TextSprite.GetSpriteDC(&hdc);
         DWORD CurrentColor = oldColor;
         DWORD NewColor = oldColor;
         TextColor(hdc, m_Text.c_str(), 0, 0, CurrentColor, NewColor, false,false);
         TextSprite.ReleaseSpriteDC(hdc);
      }
   }
}

void TextObject::Draw(int x, int y, V2SPRITEFX *fx) {
    if( m_Text.empty() ){
        return;
    }

    RECT r = {0, 0, g_Global.GetScreenW(), g_Global.GetScreenH()};
    if (fx) {
        r.left   = fx->lpClipRect->left;
        r.top    = fx->lpClipRect->top;
        r.right  = fx->lpClipRect->right;
        r.bottom = fx->lpClipRect->bottom;
    }
    // Out of the Clipping Border.
    if (x > r.right)         return;
    if (y > r.bottom)        return;
    if (x + TextSprite.GetWidth() < r.left) return;
    if (y + TextSprite.GetWidth() < r.top) return;
    
    if (bRelease){
        Redo();
    }
    
    dwUse = timeGetTime();
    TextSprite.DrawSprite(x, y, &r);    
}

void TextObject::Test(void) {
    if (!bRelease) {
        if (timeGetTime() - dwUse > 10000)
            Release();
    }
}

int TextObject::GetFontSize(void) {
    HDC hdc;
    DXDGetDC(&hdc, 8);
    int R = (int)foFont->GetFHeight(hdc);
    DXDReleaseDC(hdc, 8);
    return R;
}

int TextObject::GetHeight(void) {
    if (bRelease)
        Redo();
    
    return TextSprite.GetHeight();
}

int TextObject::GetWidthTO(LPCTSTR s) {
    if( s != NULL ){
        HDC hdc;
        DXDGetDC(&hdc, 9);
        int R = (int)foFont->GetFWidth( hdc, const_cast< char * >( s ) );
        DXDReleaseDC(hdc, 9);
        return R;
    }
    
    if (bRelease)
        Redo();
    
    return TextSprite.GetWidth();
}

void TextObject::SetMaxLines( DWORD maxLines, DWORD startLineOffset )
{
    m_MaxLines = maxLines;
    m_StartLineOffset = startLineOffset;
}

int TextObject::GetTruncatedTextLineOffset()
{
    return m_TruncatedTextLineOffset;
}

void TextObject::EnableOutline()
{
    m_Outline = true;
}

void TextObject::EnableCenterJustification()
{
    m_CenterJustify = true;
}