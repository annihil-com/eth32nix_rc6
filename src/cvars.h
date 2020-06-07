// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

// list of cvars we want to set and restore on hook / unhook

static cvarInfo_t cvarList[] = {
//	 name						ourValue
	{ "cg_drawcompass",			"0", "1", },
	{ "cg_drawcrosshairnames",	"0", "1", },
	{ "cg_teamchatheight",		"0", "8", },
};

// list of cvars that need to be set on clean screenshot
static cvarInfo_t cleanCvarList[] = {
	{ "cg_drawcompass",			"", "1", },
	{ "cg_drawcrosshairnames",	"", "1", },
	{ "cg_teamchatheight",		"", "8", },
	{ "r_shownormals",		"", "0", },	
	{ "cg_thirdperson",		"", "0", },	
	{ "cg_drawgun",		"", "1", },
	{ "r_wolffog",		"", "1", },	
	{ "r_drawfoliage",		"", "1", },		
};
