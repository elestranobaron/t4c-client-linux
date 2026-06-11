#include "pch.h"
#include <math.h>
#include "Puppet.h"
#include "TFCPlayer.h"
#include "TFCObject.h"
#include "COMM.h"
#include "Global.h"


UINT Puppet::Pow2[32];
BOOL Puppet::Pow2Init = FALSE;

void Puppet::Release(void) {
	int i;
	for (i = 0; i < MAX_PUP_PART; i++)
	{
		if (pPuppetPart[i].bAlreadyLoad)
			pPuppetPart[i].Release();
	}
	delete[]pPuppetPart;
}

BOOL Puppet::Release(DWORD Wait) {
	return FALSE;
}

Puppet::Puppet(void)
{
	pPuppetPart = new Sprite3D[MAX_PUP_PART];
	for (int i = 0; i<MAX_PUP_PART; i++)
		pPuppetPart[i].bAlreadyLoad = FALSE;


	LastAccess = timeGetTime();
	if (!Pow2Init) {
		Pow2Init = TRUE;
		for (int i = 0; i < 32; i++)
			Pow2[i] = (UINT)pow(2.00, i);
	}
}

Puppet::~Puppet(void) {
	Release();
}

#define KEEPALIVE_LOADING	if (timeGetTime() - dwLoadTime > 1500) {\
		TFCPacket Send; \
		dwLoadTime = timeGetTime();\
		Send << (short)10;\
		SEND_PACKET(Send);	\
		COMM.KeepAlive();\
		Sleep(50);\
	}


void Puppet::LoadBodyPart(void) {

	return; //NMNMNMNMNMNMNMNMNM

	DWORD dwLoadTime = timeGetTime();
	DWORD dwLoadTime2 = timeGetTime();

	pPuppetPart[0].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHandL", 0, 0, 0);
	pPuppetPart[1].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveL", 0, 0, 0);
	pPuppetPart[2].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0);
	pPuppetPart[3].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmL", 0, 0, 0);
	pPuppetPart[4].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0);
	pPuppetPart[5].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedFoot", 0, 0, 0);
	pPuppetPart[6].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedLegs", 0, 0, 0);
	pPuppetPart[7].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherPants", 0, 0, 0);
	pPuppetPart[8].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0);
	pPuppetPart[9].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainLegs", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[10].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLegsClothSet1", 0, 0, 0);
	pPuppetPart[11].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupStuddedLegs", 0, 0, 0);
	pPuppetPart[12].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedBody", 0, 0, 0);
	pPuppetPart[13].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherBody", 0, 0, 0);
	pPuppetPart[14].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0);
	pPuppetPart[15].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupChainMailBody", 0, 0, 0);
	pPuppetPart[16].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNecromanRobe", 0, 0, 0);
	pPuppetPart[17].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupStuddedBodyArmor", 0, 0, 0);
	pPuppetPart[18].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBodyClothSet1", 0, 0, 0);
	pPuppetPart[19].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[20].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedRobe", 0, 0, 0);
	pPuppetPart[21].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupMageRobe", 0, 0, 0);
	pPuppetPart[22].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHead", 0, 0, 0);
	pPuppetPart[23].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0);
	pPuppetPart[24].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupChainMailCoif", 0, 0, 0);
	pPuppetPart[25].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupHornedHelmet", 0, 0, 0);
	pPuppetPart[26].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHandR", 0, 0, 0);
	pPuppetPart[27].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveR", 0, 0, 0);
	pPuppetPart[28].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0);
	pPuppetPart[29].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmR", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[30].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0);
	pPuppetPart[31].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupBattleAxe", 0, 0, 0);
	pPuppetPart[32].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupDarkSword", 0, 0, 0);
	pPuppetPart[33].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupFlail", 0, 0, 0);
	pPuppetPart[34].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupNormalSword", 0, 0, 0);
	pPuppetPart[35].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupBattleSword", 0, 0, 0);
	pPuppetPart[36].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupRealDarkSword", 0, 0, 0);
	pPuppetPart[37].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupMorningStar", 0, 0, 0);
	pPuppetPart[38].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupGoldenMorningStar", 0, 0, 0);
	pPuppetPart[39].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupBattleDagger", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[40].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupWarHammer", 0, 0, 0);
	pPuppetPart[41].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupMace", 0, 0, 0);
	pPuppetPart[42].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupRomanShield", 0, 0, 0);
	pPuppetPart[43].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupBarossaShield", 0, 0, 0);
	pPuppetPart[44].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupOrcShield", 0, 0, 0);
	pPuppetPart[45].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherBoots", 0, 0, 0);
	pPuppetPart[46].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0);
	pPuppetPart[47].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBlackLeatherBoots", 0, 0, 0);
	pPuppetPart[48].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGoldenCrown", 0, 0, 0);
	pPuppetPart[49].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupElvenHat", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[50].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0);
	pPuppetPart[51].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupArmoredRobeUnder", 0, 0, 0);
	pPuppetPart[52].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0);
	pPuppetPart[53].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHead", 0, 0, 0);
	pPuppetPart[54].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedBody", 0, 0, 0);
	pPuppetPart[55].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedLegs", 0, 0, 0);
	pPuppetPart[56].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedFeet", 0, 0, 0);
	pPuppetPart[57].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHandR", 0, 0, 0);
	pPuppetPart[58].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHandL", 0, 0, 0);
	pPuppetPart[59].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmR", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[60].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmL", 0, 0, 0);
	pPuppetPart[61].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0);
	pPuppetPart[62].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherLegs", 0, 0, 0);
	pPuppetPart[63].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherBody", 0, 0, 0);
	pPuppetPart[64].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoStuddedLegs", 0, 0, 0);
	pPuppetPart[65].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoStuddedBody", 0, 0, 0);
	pPuppetPart[66].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainMailLegs", 0, 0, 0);
	pPuppetPart[67].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainBody", 0, 0, 0);
	pPuppetPart[68].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0);
	pPuppetPart[69].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[70].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoClothRobe", 0, 0, 0);
	pPuppetPart[71].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoClothBody", 0, 0, 0);
	pPuppetPart[72].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0);

	pPuppetPart[73].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherArms", 0, 0, 0);
	pPuppetPart[74].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherBoots", 0, 0, 0);
	pPuppetPart[75].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveL", 0, 0, 0);
	pPuppetPart[76].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveR", 0, 0, 0);
	pPuppetPart[77].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupRedCapeTop", 0, 0, 0); //NAD
	pPuppetPart[78].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0);
	pPuppetPart[79].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupWoodenStaff", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[80].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupLichStaff", 0, 0, 0);
	pPuppetPart[81].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupGemStaff", 0, 0, 0);
	pPuppetPart[82].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSimpleStaff", 0, 0, 0);
	pPuppetPart[83].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0);
	pPuppetPart[84].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0);
	pPuppetPart[85].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0);
	pPuppetPart[86].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoBlackLeatherBoots", 0, 0, 0);
	pPuppetPart[87].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainMailCoif", 0, 0, 0);
	pPuppetPart[88].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0);


	KEEPALIVE_LOADING;

	pPuppetPart[90].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherHelm", 0, 0, 0);
	pPuppetPart[91].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherHelm", 0, 0, 0);
	pPuppetPart[92].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNecromanRobe", 0, 0, 0);
	pPuppetPart[93].LoadSprite3D(13, 0, 0, 0, 0, 9, "PupShortBow", 0, 0, 0);
	pPuppetPart[94].LoadSprite3D(13, 0, 0, 0, 0, 9, "PupLongBow", 0, 0, 0);
	pPuppetPart[95].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoRedRobe", 0, 0, 0);
	pPuppetPart[96].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupOgreClub", 0, 0, 0);
	pPuppetPart[97].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoMageRobe", 0, 0, 0);
	pPuppetPart[98].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoMageRobeUnder", 0, 0, 0);
	pPuppetPart[99].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupMageRobeCollar", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[100].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupCentaurShield2", 0, 0, 0);
	pPuppetPart[101].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupCentaurShield", 0, 0, 0);
	pPuppetPart[102].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupShamanHelm", 0, 0, 0);
	pPuppetPart[103].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkavenClub", 0, 0, 0);
	pPuppetPart[104].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkavenKnife", 0, 0, 0);
	pPuppetPart[105].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkavenShield1", 0, 0, 0);
	pPuppetPart[106].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkavenShield2", 0, 0, 0);
	pPuppetPart[107].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkavenShield3", 0, 0, 0);
	pPuppetPart[108].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonAxe", 0, 0, 0);
	pPuppetPart[109].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSkeletonHelm", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[110].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0);
	pPuppetPart[111].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonSword", 0, 0, 0);
	pPuppetPart[112].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphBlackWings", 0, 0, 0);
	pPuppetPart[113].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0);
	pPuppetPart[114].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupHalberd", 0, 0, 0);

	//Dialsoft new graphics
	pPuppetPart[115].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedFeHelmet", 0, 0, 0);
	pPuppetPart[116].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupFireFlail", 0, 0, 0);
	pPuppetPart[117].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSpikeLeatherBody", 0, 0, 0);

	// Wings & derives
	pPuppetPart[118].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphDarkWings", 0, 0, 0);
	pPuppetPart[119].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 3);

	KEEPALIVE_LOADING;

	pPuppetPart[120].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 2);
	pPuppetPart[121].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 4);
	pPuppetPart[122].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 5);
	pPuppetPart[123].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 6);
	pPuppetPart[124].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 7);
	pPuppetPart[125].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 8);

	// Robe derives
	pPuppetPart[126].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 2);
	pPuppetPart[127].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 3);
	pPuppetPart[128].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 4);
	pPuppetPart[129].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 5);

	KEEPALIVE_LOADING;

	pPuppetPart[130].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 6);
	pPuppetPart[131].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 7);
	pPuppetPart[132].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 8);
	pPuppetPart[133].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 9);
	pPuppetPart[134].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 10);
	pPuppetPart[135].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 11);
	pPuppetPart[136].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 12);
	pPuppetPart[137].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 2);
	pPuppetPart[138].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 3);
	pPuppetPart[139].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 4);

	KEEPALIVE_LOADING;

	pPuppetPart[140].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 5);
	pPuppetPart[141].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 6);
	pPuppetPart[142].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 7);
	pPuppetPart[143].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 8);
	pPuppetPart[144].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 9);
	pPuppetPart[145].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 10);
	pPuppetPart[146].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 11);
	pPuppetPart[147].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 12);

	KEEPALIVE_LOADING

		//Tiamat Changes
		//GOLD
		pPuppetPart[148].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 2);
	pPuppetPart[149].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 2);
	pPuppetPart[150].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 2);
	pPuppetPart[151].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 2);
	pPuppetPart[152].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 2);
	pPuppetPart[153].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 2);
	pPuppetPart[154].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 2);
	pPuppetPart[155].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 2);

	KEEPALIVE_LOADING

		//SILVER
		pPuppetPart[156].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 3);
	pPuppetPart[157].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 3);
	pPuppetPart[158].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 3);
	pPuppetPart[159].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 3);
	pPuppetPart[160].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 3);
	pPuppetPart[161].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 3);
	pPuppetPart[162].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 3);
	pPuppetPart[163].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 3);

	KEEPALIVE_LOADING

		//BLACK
		pPuppetPart[164].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 4);
	pPuppetPart[165].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 4);
	pPuppetPart[166].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 4);
	pPuppetPart[167].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 4);
	pPuppetPart[168].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 4);
	pPuppetPart[169].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 4);
	pPuppetPart[170].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 4);
	pPuppetPart[171].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 4);

	KEEPALIVE_LOADING

		//BLUE
		pPuppetPart[172].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 5);
	pPuppetPart[173].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 5);
	pPuppetPart[174].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 5);
	pPuppetPart[175].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 5);
	pPuppetPart[176].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 5);
	pPuppetPart[177].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 5);
	pPuppetPart[178].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 5);
	pPuppetPart[179].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 5);

	KEEPALIVE_LOADING

		//GREEN
		pPuppetPart[180].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 6);
	pPuppetPart[181].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 6);
	pPuppetPart[182].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 6);
	pPuppetPart[183].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 6);
	pPuppetPart[184].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 6);
	pPuppetPart[185].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 6);
	pPuppetPart[186].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 6);
	pPuppetPart[187].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 6);

	KEEPALIVE_LOADING

		//RED
		pPuppetPart[188].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 7);
	pPuppetPart[189].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 7);
	pPuppetPart[190].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 7);
	pPuppetPart[191].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 7);
	pPuppetPart[192].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 7);
	pPuppetPart[193].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 7);
	pPuppetPart[194].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 7);
	pPuppetPart[195].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 7);

	KEEPALIVE_LOADING

		//PURPLE
		pPuppetPart[196].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 8);
	pPuppetPart[197].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 8);
	pPuppetPart[198].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 8);
	pPuppetPart[199].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 8);
	pPuppetPart[200].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 8);
	pPuppetPart[201].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 8);
	pPuppetPart[202].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 8);
	pPuppetPart[203].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 8);

	KEEPALIVE_LOADING

		//EMERALD
		pPuppetPart[204].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 9);
	pPuppetPart[205].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 9);
	pPuppetPart[206].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 9);
	pPuppetPart[207].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 9);
	pPuppetPart[208].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 9);
	pPuppetPart[209].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 9);
	pPuppetPart[210].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 9);
	pPuppetPart[211].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 9);

	KEEPALIVE_LOADING

		//FEMALE GOLD
		pPuppetPart[212].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 2);
	pPuppetPart[213].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 2);
	pPuppetPart[214].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 2);
	pPuppetPart[215].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 2);
	pPuppetPart[216].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 2);
	pPuppetPart[217].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 2);

	KEEPALIVE_LOADING

		//FEMALE SILVER
		pPuppetPart[218].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 3);
	pPuppetPart[219].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 3);
	pPuppetPart[220].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 3);
	pPuppetPart[221].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 3);
	pPuppetPart[222].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 3);
	pPuppetPart[223].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 3);

	KEEPALIVE_LOADING

		//FEMALE BLACK
		pPuppetPart[224].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 4);
	pPuppetPart[225].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 4);
	pPuppetPart[226].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 4);
	pPuppetPart[227].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 4);
	pPuppetPart[228].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 4);
	pPuppetPart[229].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 4);

	KEEPALIVE_LOADING

		//FEMALE BLUE
		pPuppetPart[230].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 5);
	pPuppetPart[231].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 5);
	pPuppetPart[232].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 5);
	pPuppetPart[233].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 5);
	pPuppetPart[234].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 5);
	pPuppetPart[235].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 5);

	KEEPALIVE_LOADING

		//FEMALE GREEN
		pPuppetPart[236].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 6);
	pPuppetPart[237].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 6);
	pPuppetPart[238].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 6);
	pPuppetPart[239].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 6);
	pPuppetPart[240].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 6);
	pPuppetPart[241].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 6);

	KEEPALIVE_LOADING

		//FEMALE RED
		pPuppetPart[242].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 7);
	pPuppetPart[243].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 7);
	pPuppetPart[244].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 7);
	pPuppetPart[245].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 7);
	pPuppetPart[246].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 7);
	pPuppetPart[247].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 7);

	KEEPALIVE_LOADING

		//FEMALE PURPLE
		pPuppetPart[248].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 8);
	pPuppetPart[249].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 8);
	pPuppetPart[250].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 8);
	pPuppetPart[251].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 8);
	pPuppetPart[252].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 8);
	pPuppetPart[253].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 8);

	KEEPALIVE_LOADING

		//FEMALE EMERALD
		pPuppetPart[254].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 9);
	pPuppetPart[255].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 9);
	pPuppetPart[256].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 9);
	pPuppetPart[257].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 9);
	pPuppetPart[258].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 9);
	pPuppetPart[259].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 9);

	//NEW WEAPONS
	pPuppetPart[260].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupTigerAxe", 0, 0, 0);
	pPuppetPart[261].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupAncientAxe", 0, 0, 0);
	pPuppetPart[262].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGobmask", 0, 0, 0);
	pPuppetPart[263].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupFlower", 0, 0, 0);
	pPuppetPart[264].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupNecroStaff", 0, 0, 0);
	//Dialsoft END
	KEEPALIVE_LOADING

		// NEW CAPES    
		pPuppetPart[265].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 2);
	pPuppetPart[266].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 3);
	pPuppetPart[267].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 4);
	pPuppetPart[268].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 5);
	pPuppetPart[269].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 6);
	pPuppetPart[270].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 7);
	pPuppetPart[271].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 8);

	KEEPALIVE_LOADING;

	pPuppetPart[272].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat1", 0, 0, 0);
	pPuppetPart[273].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat2", 0, 0, 0);
	pPuppetPart[274].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat3", 0, 0, 0);
	pPuppetPart[275].LoadSprite3D(13, 0, 0, 0, 0, 9, "NewBow01", 0, 0, 0);
	pPuppetPart[276].LoadSprite3D(13, 9, 0, 0, 0, "DwarfHammer", 0, 0, 0);
	pPuppetPart[277].LoadSprite3D(13, 9, 0, 0, 0, "AxeDestruction", 0, 0, 0);
	pPuppetPart[278].LoadSprite3D(13, 9, 0, 0, 0, "SwordAngel", 0, 0, 0);
	pPuppetPart[279].LoadSprite3D(13, 9, 0, 0, 0, "Clay2blade", 0, 0, 0);
	pPuppetPart[280].LoadSprite3D(13, 9, 0, 0, 0, "SwordAngel", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[281].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0);
	pPuppetPart[282].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 2);
	pPuppetPart[283].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 3);
	pPuppetPart[284].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 4);
	KEEPALIVE_LOADING;
	pPuppetPart[285].LoadSprite3D(13, 9, 0, 0, 0, "Ham", 0, 0, 0);
	pPuppetPart[286].LoadSprite3D(13, 9, 0, 0, 0, 9, "ArchWings", 0, 0, 0);
	pPuppetPart[287].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoArmoredRobe", 0, 0, 0);
	pPuppetPart[288].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupArmoredRobe", 0, 0, 0);
	pPuppetPart[289].LoadSprite3D(13, 9, 0, 0, 0, 9, "NoelHat", 0, 0, 0);
	pPuppetPart[290].LoadSprite3D(13, 9, 0, 0, 0, "PupOrcShield2", 0, 0, 0);
	pPuppetPart[291].LoadSprite3D(13, 9, 0, 0, 0, "SkShield", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[292].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume01", 0, 0, 0);
	pPuppetPart[293].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume02", 0, 0, 0);
	pPuppetPart[294].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume03", 0, 0, 0);
	pPuppetPart[295].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0);
	pPuppetPart[296].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Hat01", 0, 0, 0);
	pPuppetPart[297].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Mask01", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[298].LoadSprite3D(13, 9, 0, 0, 0, "V2_Viperine", 0, 0, 0);
	pPuppetPart[299].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague01", 0, 0, 0);
	pPuppetPart[300].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague02", 0, 0, 0);
	pPuppetPart[301].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague03", 0, 0, 0);
	pPuppetPart[302].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague04", 0, 0, 0);
	pPuppetPart[303].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague05", 0, 0, 0);
	KEEPALIVE_LOADING;
	//NM_TAG_ADD_SWORD
	pPuppetPart[304].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer01", 0, 0, 0);
	pPuppetPart[305].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer02", 0, 0, 0);
	pPuppetPart[306].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer03", 0, 0, 0);
	pPuppetPart[307].LoadSprite3D(13, 9, 0, 0, 0, "V2_Claymore01", 0, 0, 0);
	pPuppetPart[308].LoadSprite3D(13, 9, 0, 0, 0, "V2_Claymore02", 0, 0, 0);
	pPuppetPart[309].LoadSprite3D(13, 9, 0, 0, 0, "V2_GlaiveSlayer01", 0, 0, 0);
	pPuppetPart[310].LoadSprite3D(13, 9, 0, 0, 0, "V2_GlaiveSlayer02", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[311].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword01", 0, 0, 0);
	pPuppetPart[312].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword02", 0, 0, 0);
	pPuppetPart[313].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword03", 0, 0, 0);
	pPuppetPart[314].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword04", 0, 0, 0);
	pPuppetPart[315].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword05", 0, 0, 0);
	pPuppetPart[316].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword06", 0, 0, 0);
	pPuppetPart[317].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword07", 0, 0, 0);
	pPuppetPart[318].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword08", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[319].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer01", 0, 0, 0);
	pPuppetPart[320].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer02", 0, 0, 0);
	pPuppetPart[321].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer03", 0, 0, 0);
	pPuppetPart[322].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Claymore01", 0, 0, 0);
	pPuppetPart[323].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Claymore02", 0, 0, 0);
	pPuppetPart[324].LoadSprite3D(13, 9, 0, 0, 0, "V2_2GlaiveSlayer01", 0, 0, 0);
	pPuppetPart[325].LoadSprite3D(13, 9, 0, 0, 0, "V2_2GlaiveSlayer02", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[326].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword01", 0, 0, 0);
	pPuppetPart[327].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword02", 0, 0, 0);
	pPuppetPart[328].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword03", 0, 0, 0);
	pPuppetPart[329].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword04", 0, 0, 0);
	pPuppetPart[330].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword05", 0, 0, 0);
	pPuppetPart[331].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword06", 0, 0, 0);
	pPuppetPart[332].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword07", 0, 0, 0);
	pPuppetPart[333].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword08", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[334].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache01", 0, 0, 0);
	pPuppetPart[335].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache02", 0, 0, 0);
	pPuppetPart[336].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache03", 0, 0, 0);
	pPuppetPart[337].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache04", 0, 0, 0);
	pPuppetPart[338].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache05", 0, 0, 0);
	pPuppetPart[339].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache06", 0, 0, 0);
	pPuppetPart[340].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache07", 0, 0, 0);
	pPuppetPart[341].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache08", 0, 0, 0);
	pPuppetPart[342].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache09", 0, 0, 0);
	pPuppetPart[343].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache10", 0, 0, 0);
	pPuppetPart[344].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache11", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[345].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache01", 0, 0, 0);
	pPuppetPart[346].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache02", 0, 0, 0);
	pPuppetPart[347].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache03", 0, 0, 0);
	pPuppetPart[348].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache04", 0, 0, 0);
	pPuppetPart[349].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague01", 0, 0, 0);
	pPuppetPart[350].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague02", 0, 0, 0);
	pPuppetPart[351].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague03", 0, 0, 0);
	pPuppetPart[352].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague04", 0, 0, 0);
	pPuppetPart[353].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague05", 0, 0, 0);
	pPuppetPart[354].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague06", 0, 0, 0);
	KEEPALIVE_LOADING;


	pPuppetPart[355].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow01", 0, 0, 0);
	pPuppetPart[356].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow02", 0, 0, 0);
	pPuppetPart[357].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow03", 0, 0, 0);
	pPuppetPart[358].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow04", 0, 0, 0);
	pPuppetPart[359].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow05", 0, 0, 0);
	pPuppetPart[360].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow06", 0, 0, 0);
	pPuppetPart[361].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow07", 0, 0, 0);
	pPuppetPart[362].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow08", 0, 0, 0);
	pPuppetPart[363].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow09", 0, 0, 0);
	pPuppetPart[364].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow10", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[365].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre01", 0, 0, 0);
	pPuppetPart[366].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre02", 0, 0, 0);
	pPuppetPart[367].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre03", 0, 0, 0);
	pPuppetPart[368].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special01", 0, 0, 0);
	pPuppetPart[369].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special02", 0, 0, 0);
	pPuppetPart[370].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special03", 0, 0, 0);
	pPuppetPart[371].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special04", 0, 0, 0);
	pPuppetPart[372].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special05", 0, 0, 0);
	pPuppetPart[373].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special06", 0, 0, 0);
	pPuppetPart[374].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special07", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[375].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special01", 0, 0, 0);
	pPuppetPart[376].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special02", 0, 0, 0);
	pPuppetPart[377].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special03", 0, 0, 0);
	pPuppetPart[378].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special04", 0, 0, 0);
	pPuppetPart[379].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special05", 0, 0, 0);
	pPuppetPart[380].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special06", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[381].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer01", 0, 0, 0);
	pPuppetPart[382].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer02", 0, 0, 0);
	pPuppetPart[383].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer03", 0, 0, 0);
	pPuppetPart[384].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer04", 0, 0, 0);
	pPuppetPart[385].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer05", 0, 0, 0);
	pPuppetPart[386].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer06", 0, 0, 0);
	pPuppetPart[387].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer07", 0, 0, 0);
	pPuppetPart[388].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hammer01", 0, 0, 0);
	pPuppetPart[389].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hammer02", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[390].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache05", 0, 0, 0);
	pPuppetPart[391].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache06", 0, 0, 0);
	pPuppetPart[392].LoadSprite3D(13, 9, 0, 0, 0, "V2_Shield01", 0, 0, 0);
	pPuppetPart[393].LoadSprite3D(13, 9, 0, 0, 0, "V2_Shield02", 0, 0, 0);
	pPuppetPart[394].LoadSprite3D(13, 9, 0, 0, 0, "V2_IceSword01", 0, 0, 0);
	pPuppetPart[395].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_IceBow01", 0, 0, 0);
	pPuppetPart[396].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGobmask", 0, 0, 0, 2);
	pPuppetPart[397].LoadSprite3D(13, 9, 0, 0, 0, "V2_2IceSword01", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[398].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat4", 0, 0, 0);
	pPuppetPart[399].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat5", 0, 0, 0);
	pPuppetPart[400].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0);
	pPuppetPart[401].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0);
	pPuppetPart[402].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0);
	pPuppetPart[403].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0);
	pPuppetPart[404].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 2);
	pPuppetPart[405].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 2);
	KEEPALIVE_LOADING;
	pPuppetPart[406].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 2);
	pPuppetPart[407].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 2);
	pPuppetPart[408].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0);
	pPuppetPart[409].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 2);
	pPuppetPart[410].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 2);
	pPuppetPart[411].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM_DechuWings", 0, 0, 0);
	KEEPALIVE_LOADING;

	pPuppetPart[412].LoadSprite3D(13, 9, 0, 0, 0, 9, "T4CP_Casque", 0, 0, 0);
	pPuppetPart[413].LoadSprite3D(13, 9, 0, 0, 0, 9, "T4CP_Casquette", 0, 0, 0);
	pPuppetPart[414].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_Shield", 0, 0, 0);
	pPuppetPart[415].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_Matraque", 0, 0, 0);
	pPuppetPart[416].LoadSprite3D(13, 9, 0, 0, 0, "TankBat", 0, 0, 0);
	pPuppetPart[417].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_12", 0, 0, 0);
	pPuppetPart[418].LoadSprite3D(13, 9, 0, 0, 0, "V2_IceShield01", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[419].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBlackLeatherBoots", 0, 0, 0, 2);
	pPuppetPart[420].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoBlackLeatherBoots", 0, 0, 0, 2);
	pPuppetPart[421].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM2_DechuWings", 0, 0, 0);

	KEEPALIVE_LOADING;
	pPuppetPart[422].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 2);
	pPuppetPart[423].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 3);
	pPuppetPart[424].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 4);
	pPuppetPart[425].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 5);
	pPuppetPart[426].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 6);
	pPuppetPart[427].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 7);
	pPuppetPart[428].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 8);
	pPuppetPart[429].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 9);
	pPuppetPart[430].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 10);
	pPuppetPart[431].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 11);
	KEEPALIVE_LOADING;
	pPuppetPart[434].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 2);
	pPuppetPart[435].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 3);
	pPuppetPart[436].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 4);
	pPuppetPart[437].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 5);
	pPuppetPart[438].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 6);
	pPuppetPart[439].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 7);
	pPuppetPart[440].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 8);
	pPuppetPart[441].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 9);
	pPuppetPart[442].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 10);
	pPuppetPart[443].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 11);
	KEEPALIVE_LOADING;

	pPuppetPart[444].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0);
	pPuppetPart[445].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0, 2);
	pPuppetPart[446].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0, 3);
	pPuppetPart[447].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphDarkWings", 0, 0, 0, 2);
	pPuppetPart[448].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeOri", 0, 0, 0);
	pPuppetPart[449].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLichRobeOri", 0, 0, 0);


	KEEPALIVE_LOADING;
	pPuppetPart[450].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeNoir", 0, 0, 0);
	pPuppetPart[451].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeNoir", 0, 0, 0);
	pPuppetPart[452].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeBlanc", 0, 0, 0);
	pPuppetPart[453].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeBlanc", 0, 0, 0);
	pPuppetPart[454].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeGold", 0, 0, 0);
	pPuppetPart[455].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeGold", 0, 0, 0);
	pPuppetPart[456].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeGreen", 0, 0, 0);
	KEEPALIVE_LOADING;
	pPuppetPart[457].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeGreen", 0, 0, 0);
	pPuppetPart[458].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeKimono", 0, 0, 0);
	pPuppetPart[459].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobekimono", 0, 0, 0);
	pPuppetPart[460].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeRose", 0, 0, 0);
	pPuppetPart[461].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeRose", 0, 0, 0);
	pPuppetPart[462].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeRouge", 0, 0, 0);
	pPuppetPart[463].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeRouge", 0, 0, 0);
	KEEPALIVE_LOADING;

	pPuppetPart[464].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0);
	pPuppetPart[465].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 2);
	pPuppetPart[466].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 3);
	pPuppetPart[467].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 4);
	pPuppetPart[468].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 5);
	pPuppetPart[469].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 6);
	pPuppetPart[470].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 7);
	pPuppetPart[471].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 8);
	pPuppetPart[472].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 9);
	KEEPALIVE_LOADING;
	pPuppetPart[473].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0);
	pPuppetPart[474].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 2);
	pPuppetPart[475].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 3);
	pPuppetPart[476].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 4);
	pPuppetPart[477].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 5);
	pPuppetPart[478].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 6);
	pPuppetPart[479].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 7);
	pPuppetPart[480].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 8);
	pPuppetPart[481].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 9);
	KEEPALIVE_LOADING;
	pPuppetPart[482].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0);
	pPuppetPart[483].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 2);
	pPuppetPart[484].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 3);
	pPuppetPart[485].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 4);
	pPuppetPart[486].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 5);
	pPuppetPart[487].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 6);
	pPuppetPart[488].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 7);
	pPuppetPart[489].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 8);
	pPuppetPart[490].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 9);
	pPuppetPart[491].LoadSprite3D(13, 9, 0, 0, 0, 9, "ArchWings", 0, 0, 0, 2);
	KEEPALIVE_LOADING;

	pPuppetPart[550].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 2);
	pPuppetPart[551].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 3);
	pPuppetPart[552].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 4);
	pPuppetPart[553].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 5);
	pPuppetPart[554].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 6);
	pPuppetPart[555].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 7);
	pPuppetPart[556].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 8);
	pPuppetPart[557].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 9);
	pPuppetPart[558].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 10);

	KEEPALIVE_LOADING;

	//ORANGE
	pPuppetPart[559].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 10);
	pPuppetPart[560].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmL", 0, 0, 0, 10);
	pPuppetPart[561].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 10);
	pPuppetPart[562].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 10);
	pPuppetPart[563].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 10);
	pPuppetPart[564].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 10);
	pPuppetPart[565].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0, 10);
	pPuppetPart[566].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 10);

	KEEPALIVE_LOADING

		//FEMALE ORANGE
		pPuppetPart[567].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 10);
	pPuppetPart[568].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 10);
	pPuppetPart[569].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 10);
	pPuppetPart[570].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 10);
	pPuppetPart[571].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 10);
	pPuppetPart[572].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 10);
	pPuppetPart[573].LoadSprite3D(13, 9, 0, 0, 0, 9, "HalloweenHead", 0, 0, 0);
	KEEPALIVE_LOADING

		pPuppetPart[574].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaser", 0, 0, 0);
	pPuppetPart[575].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaserMask", 0, 0, 0);
	pPuppetPart[576].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0);
	pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
	pPuppetPart[578].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaser", 0, 0, 0, 2);
	pPuppetPart[579].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0);
	pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
	pPuppetPart[581].LoadSprite3D(13, 9, 0, 0, 0, "NM_CanneNoel", 0, 0, 0);
	pPuppetPart[582].LoadSprite3D(13, 9, 0, 0, 0, "NM_CanneNoelLongue", 0, 0, 0);

	KEEPALIVE_LOADING
		pPuppetPart[583].LoadSprite3D(13, 9, 0, 0, 0, "V2_IceSword01", 0, 0, 0);
	pPuppetPart[584].LoadSprite3D(13, 9, 0, 0, 0, "V2_2IceSword01", 0, 0, 0);
	pPuppetPart[585].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareWings", 0, 0, 0);
	pPuppetPart[586].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareWingsMask", 0, 0, 0);
	pPuppetPart[587].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareBigWings", 0, 0, 0);
	pPuppetPart[588].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareBigWingsMask", 0, 0, 0);
	pPuppetPart[589].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareMedWings", 0, 0, 0);
	pPuppetPart[590].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareMedWingsMask", 0, 0, 0);


	KEEPALIVE_LOADING;
	pPuppetPart[591].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 3);
	pPuppetPart[592].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 3);
	pPuppetPart[593].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 3);
	pPuppetPart[594].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 3);
	pPuppetPart[595].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 3);
	pPuppetPart[596].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 3);

	pPuppetPart[597].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 4);
	pPuppetPart[598].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 4);
	pPuppetPart[599].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 4);
	pPuppetPart[600].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 4);
	pPuppetPart[601].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 4);
	pPuppetPart[602].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 4);

	KEEPALIVE_LOADING;

	pPuppetPart[603].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 5);
	pPuppetPart[604].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 5);
	pPuppetPart[605].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 5);
	pPuppetPart[606].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 5);
	pPuppetPart[607].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 5);
	pPuppetPart[608].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 5);

	pPuppetPart[609].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 6);
	pPuppetPart[610].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 6);
	pPuppetPart[611].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 6);
	pPuppetPart[612].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 6);
	pPuppetPart[613].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 6);
	pPuppetPart[614].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 6);

	KEEPALIVE_LOADING;

	pPuppetPart[615].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 7);
	pPuppetPart[616].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 7);
	pPuppetPart[617].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 7);
	pPuppetPart[618].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 7);
	pPuppetPart[619].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 7);
	pPuppetPart[620].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 7);

	pPuppetPart[621].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 8);
	pPuppetPart[622].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 8);
	pPuppetPart[623].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 8);
	pPuppetPart[624].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 8);
	pPuppetPart[625].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 8);
	pPuppetPart[626].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 8);

	KEEPALIVE_LOADING;

	pPuppetPart[627].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 9);
	pPuppetPart[628].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 9);
	pPuppetPart[629].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 9);
	pPuppetPart[630].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 9);
	pPuppetPart[631].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 9);
	pPuppetPart[632].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 9);

	pPuppetPart[633].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6WhiteWings", 0, 0, 0);
	pPuppetPart[634].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6WhiteWingsMask", 0, 0, 0);
	pPuppetPart[635].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6BlackWings", 0, 0, 0);
	pPuppetPart[636].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6BlackWingsMask", 0, 0, 0);

	KEEPALIVE_LOADING;

	pPuppetPart[637].LoadSprite3D(13, 9, 0, 0, 0, "NM_GhostFauc", 0, 0, 0);
	pPuppetPart[638].LoadSprite3D(13, 9, 0, 0, 0, "NM_GhostFaucMask", 0, 0, 0);

	pPuppetPart[639].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_LutinBoots", 0, 0, 0);
	pPuppetPart[640].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_LutinBoots", 0, 0, 0, 2);
	pPuppetPart[641].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM_OreilleLapin", 0, 0, 0);

	pPuppetPart[642].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0);
	pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);


	KEEPALIVE_LOADING;
	pPuppetPart[644].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 2);
	pPuppetPart[645].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 3);
	pPuppetPart[646].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 4);
	pPuppetPart[647].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 5);

	pPuppetPart[648].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 2);
	pPuppetPart[649].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 3);
	pPuppetPart[650].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 4);
	pPuppetPart[651].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 5);

	KEEPALIVE_LOADING;
	pPuppetPart[652].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0, 2);
	pPuppetPart[653].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0, 3);
	pPuppetPart[654].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0, 4);
	pPuppetPart[655].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0, 5);

	pPuppetPart[656].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_SnowManHat", 0, 0, 0);





}

