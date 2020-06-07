// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CAimbot.h"
#include "CIrc.h"

#define SETFLOAT(x)			((int)(x * 1000))		// only keeping up to 0.001
#define GETFLOAT(x)			(x / 1000.f)

// Control definition reference: (update this if you add new control types)

// Type					Arg0			Arg1			Arg2			Arg3			Arg4
// CTRL_CHECKBOX		bool *target	N/A				N/A				N/A				N/A
// CTRL_INTDISPLAY		int *target		N/A				N/A				N/A				N/A
// CTRL_INTSLIDER		int	min			int	max			int *target		N/A				N/A
// CTRL_FLOATSLIDER		float min		float max		float *target	N/A				N/A
// CTRL_COLORPICKER		BYTE *color		N/A				N/A				N/A				N/A

// NOTE: cast arg0-arg4 to int if used, use SETFLOAT/GETFLOAT for floats

static const windef_t windows[] =
{
	// MAINVIEW
	{
		"Status",				// title
		WIN_STATUS,				// type
		GUI_MAINVIEW,			// view
		5, 422, 150, 53,		// x, y, w, h
		0,						// num controls
	},
	{
		"Weapon",				// title
		WIN_WEAPON,				// type
		GUI_MAINVIEW,			// view
		6, 365, 150, 53,		// x, y, w, h
		0,						// num controls
	},
	/*{ // sol: a window I use for setting weapon rendering parms *** DO NOT REMOVE ***
		"Weap Rendering",		// title
		WIN_STANDARD,			// type
		GUI_MAINVIEW,			// view
		160, 200, 150, 200,		// x, y, w, h
		7,						// num controls
		{
			// Type				Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_FLOATSLIDER,	"X",				5,		5,		140,	20,		SETFLOAT(-10), SETFLOAT(10), (int)&eth32.settings.weapOrigin[0] },
			{ CTRL_FLOATSLIDER,	"Y",				5,		30,		140,	20,		SETFLOAT(-10), SETFLOAT(10), (int)&eth32.settings.weapOrigin[1] },
			{ CTRL_FLOATSLIDER,	"Z",				5,		55,		140,	20,		SETFLOAT(-10), SETFLOAT(10), (int)&eth32.settings.weapOrigin[2] },
			{ CTRL_FLOATSLIDER,	"Pitch",			5,		80,		140,	20,		SETFLOAT(0), SETFLOAT(360), (int)&eth32.settings.weapAngle[0] },
			{ CTRL_FLOATSLIDER,	"Yaw",				5,		105,	140,	20,		SETFLOAT(0), SETFLOAT(360), (int)&eth32.settings.weapAngle[1] },
			{ CTRL_FLOATSLIDER,	"Roll",				5,		130,	140,	20,		SETFLOAT(0), SETFLOAT(360), (int)&eth32.settings.weapAngle[2] },
			{ CTRL_FLOATSLIDER,	"Dist",				5,		155,	140,	20,		SETFLOAT(0), SETFLOAT(120), (int)&eth32.settings.weapDist },
		},
	},*/
	{
		"mainchat",				// title
		WIN_CHAT,				// type
		GUI_MAINVIEW,			// view
		289, 379, 350, 100,		// x, y, w, h
		0,						// num controls
	},
	{
		"XhairInfo",			// title
		WIN_XHAIRINFO,			// type
		GUI_MAINVIEW,			// view
		385, 193, 100, 113,		// x, y, w, h
		0,						// num controls
	},
	{
		"Spectators",			// title
  		WIN_SPECTATOR,			// type
		GUI_MAINVIEW,			// view
		50, 193, 100, 40,		// x, y, w, h
		0,						// num controls
	},
	{
		"Respawn",				// title
		WIN_RESPAWN,			// type
		GUI_MAINVIEW,			// view
		280, 5, 80, 20,			// x, y, w, h
		0,						// num controls
	},
	{
		"Grenade",				// title
		WIN_BALLISTIC,			// type
		GUI_MAINVIEW,			// view
		280, 5, 140, 50,		// x, y, w, h
		0,						// num controls
	},
	{
		"Radar",				// title
		WIN_RADAR,				// type
		GUI_MAINVIEW,			// view
		515, 5, 120, 120,		// x, y, w, h
		0,						// num controls
	},
	{
		"Cameras",				// title
		WIN_CAMERA,				// type
		GUI_MAINVIEW,			// view
		5, 5, 5, 5,				// x, y, w, h : these don't matter though
		0,						// num controls
	},
	// MENUVIEW
	{
		"Aimbot",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 58, 150, 393,		// x, y, w, h
		16,						// num controls
		{
			// Type				Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Aim",				5,		5,		140,	23,		AIM_OFF, AIM_MAX-1, (int)&eth32.settings.aimType, (int)aimTypeText },
			{ CTRL_CHECKBOX,	"Autofire",			5,		33,		140,	8,		(int)&eth32.settings.autofire },
			{ CTRL_CHECKBOX,	"Validate Attack",	5,		46,		140,	8,		(int)&eth32.settings.atkValidate },
			{ CTRL_FLOATSLIDER, "Fov",				5,		59,		140,	20,		SETFLOAT(0.0), SETFLOAT(360.0), (int)&eth32.settings.fov },
			{ CTRL_DROPBOX,		"Target Sort",		5,		84,		140,	23,		SORT_OFF, SORT_MAX-1, (int)&eth32.settings.aimSort, (int)sortTypeText },
			{ CTRL_DROPBOX,		"Aim Priority",		5,		112,	140,	23,		0, AP_MAX-1, (int)&eth32.settings.headbody, (int)priorityTypeText },
			{ CTRL_CHECKBOX,	"Target Lock",		5,		140,	140,	8,		(int)&eth32.settings.lockTarget },
			{ CTRL_DROPBOX,		"Hitbox Style",		5,		153,	140,	23,		HITBOX_OFF, HITBOX_MAX-1, (int)&eth32.settings.hitboxType, (int)hitboxText },
			{ CTRL_DROPBOX,		"Trace Style",		5,		181,	140,	23,		TRACE_CENTER, TRACE_MAX-1, (int)&eth32.settings.traceType, (int)traceTypeText },
			{ CTRL_DROPBOX,		"Self Predict",		5,		209,	140,	23,		SPR_OFF, SPR_MAX-1, (int)&eth32.settings.predSelfType, (int)selfPredictText },
			{ CTRL_FLOATSLIDER,	"Self Predict",		5,		237,	140,	20,		SETFLOAT(-0.10), SETFLOAT(0.10), (int)&eth32.settings.predSelf },
			{ CTRL_FLOATSLIDER,	"Target Predict",	5,		262,	140,	20,		SETFLOAT(-0.10), SETFLOAT(0.10), (int)&eth32.settings.predTarget },
			{ CTRL_FLOATSLIDER, "Animation Correction",5,	287,	140,	20,		SETFLOAT(-10.0), SETFLOAT(10.0), (int)&eth32.settings.animCorrection },
			{ CTRL_FLOATSLIDER, "Dynamic Hitbox",   5,      312,    140,    20,     SETFLOAT(0.0), SETFLOAT(10.0), (int)&eth32.settings.dynamicHitboxScale },
			{ CTRL_DROPBOX,		"Cooperative Mode", 5,		337,	140,	23,		0, COOP_MAX-1, (int)&eth32.settings.portalCoopMode, (int)coopTypeText },
			{ CTRL_CHECKBOX,	"Auto Crouch",		5,		365,	140,	8,		(int)&eth32.settings.autoCrouch },
		},
	},
	{
		"Aimbot Master",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 58, 150, 35,			// x, y, w, h
		1,						// num controls
		{
			{ CTRL_DROPBOX,		"Aimbot Mode", 		5, 5,	140,	23,		0, AIMMODE_MAX-1, (int)&eth32.settings.aimMode, (int)aimModeText },
		},
	},	
	{
		"Chams",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		376, 49, 150, 89,		// x, y, w, h
		3,						// num controls
		{
			// Type				Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Players",		5,		5,		140,	23,		0, MAX_CHAMS-1, (int)&eth32.settings.chamsPlayer, (int)chamsText },
			{ CTRL_DROPBOX,		"Weapons",		5,		33,		140,	23,		0, MAX_CHAMS-1, (int)&eth32.settings.chamsWeapon, (int)chamsText },
			{ CTRL_CHECKBOX,	"My Weapon",	5,		61,		140,	8,		(int)&eth32.settings.chamsOurWeapon },
		}
	},
	{
		"Visuals",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		60, 46, 150, 230,		// x, y, w, h
		13,						// num controls
		{
			// Type				Label						X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Wallhack",					5,		5,		140,	8,		(int)&eth32.settings.wallhack },
			{ CTRL_CHECKBOX,	"do distance wallhack",		5,		18,		140,	8,		(int)&eth32.settings.doDistWallhack },
			{ CTRL_FLOATSLIDER,	"Wallhack Distance",		5,		31,		140,	20,		SETFLOAT(0.0), SETFLOAT(10000), (int)&eth32.settings.wallhackDistance },
			{ CTRL_CHECKBOX,	"Draw Blackout",			5,		55,		140,	8,		(int)&eth32.settings.blackout },
			{ CTRL_CHECKBOX,	"Weapon Zoom",				5,		68,		140,	8,		(int)&eth32.settings.weaponZoom },
			{ CTRL_FLOATSLIDER,	"Scoped Turning",			5,		81,		140,	20,		SETFLOAT(0.1), SETFLOAT(1.0), (int)&eth32.settings.scopedTurnSpeed },
			{ CTRL_INTSLIDER,	"Smoke Visibility",			5,		106,	140,	20,		0, 100, (int)&eth32.settings.smoketrnsp },
			{ CTRL_FLOATSLIDER,	"Radar Range",				5,		131,	140,	20,		SETFLOAT(100.0), SETFLOAT(10000.0), (int)&eth32.settings.radarRange },
			{ CTRL_CHECKBOX,	"Disguised ESP",			5,		156,	140,	8,		(int)&eth32.settings.drawDisguised },
			{ CTRL_CHECKBOX,	"Missile Blast Light",		5,		169,	140,	8,		(int)&eth32.settings.grenadeDlight },
			{ CTRL_CHECKBOX,	"Mortar Blast Light",		5,		182,	140,	8,		(int)&eth32.settings.mortarDlight },
			{ CTRL_CHECKBOX,	"Mortar Trace and Esp",		5,		195,	140,	8,		(int)&eth32.settings.mortarTrace },
			{ CTRL_CHECKBOX,	"Arty Markers",				5,		208,	140,	8,		(int)&eth32.settings.artyMarkers },
			
		},
	},
	{
		"Hitbox Display",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		377, 151, 150, 94,		// x, y, w, h
		5,						// num controls
		{
			// Type				Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Head",				5,		5,		140,	8,		(int)&eth32.settings.drawHeadHitbox },
			{ CTRL_CHECKBOX,	"Head Axes",		5,		18,		140,	8,		(int)&eth32.settings.drawHeadAxes },
			{ CTRL_CHECKBOX,	"Body",				5,		31,		140,	8,		(int)&eth32.settings.drawBodyHitbox },
			{ CTRL_CHECKBOX,	"Bullet Rail",		5,		44,		140,	8,		(int)&eth32.settings.drawBulletRail },
			{ CTRL_CHECKBOX,	"Rail Wallhack",	5,		70,		140,	8,		(int)&eth32.settings.railWallhack }
		},
	},
	{
		"Colors",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		109, 208, 260, 90,		// x, y, w, h
		1,						// num controls
		{
			// Type				Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_COLORPICKER, "Picker",			5,		5,		250,	65 },
		},
	},
	{
		"Esp",					// title
		WIN_ESPCONFIG,			// type
		GUI_MENUVIEW,			// view
		442, 47, 150, 230,		// x, y, w, h
		0,						// num controls
	},
	{
		"Weapon Config",		// title
		WIN_WEAPCONFIG,				// type
		GUI_MENUVIEW,			// view
		201, 58, 150, 225,		// x, y, w, h
		0,						// num controls
	},
	{
		"Custom Hitbox",		// title
		WIN_HITBOX,				// type
		GUI_MENUVIEW,			// view
		395, 59, 200, 195,		// x, y, w, h
		0,						// num controls
	},
	{
		"Camera Settings",
		WIN_CAMCONFIG,
		GUI_MENUVIEW,
		218, 48, 150, 150,
		0,
	},
	{
		"IRC",					// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		311, 44, 150, 89,		// x, y, w, h
		3,						// num controls
		{
			{ CTRL_DROPBOX,		"Chat Routing",		5,		5,		140,	23,		0, IRC_ROUTING_MAX-1, (int)&eth32.settings.ircChatMsgs, (int)ircRoutingText },
			{ CTRL_DROPBOX,		"Console Routing",	5,		33,		140,	23,		0, IRC_ROUTING_MAX-1, (int)&eth32.settings.ircConsoleMsgs, (int)ircRoutingText },
			{ CTRL_CHECKBOX,	"Mention Notify",	5,		61,		140,	8,		(int)&eth32.settings.ircMention },
		},
	},
	{
		"Portal",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		154, 45, 150, 71,		// x, y, w, h
		3,						// num controls
		{
			{ CTRL_CHECKBOX,	"Anonymous",		5,		5,		140,	8,		(int)&eth32.settings.portalAnon },
			{ CTRL_CHECKBOX,	"Rampage Notify",	5,		18,		140,	8,		(int)&eth32.settings.portalRampageNotify },
			{ CTRL_INTSLIDER,	"Rampage Minimum",	5,		31,		140,	20,		4, 32, (int)&eth32.settings.portalRampageNum },
		},
	},
	{
		"Misc",					// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		119, 47, 150, 236,		// x, y, w, h
		11,						// num controls
		{
			{ CTRL_CHECKBOX,	"Name Click",		5,		5,		140,	8,		(int)&eth32.settings.nameClick },
			//{ CTRL_CHECKBOX,	"Kill Spam",		5,		18,		140,	8,		(int)&eth32.settings.killSpam },
			{ CTRL_CHECKBOX,	"Spec list", 		5, 		31, 	140, 	8, 		(int)&eth32.settings.getSpeclist },
			{ CTRL_CHECKBOX,	"Third Person",		5,		44,		140,	8,		(int)&eth32.settings.thirdPerson },
			{ CTRL_CHECKBOX,	"Transparent Console",		5,		57,		140,	8,		(int)&eth32.settings.transparantConsole },
			{ CTRL_CHECKBOX,	"Respawn Timers",	5,		70,		140,	8,		(int)&eth32.settings.respawnTimers },
			{ CTRL_CHECKBOX,	"Auto Tapout",		5,		83,		140,	8,		(int)&eth32.settings.autoTapout },
			{ CTRL_DROPBOX,		"Auto Push",		5,		96,		140,	23,		0, MAX_AUTOPUSH-1, (int)&eth32.settings.autoPush, (int)autopushText },
			{ CTRL_FLOATSLIDER,	"Auto Push Distance",5,		124,	140,	20,		SETFLOAT(0.0), SETFLOAT(256.0), (int)&eth32.settings.autoPushDistance },
			{ CTRL_DROPBOX,		"PB Screenshot",	5,		149,	140,	23,		0, PB_SS_MAX-1, (int)&eth32.settings.pbScreenShot, (int)pbssText },
			{ CTRL_CHECKBOX,	"Panzer Suicide",	5,		176,	140,	8,		(int)&eth32.settings.panzerSuicide },
			{ CTRL_FLOATSLIDER,	"Impact Distance",	5,		189,	140,	20,		SETFLOAT(40.0), SETFLOAT(500.0), (int)&eth32.settings.panzerImpactDist },			
		},
	},
	{
		"Gui Options",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		319, 47, 150, 132,		// x, y, w, h
		7,						// num controls
		{
			{ CTRL_CHECKBOX,	"Chatbox",				5,		5,		140,	8,		(int)&eth32.settings.guiChat },
			{ CTRL_CHECKBOX,	"Centerprint",			5,		18,		140,	8,		(int)&eth32.settings.guiCenterPrint },
			{ CTRL_CHECKBOX,	"Centerprint animate", 	5, 		31, 	140, 	8, 		(int)&eth32.settings.guiCenterPrintAnim },
			{ CTRL_FLOATSLIDER,	"Centerprint size",		5,		44,		140,	20,		SETFLOAT(0.0), SETFLOAT(3.0), (int)&eth32.settings.guiCenterPrintScale },
			{ CTRL_CHECKBOX,	"Banner",				5,		69,		140,	8,		(int)&eth32.settings.guiBanner },
			{ CTRL_FLOATSLIDER,	"Banner size",			5,		82,		140,	20,		SETFLOAT(0.0), SETFLOAT(3.0), (int)&eth32.settings.BannerScale },
			{ CTRL_CHECKBOX,	"Original hud",			5,		107,	140,	8,		(int)&eth32.settings.guiOriginal },		
		},
	},	
	{
		"Sound",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		119, 280, 150, 75,		// x, y, w, h
		5,						// num controls
		{
			{ CTRL_CHECKBOX,	"Kill Sounds",		5,		5,		140,	8,		(int)&eth32.settings.killSounds },
			{ CTRL_CHECKBOX,	"Hit Sounds",		5,		18,		140,	8,		(int)&eth32.settings.hitsounds },
			{ CTRL_CHECKBOX,	"Custom Sounds",	5,		31,		140,	8,		(int)&eth32.settings.customSounds },
			{ CTRL_CHECKBOX,	"Pure only",		5,		44,		140,	8,		(int)&eth32.settings.pureSounds },
			{ CTRL_CHECKBOX,	"HQ sounds",		5,		57,		140,	8,		(int)&eth32.settings.hqSounds },
		},
	},	
	// USERSVIEW
	{
		"ETH32 Users",			// title
		WIN_USERS,				// type
		GUI_USERVIEW,			// view
		5, 5, 630, 470,			// x, y, w, h
		0,						// num controls
	},
	// CLIENTVIEW
	{
		"Clients",				// title
		WIN_CLIENTS,			// type
		GUI_CLIENTVIEW,			// view
		5, 5, 630, 470,			// x, y, w, h
		0,						// num controls
	},
	// IRCVIEW
	{
		"IRC Frontend",			// title
		WIN_IRC,				// type
		GUI_IRCVIEW,			// view
		5, 5, 630, 470,			// x, y, w, h
		0,						// num controls
	},
	// BALLISTICBOT
	{
		"Grenadebot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		325, 58, 150, 283,		// x, y, w, h
		15,						// num controls
		{
			// Type				Label					X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Grenade   Aimbot",		5,		5,		140,	8,		(int)&eth32.settings.grenadeBot },
			{ CTRL_CHECKBOX,	"Riflenade Aimbot",		5,		18,		140,	8,		(int)&eth32.settings.rifleBot },
			{ CTRL_CHECKBOX,	"Block fire",			5,		31,		140,	8,		(int)&eth32.settings.grenadeBlockFire },
			{ CTRL_CHECKBOX,	"Grenade Trajectory",	5,		44,		140,	8,		(int)&eth32.settings.valGrenTrajectory },
			{ CTRL_CHECKBOX,	"Rifle Trajectory",		5,		57,		140,	8,		(int)&eth32.settings.valRifleTrajectory },
			{ CTRL_CHECKBOX,	"Grenade Senslock",		5,		70,		140,	8,		(int)&eth32.settings.grenadeSenslock },
			{ CTRL_FLOATSLIDER,	"Rifle Z correction",	5,		93,		140,	20,		SETFLOAT(-50.0), SETFLOAT(50.0), (int)&eth32.settings.riflenadeZ },
			{ CTRL_FLOATSLIDER,	"Grenade Z correction",	5,		118,	140,	20,		SETFLOAT(-50.0), SETFLOAT(50.0), (int)&eth32.settings.grenadeZ },
			{ CTRL_INTSLIDER,	"Grenade Fire Delay",	5,		142,	140,	20,		0, 1000, (int)&eth32.settings.grenadeFireDelay },
			{ CTRL_CHECKBOX,	"Grenade Autofire",		5,		165,	140,	8,		(int)&eth32.settings.grenadeAutoFire },
			{ CTRL_CHECKBOX,	"Riflenade Autofire",	5,		178,	140,	8,		(int)&eth32.settings.rifleAutoFire },
			{ CTRL_DROPBOX,		"Target Predict",		5,		191,	140,	23,		0, RF_PREDICT_MAX-1, (int)&eth32.settings.ballisticPredict, (int)riflePredictText},
			{ CTRL_CHECKBOX,	"Check Radius Damage",	5,		220,	140,	8,		(int)&eth32.settings.ballisticRadiusDamage },
			{ CTRL_FLOATSLIDER,	"Blast radius",			5,		233,	140,	20,		SETFLOAT(30.0), SETFLOAT(1000.0), (int)&eth32.settings.radiusDamage },
			{ CTRL_CHECKBOX,	"Auto Targets",			5,		258,	140,	8,		(int)&eth32.settings.autoGrenTargets },
			{ CTRL_CHECKBOX,	"Multi Bounce",			5,		271,	140,	8,		(int)&eth32.settings.allowMultiBounce },
		}
	},
	{
		"Namestealer",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		325, 58, 150, 115,		// x, y, w, h
		4,						// num controls
		{
			// Type				Label					X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Name steal",			5,		5,		140,	8,		(int)&eth32.settings.doNamesteal },
			{ CTRL_INTSLIDER,	"Delay"		,			5,		18,		140,	20,		0, 20000, (int)&eth32.settings.NamestealDelay },
			{ CTRL_INTSLIDER,	"Init Grace",			5,		41,		140,	20,		0, 20000, (int)&eth32.settings.NamestealGrace },
			{ CTRL_DROPBOX,		"steal type",			5,		64,		140,	23,		0, NAMESTEAL_MAX-1, (int)&eth32.settings.NamestealMode, (int)namestealText },
		}
	},
	{
		"Spam",					// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		325, 200, 150, 115,		// x, y, w, h
		6,						// num controls
		{
			// Type				Label					X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Kill Spam",			5,		5,		140,	8,		(int)&eth32.settings.killSpam },
			{ CTRL_CHECKBOX,	"Multikill Spam",		5,		18,		140,	8,		(int)&eth32.settings.multikillSpam },
			{ CTRL_CHECKBOX,	"Death Spam",			5,		31,		140,	8,		(int)&eth32.settings.deathSpam },
			{ CTRL_CHECKBOX,	"Suicide Spam",			5,		44,		140,	8,		(int)&eth32.settings.suicideSpam },
			{ CTRL_CHECKBOX,	"PrivateMsg killspam",	5,		57,		140,	8,		(int)&eth32.settings.pmKillspam },
			{ CTRL_DROPBOX,		"Killspam Type",		5,		70,		140,	23,		0, KILLSPAM_MAX-1, (int)&eth32.settings.KillspamType, (int)killspamText },
		}
	},		
	{
		"Banner",
		WIN_BANNER,				// type
		GUI_MAINVIEW,			// view
		20, 20, 1, 1,			// x, y, w, h
		0,						// num controls	
	},	
};

