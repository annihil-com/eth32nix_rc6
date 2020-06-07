// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CEspWindow::CEspWindow(char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = false;
	show = eth32.settings.xhairInfo;
	barSize = 6 * eth32.cg.screenYScale;
	barPadding = 4 * eth32.cg.screenYScale;

	lxOffset = (ww >> 1) * eth32.cg.screenXScale;
	lx = x + lxOffset;

	// temp hack to size window right
	Resize(w, (10 * eth32.cg.screenYScale) + (GUI_FONTHEIGHT * 7) + (GUI_TEXTSPACING * 9));

	timeStart = 0;
	displayClientNum = 0;
}

void CEspWindow::Display(void)
{
	if (!eth32.settings.xhairInfo)
		return;

	uint32 u = Time();

	if (eth32.cg.xhairClientNum > -1) {
		show = true;
		displayClientNum = eth32.cg.xhairClientNum;
		timeStart = u;
	}
	else if (displayClientNum > -1 && (u - timeStart) < eth32.settings.xhairInfoTime) {
		show = true;
	}
	else {
		show = false;
		timeStart = 0;
	}

	if (show) {
		memset(name, 0, sizeof(name));
		
		CWindow::Display();

		player_t *player = &eth32.cg.players[displayClientNum];

		int cnt = Draw.BreakStringByWidth(player->name, w-10, NULL, GUI_FONT, false);
		if (cnt > 0 && cnt < sizeof(name))
			strncpy(name, player->name, cnt);
		else
			return;

		int drawX = x + 5 * eth32.cg.screenXScale;
		int drawY = y + 5 * eth32.cg.screenYScale;
		int drawX2 = x2 - 5 * eth32.cg.screenXScale;

		char p[16];

		Draw.TextCenter(lx, drawY, GUI_FONTSCALE, name, colorWhite, qfalse, qfalse, GUI_FONT, true);
		
		drawY += GUI_FONTHEIGHT + barPadding;

		if (IS_SPECTATOR(eth32.cg.clientNum) || IS_FRIENDLY(displayClientNum)) {
			float healthFrac = *player->health / 100.f;
			if (healthFrac > 1.0)
				healthFrac = 1.0;

			vec4_t healthColor;
			Draw.ColorForHealth(*player->health, healthColor);
			Draw.Statbar(x+GUI_SPACING, drawY, w-GUI_SPACING-GUI_SPACING, barSize, healthFrac, healthColor);
		}

		drawY += barSize + barPadding;

		sprintf(p, "%.2f", player->stats.kdRatio);
		Draw.Text(drawX, drawY, GUI_FONTSCALE, "K/D Ratio", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		//p = va("%i", player->stats.kills);
		//Draw.Text(drawX, drawY, GUI_FONTSCALE, "Kills", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		//Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		//drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		//p = va("%i", player->stats.deaths);
		//Draw.Text(drawX, drawY, GUI_FONTSCALE, "Deaths", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		//Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		//drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;
		sprintf(p, "%i", player->stats.suicides);
		Draw.Text(drawX, drawY, GUI_FONTSCALE, "Suicides", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		sprintf(p, "%i", player->stats.spree);
		Draw.Text(drawX, drawY, GUI_FONTSCALE, "Spree", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		if (player->stats.bulletsFired)
			sprintf(p, "%.1f", ((float)player->stats.bulletsHit/player->stats.bulletsFired)*100.f);
		else strcpy(p, "0.0");
		Draw.Text(drawX, drawY, GUI_FONTSCALE, "Accuracy", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

		drawY += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		sprintf(p, "%.1f", player->stats.threat);
		Draw.Text(drawX, drawY, GUI_FONTSCALE, "Threat", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(drawX2 - Draw.sizeTextWidth(GUI_FONTSCALE, p, GUI_FONT), drawY, GUI_FONTSCALE, p, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	}
}
		


		
