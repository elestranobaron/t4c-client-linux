#ifndef __Vircom_Handler_061743111897
#define __Vircom_Handler_061743111897

#include "TemplateList.h"
#include "VKey.h"
#include "INITools.h"
#include <vector>

using namespace vir;

//*******************************************************************************************//
struct HandlerDesc
//*******************************************************************************************//
// Structure of
//
{
	public:
        VKey vKey;

		UINT (* CallBack)(LPVOID);

		LPVOID lpParam;
};

//*******************************************************************************************//
class MacroHandler : public TemplateList <HandlerDesc>
//*******************************************************************************************//
// Handle all of the User-Defined Macro
//
{
	public:
		MacroHandler ( void ); // Constructor.
		~MacroHandler( void ); // Destructor.

		BOOL CallMacro( VKey vKey );
		void AddNewMacro( VKey vKey, UINT (* CallBack)(LPVOID), LPVOID);
        void DeleteMacro( VKey vKey );
        BOOL MacroExists( VKey vKey  );
        void DisableMacroCall( void );
        void EnableMacroCall( void );

};

struct IgnoreName {
	char *Name;
};

typedef struct _sMp3List
{
   char strName[1024];
   }sMp3List;

//Some are saved to confif , other only avaible in realtime mode...
struct UserGeneralConfig 
{
   //Saved
   int           *g_pNewCCColor;
   char          gstrMp3PlayList[255];
   BOOL          gdwUSEMMX;
   BOOL          gdwUSEVBOX;
   BOOL          gDisConMusic;
   BOOL          gWindowed;
   MacroHandler  gMacro;

   //Unsaved
   CINITools     gIniTools;
   std::vector< sMp3List > gMp3List;
   int           dwLastMp3PlayerState;
   unsigned int  gTextColor;       //defautl talk text color
   BOOL          gWindowedSizing;  //if client is in resizing
   int           g_dwNbrCCColor;
   BOOL          gTakeScreenShot;
   BOOL          gUpdateFPSOption;
   BOOL          gWinterrize;
   BOOL          gHIDEAllDecors;
   BOOL          gDebugLightTime;
   BOOL          gDebugNoLight;
   BOOL          gDebugLightDay;
   BOOL          gDebugLightNight;
   BOOL          gDebugGUIHidden;
   BYTE          gTIME_R;
   BYTE          gTIME_G;
   BYTE          gTIME_B;
   bool          gbEnableGameFog;
   bool          gbEnableGameRain;
   bool          gbEnableGameSnow;
   BOOL          gWebUpdateEnabled;
   int           gdwCurrentSongIndex;
   int           iLanguageID;


   //Config
   bool		     gfirstTimeAddOn;
   BOOL          gShowFPS;
   BOOL          NoConfigFileFound;

   //variable pour le reload...
   short         m_shNumberOfPlayer;
   short         m_shSelectedReloadPlayer;
   char          m_strName    [ 50][100];
   short         m_shRace     [ 50];
   short         m_shLevel    [ 50];
   WORD          m_wEquipList [50][17];
   short         m_shHairColor[ 50];
};

extern UserGeneralConfig Custom;

#endif
