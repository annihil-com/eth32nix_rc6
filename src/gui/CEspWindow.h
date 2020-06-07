// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"

class CEspWindow : public CWindow
{
public:
	CEspWindow(char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	uint32 timeStart;
	int displayClientNum;
	char name[128];
	int barSize, barPadding;
};
