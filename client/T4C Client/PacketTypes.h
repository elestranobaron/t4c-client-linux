// *******************************************************************************************
// ***                                                                                     ***
//      File Name: PacketTypes.h
//      Project:   TFC Server
//      Plateform: Windows NT 4.0 Workstation/Server
//      Creation:  6/2/1998
//      Author:    Francois Leblanc (FL)
// ***                                                                                     ***
// *******************************************************************************************
// ***                                                                                     ***
//      Change History
//
//         Date            Ver.      Author         Purpose
//         ----            ----      ------         -------
//         6/2/1998      1.0        FL             Initial developpement
//
//      Description
//          This file defines all the different packet types
//          
// ***                                                                                     ***
//	*******************************************************************************************
// ***           Copyright (c) 1997-1998 Vircom. All rights reserved.                      ***

#ifndef __PACKET_TYPES
#define __PACKET_TYPES

typedef short RQ_SIZE;

#define	RQ_MoveNorth					1 
#define	RQ_MoveNorthEast				2 
#define	RQ_MoveEast						3 
#define	RQ_MoveSouthEast				4 
#define	RQ_MoveSouth					5 
#define	RQ_MoveSouthWest				6 
#define	RQ_MoveWest						7 
#define RQ_MoveNorthWest				8 
#define RQ_GetPlayerPos					9 
#define RQ_Ack						   	10 
#define RQ_GetObject					   11 
#define RQ_DepositObject				12 
#define RQ_PutPlayerInGame				13 
#define RQ_RegisterAccount				14 
#define RQ_DeletePlayer					15 
#define RQ_SendPeriphericObjects		16 
#define RQ_GodCreateObject				17 
#define RQ_QueryPatchServerInfo2    18
#define RQ_ViewEquiped					19 
#define RQ_ExitGame						20 
#define RQ_EquipObject					21 
#define RQ_UnequipObject				22 
#define RQ_UseObject					   23 
#define RQ_Attack						   24 
#define RQ_CreatePlayer					25 
#define RQ_GetPersonnalPClist			26 
#define RQ_IndirectTalk					27 
#define RQ_Shout					   	28 
#define RQ_Page							29 
#define RQ_DirectedTalk					30 
#define RQ_Reroll					   	31 
#define RQ_CastSpell				   	32 
#define RQ_HPchanged				   	33 
#define RQ_BroadcastTextChange	   34 
#define RQ_GetUnitName					35
#define RQ_BreakConversation			36
#define RQ_LevelUp						37
#define RQ_ReturnToMenu					38
#define RQ_GetSkillList					39
#define RQ_SendTrainSkillList			40
#define RQ_SendBuyItemList				41
#define RQ_UseSkill						42
#define RQ_GetStatus				   	43
#define RQ_XPchanged				   	44
#define RQ_GetTime						45
#define RQ_FromPreInGameToInGame		46
#define RQ_YouDied						47
#define RQ_AddRemoveChatterChannel		48
#define RQ_SendChatterChannelMessage	49
#define RQ_DirectedTalkNoFeed				50 
#define RQ_GetChatterUserList2	      51

#define RQ_GetSkillStatPoints			52
#define RQ_GoldChange					53
#define RQ_ViewGroundItemIndentContent	54
#define RQ_SendTeachSkillList			55
#define RQ_SendSellItemList				56
#define RQ_TeleportPlayer				57
#define RQ_SendStatTrain				58
#define RQ_QueryItemName				59
#define RQ_GetNearItems					60
#define RQ_PlayerFastMode				61
#define RQ_SendSpellList				62
#define RQ_ServerMessage				63
#define RQ_SpellEffect                  64
#define RQ_QueryServerVersion           65
#define RQ_MessageOfDay                 66
#define RQ_ManaChanged                  67
#define RQ_PuppetInformation            68
#define RQ_UnitUpdate                   69
#define RQ_MissingUnit                  70
#define RQ_UseItemByAppearance          72
#define RQ_RemoveFromChatterChannel     74
#define RQ_GetChatterChannelList        75
#define RQ_GroupInvite                  78
#define RQ_GroupJoin                    79
#define RQ_GroupLeave                   80
#define RQ_GroupKick                    81
#define RQ_CreateEffectStatus           83
#define RQ_DispellEffectStatus          84
#define RQ_JunkItems                    85
#define RQ_ToggleChatterListening       86
#define RQ_UpdateGroupMemberHp          87
#define RQ_GroupToggleAutoSplit         88
#define RQ_TogglePage                   89
#define RQ_QueryNameExistence           90
//#define RQ_QueryPatchServerInfo         91 //obsolete after build 1.71 050
#define RQ_UpdateWeight                 92
#define RQ_Rob                          93
#define RQ_DispellRob                   94
#define RQ_ArrowMiss                    95
#define RQ_ArrowHit                     96
#define RQ_GodFlagUpdate                97
#define RQ_SeraphArrival                98
#define RQ_AuthenticateServerVersion    99
#define RQ_Remort                       100
#define RQ_ExitGameAntiPlug				101 
#define RQ_InfoMessage                  102
#define RQ_MaxCharactersPerAccountInfo  103
#define RQ_OpenURL						    105
// Chest Packets
#define RQ_ChestContents				    106
#define RQ_ChestAddItemFromBackpack		 107
#define RQ_ChestRemoveItemToBackpack	 108
#define RQ_ShowChest				     	    109
#define RQ_HideChest					       110
// Trade Packets
#define RQ_TradeContents				   111
#define RQ_TradeAddItemFromBackpack		112
#define RQ_TradeRemoveItemToBackpack	113
#define RQ_TradeInvite					116
#define RQ_TradeSetStatus				117
#define RQ_TradeCancel					118 
#define RQ_TradeClear					119
#define RQ_TradeStarted					120
#define RQ_TradeFinish					121
#define RQ_QueryItemInfo   			122
#define RQ_UpdateBackpackAfterUse   123
#define RQ_DamageUnit               124
#define RQ_HealingUnit              125

