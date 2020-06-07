// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

bool CDraw::CheckPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader)
{
	this->adjustTo640(&x, &y, &w, &h);

	if (hShader == eth32.cg.media.white) {

		if (eth32.cgMod->type == MOD_TCE && (int) w == 646)//Flash Removal
			return false;
		// sniper blackout
		if (!eth32.settings.blackout && (int) y == 0 && (int) w == 80)
			return false;
	}

	if (!eth32.settings.blackout && (hShader == eth32.cg.media.binoc || hShader == eth32.cg.media.reticleSimple))
		return false;

	this->adjustFrom640(&x, &y, &w, &h);

	return true;
}

void CDraw::drawMortarTrace(player_t *player)
{
	bool once = false;

	vec3_t nextPos, lastPos;
	vec3_t angles, forward;

	AxisToAngles(player->orHead.axis, angles);

	angles[PITCH] -= 60.f;
	AngleVectors(angles, forward, NULL, NULL);
	forward[0] *= 3000 * 1.1f;
	forward[1] *= 3000 * 1.1f;
	forward[2] *= 1500 * 1.1f;

	trajectory_t mortarTrajectory;
	mortarTrajectory.trType = TR_GRAVITY;
	mortarTrajectory.trTime = eth32.cg.time;
	VectorCopy(player->lerpOrigin, mortarTrajectory.trBase);
	VectorCopy(forward, mortarTrajectory.trDelta);

	// Calculate mortar impact
	int timeOffset = 0;
	trace_t mortarTrace;
	vec3_t mortarImpact;
	VectorCopy(mortarTrajectory.trBase, mortarImpact);
	#define TIME_STEP 20
	while (timeOffset < 10000) {
		timeOffset += TIME_STEP;
		BG_EvaluateTrajectory(&mortarTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&mortarTrace, mortarImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		if ((mortarTrace.fraction != 1)
				// Stop if we hit sky
				&& !((mortarTrace.surfaceFlags & SURF_NODRAW) || (mortarTrace.surfaceFlags & SURF_NOIMPACT))
				&& (mortarTrace.contents != 0)) {
			break;
		}
		if(!once)
		{
			VectorCopy(mortarTrajectory.trBase, lastPos);
			once = true;
		}

		Engine.MakeRailTrail( lastPos, nextPos, false, colorYellow, 350 ); // buffalo: colorYellow for now till I add in controls
		VectorCopy(nextPos, lastPos);
	}
}