bool IsRangedObject(TFCObject *obj);

void Puppet::DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object, Sprite3D *pMask)
{
	LastAccess = timeGetTime();
	//NMNMNMNM
	if (Stand == ST_ATTACK_STANDING || Stand == ST_ATTACK)
	{
		if (IsRangedObject(Object))
		{
			for (int i = 0; i < Object->nbBodyPart; i++)
			{
				if (Object->VisiblePart & Pow2[BodyOrderAR[Direction][i][0]])
				{
					if (Object->BodyPart[BodyOrderAR[Direction][i][0]]) //NMNMNM NMNMNM
						Object->BodyPart[BodyOrderAR[Direction][i][0]]->DrawSprite3DR(Direction, 0, X, Y, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrderAR[Direction][i][0]]);
				}
			}
		}
		else
		{
			for (int i = 0; i < Object->nbBodyPart; i++)
			{
				if (Object->VisiblePart & Pow2[BodyOrderA[Direction][i][0]])
				{
					if (Object->BodyPart[BodyOrderA[Direction][i][0]])//NMNMNM NMNMNM
						Object->BodyPart[BodyOrderA[Direction][i][0]]->DrawSprite3D(Direction, 0, X, Y, Stand, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrderA[Direction][i][0]]);
				}
			}
		}
	}
	else
	{
		int i = 0;
		try
		{
			if (IsRangedObject(Object))
			{
				for (i = 0; i < Object->nbBodyPart; i++)
				{
					if (Object->VisiblePart & Pow2[BodyOrderR[Direction][i][Frame]])
					{
						if (Object->BodyPart[BodyOrderR[Direction][i][Frame]])//NMNMNM NMNMNM
							Object->BodyPart[BodyOrderR[Direction][i][Frame]]->DrawSprite3D(Direction, Frame, X, Y, Stand, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrderR[Direction][i][Frame]]);
					}
				}
			}
			else
			{
				for (i = 0; i < Object->nbBodyPart; i++)
				{
					if (Object->VisiblePart & Pow2[BodyOrder[Direction][i][Frame]])
					{
						if (Object->BodyPart[BodyOrder[Direction][i][Frame]])//NMNMNM NMNMNM
							Object->BodyPart[BodyOrder[Direction][i][Frame]]->DrawSprite3D(Direction, Frame, X, Y, Stand, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrder[Direction][i][Frame]]);
					}
				}
			}
		}
		catch (...)
		{
			try
			{

				for (int j = 0; j < 75; j++)
				{
				}

			}
			catch (...)
			{
				throw;
			}
			throw;
		}
	}
}

void Puppet::DrawSprite3DA(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object, Sprite3D *pMask) {
	LastAccess = timeGetTime();
	for (int i = 0; i < Object->nbBodyPart; i++)
	{
		if (Object->VisiblePart & Pow2[BodyOrderA[Direction][i][Frame]])
		{
			if (Object->BodyPart[BodyOrderA[Direction][i][Frame]])
				Object->BodyPart[BodyOrderA[Direction][i][Frame]]->DrawSprite3DA(Direction, Frame, X, Y, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrderA[Direction][i][Frame]]);
		}
	}
}

void Puppet::DrawSprite3DR(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object, Sprite3D *pMask) {
	LastAccess = timeGetTime();
	for (int i = 0; i < Object->nbBodyPart; i++)
	{
		if (Object->VisiblePart & Pow2[BodyOrderAR[Direction][i][Frame]])
		{
			if (Object->BodyPart[BodyOrderAR[Direction][i][Frame]])
				Object->BodyPart[BodyOrderAR[Direction][i][Frame]]->DrawSprite3DR(Direction, Frame, X, Y, Clip, boOutline, bOutlineColor, Surface, Format, Object, Object->BodyPartMask[BodyOrderAR[Direction][i][Frame]]);
		}
	}
}

void Puppet::DrawCorpse(int CorpseFrame, int X, int Y, RECT *Clip, RECT *OutBound, RECT *Wrap, LPDIRECTDRAWSURFACE7 Surface, signed char Format, int AbsX, int AbsY, TFCObject *Object, Sprite3D *pMask) {
	LastAccess = timeGetTime();
	for (int i = 0; i < Object->nbBodyPart; i++) {
		if (Object->VisiblePart & Pow2[BodyOrder[1][i][CorpseFrame]]) {
			Object->BodyPart[BodyOrder[1][i][CorpseFrame]]->DrawCorpse(CorpseFrame, X, Y, Clip, OutBound, Wrap, Surface, Format, AbsX, AbsY, Object, pMask);
		}
	}
}