#define RQ_NM_GetGuildList    		126
#define RQ_NM_GuildInvite           127
#define RQ_NM_GuildJoin             128
#define RQ_NM_GuildLeave            129
#define RQ_NM_GuildKick             130
#define RQ_NM_GuildChangeSetting    131
#define RQ_NM_GuildChangeNote       132
#define RQ_NM_GuildGetLogs          133
#define RQ_NM_GuildChestShow        134
#define RQ_NM_GuildChestHide        135
#define RQ_NM_GuildChestContents	   136
#define RQ_NM_GUILDChestAddItem     137
#define RQ_NM_GUILDChestRemoveItem  138

#define RQ_NM_GetAHList             140
#define RQ_NM_AddAHItems            141
#define RQ_NM_BuyAHItems            142
#define RQ_NM_CancelAHItems         143
#define RQ_NM_InfoAHItems           144

#define RQ_ChestNormal				   148
#define RQ_ChestListe				   149
#define RQ_WeatherMsg				   150
#define RQ_GetStatus2 					151
#define RQ_GetUnitName2					152
#define RQ_GetPersonnalPClistEquitSkin 153
#define RQ_ViewBackpack2				154  
#define RQ_UseObject2					155 
#define RQ_ViewInv                  156
#define RQ_AskCompagnonName			157
#define RQ_1stApril                 158
#define RQ_UpdateSmile              159

#define RQ_NM_NMSGOLD_AchatOpt1     160
#define RQ_NM_NMSGOLD_AchatOpt2     161
#define RQ_NM_NMSGOLD_AchatOpt3     162
#define RQ_NM_NMSGOLD_AchatOpt4     163
#define RQ_NM_NMSGOLD_Acheter       164

#define RQ_NM_NMSGOLD_ListPanier     165
#define RQ_NM_NMSGOLD_UtiliserPanier 166
#define RQ_NM_NMSGOLD_CrediterPanier 167
#define RQ_NM_NMSGOLD_Sanction       168

#define RQ_RPStatus                 169
#define RQ_RP_BroadCastRP           170
#define RQ_RP_CreerRP               171
#define RQ_RP_TerminerRP            172
#define RQ_RP_RejoindreRP           173
#define RQ_RP_RejoindreAnswerRP     174
#define RQ_RP_ExpluserRP            175
#define RQ_RP_InviteRP              176
#define RQ_RP_InviteAnswerRP        177
#define RQ_RP_SignalerRP            178
//#define RQ_RP_EchangerRP            179 //obselete
#define RQ_RP_BroadCastPVP          180
#define RQ_RP_BroadCastPVPStat      181

#define RQ_QB_GetQuestList          182
#define RQ_QB_GetQuestMsg           183
#define RQ_QB_GetActiveQuest        184
#define RQ_QB_GetQuestListComplete  185
#define RQ_QB_StopQuest             208


#define RQ_GMMSG_Post               186
#define RQ_GMMSG_Get                187
#define RQ_GMMSG_Close              188


#define RQ_GetAllArealinkForWorld   189
#define RQ_GetAllPlayerPos				190
#define RQ_SvrOptions               191
#define RQ_SvrNPC                   192
#define RQ_SvrSpellList             193
#define RQ_SvrMonsterList           194
#define RQ_SvrItemsList             195
#define RQ_SvrMonsterSkin           196
#define RQ_SvrQuestFlagList    		197
#define RQ_AttackMode					198
#define RQ_NameChange					199
#define RQ_NM_DeathStatus     		200
#define RQ_NM_DeathProgress   		201
#define RQ_NM_DeathResurect   		202
#define RQ_NM_GetProfession   		203
#define RQ_NM_SendTeachFormuleList  204
#define RQ_NM_SendMakeFormule       205
#define RQ_NM_XPScrollProgress      206
#define RQ_NM_ORScrollProgress      207

//                                  208 //utiliser avec les quest book
#define RQ_SendPointsItemList       209
#define RQ_ChestNewContents			220 //RQ_ChestContents
#define RQ_ShowChestNew				   221 //RQ_ShowChest
#define RQ_HideChestNew	   		   222 //RQ_HideChest

#define RQ_GuildChestNewShow        223 //RQ_NM_GuildChestShow
#define RQ_GuildChestNewHide        224 //RQ_NM_GuildChestHide
#define RQ_GuildChestNewContents  	225 //RQ_NM_GuildChestContents

#define RQ_ARENA_SendTakeStatus     226
#define RQ_ARENA1_Join              227//OK2
#define RQ_ARENA1_Leave             228//OK2
#define RQ_ARENA1_UpdateTimeBS      229//OK2
#define RQ_ARENA1_GetWaitPlayerList 231//OK2
#define RQ_ARENA1_UpdatePlayStat    232

#define RQ_GetEventsList            233
#define RQ_UpdateFactionID          234
#define RQ_GetObjectLinkedID	   	235



enum ePlacesToSearch { PL_SEARCHBACKPACK=0, PL_SEARCHCHEST=1, PL_SEARCHMYTRADE=2, PL_SEARCHOTHERTRADE=3, PL_SEARCHGUILDCHEST=4};


#endif