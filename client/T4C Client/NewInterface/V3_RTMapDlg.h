// V3_RTMapDlg.h: interface for the V3_RTMapDlg class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"

class V3_RTMapDlg : public BoxUI  
{
public:	
	virtual ~V3_RTMapDlg();

    static V3_RTMapDlg *GetInstance();

    static unsigned int Display( void *null);
    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

private:
    V3_RTMapDlg();

    WORD *m_pTMIData;
    WORD *m_pTMIDataTmp;
    BYTE *m_pMapImage;
    BYTE *m_pMapImageM;
    BYTE *m_pRTMapS;
    BYTE *m_pRTPal;

    char m_RTFile[512];
    char m_RTFileC[512];
    int  m_dwCurrentLoadedWorld;

    void    DrawMap       (void);
    BOOL    CreateRTMap   (WORD*,USHORT,USHORT,USHORT);
    void    LoadRTWorld   (int iWorld,int dwX,int dwY);
    void    LoadMapZone   (BYTE *pMap,BYTE *pData,int dwX, int dwY,int dwW,int dwH);

    HRESULT CreateMapMask (BYTE *pData,int ptMap[15][11],int dwW,int dwH);
    int SaveCurrentImage8(BYTE *pshImage,BYTE *pPal,int dwWidth, int dwHeight,char *pstrName,BOOL bFlipV);
    


    GraphUI   m_MainMap;
    GraphUI   m_MainMapMask;
    GraphUI   m_MainMapPPos;
    GraphUI   m_rtMask;

    int       m_dwLoadX;
    int       m_dwLoadY;
    int       m_dwLoadW;

    //int       m_dwLoadZoneXS;
    //int       m_dwLoadZoneYS;
};