void Puppet::SetPuppet(TFCObject *Object) {
	LastAccess = timeGetTime();

	Object->VisiblePart = 0;

	for (int p = 0; p<32; p++)
	{
		Object->BodyPart[p] = NULL;
		Object->BodyPartMask[p] = NULL;
	}

	// HAND LEFT
	Object->VisiblePart |= Pow2[0];
	switch (Object->PuppetInfo[0]) {
	case 0:
		if (Object->Type == 10011) {
			pPuppetPart[0].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHandL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[0];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[58].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHandL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[58];
			Object->BodyPartMask[0] = NULL;
		}

		break;
	case 1:
		if (Object->Type == 10011) {
			pPuppetPart[1].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[1];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[75].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[75];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case 2:
		if (Object->Type == 10011) {
			pPuppetPart[2].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[2];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[85].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[85];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case 3:
		if (Object->Type == 10011) {
			pPuppetPart[1].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[1];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[75].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveL", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[75];
			Object->BodyPartMask[0] = NULL;
		}
		break;
		//Tiamat changes
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[148].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 2);
			Object->BodyPart[0] = &pPuppetPart[148];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[217].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 2);
			Object->BodyPart[0] = &pPuppetPart[217];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[156].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 3);
			Object->BodyPart[0] = &pPuppetPart[156];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[223].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 3);
			Object->BodyPart[0] = &pPuppetPart[223];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[164].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 4);
			Object->BodyPart[0] = &pPuppetPart[164];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[229].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 4);
			Object->BodyPart[0] = &pPuppetPart[229];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[172].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 5);
			Object->BodyPart[0] = &pPuppetPart[172];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[235].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 5);
			Object->BodyPart[0] = &pPuppetPart[235];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[180].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 6);
			Object->BodyPart[0] = &pPuppetPart[180];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[241].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 6);
			Object->BodyPart[0] = &pPuppetPart[241];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[188].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 7);
			Object->BodyPart[0] = &pPuppetPart[188];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[247].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 7);
			Object->BodyPart[0] = &pPuppetPart[247];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[196].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 8);
			Object->BodyPart[0] = &pPuppetPart[196];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[253].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 8);
			Object->BodyPart[0] = &pPuppetPart[253];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[204].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 9);
			Object->BodyPart[0] = &pPuppetPart[204];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[259].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 9);
			Object->BodyPart[0] = &pPuppetPart[259];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[559].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveL", 0, 0, 0, 10);
			Object->BodyPart[0] = &pPuppetPart[559];
			Object->BodyPartMask[0] = NULL;
		}
		else {
			pPuppetPart[572].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveL", 0, 0, 0, 10);
			Object->BodyPart[0] = &pPuppetPart[572];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01GR:
		if (Object->Type == 10011)
		{
			pPuppetPart[402].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[402];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[402].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0);
			Object->BodyPart[0] = &pPuppetPart[402];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01BL:
		if (Object->Type == 10011)
		{
			pPuppetPart[406].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 2);
			Object->BodyPart[0] = &pPuppetPart[406];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[406].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 2);
			Object->BodyPart[0] = &pPuppetPart[406];
			Object->BodyPartMask[0] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_GREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[595].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 3);
			Object->BodyPart[0] = &pPuppetPart[595];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[595].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 3);
			Object->BodyPart[0] = &pPuppetPart[595];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_RED:
		if (Object->Type == 10011)
		{
			pPuppetPart[601].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 4);
			Object->BodyPart[0] = &pPuppetPart[601];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[601].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 4);
			Object->BodyPart[0] = &pPuppetPart[601];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
		if (Object->Type == 10011)
		{
			pPuppetPart[607].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 5);
			Object->BodyPart[0] = &pPuppetPart[607];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[607].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 5);
			Object->BodyPart[0] = &pPuppetPart[607];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
		if (Object->Type == 10011)
		{
			pPuppetPart[613].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 6);
			Object->BodyPart[0] = &pPuppetPart[613];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[613].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 6);
			Object->BodyPart[0] = &pPuppetPart[613];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[619].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 7);
			Object->BodyPart[0] = &pPuppetPart[619];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[619].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 7);
			Object->BodyPart[0] = &pPuppetPart[619];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
		if (Object->Type == 10011)
		{
			pPuppetPart[625].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 8);
			Object->BodyPart[0] = &pPuppetPart[625];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[625].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 8);
			Object->BodyPart[0] = &pPuppetPart[625];
			Object->BodyPartMask[0] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		if (Object->Type == 10011)
		{
			pPuppetPart[631].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 9);
			Object->BodyPart[0] = &pPuppetPart[631];
			Object->BodyPartMask[0] = NULL;
		}
		else
		{
			pPuppetPart[631].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorRGlove01", 0, 0, 0, 9);
			Object->BodyPart[0] = &pPuppetPart[631];
			Object->BodyPartMask[0] = NULL;
		}
		break;


		//Tiamat Changes End
	}

	// ARM LEFT
	Object->VisiblePart |= Pow2[1];
	switch (Object->PuppetInfo[1]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[3].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmL", 0, 0, 0);
			Object->BodyPart[1] = &pPuppetPart[3];
			Object->BodyPartMask[1] = NULL;
		}
		else {
			pPuppetPart[60].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmL", 0, 0, 0);
			Object->BodyPart[1] = &pPuppetPart[60];
			Object->BodyPartMask[1] = NULL;
		}
		break;
	case PUPEQ_SET1:
		if (Object->Type == 10011)
		{
			pPuppetPart[3].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmL", 0, 0, 0);
			Object->BodyPart[1] = &pPuppetPart[3];
			Object->BodyPartMask[1] = NULL;
		}
		else
		{
			Object->VisiblePart -= Pow2[1];
		}
		break;
	case PUPEQ_LEATHER:
	case PUPEQ_SPIKEDLEATHER:  // Dialsoft spiked armor arm left
		Object->VisiblePart -= Pow2[1];
		break;
		//Tiamat Changes Start
	case PUPEQ_PLATE_GOLD:
	case PUPEQ_PLATE_SILVER:
	case PUPEQ_PLATE_BLACK:
	case PUPEQ_PLATE_BLUE:
	case PUPEQ_PLATE_GREEN:
	case PUPEQ_PLATE_RED:
	case PUPEQ_PLATE_PURPLE:
	case PUPEQ_PLATE_EMERALD:
	case PUPEQ_PLATE_ORANGE:
	case PUPEQ_PLATE:
	case PUPEQ_V2_MAN_ARMOR01GR:
	case PUPEQ_V2_MAN_ARMOR01BL:
	case PUPEQ_V2_MAN_ARMOR01_GREEN:
	case PUPEQ_V2_MAN_ARMOR01_RED:
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		Object->VisiblePart -= Pow2[1];
		break;
		//Tiamat changes End
	case PUPEQ_CHAIN:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_NECROROBE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_WHITEROBE:
	case PUPEQ_LICHROBE:
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_REDROBE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_MAGEROBE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_STUDDED:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_BLUE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_GREEN:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_DARKBLUE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_VIOLET:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_PURPLE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_YELLOW:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_GRAY:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_MAUVE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_ORANGE:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_BRUN:
		Object->VisiblePart -= Pow2[1];
		break;
	case PUPEQ_FWHITEROBE_DARK:
		Object->VisiblePart -= Pow2[1];
		break;
	};

	// FOOT
	Object->VisiblePart |= Pow2[2];
	switch (Object->PuppetInfo[2]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[5].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedFoot", 0, 0, 0);
			Object->BodyPart[2] = &pPuppetPart[5];
			Object->BodyPartMask[2] = NULL;
		}
		else {
			pPuppetPart[56].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedFeet", 0, 0, 0);
			Object->BodyPart[2] = &pPuppetPart[56];
			Object->BodyPartMask[2] = NULL;
		}
		break;
	case PUPEQ_LEATHER:
		Object->VisiblePart -= Pow2[2];
		break;
		//Tiamat changes start
	case PUPEQ_PLATE_GOLD:
	case PUPEQ_PLATE_SILVER:
	case PUPEQ_PLATE_BLACK:
	case PUPEQ_PLATE_BLUE:
	case PUPEQ_PLATE_GREEN:
	case PUPEQ_PLATE_RED:
	case PUPEQ_PLATE_PURPLE:
	case PUPEQ_PLATE_EMERALD:
	case PUPEQ_PLATE_ORANGE:
	case PUPEQ_PLATE:
	case PUPEQ_V2_MAN_ARMOR01GR:
	case PUPEQ_V2_MAN_ARMOR01BL:
	case PUPEQ_V2_MAN_ARMOR01_GREEN:
	case PUPEQ_V2_MAN_ARMOR01_RED:
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		Object->VisiblePart -= Pow2[2];
		break;
		//Tiamat changes end
	case PUPEQ_CHAIN:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_BLACKLEATHER:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_WHITELEATHER:
	case PUPEQ_LUTIN_RBOOT:
	case PUPEQ_LUTIN_GBOOT:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_NECROROBE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_WHITEROBE:
	case PUPEQ_LICHROBE:
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_REDROBE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_MAGEROBE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_BLUE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_GREEN:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_DARKBLUE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_VIOLET:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_PURPLE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_YELLOW:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_GRAY:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_MAUVE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_ORANGE:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_BRUN:
		Object->VisiblePart -= Pow2[2];
		break;
	case PUPEQ_FWHITEROBE_DARK:
		Object->VisiblePart -= Pow2[2];
		break;

	}

	// LEGS
	Object->VisiblePart |= Pow2[3];
	switch (Object->PuppetInfo[3]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[6].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[6];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[55].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[55];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_LEATHER:
		if (Object->Type == 10011) {
			pPuppetPart[7].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherPants", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[7];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[62].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[62];
			Object->BodyPartMask[3] = NULL;
		}
		break;
		//Tiamat Changes start
	case PUPEQ_PLATE:
		if (Object->Type == 10011) {
			pPuppetPart[8].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[8];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[69].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[69];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[150].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 2);
			Object->BodyPart[3] = &pPuppetPart[150];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[214].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 2);
			Object->BodyPart[3] = &pPuppetPart[214];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[158].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 3);
			Object->BodyPart[3] = &pPuppetPart[158];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[220].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 3);
			Object->BodyPart[3] = &pPuppetPart[220];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[166].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 4);
			Object->BodyPart[3] = &pPuppetPart[166];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[226].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 4);
			Object->BodyPart[3] = &pPuppetPart[226];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[174].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 5);
			Object->BodyPart[3] = &pPuppetPart[174];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[232].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 5);
			Object->BodyPart[3] = &pPuppetPart[232];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[182].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 6);
			Object->BodyPart[3] = &pPuppetPart[182];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[238].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 6);
			Object->BodyPart[3] = &pPuppetPart[238];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[190].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 7);
			Object->BodyPart[3] = &pPuppetPart[190];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[244].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 7);
			Object->BodyPart[3] = &pPuppetPart[244];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[198].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 8);
			Object->BodyPart[3] = &pPuppetPart[198];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[250].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 8);
			Object->BodyPart[3] = &pPuppetPart[250];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[206].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 9);
			Object->BodyPart[3] = &pPuppetPart[206];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[256].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 9);
			Object->BodyPart[3] = &pPuppetPart[256];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[561].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateLegs", 0, 0, 0, 10);
			Object->BodyPart[3] = &pPuppetPart[561];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[569].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateLegs", 0, 0, 0, 10);
			Object->BodyPart[3] = &pPuppetPart[569];
			Object->BodyPartMask[3] = NULL;
		}
		break;
		//Tiamat changes end
	case PUPEQ_CHAIN:
		if (Object->Type == 10011) {
			pPuppetPart[9].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupChainMailLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[9];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[66].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[66];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_SET1:
		if (Object->Type == 10011) {
			pPuppetPart[10].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLegsClothSet1", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[10];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			Object->VisiblePart -= Pow2[3];
		}
		break;
	case PUPEQ_STUDDED:
		if (Object->Type == 10011) {
			pPuppetPart[11].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupStuddedLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[11];
			Object->BodyPartMask[3] = NULL;
		}
		else {
			pPuppetPart[64].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoStuddedLegs", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[64];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_NECROROBE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_WHITEROBE:
	case PUPEQ_LICHROBE:
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_REDROBE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_MAGEROBE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_BLUE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_GREEN:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_DARKBLUE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_VIOLET:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_PURPLE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_YELLOW:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_GRAY:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_MAUVE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_ORANGE:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_BRUN:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_FWHITEROBE_DARK:
		Object->VisiblePart -= Pow2[3];
		break;
	case PUPEQ_V2_MAN_ARMOR01GR:
		if (Object->Type == 10011)
		{
			pPuppetPart[401].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[401];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[401].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0);
			Object->BodyPart[3] = &pPuppetPart[401];
			Object->BodyPartMask[3] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01BL:
		if (Object->Type == 10011)
		{
			pPuppetPart[405].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 2);
			Object->BodyPart[3] = &pPuppetPart[405];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[405].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 2);
			Object->BodyPart[3] = &pPuppetPart[405];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_GREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[592].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 3);
			Object->BodyPart[3] = &pPuppetPart[592];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[592].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 3);
			Object->BodyPart[3] = &pPuppetPart[592];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_RED:
		if (Object->Type == 10011)
		{
			pPuppetPart[598].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 4);
			Object->BodyPart[3] = &pPuppetPart[598];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[598].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 4);
			Object->BodyPart[3] = &pPuppetPart[598];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_BLUE:
		if (Object->Type == 10011)
		{
			pPuppetPart[604].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 5);
			Object->BodyPart[3] = &pPuppetPart[604];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[604].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 5);
			Object->BodyPart[3] = &pPuppetPart[604];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_AQUA:
		if (Object->Type == 10011)
		{
			pPuppetPart[610].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 6);
			Object->BodyPart[3] = &pPuppetPart[610];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[610].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 6);
			Object->BodyPart[3] = &pPuppetPart[610];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_GOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[616].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 7);
			Object->BodyPart[3] = &pPuppetPart[616];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[616].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 7);
			Object->BodyPart[3] = &pPuppetPart[616];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
		if (Object->Type == 10011)
		{
			pPuppetPart[622].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 8);
			Object->BodyPart[3] = &pPuppetPart[622];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[622].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 8);
			Object->BodyPart[3] = &pPuppetPart[622];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		if (Object->Type == 10011)
		{
			pPuppetPart[628].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 9);
			Object->BodyPart[3] = &pPuppetPart[628];
			Object->BodyPartMask[3] = NULL;
		}
		else
		{
			pPuppetPart[628].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLegs01", 0, 0, 0, 9);
			Object->BodyPart[3] = &pPuppetPart[628];
			Object->BodyPartMask[3] = NULL;
		}
		break;

	}

	// BODY
	Object->VisiblePart |= Pow2[4];
	switch (Object->PuppetInfo[4]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[12].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[12];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[54].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[54];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_LEATHER:
		if (Object->Type == 10011) {
			pPuppetPart[13].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[13];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[73].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherArms", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[73];
			Object->BodyPartMask[4] = NULL;
		}
		break;
		//Tiamat Changes
	case PUPEQ_PLATE:
		if (Object->Type == 10011) {
			pPuppetPart[14].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[14];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[68].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[68];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[151].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[151];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[213].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[213];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[159].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[159];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[219].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[219];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[167].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[167];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[225].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[225];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[175].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[175];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[231].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[231];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[183].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[183];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[237].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[237];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[191].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[191];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[243].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[243];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[199].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[199];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[249].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[249];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[207].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[207];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[255].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[255];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[562].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateBody", 0, 0, 0, 10);
			Object->BodyPart[4] = &pPuppetPart[562];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[568].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBody", 0, 0, 0, 10);
			Object->BodyPart[4] = &pPuppetPart[568];
			Object->BodyPartMask[4] = NULL;
		}
		break;
		//Tiamat Changes End
	case PUPEQ_CHAIN:
		if (Object->Type == 10011) {
			pPuppetPart[15].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupChainMailBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[15];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[67].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[67];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NECROROBE:
		if (Object->Type == 10011) {
			pPuppetPart[16].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNecromanRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[16];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[92].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNecromanRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[92];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_WHITEROBE:
		if (Object->Type == 10011) {
			pPuppetPart[19].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[19];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[52].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[52];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_LICHROBE://NAD
		if (Object->Type == 10011)
		{
			pPuppetPart[287].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoArmoredRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[287];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[287].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoArmoredRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[287];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBE://NAD
		if (Object->Type == 10011)
		{
			pPuppetPart[448].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeOri", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[448];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[449].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLichRobeOri", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[449];
			Object->BodyPartMask[4] = NULL;
		}
		break;







	case PUPEQ_NEWLICHROBEBLACK:
		if (Object->Type == 10011)
		{
			pPuppetPart[450].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeNoir", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[450];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[451].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeNoir", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[451];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBEWHITE:
		if (Object->Type == 10011)
		{
			pPuppetPart[452].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeBlanc", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[452];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[453].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeBlanc", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[453];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBEGOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[454].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeGold", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[454];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[455].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeGold", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[455];
			Object->BodyPartMask[4] = NULL;
		}
		break;





	case PUPEQ_NEWLICHROBEGREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[456].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeGreen", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[456];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[457].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeGreen", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[457];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBEKIMONO:
		if (Object->Type == 10011)
		{
			pPuppetPart[458].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeKimono", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[458];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[459].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobekimono", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[459];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBEPINK:
		if (Object->Type == 10011)
		{
			pPuppetPart[460].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeRose", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[460];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[461].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeRose", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[461];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_NEWLICHROBERED:
		if (Object->Type == 10011)
		{
			pPuppetPart[462].LoadSprite3D(13, 9, 0, 0, 0, 9, "ManLichRobeRouge", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[462];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[463].LoadSprite3D(13, 9, 0, 0, 0, 9, "WomanLichRobeRouge", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[463];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_ARMORED_ROBE:
		if (Object->Type == 10011)
		{
			pPuppetPart[288].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupArmoredRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[288];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[287].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoArmoredRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[287];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_REDROBE:
		if (Object->Type == 10011) {
			pPuppetPart[20].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[20];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[95].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoRedRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[95];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_MAGEROBE:
		if (Object->Type == 10011) {
			pPuppetPart[21].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupMageRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[21];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[97].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoMageRobe", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[97];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_STUDDED:
		if (Object->Type == 10011) {
			pPuppetPart[17].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupStuddedBodyArmor", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[17];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[65].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoStuddedBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[65];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_SET1:
		if (Object->Type == 10011) {
			pPuppetPart[18].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBodyClothSet1", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[18];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[71].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoClothBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[71];
			Object->BodyPartMask[4] = NULL;
		}
		break;
		//Dialsoft added spiked armor body
	case PUPEQ_SPIKEDLEATHER:
		if (Object->Type == 10011)
		{
			pPuppetPart[117].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSpikeLeatherBody", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[117];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[73].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherArms", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[73];
			Object->BodyPartMask[4] = NULL;
			//Note : no woman skin for the Spiked leather yet
			//using classical leather instead
		}
		break;
	case PUPEQ_FWHITEROBE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[126].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[126];  // 4 = PUP_BODY
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[137].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[137];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[127].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[127];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[138].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[138];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_DARKBLUE:
		if (Object->Type == 10011) {
			pPuppetPart[128].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[128];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[139].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[139];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_VIOLET:
		if (Object->Type == 10011) {
			pPuppetPart[129].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[129];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[140].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[140];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[130].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[130];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[141].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[141];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_YELLOW:
		if (Object->Type == 10011) {
			pPuppetPart[131].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[131];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[142].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[142];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_GRAY:
		if (Object->Type == 10011) {
			pPuppetPart[132].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[132];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[143].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[143];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_MAUVE:
		if (Object->Type == 10011) {
			pPuppetPart[133].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[133];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[144].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[144];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[134].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 10);
			Object->BodyPart[4] = &pPuppetPart[134];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[145].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 10);
			Object->BodyPart[4] = &pPuppetPart[145];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_BRUN:
		if (Object->Type == 10011) {
			pPuppetPart[135].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 11);
			Object->BodyPart[4] = &pPuppetPart[135];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[146].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 11);
			Object->BodyPart[4] = &pPuppetPart[146];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_FWHITEROBE_DARK:
		if (Object->Type == 10011) {
			pPuppetPart[136].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupWhiteRobe", 0, 0, 0, 12);
			Object->BodyPart[4] = &pPuppetPart[136];
			Object->BodyPartMask[4] = NULL;
		}
		else {
			pPuppetPart[147].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoWhiteRobe", 0, 0, 0, 12);
			Object->BodyPart[4] = &pPuppetPart[147];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01GR:
		if (Object->Type == 10011)
		{
			pPuppetPart[400].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[400];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[400].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0);
			Object->BodyPart[4] = &pPuppetPart[400];
			Object->BodyPartMask[4] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01BL:
		if (Object->Type == 10011)
		{
			pPuppetPart[404].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[404];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[404].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 2);
			Object->BodyPart[4] = &pPuppetPart[404];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_GREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[591].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[591];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[591].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 3);
			Object->BodyPart[4] = &pPuppetPart[591];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_RED:
		if (Object->Type == 10011)
		{
			pPuppetPart[597].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[597];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[597].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 4);
			Object->BodyPart[4] = &pPuppetPart[597];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_BLUE:
		if (Object->Type == 10011)
		{
			pPuppetPart[603].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[603];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[603].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 5);
			Object->BodyPart[4] = &pPuppetPart[603];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_AQUA:
		if (Object->Type == 10011)
		{
			pPuppetPart[609].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[609];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[609].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 6);
			Object->BodyPart[4] = &pPuppetPart[609];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_GOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[615].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[615];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[615].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 7);
			Object->BodyPart[4] = &pPuppetPart[615];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
		if (Object->Type == 10011)
		{
			pPuppetPart[621].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[621];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[621].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 8);
			Object->BodyPart[4] = &pPuppetPart[621];
			Object->BodyPartMask[4] = NULL;
		}
		break;

	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		if (Object->Type == 10011)
		{
			pPuppetPart[627].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[627];
			Object->BodyPartMask[4] = NULL;
		}
		else
		{
			pPuppetPart[627].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBody01", 0, 0, 0, 9);
			Object->BodyPart[4] = &pPuppetPart[627];
			Object->BodyPartMask[4] = NULL;
		}
		break;

		//Dialsoft end
	}

	// HEAD
	Object->VisiblePart |= Pow2[5];
	switch (Object->PuppetInfo[5]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[22].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHead", 0, 0, 0);
			Object->BodyPart[5] = &pPuppetPart[22];
			Object->BodyPartMask[5] = NULL;
		}
		else {
			pPuppetPart[53].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHead", 0, 0, 0);
			Object->BodyPart[5] = &pPuppetPart[53];
			Object->BodyPartMask[5] = NULL;
		}
		break;
	case PUPEQ_LEATHER:
		Object->VisiblePart -= Pow2[5];
		break;
		//Tiamat Changes
	case PUPEQ_PLATE_GOLD:
	case PUPEQ_PLATE_SILVER:
	case PUPEQ_PLATE_BLACK:
	case PUPEQ_PLATE_BLUE:
	case PUPEQ_PLATE_GREEN:
	case PUPEQ_PLATE_RED:
	case PUPEQ_PLATE_PURPLE:
	case PUPEQ_PLATE_EMERALD:
	case PUPEQ_PLATE_ORANGE:
	case PUPEQ_PLATE:
		Object->VisiblePart -= Pow2[5];
		break;
		//Tiamat Changes end
	case PUPEQ_CHAIN:
		Object->VisiblePart -= Pow2[5];
		break;
	case PUPEQ_HORNED:
		Object->VisiblePart -= Pow2[5];
		break;
	}


	// HAND R   
	Object->VisiblePart |= Pow2[6];
	switch (Object->PuppetInfo[6]) {
	case 0:
		if (Object->Type == 10011) {
			pPuppetPart[26].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedHandR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[26];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[57].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedHandR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[57];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case 1:
		if (Object->Type == 10011) {
			pPuppetPart[27].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[27];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[76].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[76];
			Object->BodyPartMask[6] = NULL;
		}
		break;
		//Tiamat Changes
	case 2:
		if (Object->Type == 10011) {
			pPuppetPart[28].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[28];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[84].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[84];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[153].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 2);
			Object->BodyPart[6] = &pPuppetPart[153];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[216].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 2);
			Object->BodyPart[6] = &pPuppetPart[216];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[161].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 3);
			Object->BodyPart[6] = &pPuppetPart[161];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[222].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 3);
			Object->BodyPart[6] = &pPuppetPart[222];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[169].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 4);
			Object->BodyPart[6] = &pPuppetPart[169];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[228].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 4);
			Object->BodyPart[6] = &pPuppetPart[228];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[177].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 5);
			Object->BodyPart[6] = &pPuppetPart[177];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[234].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 5);
			Object->BodyPart[6] = &pPuppetPart[234];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[185].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 6);
			Object->BodyPart[6] = &pPuppetPart[185];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[240].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 6);
			Object->BodyPart[6] = &pPuppetPart[240];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[193].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 7);
			Object->BodyPart[6] = &pPuppetPart[193];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[246].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 7);
			Object->BodyPart[6] = &pPuppetPart[246];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[201].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 8);
			Object->BodyPart[6] = &pPuppetPart[201];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[252].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 8);
			Object->BodyPart[6] = &pPuppetPart[252];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[209].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 9);
			Object->BodyPart[6] = &pPuppetPart[209];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[258].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 9);
			Object->BodyPart[6] = &pPuppetPart[258];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[564].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateGloveR", 0, 0, 0, 10);
			Object->BodyPart[6] = &pPuppetPart[564];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[571].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateGloveR", 0, 0, 0, 10);
			Object->BodyPart[6] = &pPuppetPart[571];
			Object->BodyPartMask[6] = NULL;
		}
		break;
		//Tiamat changes end
	case 3:
		if (Object->Type == 10011) {
			pPuppetPart[27].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[27];
			Object->BodyPartMask[6] = NULL;
		}
		else {
			pPuppetPart[76].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherGloveR", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[76];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01GR:
		if (Object->Type == 10011)
		{
			pPuppetPart[408].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[408];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[408].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0);
			Object->BodyPart[6] = &pPuppetPart[408];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01BL:
		if (Object->Type == 10011)
		{
			pPuppetPart[409].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 2);
			Object->BodyPart[6] = &pPuppetPart[409];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[409].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 2);
			Object->BodyPart[6] = &pPuppetPart[409];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_GREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[593].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 3);
			Object->BodyPart[6] = &pPuppetPart[593];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[593].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 3);
			Object->BodyPart[6] = &pPuppetPart[593];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_RED:
		if (Object->Type == 10011)
		{
			pPuppetPart[599].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 4);
			Object->BodyPart[6] = &pPuppetPart[599];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[599].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 4);
			Object->BodyPart[6] = &pPuppetPart[599];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
		if (Object->Type == 10011)
		{
			pPuppetPart[605].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 5);
			Object->BodyPart[6] = &pPuppetPart[605];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[605].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 5);
			Object->BodyPart[6] = &pPuppetPart[605];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
		if (Object->Type == 10011)
		{
			pPuppetPart[611].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 6);
			Object->BodyPart[6] = &pPuppetPart[611];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[611].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 6);
			Object->BodyPart[6] = &pPuppetPart[611];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[617].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 7);
			Object->BodyPart[6] = &pPuppetPart[617];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[617].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 7);
			Object->BodyPart[6] = &pPuppetPart[617];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
		if (Object->Type == 10011)
		{
			pPuppetPart[623].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 8);
			Object->BodyPart[6] = &pPuppetPart[623];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[623].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 8);
			Object->BodyPart[6] = &pPuppetPart[623];
			Object->BodyPartMask[6] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		if (Object->Type == 10011)
		{
			pPuppetPart[629].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 9);
			Object->BodyPart[6] = &pPuppetPart[629];
			Object->BodyPartMask[6] = NULL;
		}
		else
		{
			pPuppetPart[629].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorLGlove01", 0, 0, 0, 9);
			Object->BodyPart[6] = &pPuppetPart[629];
			Object->BodyPartMask[6] = NULL;
		}
		break;



	}

	// ARM R
	Object->VisiblePart |= Pow2[7];
	switch (Object->PuppetInfo[7]) {
	case PUPEQ_NACKED:
		if (Object->Type == 10011) {
			pPuppetPart[29].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmR", 0, 0, 0);
			Object->BodyPart[7] = &pPuppetPart[29];
			Object->BodyPartMask[7] = NULL;
		}
		else {
			pPuppetPart[59].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmR", 0, 0, 0);
			Object->BodyPart[7] = &pPuppetPart[59];
			Object->BodyPartMask[7] = NULL;
		}
		break;
	case PUPEQ_SET1:
		if (Object->Type == 10011)
		{
			pPuppetPart[29].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmR", 0, 0, 0);
			Object->BodyPart[7] = &pPuppetPart[29];
			Object->BodyPartMask[7] = NULL;
		}
		else
		{
			Object->VisiblePart -= Pow2[7];
		}
		break;
	case PUPEQ_LEATHER:
	case PUPEQ_SPIKEDLEATHER:  // Dialsoft spiked armor arm right
		Object->VisiblePart -= Pow2[7];
		break;
		//Tiamat changes
	case PUPEQ_PLATE_GOLD:
	case PUPEQ_PLATE_SILVER:
	case PUPEQ_PLATE_BLACK:
	case PUPEQ_PLATE_BLUE:
	case PUPEQ_PLATE_GREEN:
	case PUPEQ_PLATE_RED:
	case PUPEQ_PLATE_PURPLE:
	case PUPEQ_PLATE_EMERALD:
	case PUPEQ_PLATE_ORANGE:
	case PUPEQ_PLATE:
		Object->VisiblePart -= Pow2[7];
		/*         if (Object->Type == 10011) {
		pPuppetPart[30].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateArmR", 0, 0, 0);
		Object->BodyPart[7] = &pPuppetPart[30];
		Object->VisiblePart -= Pow2[6];
		} else {
		Object->VisiblePart -= Pow2[7];
		} */
		break;
		//Tiamat changes end
	case PUPEQ_CHAIN:
		Object->VisiblePart -= Pow2[7];
		break;
	case PUPEQ_NECROROBE:
		Object->VisiblePart -= Pow2[7];
		break;
	case PUPEQ_WHITEROBE:
	case PUPEQ_LICHROBE:
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
	case PUPEQ_REDROBE:
	case PUPEQ_MAGEROBE:
	case PUPEQ_STUDDED:
	case PUPEQ_FWHITEROBE_BLUE:
	case PUPEQ_FWHITEROBE_GREEN:
	case PUPEQ_FWHITEROBE_DARKBLUE:
	case PUPEQ_FWHITEROBE_VIOLET:
	case PUPEQ_FWHITEROBE_PURPLE:
	case PUPEQ_FWHITEROBE_YELLOW:
	case PUPEQ_FWHITEROBE_GRAY:
	case PUPEQ_FWHITEROBE_MAUVE:
	case PUPEQ_FWHITEROBE_ORANGE:
	case PUPEQ_FWHITEROBE_BRUN:
	case PUPEQ_FWHITEROBE_DARK:
	case PUPEQ_V2_MAN_ARMOR01GR:
	case PUPEQ_V2_MAN_ARMOR01BL:
	case PUPEQ_V2_MAN_ARMOR01_GREEN:
	case PUPEQ_V2_MAN_ARMOR01_RED:
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		Object->VisiblePart -= Pow2[7];
		break;
	}

	// WEAPON   
	Object->VisiblePart |= Pow2[8];
	switch (Object->PuppetInfo[8]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[8];
		break;
	case PUPEQ_AXE:
		pPuppetPart[31].LoadSprite3D(13, 9, 0, 0, 0, "PupBattleAxe", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[31];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_DARK:
		pPuppetPart[32].LoadSprite3D(13, 9, 0, 0, 0, "PupDarkSword", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[32];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_FLAIL:
		pPuppetPart[33].LoadSprite3D(13, 9, 0, 0, 0, "PupFlail", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[33];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_NORMALSWORD:
		pPuppetPart[34].LoadSprite3D(13, 9, 0, 0, 0, "PupNormalSword", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[34];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_OGRECLUB:
		pPuppetPart[96].LoadSprite3D(13, 9, 0, 0, 0, "PupOgreClub", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[96];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_BATTLESWORD:
		pPuppetPart[35].LoadSprite3D(13, 9, 0, 0, 0, "PupBattleSword", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[35];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_REALDARK:
		pPuppetPart[36].LoadSprite3D(13, 9, 0, 0, 0, "PupRealDarkSword", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[36];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_MORNINGSTAR:
		pPuppetPart[37].LoadSprite3D(13, 9, 0, 0, 0, "PupMorningStar", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[37];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_GOLDENSTAR:
		pPuppetPart[38].LoadSprite3D(13, 9, 0, 0, 0, "PupGoldenMorningStar", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[38];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_BATTLEDAGGER:
		pPuppetPart[39].LoadSprite3D(13, 9, 0, 0, 0, "PupBattleDagger", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[39];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_WARHAMMER:
		pPuppetPart[40].LoadSprite3D(13, 9, 0, 0, 0, "PupWarhammer", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[40];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_MACE:
		pPuppetPart[41].LoadSprite3D(13, 9, 0, 0, 0, "PupMace", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[41];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_STAFF1:
		pPuppetPart[79].LoadSprite3D(13, 9, 0, 0, 0, "PupWoodenStaff", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[79];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_STAFF2:
		pPuppetPart[80].LoadSprite3D(13, 9, 0, 0, 0, "PupLichStaff", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[80];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_STAFF3:
		pPuppetPart[81].LoadSprite3D(13, 9, 0, 0, 0, "PupGemStaff", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[81];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_STAFF4:
		pPuppetPart[82].LoadSprite3D(13, 9, 0, 0, 0, "PupSimpleStaff", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[82];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SMALL_BOW:
		pPuppetPart[93].LoadSprite3D(13, 0, 0, 0, 0, 9, "PupShortBow", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[93];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_LARGE_BOW:
		pPuppetPart[94].LoadSprite3D(13, 0, 0, 0, 0, 9, "PupLongBow", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[94];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_NEW_BOW01:
		pPuppetPart[275].LoadSprite3D(13, 0, 0, 0, 0, 9, "NewBow01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[275];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_DWARF_HAMMER:
		pPuppetPart[276].LoadSprite3D(13, 9, 0, 0, 0, "DwarfHammer", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[276];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_AXE_DESTRUCTION:
		pPuppetPart[277].LoadSprite3D(13, 9, 0, 0, 0, "AxeDestruction", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[277];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SWORD_ANGEL:
		pPuppetPart[278].LoadSprite3D(13, 9, 0, 0, 0, "SwordAngel", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[278];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_CLAY_2BLADES:
		pPuppetPart[279].LoadSprite3D(13, 9, 0, 0, 0, "Clay2blade", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[279];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SWORD_HERO:
		pPuppetPart[280].LoadSprite3D(13, 9, 0, 0, 0, "SwordAngel", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[280];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SKAVENCLUB:
		pPuppetPart[103].LoadSprite3D(13, 9, 0, 0, 0, "PupSkavenClub", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[103];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SKAVENKNIFE:
		pPuppetPart[104].LoadSprite3D(13, 9, 0, 0, 0, "PupSkavenKnife", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[104];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SKELAXE:
		pPuppetPart[108].LoadSprite3D(13, 9, 0, 0, 0, "PupSkeletonAxe", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[108];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_HALBERD:
		pPuppetPart[114].LoadSprite3D(13, 9, 0, 0, 0, "PupHalberd", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[114];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_SKELSWORD:
		pPuppetPart[111].LoadSprite3D(13, 9, 0, 0, 0, "PupSkeletonSword", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[111];
		Object->BodyPartMask[8] = NULL;
		break;
		//Dialsoft new weapons
	case PUPEQ_FIREFLAIL:
		pPuppetPart[116].LoadSprite3D(13, 9, 0, 0, 0, "PupFireFlail", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[116];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_TIGERAXE:
		pPuppetPart[260].LoadSprite3D(13, 9, 0, 0, 0, "PupTigerAxe", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[260];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_ANCIENTAXE:
		pPuppetPart[261].LoadSprite3D(13, 9, 0, 0, 0, "PupAncientAxe", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[261];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_FLOWERS:
		pPuppetPart[263].LoadSprite3D(13, 9, 0, 0, 0, "PupFlower", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[263];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_NECROSTAFF:
		pPuppetPart[264].LoadSprite3D(13, 9, 0, 0, 0, "PupNecroStaff", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[264];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_HAMM:
		pPuppetPart[285].LoadSprite3D(13, 9, 0, 0, 0, "Ham", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[285];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_VIP:
		pPuppetPart[298].LoadSprite3D(13, 9, 0, 0, 0, "V2_Viperine", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[298];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_01:
		pPuppetPart[299].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[299];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_02:
		pPuppetPart[300].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[300];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_03:
		pPuppetPart[301].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[301];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_04:
		pPuppetPart[302].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[302];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_DAGGER_05:
		pPuppetPart[303].LoadSprite3D(13, 9, 0, 0, 0, "V2_Dague05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[303];
		Object->BodyPartMask[8] = NULL;
		break;

		//NM_TAG_ADD_SWORD
	case PUPEQ_V2_BUSSWORD01:
		pPuppetPart[304].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[304];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BUSSWORD02:
		pPuppetPart[305].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[305];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BUSSWORD03:
		pPuppetPart[306].LoadSprite3D(13, 9, 0, 0, 0, "V2_BusterSlayer03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[306];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_CLAYSWORD01:
		pPuppetPart[307].LoadSprite3D(13, 9, 0, 0, 0, "V2_Claymore01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[307];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_CLAYSWORD02:
		pPuppetPart[308].LoadSprite3D(13, 9, 0, 0, 0, "V2_Claymore02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[308];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_GLAIVESWORD01:
		pPuppetPart[309].LoadSprite3D(13, 9, 0, 0, 0, "V2_GlaiveSlayer01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[309];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_GLAIVESWORD02:
		pPuppetPart[310].LoadSprite3D(13, 9, 0, 0, 0, "V2_GlaiveSlayer02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[310];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_GLAIVESWORD02_ICE:
		pPuppetPart[394].LoadSprite3D(13, 9, 0, 0, 0, "V2_GlaiveSlayer02", 0, 0, 0, 2);
		Object->BodyPart[8] = &pPuppetPart[394];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_GLAIVESWORD03_ICE:
		pPuppetPart[583].LoadSprite3D(13, 9, 0, 0, 0, "V2_IceSword01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[583];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD01:
		pPuppetPart[311].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[311];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD02:
		pPuppetPart[312].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[312];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD03:
		pPuppetPart[313].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[313];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD04:
		pPuppetPart[314].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[314];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD05:
		pPuppetPart[315].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[315];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD06:
		pPuppetPart[316].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword06", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[316];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD07:
		pPuppetPart[317].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword07", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[317];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SWORD08:
		pPuppetPart[318].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sword08", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[318];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE01:
		pPuppetPart[334].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[334];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE02:
		pPuppetPart[335].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[335];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE03:
		pPuppetPart[336].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[336];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE04:
		pPuppetPart[337].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[337];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE05:
		pPuppetPart[338].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[338];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE06:
		pPuppetPart[339].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache06", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[339];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE07:
		pPuppetPart[340].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache07", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[340];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE08:
		pPuppetPart[341].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache08", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[341];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE09:
		pPuppetPart[342].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache09", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[342];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE10:
		pPuppetPart[343].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache10", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[343];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HACHE11:
		pPuppetPart[344].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hache11", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[344];
		Object->BodyPartMask[8] = NULL;
		break;


	case PUPEQ_V2_BOW01:
		pPuppetPart[355].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[355];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW02:
		pPuppetPart[356].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[356];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW03:
		pPuppetPart[357].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[357];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW04:
		pPuppetPart[358].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[358];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW05:
		pPuppetPart[359].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[359];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW06:
		pPuppetPart[360].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow06", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[360];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW07:
		pPuppetPart[361].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow07", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[361];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW08:
		pPuppetPart[362].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow08", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[362];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW09:
		pPuppetPart[363].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow09", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[363];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_BOW10:
		pPuppetPart[364].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_Bow10", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[364];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_V2_SCEPTRE01:
		pPuppetPart[365].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[365];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SCEPTRE02:
		pPuppetPart[366].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[366];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SCEPTRE03:
		pPuppetPart[367].LoadSprite3D(13, 9, 0, 0, 0, "V2_Sceptre03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[367];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_V2_SP01:
		pPuppetPart[368].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[368];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP02:
		pPuppetPart[369].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[369];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP03:
		pPuppetPart[370].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[370];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP04:
		pPuppetPart[371].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[371];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP05:
		pPuppetPart[372].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[372];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP06:
		pPuppetPart[373].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special06", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[373];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_SP07:
		pPuppetPart[374].LoadSprite3D(13, 9, 0, 0, 0, "V2_Special07", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[374];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_V2_HAMMER01:
		pPuppetPart[381].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[381];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER02:
		pPuppetPart[382].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer02", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[382];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER03:
		pPuppetPart[383].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer03", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[383];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER04:
		pPuppetPart[384].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer04", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[384];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER05:
		pPuppetPart[385].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer05", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[385];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER06:
		pPuppetPart[386].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer06", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[386];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_V2_HAMMER07:
		pPuppetPart[387].LoadSprite3D(13, 9, 0, 0, 0, "V2_Hammer07", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[387];
		Object->BodyPartMask[8] = NULL;
		break;


	case PUPEQ_V2_BOW05_ICE:
		pPuppetPart[395].LoadSprite3D(13, 0, 0, 0, 0, 9, "V2_IceBow01", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[395];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_T4CP_MATRAQUE:
		pPuppetPart[415].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_Matraque", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[415];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_T4CP_BAT:
		pPuppetPart[416].LoadSprite3D(13, 9, 0, 0, 0, "TankBat", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[416];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_T4CP_12:
		pPuppetPart[417].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_12", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[417];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_T4C_SABRE_LASER_G:
		pPuppetPart[574].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaser", 0, 0, 0);
		pPuppetPart[575].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaserMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[574];
		Object->BodyPartMask[8] = &pPuppetPart[575];
		break;

	case PUPEQ_T4C_FIRESWORD01:
		pPuppetPart[576].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0);
		pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[576];
		Object->BodyPartMask[8] = &pPuppetPart[577];
		break;

	case PUPEQ_T4C_SABRE_LASER_R:
		pPuppetPart[578].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaser", 0, 0, 0, 2);
		pPuppetPart[575].LoadSprite3D(13, 9, 0, 0, 0, "NM_SabreLaserMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[578];
		Object->BodyPartMask[8] = &pPuppetPart[575];
		break;

	case PUPEQ_T4C_NOEL_CANNE:
		pPuppetPart[581].LoadSprite3D(13, 9, 0, 0, 0, "NM_CanneNoel", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[581];
		Object->BodyPartMask[8] = NULL;
		break;
	case PUPEQ_T4C_NOEL_CANNE_LONGUE:
		pPuppetPart[582].LoadSprite3D(13, 9, 0, 0, 0, "NM_CanneNoelLongue", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[582];
		Object->BodyPartMask[8] = NULL;
		break;

	case PUPEQ_T4C_GHOST_FAUC:
		pPuppetPart[637].LoadSprite3D(13, 9, 0, 0, 0, "NM_GhostFauc", 0, 0, 0);
		pPuppetPart[638].LoadSprite3D(13, 9, 0, 0, 0, "NM_GhostFaucMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[637];
		Object->BodyPartMask[8] = &pPuppetPart[638];
		break;

	case PUPEQ_T4C_NM_DEATH_AXE:
		pPuppetPart[642].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0);
		pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[642];
		Object->BodyPartMask[8] = &pPuppetPart[643];
		break;

	case PUPEQ_T4C_NM_DEATH_AXE_2:
		pPuppetPart[644].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 2);
		pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[644];
		Object->BodyPartMask[8] = &pPuppetPart[643];
		break;
	case PUPEQ_T4C_NM_DEATH_AXE_3:
		pPuppetPart[645].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 3);
		pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[645];
		Object->BodyPartMask[8] = &pPuppetPart[643];
		break;
	case PUPEQ_T4C_NM_DEATH_AXE_4:
		pPuppetPart[646].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 4);
		pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[646];
		Object->BodyPartMask[8] = &pPuppetPart[643];
		break;
	case PUPEQ_T4C_NM_DEATH_AXE_5:
		pPuppetPart[647].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxe", 0, 0, 0, 5);
		pPuppetPart[643].LoadSprite3D(13, 9, 0, 0, 0, "NMS_NMDeathAxeMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[647];
		Object->BodyPartMask[8] = &pPuppetPart[643];
		break;


	case PUPEQ_T4C_FIRESWORD01_2:
		pPuppetPart[648].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 2);
		pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[648];
		Object->BodyPartMask[8] = &pPuppetPart[577];
		break;
	case PUPEQ_T4C_FIRESWORD01_3:
		pPuppetPart[649].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 3);
		pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[649];
		Object->BodyPartMask[8] = &pPuppetPart[577];
		break;
	case PUPEQ_T4C_FIRESWORD01_4:
		pPuppetPart[650].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 4);
		pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[650];
		Object->BodyPartMask[8] = &pPuppetPart[577];
		break;
	case PUPEQ_T4C_FIRESWORD01_5:
		pPuppetPart[651].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 5);
		pPuppetPart[577].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Mask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[651];
		Object->BodyPartMask[8] = &pPuppetPart[577];
		break;

	case PUPEQ_T4C_FIRESWORD01DUAL_2:
		pPuppetPart[652].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 2);
		pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[652];
		Object->BodyPartMask[8] = &pPuppetPart[580];
		break;
	case PUPEQ_T4C_FIRESWORD01DUAL_3:
		pPuppetPart[653].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 3);
		pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[653];
		Object->BodyPartMask[8] = &pPuppetPart[580];
		break;
	case PUPEQ_T4C_FIRESWORD01DUAL_4:
		pPuppetPart[654].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 4);
		pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[654];
		Object->BodyPartMask[8] = &pPuppetPart[580];
		break;
	case PUPEQ_T4C_FIRESWORD01DUAL_5:
		pPuppetPart[655].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01", 0, 0, 0, 5);
		pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
		Object->BodyPart[8] = &pPuppetPart[655];
		Object->BodyPartMask[8] = &pPuppetPart[580];
		break;














		//Dialsoft end

	}

	// Shield
	Object->VisiblePart |= Pow2[9];
	switch (Object->PuppetInfo[9]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[9];
		break;
	case PUPEQ_ROMANSHIELD:
		pPuppetPart[42].LoadSprite3D(13, 9, 0, 0, 0, "PupRomanShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[42];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_BAROSSASHIELD:
		pPuppetPart[43].LoadSprite3D(13, 9, 0, 0, 0, "PupBarossaShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[43];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_ORCSHIELD:
		pPuppetPart[44].LoadSprite3D(13, 9, 0, 0, 0, "PupOrcShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[44];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_CENTAURSHIELD1:
		pPuppetPart[101].LoadSprite3D(13, 9, 0, 0, 0, "PupCentaurShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[101];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_CENTAURSHIELD2:
		pPuppetPart[100].LoadSprite3D(13, 9, 0, 0, 0, "PupCentaurShield2", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[100];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKAVENSHIELD1:
		pPuppetPart[105].LoadSprite3D(13, 9, 0, 0, 0, "PupSkavenShield1", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[105];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKAVENSHIELD2:
		pPuppetPart[106].LoadSprite3D(13, 9, 0, 0, 0, "PupSkavenShield2", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[106];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKAVENSHIELD3:
		pPuppetPart[107].LoadSprite3D(13, 9, 0, 0, 0, "PupSkavenShield3", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[107];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD:
		pPuppetPart[110].LoadSprite3D(13, 9, 0, 0, 0, "PupSkeletonShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[110];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_GOLD:
		pPuppetPart[550].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 2);
		Object->BodyPart[9] = &pPuppetPart[550];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_SILVER:
		pPuppetPart[551].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 3);
		Object->BodyPart[9] = &pPuppetPart[551];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_BLACK:
		pPuppetPart[552].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 4);
		Object->BodyPart[9] = &pPuppetPart[552];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_BLUE:
		pPuppetPart[553].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 5);
		Object->BodyPart[9] = &pPuppetPart[553];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_GREEN:
		pPuppetPart[554].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 6);
		Object->BodyPart[9] = &pPuppetPart[554];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_RED:
		pPuppetPart[555].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 7);
		Object->BodyPart[9] = &pPuppetPart[555];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_PURPLE:
		pPuppetPart[556].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 8);
		Object->BodyPart[9] = &pPuppetPart[556];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_EMERALD:
		pPuppetPart[557].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 9);
		Object->BodyPart[9] = &pPuppetPart[557];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKELSHIELD_ORANGE:
		pPuppetPart[558].LoadSprite3D(13, 9, 0, 0, 0, 0, "PupSkeletonShield", 0, 0, 0, 10);
		Object->BodyPart[9] = &pPuppetPart[558];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_ORCSHIELD2:
		pPuppetPart[290].LoadSprite3D(13, 9, 0, 0, 0, "PupOrcShield2", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[290];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_SKSHIELD:
		pPuppetPart[291].LoadSprite3D(13, 9, 0, 0, 0, "SkShield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[291];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_V2_SHIELD01:
		pPuppetPart[392].LoadSprite3D(13, 9, 0, 0, 0, "V2_Shield01", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[392];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_V2_SHIELD02:
		pPuppetPart[393].LoadSprite3D(13, 9, 0, 0, 0, "V2_Shield02", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[393];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_T4CP_SHIELD:
		pPuppetPart[414].LoadSprite3D(13, 9, 0, 0, 0, "T4CP_Shield", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[414];
		Object->BodyPartMask[9] = NULL;
		break;
	case PUPEQ_V2_ICESHIELD01:
		pPuppetPart[418].LoadSprite3D(13, 9, 0, 0, 0, "V2_IceShield01", 0, 0, 0);
		Object->BodyPart[9] = &pPuppetPart[418];
		Object->BodyPartMask[9] = NULL;
		break;



	}

	// BOOT
	Object->VisiblePart |= Pow2[10];
	switch (Object->PuppetInfo[10]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_LEATHER:
		if (Object->Type == 10011) {
			pPuppetPart[45].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherBoots", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[45];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[74].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherBoots", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[74];
			Object->BodyPartMask[10] = NULL;
		}
		break;
		//Tiamat changes start
	case PUPEQ_PLATE:
		if (Object->Type == 10011) {
			pPuppetPart[46].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[46];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[83].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[83];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[155].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[155];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[215].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[215];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[163].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 3);
			Object->BodyPart[10] = &pPuppetPart[163];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[221].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 3);
			Object->BodyPart[10] = &pPuppetPart[221];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[171].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 4);
			Object->BodyPart[10] = &pPuppetPart[171];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[227].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 4);
			Object->BodyPart[10] = &pPuppetPart[227];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[179].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 5);
			Object->BodyPart[10] = &pPuppetPart[179];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[233].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 5);
			Object->BodyPart[10] = &pPuppetPart[233];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[187].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 6);
			Object->BodyPart[10] = &pPuppetPart[187];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[239].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 6);
			Object->BodyPart[10] = &pPuppetPart[239];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[195].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 7);
			Object->BodyPart[10] = &pPuppetPart[195];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[245].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 7);
			Object->BodyPart[10] = &pPuppetPart[245];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[203].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 8);
			Object->BodyPart[10] = &pPuppetPart[203];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[251].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 8);
			Object->BodyPart[10] = &pPuppetPart[251];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[211].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 9);
			Object->BodyPart[10] = &pPuppetPart[211];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[257].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 9);
			Object->BodyPart[10] = &pPuppetPart[257];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[566].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateFoot", 0, 0, 0, 10);
			Object->BodyPart[10] = &pPuppetPart[566];
			Object->BodyPartMask[10] = NULL;
		}
		else {
			pPuppetPart[570].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateBoots", 0, 0, 0, 10);
			Object->BodyPart[10] = &pPuppetPart[570];
			Object->BodyPartMask[10] = NULL;
		}
		break;
		//Tiamat Changes End
	case PUPEQ_BLACKLEATHER:
		if (Object->Type == 10011)
		{
			pPuppetPart[47].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBlackLeatherBoots", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[47];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[86].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoBlackLeatherBoots", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[86];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_WHITELEATHER:
		if (Object->Type == 10011)
		{
			pPuppetPart[419].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupBlackLeatherBoots", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[419];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[420].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoBlackLeatherBoots", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[420];
			Object->BodyPartMask[10] = NULL;
		}
		break;

	case PUPEQ_LUTIN_RBOOT:
		pPuppetPart[639].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_LutinBoots", 0, 0, 0);
		Object->BodyPart[10] = &pPuppetPart[639];
		Object->BodyPartMask[10] = NULL;
		break;
	case PUPEQ_LUTIN_GBOOT:
		pPuppetPart[640].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_LutinBoots", 0, 0, 0, 2);
		Object->BodyPart[10] = &pPuppetPart[640];
		Object->BodyPartMask[10] = NULL;
		break;




	case PUPEQ_NECROROBE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_WHITEROBE:
	case PUPEQ_LICHROBE:
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_REDROBE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_MAGEROBE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_BLUE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_GREEN:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_DARKBLUE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_VIOLET:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_PURPLE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_YELLOW:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_GRAY:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_MAUVE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_ORANGE:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_BRUN:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_FWHITEROBE_DARK:
		Object->VisiblePart -= Pow2[10];
		break;
	case PUPEQ_V2_MAN_ARMOR01GR:
		if (Object->Type == 10011)
		{
			pPuppetPart[403].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[403];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[403].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0);
			Object->BodyPart[10] = &pPuppetPart[403];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01BL:
		if (Object->Type == 10011)
		{
			pPuppetPart[407].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[407];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[407].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 2);
			Object->BodyPart[10] = &pPuppetPart[407];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_GREEN:
		if (Object->Type == 10011)
		{
			pPuppetPart[594].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 3);
			Object->BodyPart[10] = &pPuppetPart[594];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[594].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 3);
			Object->BodyPart[10] = &pPuppetPart[594];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_RED:
		if (Object->Type == 10011)
		{
			pPuppetPart[600].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 4);
			Object->BodyPart[10] = &pPuppetPart[600];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[600].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 4);
			Object->BodyPart[10] = &pPuppetPart[600];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_BLUE:
		if (Object->Type == 10011)
		{
			pPuppetPart[606].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 5);
			Object->BodyPart[10] = &pPuppetPart[606];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[606].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 5);
			Object->BodyPart[10] = &pPuppetPart[606];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_AQUA:
		if (Object->Type == 10011)
		{
			pPuppetPart[612].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 6);
			Object->BodyPart[10] = &pPuppetPart[612];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[612].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 6);
			Object->BodyPart[10] = &pPuppetPart[612];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_GOLD:
		if (Object->Type == 10011)
		{
			pPuppetPart[618].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 7);
			Object->BodyPart[10] = &pPuppetPart[618];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[618].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 7);
			Object->BodyPart[10] = &pPuppetPart[618];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_ORANGE:
		if (Object->Type == 10011)
		{
			pPuppetPart[624].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 8);
			Object->BodyPart[10] = &pPuppetPart[624];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[624].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 8);
			Object->BodyPart[10] = &pPuppetPart[624];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	case PUPEQ_V2_MAN_ARMOR01_FIRE:
		if (Object->Type == 10011)
		{
			pPuppetPart[630].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 9);
			Object->BodyPart[10] = &pPuppetPart[630];
			Object->BodyPartMask[10] = NULL;
		}
		else
		{
			pPuppetPart[630].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_ManArmorBoots01", 0, 0, 0, 9);
			Object->BodyPart[10] = &pPuppetPart[630];
			Object->BodyPartMask[10] = NULL;
		}
		break;
	}

	// HAT
	Object->VisiblePart |= Pow2[11];
	switch (Object->PuppetInfo[11]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[11];
		break;
	case PUPEQ_GOLDENCROWN:
		pPuppetPart[48].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGoldenCrown", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[48];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_ELVEN:
		pPuppetPart[49].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupElvenHat", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[49];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_WITCHHAT1:
		pPuppetPart[272].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat1", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[272];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_WITCHHAT2:
		pPuppetPart[273].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat2", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[273];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_WITCHHAT3:
		pPuppetPart[274].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat3", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[274];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_WITCHHAT4:
		pPuppetPart[398].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat4", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[398];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_WITCHHAT5:
		pPuppetPart[399].LoadSprite3D(13, 9, 0, 0, 0, 9, "WitchHat5", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[399];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_NOEL_HAT:
		pPuppetPart[289].LoadSprite3D(13, 9, 0, 0, 0, 9, "NoelHat", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[289];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_T4CP_CASQUE:
		pPuppetPart[412].LoadSprite3D(13, 9, 0, 0, 0, 9, "T4CP_Casque", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[412];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_T4CP_CASQUETTE:
		pPuppetPart[413].LoadSprite3D(13, 9, 0, 0, 0, 9, "T4CP_Casquette", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[413];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAUME01:
		pPuppetPart[292].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume01", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[292];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAUME02:
		pPuppetPart[293].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume02", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[293];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAUME03:
		pPuppetPart[294].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume03", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[294];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAUME04:
		pPuppetPart[295].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[295];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAT01:
		pPuppetPart[296].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Hat01", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[296];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_V2_HAUME04_BL:
		pPuppetPart[410].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 2);
		Object->BodyPart[11] = &pPuppetPart[410];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_GREEN:
		pPuppetPart[596].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 3);
		Object->BodyPart[11] = &pPuppetPart[596];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_RED:
		pPuppetPart[602].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 4);
		Object->BodyPart[11] = &pPuppetPart[602];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_BLUE:
		pPuppetPart[608].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 5);
		Object->BodyPart[11] = &pPuppetPart[608];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_AQUA:
		pPuppetPart[614].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 6);
		Object->BodyPart[11] = &pPuppetPart[614];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_GOLD:
		pPuppetPart[620].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 7);
		Object->BodyPart[11] = &pPuppetPart[620];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_ORANGE:
		pPuppetPart[626].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 8);
		Object->BodyPart[11] = &pPuppetPart[626];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_V2_HAUME04_FIRE:
		pPuppetPart[632].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Haume04", 0, 0, 0, 9);
		Object->BodyPart[11] = &pPuppetPart[632];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_NMS_HALOWEEN_HEAD:
		pPuppetPart[573].LoadSprite3D(13, 9, 0, 0, 0, 9, "HalloweenHead", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[573];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_OREILLELAPIN:
		pPuppetPart[641].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM_OreilleLapin", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[641];
		Object->BodyPartMask[11] = NULL;
		break;

	case PUPEQ_SNOWMAN_HAT_BLACK:
		pPuppetPart[656].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_SnowManHat", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[656];
		Object->BodyPartMask[11] = NULL;
		break;




	case PUPEQ_LEATHER:
		if (Object->Type == 10011) {
			pPuppetPart[90].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupLeatherHelm", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[90];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[91].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherHelm", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[91];
			Object->BodyPartMask[11] = NULL;
		}
		break;
		//Tiamat changes start
	case PUPEQ_PLATE:
		if (Object->Type == 10011) {
			pPuppetPart[23].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[23];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[61].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[61];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_GOLD:
		if (Object->Type == 10011) {
			pPuppetPart[152].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 2);
			Object->BodyPart[11] = &pPuppetPart[152];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[212].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 2);
			Object->BodyPart[11] = &pPuppetPart[212];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_SILVER:
		if (Object->Type == 10011) {
			pPuppetPart[160].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 3);
			Object->BodyPart[11] = &pPuppetPart[160];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[218].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 3);
			Object->BodyPart[11] = &pPuppetPart[218];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLACK:
		if (Object->Type == 10011) {
			pPuppetPart[168].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 4);
			Object->BodyPart[11] = &pPuppetPart[168];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[224].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 4);
			Object->BodyPart[11] = &pPuppetPart[224];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_BLUE:
		if (Object->Type == 10011) {
			pPuppetPart[176].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 5);
			Object->BodyPart[11] = &pPuppetPart[176];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[230].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 5);
			Object->BodyPart[11] = &pPuppetPart[230];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_GREEN:
		if (Object->Type == 10011) {
			pPuppetPart[184].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 6);
			Object->BodyPart[11] = &pPuppetPart[184];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[236].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 6);
			Object->BodyPart[11] = &pPuppetPart[236];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_RED:
		if (Object->Type == 10011) {
			pPuppetPart[192].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 7);
			Object->BodyPart[11] = &pPuppetPart[192];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[242].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 7);
			Object->BodyPart[11] = &pPuppetPart[242];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_PURPLE:
		if (Object->Type == 10011) {
			pPuppetPart[200].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 8);
			Object->BodyPart[11] = &pPuppetPart[200];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[248].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 8);
			Object->BodyPart[11] = &pPuppetPart[248];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_EMERALD:
		if (Object->Type == 10011) {
			pPuppetPart[208].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 9);
			Object->BodyPart[11] = &pPuppetPart[208];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[254].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 9);
			Object->BodyPart[11] = &pPuppetPart[254];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_PLATE_ORANGE:
		if (Object->Type == 10011) {
			pPuppetPart[563].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupPlateHelm", 0, 0, 0, 10);
			Object->BodyPart[11] = &pPuppetPart[563];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[567].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoPlateHelm", 0, 0, 0, 10);
			Object->BodyPart[11] = &pPuppetPart[567];
			Object->BodyPartMask[11] = NULL;
		}
		break;
		// Tiamat changes end
	case PUPEQ_CHAIN:
		if (Object->Type == 10011) {
			pPuppetPart[24].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupChainMailCoif", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[24];
			Object->BodyPartMask[11] = NULL;
		}
		else {
			pPuppetPart[87].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoChainMailCoif", 0, 0, 0);
			Object->BodyPart[11] = &pPuppetPart[87];
			Object->BodyPartMask[11] = NULL;
		}
		break;
	case PUPEQ_HORNED:
		pPuppetPart[25].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupHornedHelmet", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[25];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_SHAMAN:
		pPuppetPart[102].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupShamanHelm", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[102];
		Object->BodyPartMask[11] = NULL;
		break;
	case PUPEQ_SKEL:
		pPuppetPart[109].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSkeletonHelm", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[109];
		Object->BodyPartMask[11] = NULL;
		break;
		//Dialsoft new helmets
	case PUPEQ_REDFEATHER:
		pPuppetPart[115].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedFeHelmet", 0, 0, 0);
		Object->BodyPart[11] = &pPuppetPart[115];
		Object->BodyPartMask[11] = NULL;
		break;
		//Dialsoft end
	}

	// CAPE
	Object->VisiblePart |= Pow2[12];
	switch (Object->PuppetInfo[12]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[12];
		break;
	case PUPEQ_REDCAPE:
		pPuppetPart[50].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[50];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BLACKWINGS:
		pPuppetPart[112].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphBlackWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[112];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NM_DECHUWINGS:
		pPuppetPart[411].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM_DechuWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[411];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NM2_DECHUWINGS:
		pPuppetPart[421].LoadSprite3D(13, 9, 0, 0, 0, 9, "NM2_DechuWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[421];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_nDECHUWINGS01:
		pPuppetPart[444].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[444];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_nDECHUWINGS02:
		pPuppetPart[445].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[445];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_nDECHUWINGS03:
		pPuppetPart[446].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_nDechuWings", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[446];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_DARKWINGS_PINK:
		pPuppetPart[447].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphDarkWings", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[447];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_WHITEWINGS:
		pPuppetPart[113].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[113];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_DARKWINGS:
		pPuppetPart[118].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphDarkWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[118];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_REDWINGS:
		pPuppetPart[120].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[120];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NEUTRALWINGS:
		pPuppetPart[119].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[119];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_GOLDWINGS:
		pPuppetPart[121].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[121];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_DARKREDWINGS:
		pPuppetPart[122].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 5);
		Object->BodyPart[12] = &pPuppetPart[122];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_DARKGOLDWINGS:
		pPuppetPart[123].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 6);
		Object->BodyPart[12] = &pPuppetPart[123];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_LIGHTBLUEWINGS:
		pPuppetPart[124].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 7);
		Object->BodyPart[12] = &pPuppetPart[124];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_ORANGEWINGS:
		pPuppetPart[125].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupSeraphWhiteWings", 0, 0, 0, 8);
		Object->BodyPart[12] = &pPuppetPart[125];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_GREENCAPE:
		pPuppetPart[265].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[265];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_YELLOWCAPE:
		pPuppetPart[266].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[266];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_SILVERCAPE:
		pPuppetPart[267].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[267];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_PURPLECAPE:
		pPuppetPart[268].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 5);
		Object->BodyPart[12] = &pPuppetPart[268];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BLACKCAPE:
		pPuppetPart[269].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 6);
		Object->BodyPart[12] = &pPuppetPart[269];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_AQUACAPE:
		pPuppetPart[270].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 7);
		Object->BodyPart[12] = &pPuppetPart[270];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BLUECAPE:
		pPuppetPart[271].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCape", 0, 0, 0, 8);
		Object->BodyPart[12] = &pPuppetPart[271];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BUTTERFLYWINGS1:
		pPuppetPart[281].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[281];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BUTTERFLYWINGS2:
		pPuppetPart[282].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[282];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BUTTERFLYWINGS3:
		pPuppetPart[283].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[283];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_BUTTERFLYWINGS4:
		pPuppetPart[284].LoadSprite3D(13, 9, 0, 0, 0, 9, "ButterFlyWing", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[284];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_ARCHWINGSW:
		pPuppetPart[286].LoadSprite3D(13, 9, 0, 0, 0, 9, "ArchWings", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[286];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_ARCHWINGSP:
		pPuppetPart[491].LoadSprite3D(13, 9, 0, 0, 0, 9, "ArchWings", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[491];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NIGHTMAREWINGS:
		pPuppetPart[585].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareWings", 0, 0, 0);
		pPuppetPart[586].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareWingsMask", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[585];
		Object->BodyPartMask[12] = &pPuppetPart[586];
		break;
	case PUPEQ_NIGHTMAREBIGWINGS:
		pPuppetPart[587].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareBigWings", 0, 0, 0);
		pPuppetPart[588].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareBigWingsMask", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[587];
		Object->BodyPartMask[12] = &pPuppetPart[588];
		break;
	case PUPEQ_NIGHTMAREMEDWINGS:
		pPuppetPart[589].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareMedWings", 0, 0, 0);
		pPuppetPart[590].LoadSprite3D(13, 9, 0, 0, 0, 9, "NightmareMedWingsMask", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[589];
		Object->BodyPartMask[12] = &pPuppetPart[590];
		break;

	case PUPEQ_NMS_X6WHITE_WINGS:
		pPuppetPart[633].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6WhiteWings", 0, 0, 0);
		pPuppetPart[634].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6WhiteWingsMask", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[633];
		Object->BodyPartMask[12] = &pPuppetPart[634];
		break;
	case PUPEQ_NMS_X6BLACK_WINGS:
		pPuppetPart[635].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6BlackWings", 0, 0, 0);
		pPuppetPart[636].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMSX6BlackWingsMask", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[635];
		Object->BodyPartMask[12] = &pPuppetPart[636];
		break;




	case PUPEQ_NMS_NEW_CAPE_01_C0:
		pPuppetPart[464].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[464];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C1:
		pPuppetPart[465].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[465];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C2:
		pPuppetPart[466].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[466];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C3:
		pPuppetPart[467].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[467];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C4:
		pPuppetPart[468].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 5);
		Object->BodyPart[12] = &pPuppetPart[468];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C5:
		pPuppetPart[469].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 6);
		Object->BodyPart[12] = &pPuppetPart[469];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C6:
		pPuppetPart[470].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 7);
		Object->BodyPart[12] = &pPuppetPart[470];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C7:
		pPuppetPart[471].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 8);
		Object->BodyPart[12] = &pPuppetPart[471];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPE_01_C8:
		pPuppetPart[472].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCape01", 0, 0, 0, 9);
		Object->BodyPart[12] = &pPuppetPart[472];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C0:
		pPuppetPart[473].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[473];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C1:
		pPuppetPart[474].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[474];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C2:
		pPuppetPart[475].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[475];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C3:
		pPuppetPart[476].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[476];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C4:
		pPuppetPart[477].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 5);
		Object->BodyPart[12] = &pPuppetPart[477];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C5:
		pPuppetPart[478].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 6);
		Object->BodyPart[12] = &pPuppetPart[478];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C6:
		pPuppetPart[479].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 7);
		Object->BodyPart[12] = &pPuppetPart[479];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C7:
		pPuppetPart[480].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 8);
		Object->BodyPart[12] = &pPuppetPart[480];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPEGG_01_C8:
		pPuppetPart[481].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeGarde01", 0, 0, 0, 9);
		Object->BodyPart[12] = &pPuppetPart[481];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C0:
		pPuppetPart[482].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0);
		Object->BodyPart[12] = &pPuppetPart[482];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C1:
		pPuppetPart[483].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 2);
		Object->BodyPart[12] = &pPuppetPart[483];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C2:
		pPuppetPart[484].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 3);
		Object->BodyPart[12] = &pPuppetPart[484];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C3:
		pPuppetPart[485].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 4);
		Object->BodyPart[12] = &pPuppetPart[485];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C4:
		pPuppetPart[486].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 5);
		Object->BodyPart[12] = &pPuppetPart[486];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C5:
		pPuppetPart[487].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 6);
		Object->BodyPart[12] = &pPuppetPart[487];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C6:
		pPuppetPart[488].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 7);
		Object->BodyPart[12] = &pPuppetPart[488];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C7:
		pPuppetPart[489].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 8);
		Object->BodyPart[12] = &pPuppetPart[489];
		Object->BodyPartMask[12] = NULL;
		break;
	case PUPEQ_NMS_NEW_CAPELL_01_C8:
		pPuppetPart[490].LoadSprite3D(13, 9, 0, 0, 0, 9, "NMS_NewCapeLogo01", 0, 0, 0, 9);
		Object->BodyPart[12] = &pPuppetPart[490];
		Object->BodyPartMask[12] = NULL;
		break;


	}

	// BACKBODY
	Object->VisiblePart |= Pow2[13];
	switch (Object->PuppetInfo[13]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[13];
		break;
	case PUPEQ_ARMORED_ROBE:
		if (Object->Type == 10011)
		{
			pPuppetPart[51].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupArmoredRobeUnder", 0, 0, 0);
			Object->BodyPart[13] = &pPuppetPart[51];
			Object->BodyPartMask[13] = NULL;
		}
		else
		{
			Object->VisiblePart -= Pow2[13];
		}
		break;
	case PUPEQ_MAGEROBE:
		if (Object->Type == 10011)
		{
			Object->VisiblePart -= Pow2[13];
		}
		else
		{
			pPuppetPart[98].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoMageRobeUnder", 0, 0, 0);
			Object->BodyPart[13] = &pPuppetPart[98];
			Object->BodyPartMask[13] = NULL;
		}
		break;
	case PUPEQ_LEATHER:
		if (Object->Type == 10011) {
			Object->VisiblePart -= Pow2[13];
		}
		else {
			pPuppetPart[63].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherBody", 0, 0, 0);
			Object->BodyPart[13] = &pPuppetPart[63];
			Object->BodyPartMask[13] = NULL;
		}
		break;
	case PUPEQ_SPIKEDLEATHER:
		if (Object->Type == 10011)
		{
			Object->VisiblePart -= Pow2[13];
		}
		else
		{
			pPuppetPart[63].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoLeatherBody", 0, 0, 0);
			Object->BodyPart[13] = &pPuppetPart[63];
			Object->BodyPartMask[13] = NULL;
		}

	}

	// CAPE 2
	Object->VisiblePart |= Pow2[14];
	switch (Object->PuppetInfo[14]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[14];
		break;
	case PUPEQ_REDCAPE:
		pPuppetPart[77].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupRedCapeTop", 0, 0, 0);
		Object->BodyPart[14] = &pPuppetPart[77];
		Object->BodyPartMask[14] = NULL;
		break;
	case PUPEQ_MAGEROBE:
		pPuppetPart[99].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupMageRobeCollar", 0, 0, 0);
		Object->BodyPart[14] = &pPuppetPart[99];
		Object->BodyPartMask[14] = NULL;
		break;
	}

	// HAIR
	//being Tiamat Changes
	Object->VisiblePart |= Pow2[15];

	if (Object->PuppetInfo[11] == PUPEQ_PLATE_GOLD || Object->PuppetInfo[11] == PUPEQ_PLATE || Object->PuppetInfo[11] == PUPEQ_CHAIN ||
		Object->PuppetInfo[11] == PUPEQ_PLATE_SILVER || Object->PuppetInfo[11] == PUPEQ_HORNED || Object->PuppetInfo[11] == PUPEQ_LEATHER ||
		Object->PuppetInfo[11] == PUPEQ_PLATE_BLUE || Object->PuppetInfo[11] == PUPEQ_PLATE_GREEN || Object->PuppetInfo[11] == PUPEQ_PLATE_RED ||
		Object->PuppetInfo[11] == PUPEQ_PLATE_PURPLE || Object->PuppetInfo[11] == PUPEQ_PLATE_EMERALD ||
		Object->PuppetInfo[11] == PUPEQ_PLATE_BLACK || Object->PuppetInfo[11] == PUPEQ_STUDDED ||
		Object->PuppetInfo[11] == PUPEQ_HORNED ||
		//Object->PuppetInfo[11] == PUPEQ_WITCHHAT     || Object->PuppetInfo[11] == PUPEQ_WITCHHATRED  || Object->PuppetInfo[11] == PUPEQ_WITCHHATBLUE  || 
		//Object->PuppetInfo[11] == PUPEQ_NOEL_HAT     ||
		Object->PuppetInfo[11] == PUPEQ_V2_HAUME01 || Object->PuppetInfo[11] == PUPEQ_V2_HAUME02 || Object->PuppetInfo[11] == PUPEQ_V2_HAUME04 || Object->PuppetInfo[11] == PUPEQ_V2_HAUME04_BL ||
		Object->PuppetInfo[11] == PUPEQ_NMS_HALOWEEN_HEAD

		)
	{
		Object->VisiblePart -= Pow2[15];
	}
	else
	{
		if (Object->Type == 10011)
		{
			switch (Object->PuppetInfo[15])
			{
			case PUPEQ_NACKED:
				pPuppetPart[88].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0);
				Object->BodyPart[15] = &pPuppetPart[88];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_2:
				pPuppetPart[422].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 2);
				Object->BodyPart[15] = &pPuppetPart[422];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_3:
				pPuppetPart[423].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 3);
				Object->BodyPart[15] = &pPuppetPart[423];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_4:
				pPuppetPart[424].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 4);
				Object->BodyPart[15] = &pPuppetPart[424];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_5:
				pPuppetPart[425].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 5);
				Object->BodyPart[15] = &pPuppetPart[425];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_6:
				pPuppetPart[426].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 6);
				Object->BodyPart[15] = &pPuppetPart[426];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_7:
				pPuppetPart[427].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 7);
				Object->BodyPart[15] = &pPuppetPart[427];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_8:
				pPuppetPart[428].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 8);
				Object->BodyPart[15] = &pPuppetPart[428];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_9:
				pPuppetPart[429].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 9);
				Object->BodyPart[15] = &pPuppetPart[429];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_10:
				pPuppetPart[430].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 10);
				Object->BodyPart[15] = &pPuppetPart[430];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_11:
				pPuppetPart[431].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNormalHair", 0, 0, 0, 11);
				Object->BodyPart[15] = &pPuppetPart[431];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_COUPER:
				Object->VisiblePart -= Pow2[15];
				break;
			}
		}
		else
		{
			switch (Object->PuppetInfo[15])
			{
			case PUPEQ_NACKED:
				pPuppetPart[78].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0);
				Object->BodyPart[15] = &pPuppetPart[78];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_2:
				pPuppetPart[434].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 2);
				Object->BodyPart[15] = &pPuppetPart[434];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_3:
				pPuppetPart[435].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 3);
				Object->BodyPart[15] = &pPuppetPart[435];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_4:
				pPuppetPart[436].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 4);
				Object->BodyPart[15] = &pPuppetPart[436];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_5:
				pPuppetPart[437].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 5);
				Object->BodyPart[15] = &pPuppetPart[437];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_6:
				pPuppetPart[438].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 6);
				Object->BodyPart[15] = &pPuppetPart[438];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_7:
				pPuppetPart[439].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 7);
				Object->BodyPart[15] = &pPuppetPart[439];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_8:
				pPuppetPart[440].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 8);
				Object->BodyPart[15] = &pPuppetPart[440];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_9:
				pPuppetPart[441].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 9);
				Object->BodyPart[15] = &pPuppetPart[441];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_10:
				pPuppetPart[442].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 10);
				Object->BodyPart[15] = &pPuppetPart[442];
				Object->BodyPartMask[15] = NULL;
				break;
			case PUPEQ_NACKED_HAIR_11:
				pPuppetPart[443].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoHairPonyTail", 0, 0, 0, 11);
				Object->BodyPart[15] = &pPuppetPart[443];
				Object->BodyPartMask[15] = NULL;
				break;

			case PUPEQ_NACKED_HAIR_COUPER:
				Object->VisiblePart -= Pow2[15];
				break;
			}
		}
	}

	// ROBE LEGS
	Object->VisiblePart |= Pow2[16];
	switch (Object->PuppetInfo[16]) {
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[16];
		break;
	case PUPEQ_SET1:
		if (Object->Type == 10011)
		{
			Object->VisiblePart -= Pow2[16];
		}
		else
		{
			pPuppetPart[70].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoClothRobe", 0, 0, 0);
			Object->BodyPart[16] = &pPuppetPart[70];
			Object->BodyPartMask[16] = NULL;
		}
		break;
	case PUPEQ_LICHROBE:
		pPuppetPart[60].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmL", 0, 0, 0);
		Object->BodyPart[16] = &pPuppetPart[60];
		Object->BodyPartMask[16] = NULL;
		break;
	case PUPEQ_NEWLICHROBE:
	case PUPEQ_NEWLICHROBEBLACK:
	case PUPEQ_NEWLICHROBEWHITE:
	case PUPEQ_NEWLICHROBEGOLD:
	case PUPEQ_NEWLICHROBEGREEN:
	case PUPEQ_NEWLICHROBEKIMONO:
	case PUPEQ_NEWLICHROBEPINK:
	case PUPEQ_NEWLICHROBERED:
	case PUPEQ_ARMORED_ROBE:
		if (Object->Type == 10011)
		{
			pPuppetPart[60].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupNakedArmL", 0, 0, 0);
			Object->BodyPart[16] = &pPuppetPart[60];
			Object->BodyPartMask[16] = NULL;
		}
		else
		{
			pPuppetPart[60].LoadSprite3D(13, 9, 0, 0, 0, 9, "WoNakedArmL", 0, 0, 0);
			Object->BodyPart[16] = &pPuppetPart[60];
			Object->BodyPartMask[16] = NULL;
		}

		break;
	}
	Object->VisiblePart |= Pow2[17];
	switch (Object->PuppetInfo[17])
	{
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[17];
		break;
	case PUPEQ_GOBMASK:
		pPuppetPart[262].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGobmask", 0, 0, 0);
		Object->BodyPart[17] = &pPuppetPart[262];
		Object->BodyPartMask[17] = NULL;
		break;
	case PUPEQ_V2_MASK01:
		pPuppetPart[297].LoadSprite3D(13, 9, 0, 0, 0, 9, "V2_Mask01", 0, 0, 0);
		Object->BodyPart[17] = &pPuppetPart[297];
		Object->BodyPartMask[17] = NULL;
		break;
	case PUPEQ_V2_GOBMASK_ICE:
		pPuppetPart[396].LoadSprite3D(13, 9, 0, 0, 0, 9, "PupGobmask", 0, 0, 0, 2);
		Object->BodyPart[17] = &pPuppetPart[396];
		Object->BodyPartMask[17] = NULL;
		break;
	}

	Object->VisiblePart |= Pow2[18];
	switch (Object->PuppetInfo[18])
	{
	case PUPEQ_NACKED:
		Object->VisiblePart -= Pow2[18];
		break;
		//NM_TAG_ADD_SWORD_hand2
	case PUPEQ_V2_2BUSSWORD01:
		pPuppetPart[319].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[319];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2BUSSWORD02:
		pPuppetPart[320].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[320];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2BUSSWORD03:
		pPuppetPart[321].LoadSprite3D(13, 9, 0, 0, 0, "V2_2BusterSlayer03", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[321];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2CLAYSWORD01:
		pPuppetPart[322].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Claymore01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[322];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2CLAYSWORD02:
		pPuppetPart[323].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Claymore02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[323];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2GLAIVESWORD01:
		pPuppetPart[324].LoadSprite3D(13, 9, 0, 0, 0, "V2_2GlaiveSlayer01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[324];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2GLAIVESWORD02:
		pPuppetPart[325].LoadSprite3D(13, 9, 0, 0, 0, "V2_2GlaiveSlayer02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[325];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2GLAIVESWORD02_ICE:
		pPuppetPart[397].LoadSprite3D(13, 9, 0, 0, 0, "V2_2GlaiveSlayer02", 0, 0, 0, 2);
		Object->BodyPart[18] = &pPuppetPart[397];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2GLAIVESWORD03_ICE:
		pPuppetPart[584].LoadSprite3D(13, 9, 0, 0, 0, "V2_2IceSword01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[584];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD01:
		pPuppetPart[326].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[326];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD02:
		pPuppetPart[327].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[327];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD03:
		pPuppetPart[328].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword03", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[328];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD04:
		pPuppetPart[329].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword04", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[329];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD05:
		pPuppetPart[330].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword05", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[330];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD06:
		pPuppetPart[331].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword06", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[331];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD07:
		pPuppetPart[332].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword07", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[332];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SWORD08:
		pPuppetPart[333].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Sword08", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[333];
		Object->BodyPartMask[18] = NULL;
		break;

	case PUPEQ_V2_2HACHE01:
		pPuppetPart[345].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[345];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HACHE02:
		pPuppetPart[346].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[346];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HACHE03:
		pPuppetPart[347].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache03", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[347];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HACHE04:
		pPuppetPart[348].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache04", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[348];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HACHE05:
		pPuppetPart[390].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache05", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[390];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HACHE06:
		pPuppetPart[391].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hache06", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[391];
		Object->BodyPartMask[18] = NULL;
		break;


	case PUPEQ_V2_2DAGUE01:
		pPuppetPart[349].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[349];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2DAGUE02:
		pPuppetPart[350].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[350];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2DAGUE03:
		pPuppetPart[351].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague03", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[351];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2DAGUE04:
		pPuppetPart[352].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague04", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[352];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2DAGUE05:
		pPuppetPart[353].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague05", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[353];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2DAGUE06:
		pPuppetPart[354].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Dague06", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[354];
		Object->BodyPartMask[18] = NULL;
		break;

	case PUPEQ_V2_2SP01:
		pPuppetPart[375].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[375];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SP02:
		pPuppetPart[376].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[376];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SP03:
		pPuppetPart[377].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special03", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[377];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SP04:
		pPuppetPart[378].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special04", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[378];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SP05:
		pPuppetPart[379].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special05", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[379];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2SP06:
		pPuppetPart[380].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Special06", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[380];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HAMMER01:
		pPuppetPart[388].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hammer01", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[388];
		Object->BodyPartMask[18] = NULL;
		break;
	case PUPEQ_V2_2HAMMER02:
		pPuppetPart[389].LoadSprite3D(13, 9, 0, 0, 0, "V2_2Hammer02", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[389];
		Object->BodyPartMask[18] = NULL;
		break;

	case PUPEQ_T4C_FIRESWORD01DUAL:
		pPuppetPart[579].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01Dual", 0, 0, 0);
		pPuppetPart[580].LoadSprite3D(13, 9, 0, 0, 0, "NM_FireSword01DualMask", 0, 0, 0);
		Object->BodyPart[18] = &pPuppetPart[579];
		Object->BodyPartMask[18] = &pPuppetPart[580];
		break;





	}

	Object->nbBodyPart = 19;



	//##NMNMNM --[ Z-Order des partie du player a corriger certain...]
	// Ranged weapons!

	//Ranged Walk
	int i = 0;
	for (i = 0; i < 13; i++) {
		// 045	       
		BodyOrderR[1][0][i] = PUP_CAPE;
		BodyOrderR[1][1][i] = PUP_BACKBODY;
		BodyOrderR[1][2][i] = PUP_HAND_RIGHT;
		BodyOrderR[1][3][i] = PUP_ARM_RIGHT;
		BodyOrderR[1][4][i] = PUP_FOOT;
		BodyOrderR[1][5][i] = PUP_LEGS;
		BodyOrderR[1][6][i] = PUP_BOOT;
		BodyOrderR[1][7][i] = PUP_ROBELEGS;
		BodyOrderR[1][8][i] = PUP_BODY;
		BodyOrderR[1][9][i] = PUP_CAPE_2;
		BodyOrderR[1][10][i] = PUP_WEAPON;
		BodyOrderR[1][11][i] = PUP_HAND_LEFT;
		BodyOrderR[1][12][i] = PUP_ARM_LEFT;
		BodyOrderR[1][13][i] = PUP_HEAD;
		BodyOrderR[1][14][i] = PUP_HAIR;
		BodyOrderR[1][15][i] = PUP_HAT;
		BodyOrderR[1][16][i] = PUP_MASK;
		BodyOrderR[1][17][i] = PUP_SHIELD;
		BodyOrderR[1][18][i] = PUP_WEAPON2;

		// 180
		BodyOrderR[8][0][i] = PUP_WEAPON;
		BodyOrderR[8][1][i] = PUP_HAND_RIGHT;
		BodyOrderR[8][2][i] = PUP_HAND_LEFT;
		BodyOrderR[8][3][i] = PUP_FOOT;
		BodyOrderR[8][4][i] = PUP_LEGS;
		BodyOrderR[8][5][i] = PUP_BOOT;
		BodyOrderR[8][6][i] = PUP_ROBELEGS;
		BodyOrderR[8][7][i] = PUP_ARM_RIGHT;
		BodyOrderR[8][8][i] = PUP_ARM_LEFT;
		BodyOrderR[8][9][i] = PUP_SHIELD;
		BodyOrderR[8][10][i] = PUP_WEAPON2;
		BodyOrderR[8][11][i] = PUP_CAPE_2;
		BodyOrderR[8][12][i] = PUP_BODY;
		BodyOrderR[8][13][i] = PUP_MASK;
		BodyOrderR[8][14][i] = PUP_HEAD;
		BodyOrderR[8][15][i] = PUP_HAIR;
		BodyOrderR[8][16][i] = PUP_HAT;
		BodyOrderR[8][17][i] = PUP_BACKBODY;
		BodyOrderR[8][18][i] = PUP_CAPE;

		// 000
		BodyOrderR[2][0][i] = PUP_CAPE;
		BodyOrderR[2][1][i] = PUP_BACKBODY;
		BodyOrderR[2][2][i] = PUP_FOOT;
		BodyOrderR[2][3][i] = PUP_LEGS;
		BodyOrderR[2][4][i] = PUP_BOOT;
		BodyOrderR[2][5][i] = PUP_ROBELEGS;
		BodyOrderR[2][6][i] = PUP_BODY;
		BodyOrderR[2][7][i] = PUP_CAPE_2;
		BodyOrderR[2][8][i] = PUP_WEAPON;
		BodyOrderR[2][9][i] = PUP_HAND_RIGHT;
		BodyOrderR[2][10][i] = PUP_HAND_LEFT;
		BodyOrderR[2][11][i] = PUP_ARM_RIGHT;
		BodyOrderR[2][12][i] = PUP_ARM_LEFT;
		BodyOrderR[2][13][i] = PUP_HEAD;
		BodyOrderR[2][14][i] = PUP_HAIR;
		BodyOrderR[2][15][i] = PUP_HAT;
		BodyOrderR[2][16][i] = PUP_MASK;
		BodyOrderR[2][17][i] = PUP_SHIELD;
		BodyOrderR[2][18][i] = PUP_WEAPON2;

		// 315 

		BodyOrderR[3][0][i] = PUP_CAPE;
		BodyOrderR[3][1][i] = PUP_BACKBODY;
		BodyOrderR[3][2][i] = PUP_HAND_RIGHT;
		BodyOrderR[3][3][i] = PUP_ARM_RIGHT;
		BodyOrderR[3][4][i] = PUP_FOOT;
		BodyOrderR[3][5][i] = PUP_LEGS;
		BodyOrderR[3][6][i] = PUP_BOOT;
		BodyOrderR[3][7][i] = PUP_ROBELEGS;
		BodyOrderR[3][8][i] = PUP_BODY;
		BodyOrderR[3][9][i] = PUP_CAPE_2;
		BodyOrderR[3][10][i] = PUP_WEAPON;
		BodyOrderR[3][11][i] = PUP_HAND_LEFT;
		BodyOrderR[3][12][i] = PUP_ARM_LEFT;
		BodyOrderR[3][13][i] = PUP_HEAD;
		BodyOrderR[3][14][i] = PUP_HAIR;
		BodyOrderR[3][15][i] = PUP_HAT;
		BodyOrderR[3][16][i] = PUP_MASK;
		BodyOrderR[3][17][i] = PUP_SHIELD;
		BodyOrderR[3][18][i] = PUP_WEAPON2;

		// 090	       
		BodyOrderR[4][0][i] = PUP_HAND_RIGHT;
		BodyOrderR[4][1][i] = PUP_ARM_RIGHT;
		BodyOrderR[4][2][i] = PUP_FOOT;
		BodyOrderR[4][3][i] = PUP_LEGS;
		BodyOrderR[4][4][i] = PUP_BOOT;
		BodyOrderR[4][5][i] = PUP_ROBELEGS;
		BodyOrderR[4][6][i] = PUP_CAPE_2;
		BodyOrderR[4][7][i] = PUP_BODY;
		BodyOrderR[4][8][i] = PUP_HEAD;
		BodyOrderR[4][9][i] = PUP_HAIR;
		BodyOrderR[4][10][i] = PUP_HAT;
		BodyOrderR[4][11][i] = PUP_MASK;
		BodyOrderR[4][12][i] = PUP_CAPE;
		BodyOrderR[4][13][i] = PUP_WEAPON;
		BodyOrderR[4][14][i] = PUP_HAND_LEFT;
		BodyOrderR[4][15][i] = PUP_ARM_LEFT;
		BodyOrderR[4][16][i] = PUP_BACKBODY;
		BodyOrderR[4][17][i] = PUP_SHIELD;
		BodyOrderR[4][18][i] = PUP_WEAPON2;

		// 270

		BodyOrderR[6][0][i] = PUP_HAND_RIGHT;
		BodyOrderR[6][1][i] = PUP_ARM_RIGHT;
		BodyOrderR[6][2][i] = PUP_FOOT;
		BodyOrderR[6][3][i] = PUP_LEGS;
		BodyOrderR[6][4][i] = PUP_BOOT;
		BodyOrderR[6][5][i] = PUP_ROBELEGS;
		BodyOrderR[6][6][i] = PUP_CAPE_2;
		BodyOrderR[6][7][i] = PUP_BODY;
		BodyOrderR[6][8][i] = PUP_HEAD;
		BodyOrderR[6][9][i] = PUP_HAIR;
		BodyOrderR[6][10][i] = PUP_HAT;
		BodyOrderR[6][11][i] = PUP_MASK;
		BodyOrderR[6][12][i] = PUP_BACKBODY;
		BodyOrderR[6][13][i] = PUP_CAPE;
		BodyOrderR[6][14][i] = PUP_WEAPON;
		BodyOrderR[6][15][i] = PUP_HAND_LEFT;
		BodyOrderR[6][16][i] = PUP_ARM_LEFT;
		BodyOrderR[6][17][i] = PUP_SHIELD;
		BodyOrderR[6][18][i] = PUP_WEAPON2;

		// 135

		BodyOrderR[7][0][i] = PUP_HAND_RIGHT;
		BodyOrderR[7][1][i] = PUP_ARM_RIGHT;
		BodyOrderR[7][2][i] = PUP_FOOT;
		BodyOrderR[7][3][i] = PUP_LEGS;
		BodyOrderR[7][4][i] = PUP_BOOT;
		BodyOrderR[7][5][i] = PUP_ROBELEGS;
		BodyOrderR[7][6][i] = PUP_CAPE_2;
		BodyOrderR[7][7][i] = PUP_BODY;
		BodyOrderR[7][8][i] = PUP_MASK;
		BodyOrderR[7][9][i] = PUP_HEAD;
		BodyOrderR[7][10][i] = PUP_HAIR;
		BodyOrderR[7][11][i] = PUP_HAT;
		BodyOrderR[7][12][i] = PUP_WEAPON;
		BodyOrderR[7][13][i] = PUP_HAND_LEFT;
		BodyOrderR[7][14][i] = PUP_ARM_LEFT;
		BodyOrderR[7][15][i] = PUP_BACKBODY;
		BodyOrderR[7][16][i] = PUP_CAPE;
		BodyOrderR[7][17][i] = PUP_SHIELD;
		BodyOrderR[7][18][i] = PUP_WEAPON2;

		// 225			  	       
		BodyOrderR[9][0][i] = PUP_HAND_RIGHT;
		BodyOrderR[9][1][i] = PUP_ARM_RIGHT;
		BodyOrderR[9][2][i] = PUP_FOOT;
		BodyOrderR[9][3][i] = PUP_LEGS;
		BodyOrderR[9][4][i] = PUP_BOOT;
		BodyOrderR[9][5][i] = PUP_ROBELEGS;
		BodyOrderR[9][6][i] = PUP_CAPE_2;
		BodyOrderR[9][7][i] = PUP_BODY;
		BodyOrderR[9][8][i] = PUP_MASK;
		BodyOrderR[9][9][i] = PUP_HEAD;
		BodyOrderR[9][10][i] = PUP_HAIR;
		BodyOrderR[9][11][i] = PUP_HAT;
		BodyOrderR[9][12][i] = PUP_WEAPON;
		BodyOrderR[9][13][i] = PUP_HAND_LEFT;
		BodyOrderR[9][14][i] = PUP_ARM_LEFT;
		BodyOrderR[9][15][i] = PUP_BACKBODY;
		BodyOrderR[9][16][i] = PUP_CAPE;
		BodyOrderR[9][17][i] = PUP_SHIELD;
		BodyOrderR[9][18][i] = PUP_WEAPON2;
	}

	// ATTACK Ranged
	for (i = 0; i < 9; i++) {
		// 045
		BodyOrderAR[1][0][i] = PUP_BACKBODY;
		BodyOrderAR[1][1][i] = PUP_HAND_RIGHT;
		BodyOrderAR[1][2][i] = PUP_ARM_RIGHT;
		BodyOrderAR[1][3][i] = PUP_FOOT;
		BodyOrderAR[1][4][i] = PUP_LEGS;
		BodyOrderAR[1][5][i] = PUP_BOOT;
		BodyOrderAR[1][6][i] = PUP_ROBELEGS;
		BodyOrderAR[1][7][i] = PUP_BODY;
		BodyOrderAR[1][8][i] = PUP_WEAPON;
		BodyOrderAR[1][9][i] = PUP_HAND_LEFT;
		BodyOrderAR[1][10][i] = PUP_ARM_LEFT;
		BodyOrderAR[1][11][i] = PUP_HEAD;
		BodyOrderAR[1][12][i] = PUP_HAIR;
		BodyOrderAR[1][13][i] = PUP_HAT;
		BodyOrderAR[1][14][i] = PUP_MASK;
		BodyOrderAR[1][15][i] = PUP_CAPE;
		BodyOrderAR[1][16][i] = PUP_CAPE_2;
		BodyOrderAR[1][17][i] = PUP_SHIELD;
		BodyOrderAR[1][18][i] = PUP_WEAPON2;

		// 315			  
		BodyOrderAR[3][0][i] = PUP_BACKBODY;
		BodyOrderAR[3][1][i] = PUP_HAND_RIGHT;
		BodyOrderAR[3][2][i] = PUP_ARM_RIGHT;
		BodyOrderAR[3][3][i] = PUP_FOOT;
		BodyOrderAR[3][4][i] = PUP_LEGS;
		BodyOrderAR[3][5][i] = PUP_BOOT;
		BodyOrderAR[3][6][i] = PUP_ROBELEGS;
		BodyOrderAR[3][7][i] = PUP_BODY;
		BodyOrderAR[3][8][i] = PUP_WEAPON;
		BodyOrderAR[3][9][i] = PUP_HAND_LEFT;
		BodyOrderAR[3][10][i] = PUP_ARM_LEFT;
		BodyOrderAR[3][11][i] = PUP_HEAD;
		BodyOrderAR[3][12][i] = PUP_HAIR;
		BodyOrderAR[3][13][i] = PUP_HAT;
		BodyOrderAR[3][14][i] = PUP_MASK;
		BodyOrderAR[3][15][i] = PUP_CAPE;
		BodyOrderAR[3][16][i] = PUP_CAPE_2;
		BodyOrderAR[3][17][i] = PUP_SHIELD;
		BodyOrderAR[3][18][i] = PUP_WEAPON2;

	}

	for (i = 0; i < 2; i++) {
		// 090        
		BodyOrderAR[4][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[4][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[4][2][i] = PUP_FOOT;
		BodyOrderAR[4][3][i] = PUP_LEGS;
		BodyOrderAR[4][4][i] = PUP_BOOT;
		BodyOrderAR[4][5][i] = PUP_ROBELEGS;
		BodyOrderAR[4][6][i] = PUP_CAPE_2;
		BodyOrderAR[4][7][i] = PUP_BODY;
		BodyOrderAR[4][8][i] = PUP_HEAD;
		BodyOrderAR[4][9][i] = PUP_HAIR;
		BodyOrderAR[4][10][i] = PUP_HAT;
		BodyOrderAR[4][11][i] = PUP_MASK;
		BodyOrderAR[4][12][i] = PUP_WEAPON;
		BodyOrderAR[4][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[4][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[4][15][i] = PUP_BACKBODY;
		BodyOrderAR[4][16][i] = PUP_CAPE;
		BodyOrderAR[4][17][i] = PUP_SHIELD;
		BodyOrderAR[4][18][i] = PUP_WEAPON2;

		// 270
		BodyOrderAR[6][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[6][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[6][2][i] = PUP_FOOT;
		BodyOrderAR[6][3][i] = PUP_LEGS;
		BodyOrderAR[6][4][i] = PUP_BOOT;
		BodyOrderAR[6][5][i] = PUP_ROBELEGS;
		BodyOrderAR[6][6][i] = PUP_CAPE_2;
		BodyOrderAR[6][7][i] = PUP_BODY;
		BodyOrderAR[6][8][i] = PUP_HEAD;
		BodyOrderAR[6][9][i] = PUP_HAIR;
		BodyOrderAR[6][10][i] = PUP_HAT;
		BodyOrderAR[6][11][i] = PUP_MASK;
		BodyOrderAR[6][12][i] = PUP_WEAPON;
		BodyOrderAR[6][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[6][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[6][15][i] = PUP_BACKBODY;
		BodyOrderAR[6][16][i] = PUP_CAPE;
		BodyOrderAR[6][17][i] = PUP_SHIELD;
		BodyOrderAR[6][18][i] = PUP_WEAPON2;
	}

	for (i = 2; i < 8; i++) {
		// 090        
		BodyOrderAR[4][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[4][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[4][2][i] = PUP_FOOT;
		BodyOrderAR[4][3][i] = PUP_LEGS;
		BodyOrderAR[4][4][i] = PUP_BOOT;
		BodyOrderAR[4][5][i] = PUP_ROBELEGS;
		BodyOrderAR[4][6][i] = PUP_WEAPON;
		BodyOrderAR[4][7][i] = PUP_CAPE_2;
		BodyOrderAR[4][8][i] = PUP_BODY;
		BodyOrderAR[4][9][i] = PUP_HEAD;
		BodyOrderAR[4][10][i] = PUP_HAIR;
		BodyOrderAR[4][11][i] = PUP_HAT;
		BodyOrderAR[4][12][i] = PUP_MASK;
		BodyOrderAR[4][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[4][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[4][15][i] = PUP_BACKBODY;
		BodyOrderAR[4][16][i] = PUP_CAPE;
		BodyOrderAR[4][17][i] = PUP_SHIELD;
		BodyOrderAR[4][18][i] = PUP_WEAPON2;

		// 270
		BodyOrderAR[6][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[6][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[6][2][i] = PUP_FOOT;
		BodyOrderAR[6][3][i] = PUP_LEGS;
		BodyOrderAR[6][4][i] = PUP_BOOT;
		BodyOrderAR[6][5][i] = PUP_ROBELEGS;
		BodyOrderAR[6][6][i] = PUP_WEAPON;
		BodyOrderAR[6][7][i] = PUP_CAPE_2;
		BodyOrderAR[6][8][i] = PUP_BODY;
		BodyOrderAR[6][9][i] = PUP_HEAD;
		BodyOrderAR[6][10][i] = PUP_HAIR;
		BodyOrderAR[6][11][i] = PUP_HAT;
		BodyOrderAR[6][12][i] = PUP_MASK;
		BodyOrderAR[6][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[6][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[6][15][i] = PUP_BACKBODY;
		BodyOrderAR[6][16][i] = PUP_CAPE;
		BodyOrderAR[6][17][i] = PUP_SHIELD;
		BodyOrderAR[6][18][i] = PUP_WEAPON2;
	}

	for (i = 8; i < 9; i++) {
		// 090        
		BodyOrderAR[4][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[4][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[4][2][i] = PUP_FOOT;
		BodyOrderAR[4][3][i] = PUP_LEGS;
		BodyOrderAR[4][4][i] = PUP_BOOT;
		BodyOrderAR[4][5][i] = PUP_ROBELEGS;
		BodyOrderAR[4][6][i] = PUP_CAPE_2;
		BodyOrderAR[4][7][i] = PUP_BODY;
		BodyOrderAR[4][8][i] = PUP_HEAD;
		BodyOrderAR[4][9][i] = PUP_HAIR;
		BodyOrderAR[4][10][i] = PUP_HAT;
		BodyOrderAR[4][11][i] = PUP_MASK;
		BodyOrderAR[4][12][i] = PUP_WEAPON;
		BodyOrderAR[4][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[4][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[4][15][i] = PUP_BACKBODY;
		BodyOrderAR[4][16][i] = PUP_CAPE;
		BodyOrderAR[4][17][i] = PUP_SHIELD;
		BodyOrderAR[4][18][i] = PUP_WEAPON2;

		// 270
		BodyOrderAR[6][0][i] = PUP_HAND_RIGHT;
		BodyOrderAR[6][1][i] = PUP_ARM_RIGHT;
		BodyOrderAR[6][2][i] = PUP_FOOT;
		BodyOrderAR[6][3][i] = PUP_LEGS;
		BodyOrderAR[6][4][i] = PUP_BOOT;
		BodyOrderAR[6][5][i] = PUP_ROBELEGS;
		BodyOrderAR[6][6][i] = PUP_CAPE_2;
		BodyOrderAR[6][7][i] = PUP_BODY;
		BodyOrderAR[6][8][i] = PUP_HEAD;
		BodyOrderAR[6][9][i] = PUP_HAIR;
		BodyOrderAR[6][10][i] = PUP_HAT;
		BodyOrderAR[6][11][i] = PUP_MASK;
		BodyOrderAR[6][12][i] = PUP_WEAPON;
		BodyOrderAR[6][13][i] = PUP_HAND_LEFT;
		BodyOrderAR[6][14][i] = PUP_ARM_LEFT;
		BodyOrderAR[6][15][i] = PUP_BACKBODY;
		BodyOrderAR[6][16][i] = PUP_CAPE;
		BodyOrderAR[6][17][i] = PUP_SHIELD;
		BodyOrderAR[6][18][i] = PUP_WEAPON2;
	}

	// 180
	for (i = 0; i < 2; i++) {

		BodyOrderAR[8][0][i] = PUP_WEAPON2;
		BodyOrderAR[8][1][i] = PUP_SHIELD;
		BodyOrderAR[8][2][i] = PUP_WEAPON;
		BodyOrderAR[8][3][i] = PUP_HAND_RIGHT;
		BodyOrderAR[8][4][i] = PUP_HAND_LEFT;
		BodyOrderAR[8][5][i] = PUP_FOOT;
		BodyOrderAR[8][6][i] = PUP_LEGS;
		BodyOrderAR[8][7][i] = PUP_BOOT;
		BodyOrderAR[8][8][i] = PUP_ROBELEGS;
		BodyOrderAR[8][9][i] = PUP_ARM_RIGHT;
		BodyOrderAR[8][10][i] = PUP_ARM_LEFT;
		BodyOrderAR[8][11][i] = PUP_CAPE_2;
		BodyOrderAR[8][12][i] = PUP_BODY;
		BodyOrderAR[8][13][i] = PUP_MASK;
		BodyOrderAR[8][14][i] = PUP_HEAD;
		BodyOrderAR[8][15][i] = PUP_HAIR;
		BodyOrderAR[8][16][i] = PUP_HAT;
		BodyOrderAR[8][17][i] = PUP_BACKBODY;
		BodyOrderAR[8][18][i] = PUP_CAPE;
	}

	// 180
	for (i = 2; i < 5; i++) {
		BodyOrderAR[8][0][i] = PUP_WEAPON2;
		BodyOrderAR[8][1][i] = PUP_SHIELD;
		BodyOrderAR[8][2][i] = PUP_HAND_LEFT;
		BodyOrderAR[8][3][i] = PUP_FOOT;
		BodyOrderAR[8][4][i] = PUP_LEGS;
		BodyOrderAR[8][5][i] = PUP_BOOT;
		BodyOrderAR[8][6][i] = PUP_ROBELEGS;
		BodyOrderAR[8][7][i] = PUP_ARM_RIGHT;
		BodyOrderAR[8][8][i] = PUP_ARM_LEFT;
		BodyOrderAR[8][9][i] = PUP_CAPE_2;
		BodyOrderAR[8][10][i] = PUP_BODY;
		BodyOrderAR[8][11][i] = PUP_MASK;
		BodyOrderAR[8][12][i] = PUP_HEAD;
		BodyOrderAR[8][13][i] = PUP_HAIR;
		BodyOrderAR[8][14][i] = PUP_HAT;
		BodyOrderAR[8][15][i] = PUP_BACKBODY;
		BodyOrderAR[8][16][i] = PUP_CAPE;
		BodyOrderAR[8][17][i] = PUP_WEAPON;
		BodyOrderAR[8][18][i] = PUP_HAND_RIGHT;
	}

	// 180
	for (i = 5; i < 9; i++) {
		BodyOrderAR[8][0][i] = PUP_WEAPON2;
		BodyOrderAR[8][1][i] = PUP_SHIELD;
		BodyOrderAR[8][2][i] = PUP_WEAPON;
		BodyOrderAR[8][3][i] = PUP_HAND_RIGHT;
		BodyOrderAR[8][4][i] = PUP_HAND_LEFT;
		BodyOrderAR[8][5][i] = PUP_FOOT;
		BodyOrderAR[8][6][i] = PUP_LEGS;
		BodyOrderAR[8][7][i] = PUP_BOOT;
		BodyOrderAR[8][8][i] = PUP_ROBELEGS;
		BodyOrderAR[8][9][i] = PUP_ARM_RIGHT;
		BodyOrderAR[8][10][i] = PUP_ARM_LEFT;
		BodyOrderAR[8][11][i] = PUP_CAPE_2;
		BodyOrderAR[8][12][i] = PUP_BODY;
		BodyOrderAR[8][13][i] = PUP_MASK;
		BodyOrderAR[8][14][i] = PUP_HEAD;
		BodyOrderAR[8][15][i] = PUP_HAIR;
		BodyOrderAR[8][16][i] = PUP_HAT;
		BodyOrderAR[8][17][i] = PUP_BACKBODY;
		BodyOrderAR[8][18][i] = PUP_CAPE;
	}

	// 135
	for (i = 0; i < 2; i++) {
		BodyOrderAR[7][0][i] = PUP_WEAPON2;
		BodyOrderAR[7][1][i] = PUP_SHIELD;
		BodyOrderAR[7][2][i] = PUP_WEAPON;
		BodyOrderAR[7][3][i] = PUP_HAND_RIGHT;
		BodyOrderAR[7][4][i] = PUP_ARM_RIGHT;
		BodyOrderAR[7][5][i] = PUP_FOOT;
		BodyOrderAR[7][6][i] = PUP_LEGS;
		BodyOrderAR[7][7][i] = PUP_BOOT;
		BodyOrderAR[7][8][i] = PUP_ROBELEGS;
		BodyOrderAR[7][9][i] = PUP_CAPE_2;
		BodyOrderAR[7][10][i] = PUP_BODY;
		BodyOrderAR[7][11][i] = PUP_MASK;
		BodyOrderAR[7][12][i] = PUP_HEAD;
		BodyOrderAR[7][13][i] = PUP_HAIR;
		BodyOrderAR[7][14][i] = PUP_HAT;
		BodyOrderAR[7][15][i] = PUP_HAND_LEFT;
		BodyOrderAR[7][16][i] = PUP_ARM_LEFT;
		BodyOrderAR[7][17][i] = PUP_BACKBODY;
		BodyOrderAR[7][18][i] = PUP_CAPE;
	}

	// 135
	for (i = 2; i < 9; i++) {
		BodyOrderAR[7][0][i] = PUP_WEAPON;
		BodyOrderAR[7][1][i] = PUP_HAND_RIGHT;
		BodyOrderAR[7][2][i] = PUP_ARM_RIGHT;
		BodyOrderAR[7][3][i] = PUP_FOOT;
		BodyOrderAR[7][4][i] = PUP_LEGS;
		BodyOrderAR[7][5][i] = PUP_BOOT;
		BodyOrderAR[7][6][i] = PUP_ROBELEGS;
		BodyOrderAR[7][7][i] = PUP_CAPE_2;
		BodyOrderAR[7][8][i] = PUP_HAND_LEFT;
		BodyOrderAR[7][9][i] = PUP_ARM_LEFT;
		BodyOrderAR[7][10][i] = PUP_BODY;
		BodyOrderAR[7][11][i] = PUP_MASK;
		BodyOrderAR[7][12][i] = PUP_HEAD;
		BodyOrderAR[7][13][i] = PUP_HAIR;
		BodyOrderAR[7][14][i] = PUP_HAT;
		BodyOrderAR[7][15][i] = PUP_BACKBODY;
		BodyOrderAR[7][16][i] = PUP_CAPE;
		BodyOrderAR[7][17][i] = PUP_SHIELD;
		BodyOrderAR[7][18][i] = PUP_WEAPON2;
	}

	// 225
	for (i = 0; i < 2; i++) {
		BodyOrderAR[9][0][i] = PUP_WEAPON2;
		BodyOrderAR[9][1][i] = PUP_SHIELD;
		BodyOrderAR[9][2][i] = PUP_WEAPON;
		BodyOrderAR[9][3][i] = PUP_HAND_RIGHT;
		BodyOrderAR[9][4][i] = PUP_ARM_RIGHT;
		BodyOrderAR[9][5][i] = PUP_FOOT;
		BodyOrderAR[9][6][i] = PUP_LEGS;
		BodyOrderAR[9][7][i] = PUP_BOOT;
		BodyOrderAR[9][8][i] = PUP_ROBELEGS;
		BodyOrderAR[9][9][i] = PUP_CAPE_2;
		BodyOrderAR[9][10][i] = PUP_BODY;
		BodyOrderAR[9][11][i] = PUP_MASK;
		BodyOrderAR[9][12][i] = PUP_HEAD;
		BodyOrderAR[9][13][i] = PUP_HAIR;
		BodyOrderAR[9][14][i] = PUP_HAT;
		BodyOrderAR[9][15][i] = PUP_HAND_LEFT;
		BodyOrderAR[9][16][i] = PUP_ARM_LEFT;
		BodyOrderAR[9][17][i] = PUP_BACKBODY;
		BodyOrderAR[9][18][i] = PUP_CAPE;
	}

	// 225		
	for (i = 2; i < 9; i++) {
		BodyOrderAR[9][0][i] = PUP_WEAPON;
		BodyOrderAR[9][1][i] = PUP_HAND_RIGHT;
		BodyOrderAR[9][2][i] = PUP_ARM_RIGHT;
		BodyOrderAR[9][3][i] = PUP_FOOT;
		BodyOrderAR[9][4][i] = PUP_LEGS;
		BodyOrderAR[9][5][i] = PUP_BOOT;
		BodyOrderAR[9][6][i] = PUP_ROBELEGS;
		BodyOrderAR[9][7][i] = PUP_CAPE_2;
		BodyOrderAR[9][8][i] = PUP_HAND_LEFT;
		BodyOrderAR[9][9][i] = PUP_ARM_LEFT;
		BodyOrderAR[9][10][i] = PUP_BODY;
		BodyOrderAR[9][11][i] = PUP_MASK;
		BodyOrderAR[9][12][i] = PUP_HEAD;
		BodyOrderAR[9][13][i] = PUP_HAIR;
		BodyOrderAR[9][14][i] = PUP_HAT;
		BodyOrderAR[9][15][i] = PUP_BACKBODY;
		BodyOrderAR[9][16][i] = PUP_CAPE;
		BodyOrderAR[9][17][i] = PUP_SHIELD;
		BodyOrderAR[9][18][i] = PUP_WEAPON2;
	}

	// 000
	for (i = 0; i < 5; i++) {
		BodyOrderAR[2][0][i] = PUP_CAPE;
		BodyOrderAR[2][1][i] = PUP_BACKBODY;
		BodyOrderAR[2][2][i] = PUP_FOOT;
		BodyOrderAR[2][3][i] = PUP_LEGS;
		BodyOrderAR[2][4][i] = PUP_BOOT;
		BodyOrderAR[2][5][i] = PUP_ROBELEGS;
		BodyOrderAR[2][6][i] = PUP_BODY;
		BodyOrderAR[2][7][i] = PUP_CAPE_2;
		BodyOrderAR[2][8][i] = PUP_HEAD;
		BodyOrderAR[2][9][i] = PUP_HAIR;
		BodyOrderAR[2][10][i] = PUP_HAT;
		BodyOrderAR[2][11][i] = PUP_MASK;
		BodyOrderAR[2][12][i] = PUP_SHIELD;
		BodyOrderAR[2][13][i] = PUP_WEAPON2;
		BodyOrderAR[2][14][i] = PUP_HAND_RIGHT;
		BodyOrderAR[2][15][i] = PUP_ARM_RIGHT;
		BodyOrderAR[2][16][i] = PUP_HAND_LEFT;
		BodyOrderAR[2][17][i] = PUP_WEAPON;
		BodyOrderAR[2][18][i] = PUP_ARM_LEFT;
	}

	// 000
	for (i = 5; i < 9; i++) {
		BodyOrderAR[2][0][i] = PUP_WEAPON2;
		BodyOrderAR[2][1][i] = PUP_SHIELD;
		BodyOrderAR[2][2][i] = PUP_CAPE;
		BodyOrderAR[2][3][i] = PUP_BACKBODY;
		BodyOrderAR[2][4][i] = PUP_FOOT;
		BodyOrderAR[2][5][i] = PUP_LEGS;
		BodyOrderAR[2][6][i] = PUP_BOOT;
		BodyOrderAR[2][7][i] = PUP_ROBELEGS;
		BodyOrderAR[2][8][i] = PUP_BODY;
		BodyOrderAR[2][9][i] = PUP_CAPE_2;
		BodyOrderAR[2][10][i] = PUP_HEAD;
		BodyOrderAR[2][11][i] = PUP_HAIR;
		BodyOrderAR[2][12][i] = PUP_HAT;
		BodyOrderAR[2][13][i] = PUP_MASK;
		BodyOrderAR[2][14][i] = PUP_HAND_RIGHT;
		BodyOrderAR[2][15][i] = PUP_ARM_RIGHT;
		BodyOrderAR[2][16][i] = PUP_HAND_LEFT;
		BodyOrderAR[2][17][i] = PUP_WEAPON;
		BodyOrderAR[2][18][i] = PUP_ARM_LEFT;
	}

	// Non-ranged weapons!

	// WALKING
	for (i = 0; i < 13; i++) {
		// 045
		BodyOrder[1][0][i] = PUP_WEAPON;
		BodyOrder[1][1][i] = PUP_CAPE;
		BodyOrder[1][2][i] = PUP_BACKBODY;
		BodyOrder[1][3][i] = PUP_HAND_RIGHT;
		BodyOrder[1][4][i] = PUP_ARM_RIGHT;
		BodyOrder[1][5][i] = PUP_FOOT;
		BodyOrder[1][6][i] = PUP_LEGS;
		BodyOrder[1][7][i] = PUP_BOOT;
		BodyOrder[1][8][i] = PUP_ROBELEGS;
		BodyOrder[1][9][i] = PUP_BODY;
		BodyOrder[1][10][i] = PUP_CAPE_2;
		BodyOrder[1][11][i] = PUP_HAND_LEFT;
		BodyOrder[1][12][i] = PUP_ARM_LEFT;
		BodyOrder[1][13][i] = PUP_HEAD;
		BodyOrder[1][14][i] = PUP_HAIR;
		BodyOrder[1][15][i] = PUP_HAT;
		BodyOrder[1][16][i] = PUP_MASK;
		BodyOrder[1][17][i] = PUP_SHIELD;
		BodyOrder[1][18][i] = PUP_WEAPON2;

		// 180
		BodyOrder[8][0][i] = PUP_WEAPON;
		BodyOrder[8][1][i] = PUP_WEAPON2;
		BodyOrder[8][2][i] = PUP_HAND_RIGHT;
		BodyOrder[8][3][i] = PUP_HAND_LEFT;
		BodyOrder[8][4][i] = PUP_FOOT;
		BodyOrder[8][5][i] = PUP_LEGS;
		BodyOrder[8][6][i] = PUP_BOOT;
		BodyOrder[8][7][i] = PUP_ROBELEGS;
		BodyOrder[8][8][i] = PUP_ARM_RIGHT;
		BodyOrder[8][9][i] = PUP_ARM_LEFT;
		BodyOrder[8][10][i] = PUP_SHIELD;
		BodyOrder[8][11][i] = PUP_CAPE_2;
		BodyOrder[8][12][i] = PUP_BODY;
		BodyOrder[8][13][i] = PUP_MASK;
		BodyOrder[8][14][i] = PUP_HEAD;
		BodyOrder[8][15][i] = PUP_HAIR;
		BodyOrder[8][16][i] = PUP_HAT;
		BodyOrder[8][17][i] = PUP_BACKBODY;
		BodyOrder[8][18][i] = PUP_CAPE;

		// 000
		BodyOrder[2][0][i] = PUP_CAPE;
		BodyOrder[2][1][i] = PUP_BACKBODY;
		BodyOrder[2][2][i] = PUP_FOOT;
		BodyOrder[2][3][i] = PUP_LEGS;
		BodyOrder[2][4][i] = PUP_BOOT;
		BodyOrder[2][5][i] = PUP_ROBELEGS;
		BodyOrder[2][6][i] = PUP_BODY;
		BodyOrder[2][7][i] = PUP_CAPE_2;
		BodyOrder[2][8][i] = PUP_HAND_RIGHT;
		BodyOrder[2][9][i] = PUP_HAND_LEFT;
		BodyOrder[2][10][i] = PUP_ARM_RIGHT;
		BodyOrder[2][11][i] = PUP_ARM_LEFT;
		BodyOrder[2][12][i] = PUP_HEAD;
		BodyOrder[2][13][i] = PUP_HAIR;
		BodyOrder[2][14][i] = PUP_HAT;
		BodyOrder[2][15][i] = PUP_MASK;
		BodyOrder[2][16][i] = PUP_WEAPON;
		BodyOrder[2][17][i] = PUP_SHIELD;
		BodyOrder[2][18][i] = PUP_WEAPON2;

		// 315
		BodyOrder[3][0][i] = PUP_WEAPON;
		BodyOrder[3][1][i] = PUP_CAPE;
		BodyOrder[3][2][i] = PUP_BACKBODY;
		BodyOrder[3][3][i] = PUP_HAND_RIGHT;
		BodyOrder[3][4][i] = PUP_ARM_RIGHT;
		BodyOrder[3][5][i] = PUP_FOOT;
		BodyOrder[3][6][i] = PUP_LEGS;
		BodyOrder[3][7][i] = PUP_BOOT;
		BodyOrder[3][8][i] = PUP_ROBELEGS;
		BodyOrder[3][9][i] = PUP_BODY;
		BodyOrder[3][10][i] = PUP_CAPE_2;
		BodyOrder[3][11][i] = PUP_HAND_LEFT;
		BodyOrder[3][12][i] = PUP_ARM_LEFT;
		BodyOrder[3][13][i] = PUP_HEAD;
		BodyOrder[3][14][i] = PUP_HAIR;
		BodyOrder[3][15][i] = PUP_HAT;
		BodyOrder[3][16][i] = PUP_MASK;
		BodyOrder[3][17][i] = PUP_SHIELD;
		BodyOrder[3][18][i] = PUP_WEAPON2;

		// 090
		BodyOrder[4][0][i] = PUP_WEAPON;
		BodyOrder[4][1][i] = PUP_HAND_RIGHT;
		BodyOrder[4][2][i] = PUP_ARM_RIGHT;
		BodyOrder[4][3][i] = PUP_FOOT;
		BodyOrder[4][4][i] = PUP_LEGS;
		BodyOrder[4][5][i] = PUP_BOOT;
		BodyOrder[4][6][i] = PUP_ROBELEGS;
		BodyOrder[4][7][i] = PUP_CAPE_2;
		BodyOrder[4][8][i] = PUP_BODY;
		BodyOrder[4][9][i] = PUP_HEAD;
		BodyOrder[4][10][i] = PUP_HAIR;
		BodyOrder[4][11][i] = PUP_HAT;
		BodyOrder[4][12][i] = PUP_MASK;
		BodyOrder[4][13][i] = PUP_CAPE;
		BodyOrder[4][14][i] = PUP_HAND_LEFT;
		BodyOrder[4][15][i] = PUP_ARM_LEFT;
		BodyOrder[4][16][i] = PUP_BACKBODY;
		BodyOrder[4][17][i] = PUP_SHIELD;
		BodyOrder[4][18][i] = PUP_WEAPON2;

		// 270
		BodyOrder[6][0][i] = PUP_WEAPON;
		BodyOrder[6][1][i] = PUP_HAND_RIGHT;
		BodyOrder[6][2][i] = PUP_ARM_RIGHT;
		BodyOrder[6][3][i] = PUP_FOOT;
		BodyOrder[6][4][i] = PUP_LEGS;
		BodyOrder[6][5][i] = PUP_BOOT;
		BodyOrder[6][6][i] = PUP_ROBELEGS;
		BodyOrder[6][7][i] = PUP_CAPE_2;
		BodyOrder[6][8][i] = PUP_BODY;
		BodyOrder[6][9][i] = PUP_HEAD;
		BodyOrder[6][10][i] = PUP_HAIR;
		BodyOrder[6][11][i] = PUP_HAT;
		BodyOrder[6][12][i] = PUP_MASK;
		BodyOrder[6][13][i] = PUP_BACKBODY;
		BodyOrder[6][14][i] = PUP_CAPE;
		BodyOrder[6][15][i] = PUP_HAND_LEFT;
		BodyOrder[6][16][i] = PUP_ARM_LEFT;
		BodyOrder[6][17][i] = PUP_SHIELD;
		BodyOrder[6][18][i] = PUP_WEAPON2;

		// 135
		BodyOrder[7][0][i] = PUP_WEAPON;
		BodyOrder[7][1][i] = PUP_HAND_RIGHT;
		BodyOrder[7][2][i] = PUP_ARM_RIGHT;
		BodyOrder[7][3][i] = PUP_FOOT;
		BodyOrder[7][4][i] = PUP_LEGS;
		BodyOrder[7][5][i] = PUP_BOOT;
		BodyOrder[7][6][i] = PUP_ROBELEGS;
		BodyOrder[7][7][i] = PUP_CAPE_2;
		BodyOrder[7][8][i] = PUP_BODY;
		BodyOrder[7][9][i] = PUP_MASK;
		BodyOrder[7][10][i] = PUP_HEAD;
		BodyOrder[7][11][i] = PUP_HAIR;
		BodyOrder[7][12][i] = PUP_HAT;
		BodyOrder[7][13][i] = PUP_HAND_LEFT;
		BodyOrder[7][14][i] = PUP_ARM_LEFT;
		BodyOrder[7][15][i] = PUP_BACKBODY;
		BodyOrder[7][16][i] = PUP_WEAPON2;
		BodyOrder[7][17][i] = PUP_CAPE;
		BodyOrder[7][18][i] = PUP_SHIELD;



		// 225			  
		BodyOrder[9][0][i] = PUP_WEAPON;
		BodyOrder[9][1][i] = PUP_HAND_RIGHT;
		BodyOrder[9][2][i] = PUP_ARM_RIGHT;
		BodyOrder[9][3][i] = PUP_FOOT;
		BodyOrder[9][4][i] = PUP_LEGS;
		BodyOrder[9][5][i] = PUP_BOOT;
		BodyOrder[9][6][i] = PUP_ROBELEGS;
		BodyOrder[9][7][i] = PUP_CAPE_2;
		BodyOrder[9][8][i] = PUP_BODY;
		BodyOrder[9][9][i] = PUP_MASK;
		BodyOrder[9][10][i] = PUP_HEAD;
		BodyOrder[9][11][i] = PUP_HAIR;
		BodyOrder[9][12][i] = PUP_HAT;
		BodyOrder[9][13][i] = PUP_HAND_LEFT;
		BodyOrder[9][14][i] = PUP_ARM_LEFT;
		BodyOrder[9][15][i] = PUP_BACKBODY;
		BodyOrder[9][16][i] = PUP_WEAPON2;
		BodyOrder[9][17][i] = PUP_CAPE;
		BodyOrder[9][18][i] = PUP_SHIELD;
	}

	// ATTACK
	for (i = 0; i < 9; i++)
	{
		// 090
		BodyOrderA[4][0][i] = PUP_WEAPON;
		BodyOrderA[4][1][i] = PUP_HAND_RIGHT;
		BodyOrderA[4][2][i] = PUP_ARM_RIGHT;
		BodyOrderA[4][3][i] = PUP_FOOT;
		BodyOrderA[4][4][i] = PUP_LEGS;
		BodyOrderA[4][5][i] = PUP_BOOT;
		BodyOrderA[4][6][i] = PUP_ROBELEGS;
		BodyOrderA[4][7][i] = PUP_CAPE_2;
		BodyOrderA[4][8][i] = PUP_BODY;
		BodyOrderA[4][9][i] = PUP_HEAD;
		BodyOrderA[4][10][i] = PUP_HAIR;
		BodyOrderA[4][11][i] = PUP_HAT;
		BodyOrderA[4][12][i] = PUP_MASK;
		BodyOrderA[4][13][i] = PUP_HAND_LEFT;
		BodyOrderA[4][14][i] = PUP_ARM_LEFT;
		BodyOrderA[4][15][i] = PUP_BACKBODY;
		BodyOrderA[4][16][i] = PUP_SHIELD;
		BodyOrderA[4][17][i] = PUP_WEAPON2;
		BodyOrderA[4][18][i] = PUP_CAPE;


		// 270
		BodyOrderA[6][0][i] = PUP_WEAPON;
		BodyOrderA[6][1][i] = PUP_HAND_RIGHT;
		BodyOrderA[6][2][i] = PUP_ARM_RIGHT;
		BodyOrderA[6][3][i] = PUP_FOOT;
		BodyOrderA[6][4][i] = PUP_LEGS;
		BodyOrderA[6][5][i] = PUP_BOOT;
		BodyOrderA[6][6][i] = PUP_ROBELEGS;
		BodyOrderA[6][7][i] = PUP_CAPE_2;
		BodyOrderA[6][8][i] = PUP_BODY;
		BodyOrderA[6][9][i] = PUP_HEAD;
		BodyOrderA[6][10][i] = PUP_HAIR;
		BodyOrderA[6][11][i] = PUP_HAT;
		BodyOrderA[6][12][i] = PUP_MASK;
		BodyOrderA[6][13][i] = PUP_HAND_LEFT;
		BodyOrderA[6][14][i] = PUP_ARM_LEFT;
		BodyOrderA[6][15][i] = PUP_BACKBODY;
		BodyOrderA[6][16][i] = PUP_SHIELD;
		BodyOrderA[6][17][i] = PUP_WEAPON2;
		BodyOrderA[6][18][i] = PUP_CAPE;

	}

	// 180
	for (i = 0; i < 2; i++) {
		BodyOrderA[8][0][i] = PUP_WEAPON2;
		BodyOrderA[8][1][i] = PUP_SHIELD;
		BodyOrderA[8][2][i] = PUP_WEAPON;
		BodyOrderA[8][3][i] = PUP_HAND_RIGHT;
		BodyOrderA[8][4][i] = PUP_HAND_LEFT;
		BodyOrderA[8][5][i] = PUP_FOOT;
		BodyOrderA[8][6][i] = PUP_LEGS;
		BodyOrderA[8][7][i] = PUP_BOOT;
		BodyOrderA[8][8][i] = PUP_ROBELEGS;
		BodyOrderA[8][9][i] = PUP_ARM_RIGHT;
		BodyOrderA[8][10][i] = PUP_ARM_LEFT;
		BodyOrderA[8][11][i] = PUP_CAPE_2;
		BodyOrderA[8][12][i] = PUP_BODY;
		BodyOrderA[8][13][i] = PUP_MASK;
		BodyOrderA[8][14][i] = PUP_HEAD;
		BodyOrderA[8][15][i] = PUP_HAIR;
		BodyOrderA[8][16][i] = PUP_HAT;
		BodyOrderA[8][17][i] = PUP_BACKBODY;
		BodyOrderA[8][18][i] = PUP_CAPE;
	}

	// 180
	for (i = 2; i < 5; i++) {
		BodyOrderA[8][0][i] = PUP_WEAPON2;
		BodyOrderA[8][1][i] = PUP_SHIELD;
		BodyOrderA[8][2][i] = PUP_HAND_LEFT;
		BodyOrderA[8][3][i] = PUP_FOOT;
		BodyOrderA[8][4][i] = PUP_LEGS;
		BodyOrderA[8][5][i] = PUP_BOOT;
		BodyOrderA[8][6][i] = PUP_ROBELEGS;
		BodyOrderA[8][7][i] = PUP_ARM_RIGHT;
		BodyOrderA[8][8][i] = PUP_ARM_LEFT;
		BodyOrderA[8][9][i] = PUP_CAPE_2;
		BodyOrderA[8][10][i] = PUP_BODY;
		BodyOrderA[8][11][i] = PUP_MASK;
		BodyOrderA[8][12][i] = PUP_HEAD;
		BodyOrderA[8][13][i] = PUP_HAIR;
		BodyOrderA[8][14][i] = PUP_HAT;
		BodyOrderA[8][15][i] = PUP_BACKBODY;
		BodyOrderA[8][16][i] = PUP_CAPE;
		BodyOrderA[8][17][i] = PUP_WEAPON;
		BodyOrderA[8][18][i] = PUP_HAND_RIGHT;
	}

	// 180
	for (i = 5; i < 9; i++) {
		BodyOrderA[8][0][i] = PUP_WEAPON2;
		BodyOrderA[8][1][i] = PUP_SHIELD;
		BodyOrderA[8][2][i] = PUP_WEAPON;
		BodyOrderA[8][3][i] = PUP_HAND_RIGHT;
		BodyOrderA[8][4][i] = PUP_HAND_LEFT;
		BodyOrderA[8][5][i] = PUP_FOOT;
		BodyOrderA[8][6][i] = PUP_LEGS;
		BodyOrderA[8][7][i] = PUP_BOOT;
		BodyOrderA[8][8][i] = PUP_ROBELEGS;
		BodyOrderA[8][9][i] = PUP_ARM_RIGHT;
		BodyOrderA[8][10][i] = PUP_ARM_LEFT;
		BodyOrderA[8][11][i] = PUP_CAPE_2;
		BodyOrderA[8][12][i] = PUP_BODY;
		BodyOrderA[8][13][i] = PUP_MASK;
		BodyOrderA[8][14][i] = PUP_HEAD;
		BodyOrderA[8][15][i] = PUP_HAIR;
		BodyOrderA[8][16][i] = PUP_HAT;
		BodyOrderA[8][17][i] = PUP_BACKBODY;
		BodyOrderA[8][18][i] = PUP_CAPE;
	}

	for (i = 0; i < 5; i++)
	{
		// 045
		BodyOrderA[1][0][i] = PUP_BACKBODY;
		BodyOrderA[1][1][i] = PUP_WEAPON;
		BodyOrderA[1][2][i] = PUP_HAND_RIGHT;
		BodyOrderA[1][3][i] = PUP_ARM_RIGHT;
		BodyOrderA[1][4][i] = PUP_FOOT;
		BodyOrderA[1][5][i] = PUP_LEGS;
		BodyOrderA[1][6][i] = PUP_BOOT;
		BodyOrderA[1][7][i] = PUP_ROBELEGS;
		BodyOrderA[1][8][i] = PUP_BODY;
		BodyOrderA[1][9][i] = PUP_HEAD;
		BodyOrderA[1][10][i] = PUP_HAIR;
		BodyOrderA[1][11][i] = PUP_HAT;
		BodyOrderA[1][12][i] = PUP_MASK;
		BodyOrderA[1][13][i] = PUP_CAPE;
		BodyOrderA[1][14][i] = PUP_CAPE_2;
		BodyOrderA[1][15][i] = PUP_HAND_LEFT;
		BodyOrderA[1][16][i] = PUP_ARM_LEFT;
		BodyOrderA[1][17][i] = PUP_SHIELD;
		BodyOrderA[1][18][i] = PUP_WEAPON2;
		// 315			  
		BodyOrderA[3][0][i] = PUP_BACKBODY;
		BodyOrderA[3][1][i] = PUP_WEAPON;
		BodyOrderA[3][2][i] = PUP_HAND_RIGHT;
		BodyOrderA[3][3][i] = PUP_ARM_RIGHT;
		BodyOrderA[3][4][i] = PUP_FOOT;
		BodyOrderA[3][5][i] = PUP_LEGS;
		BodyOrderA[3][6][i] = PUP_BOOT;
		BodyOrderA[3][7][i] = PUP_ROBELEGS;
		BodyOrderA[3][8][i] = PUP_BODY;
		BodyOrderA[3][9][i] = PUP_HEAD;
		BodyOrderA[3][10][i] = PUP_HAIR;
		BodyOrderA[3][11][i] = PUP_HAT;
		BodyOrderA[3][12][i] = PUP_MASK;
		BodyOrderA[3][13][i] = PUP_CAPE;
		BodyOrderA[3][14][i] = PUP_CAPE_2;
		BodyOrderA[3][15][i] = PUP_HAND_LEFT;
		BodyOrderA[3][16][i] = PUP_ARM_LEFT;
		BodyOrderA[3][17][i] = PUP_SHIELD;
		BodyOrderA[3][18][i] = PUP_WEAPON2;
		/// 135
		BodyOrderA[7][0][i] = PUP_WEAPON2;
		BodyOrderA[7][1][i] = PUP_SHIELD;
		BodyOrderA[7][2][i] = PUP_WEAPON;
		BodyOrderA[7][3][i] = PUP_HAND_RIGHT;
		BodyOrderA[7][4][i] = PUP_ARM_RIGHT;
		BodyOrderA[7][5][i] = PUP_FOOT;
		BodyOrderA[7][6][i] = PUP_LEGS;
		BodyOrderA[7][7][i] = PUP_BOOT;
		BodyOrderA[7][8][i] = PUP_ROBELEGS;
		BodyOrderA[7][9][i] = PUP_CAPE_2;
		BodyOrderA[7][10][i] = PUP_BODY;
		BodyOrderA[7][11][i] = PUP_MASK;
		BodyOrderA[7][12][i] = PUP_HEAD;
		BodyOrderA[7][13][i] = PUP_HAIR;
		BodyOrderA[7][14][i] = PUP_HAT;
		BodyOrderA[7][15][i] = PUP_HAND_LEFT;
		BodyOrderA[7][16][i] = PUP_ARM_LEFT;
		BodyOrderA[7][17][i] = PUP_BACKBODY;
		BodyOrderA[7][18][i] = PUP_CAPE;
	}

	// 135
	for (i = 5; i < 9; i++)
	{
		// 045
		BodyOrderA[1][0][i] = PUP_BACKBODY;
		BodyOrderA[1][1][i] = PUP_WEAPON;
		BodyOrderA[1][2][i] = PUP_HAND_RIGHT;
		BodyOrderA[1][3][i] = PUP_ARM_RIGHT;
		BodyOrderA[1][4][i] = PUP_FOOT;
		BodyOrderA[1][5][i] = PUP_LEGS;
		BodyOrderA[1][6][i] = PUP_BOOT;
		BodyOrderA[1][7][i] = PUP_ROBELEGS;
		BodyOrderA[1][8][i] = PUP_BODY;
		BodyOrderA[1][9][i] = PUP_CAPE;
		BodyOrderA[1][10][i] = PUP_CAPE_2;
		BodyOrderA[1][11][i] = PUP_HAND_LEFT;
		BodyOrderA[1][12][i] = PUP_ARM_LEFT;
		BodyOrderA[1][13][i] = PUP_HEAD;
		BodyOrderA[1][14][i] = PUP_HAIR;
		BodyOrderA[1][15][i] = PUP_HAT;
		BodyOrderA[1][16][i] = PUP_MASK;
		BodyOrderA[1][17][i] = PUP_SHIELD;
		BodyOrderA[1][18][i] = PUP_WEAPON2;
		// 315			  
		BodyOrderA[3][0][i] = PUP_BACKBODY;
		BodyOrderA[3][1][i] = PUP_WEAPON;
		BodyOrderA[3][2][i] = PUP_HAND_RIGHT;
		BodyOrderA[3][3][i] = PUP_ARM_RIGHT;
		BodyOrderA[3][4][i] = PUP_FOOT;
		BodyOrderA[3][5][i] = PUP_LEGS;
		BodyOrderA[3][6][i] = PUP_BOOT;
		BodyOrderA[3][7][i] = PUP_ROBELEGS;
		BodyOrderA[3][8][i] = PUP_BODY;
		BodyOrderA[3][9][i] = PUP_CAPE;
		BodyOrderA[3][10][i] = PUP_CAPE_2;
		BodyOrderA[3][11][i] = PUP_HAND_LEFT;
		BodyOrderA[3][12][i] = PUP_ARM_LEFT;
		BodyOrderA[3][13][i] = PUP_HEAD;
		BodyOrderA[3][14][i] = PUP_HAIR;
		BodyOrderA[3][15][i] = PUP_HAT;
		BodyOrderA[3][16][i] = PUP_MASK;
		BodyOrderA[3][17][i] = PUP_SHIELD;
		BodyOrderA[3][18][i] = PUP_WEAPON2;






		//135
		BodyOrderA[7][0][i] = PUP_WEAPON;
		BodyOrderA[7][1][i] = PUP_HAND_RIGHT;
		BodyOrderA[7][2][i] = PUP_ARM_RIGHT;
		BodyOrderA[7][3][i] = PUP_FOOT;
		BodyOrderA[7][4][i] = PUP_LEGS;
		BodyOrderA[7][5][i] = PUP_BOOT;
		BodyOrderA[7][6][i] = PUP_ROBELEGS;
		BodyOrderA[7][7][i] = PUP_CAPE_2;
		BodyOrderA[7][8][i] = PUP_BODY;
		BodyOrderA[7][9][i] = PUP_MASK;
		BodyOrderA[7][10][i] = PUP_HEAD;
		BodyOrderA[7][11][i] = PUP_HAIR;
		BodyOrderA[7][12][i] = PUP_HAT;
		BodyOrderA[7][13][i] = PUP_HAND_LEFT;
		BodyOrderA[7][14][i] = PUP_ARM_LEFT;
		BodyOrderA[7][15][i] = PUP_BACKBODY;
		BodyOrderA[7][16][i] = PUP_CAPE;
		BodyOrderA[7][17][i] = PUP_SHIELD;
		BodyOrderA[7][18][i] = PUP_WEAPON2;
	}

	// 225
	for (i = 0; i < 5; i++) {
		BodyOrderA[9][0][i] = PUP_WEAPON2;
		BodyOrderA[9][1][i] = PUP_SHIELD;
		BodyOrderA[9][2][i] = PUP_WEAPON;
		BodyOrderA[9][3][i] = PUP_HAND_RIGHT;
		BodyOrderA[9][4][i] = PUP_ARM_RIGHT;
		BodyOrderA[9][5][i] = PUP_FOOT;
		BodyOrderA[9][6][i] = PUP_LEGS;
		BodyOrderA[9][7][i] = PUP_BOOT;
		BodyOrderA[9][8][i] = PUP_ROBELEGS;
		BodyOrderA[9][9][i] = PUP_CAPE_2;
		BodyOrderA[9][10][i] = PUP_MASK;
		BodyOrderA[9][11][i] = PUP_BODY;
		BodyOrderA[9][12][i] = PUP_HEAD;
		BodyOrderA[9][13][i] = PUP_HAIR;
		BodyOrderA[9][14][i] = PUP_HAT;
		BodyOrderA[9][15][i] = PUP_HAND_LEFT;
		BodyOrderA[9][16][i] = PUP_ARM_LEFT;
		BodyOrderA[9][17][i] = PUP_BACKBODY;
		BodyOrderA[9][18][i] = PUP_CAPE;
	}

	// 225		
	for (i = 5; i < 9; i++) {
		BodyOrderA[9][0][i] = PUP_WEAPON;
		BodyOrderA[9][1][i] = PUP_HAND_RIGHT;
		BodyOrderA[9][2][i] = PUP_ARM_RIGHT;
		BodyOrderA[9][3][i] = PUP_FOOT;
		BodyOrderA[9][4][i] = PUP_LEGS;
		BodyOrderA[9][5][i] = PUP_BOOT;
		BodyOrderA[9][6][i] = PUP_ROBELEGS;
		BodyOrderA[9][7][i] = PUP_CAPE_2;
		BodyOrderA[9][8][i] = PUP_BODY;
		BodyOrderA[9][9][i] = PUP_MASK;
		BodyOrderA[9][10][i] = PUP_HEAD;
		BodyOrderA[9][11][i] = PUP_HAIR;
		BodyOrderA[9][12][i] = PUP_HAT;
		BodyOrderA[9][13][i] = PUP_HAND_LEFT;
		BodyOrderA[9][14][i] = PUP_ARM_LEFT;
		BodyOrderA[9][15][i] = PUP_BACKBODY;
		BodyOrderA[9][16][i] = PUP_CAPE;
		BodyOrderA[9][17][i] = PUP_SHIELD;
		BodyOrderA[9][18][i] = PUP_WEAPON2;
	}

	// 000
	for (i = 0; i < 5; i++) {
		BodyOrderA[2][0][i] = PUP_CAPE;
		BodyOrderA[2][1][i] = PUP_BACKBODY;
		BodyOrderA[2][2][i] = PUP_FOOT;
		BodyOrderA[2][3][i] = PUP_LEGS;
		BodyOrderA[2][4][i] = PUP_BOOT;
		BodyOrderA[2][5][i] = PUP_ROBELEGS;
		BodyOrderA[2][6][i] = PUP_WEAPON;
		BodyOrderA[2][7][i] = PUP_HAND_RIGHT;
		BodyOrderA[2][8][i] = PUP_ARM_RIGHT;
		BodyOrderA[2][9][i] = PUP_BODY;
		BodyOrderA[2][10][i] = PUP_HAND_LEFT;
		BodyOrderA[2][11][i] = PUP_ARM_LEFT;
		BodyOrderA[2][12][i] = PUP_CAPE_2;
		BodyOrderA[2][13][i] = PUP_HEAD;
		BodyOrderA[2][14][i] = PUP_HAIR;
		BodyOrderA[2][15][i] = PUP_HAT;
		BodyOrderA[2][16][i] = PUP_MASK;
		BodyOrderA[2][17][i] = PUP_SHIELD;
		BodyOrderA[2][18][i] = PUP_WEAPON2;
	}

	// 000
	for (i = 5; i < 9; i++) {
		BodyOrderA[2][0][i] = PUP_WEAPON2;
		BodyOrderA[2][1][i] = PUP_SHIELD;
		BodyOrderA[2][2][i] = PUP_CAPE;
		BodyOrderA[2][3][i] = PUP_BACKBODY;
		BodyOrderA[2][4][i] = PUP_FOOT;
		BodyOrderA[2][5][i] = PUP_LEGS;
		BodyOrderA[2][6][i] = PUP_BOOT;
		BodyOrderA[2][7][i] = PUP_ROBELEGS;
		BodyOrderA[2][8][i] = PUP_WEAPON;
		BodyOrderA[2][9][i] = PUP_HAND_RIGHT;
		BodyOrderA[2][10][i] = PUP_HAND_LEFT;
		BodyOrderA[2][11][i] = PUP_ARM_RIGHT;
		BodyOrderA[2][12][i] = PUP_ARM_LEFT;
		BodyOrderA[2][13][i] = PUP_BODY;
		BodyOrderA[2][14][i] = PUP_CAPE_2;
		BodyOrderA[2][15][i] = PUP_HEAD;
		BodyOrderA[2][16][i] = PUP_HAIR;
		BodyOrderA[2][17][i] = PUP_HAT;
		BodyOrderA[2][18][i] = PUP_MASK;
	}

	fTransparent = Object->BodyPart[0]->fTransparent;
	TransColor = Object->BodyPart[0]->TransColor;

	MaxWidth = 215;  //115
	MaxHeight = 200; //100
	MaxxCor = -100; //-50
	MaxyCor = -130; //-80

	Type = Object->BodyPart[0]->Type;
	Faces = Object->BodyPart[0]->Faces;
	FacesA = Object->BodyPart[0]->FacesA;
	FacesC = Object->BodyPart[0]->FacesC;
	FacesR = Object->BodyPart[0]->FacesR;  //NMNMNM_Faces
	FacesStA = Object->BodyPart[0]->FacesStA;
	FacesStM = Object->BodyPart[0]->FacesStM;
}