static const assetdef_t assetDefs[] =
{
//	  Key				Type					Target
	{ "titlecolor",		ASSET_VEC4,				(void*)eth32.guiAssets.titleColor },
	{ "textcolor1",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor1 },
	{ "textcolor2",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor2 },
	{ "titleleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleLeft },
	{ "titlecenter",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleCenter },
	{ "titleright",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleRight },
	{ "winleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winLeft },
	{ "wintop",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTop },
	{ "wintopl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTopLeft },
	{ "wincenter",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winCenter },
	{ "txtinputl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputLeft },
	{ "txtinputc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputCenter },
	{ "txtinputr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputRight },
	{ "btnl",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnLeft },
	{ "btnc",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnCenter },
	{ "btnr",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnRight },
	{ "btnsell",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselLeft },
	{ "btnselc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselCenter },
	{ "btnselr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselRight },
	{ "check",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.check },
	{ "checkbox",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.checkBox },
	{ "mouse",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.mousePtr },
	{ "dropboxarrow",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.dropboxArrow },
	{ "scrollarrow",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.scrollbarArrow },
	{ "scrollbtn",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.scrollbarBtn },
	{ "scrolltrack",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.scrollbarTrack },
	{ "sliderbtn",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderBtn },
	{ "slidertrack",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderTrack },
	{ "camcorner",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.camCorner },
};
