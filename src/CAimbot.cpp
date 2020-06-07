// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include <stdlib.h>

#ifndef min
#define min( x, y ) ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#endif
#ifndef max
#define max( x, y ) ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#endif

CAimbot Aimbot;

extern xcommand_t orig_cc_start_attack;
extern xcommand_t orig_cc_end_attack;
extern xcommand_t orig_cc_StartCrouch;
extern xcommand_t orig_cc_EndCrouch;
extern void SnapVectorTowards( vec3_t v, vec3_t to );

// There !!!HAS TO BE!!! a corresponding string for each select type (except SORT_MAX)
const char *sortTypeText[SORT_MAX] =
{
	"Off",
	"Distance",
	"Crosshair",
	"K/D ratio",
	"Accuracy",
	"Targeting us",
	"Threat"
};

const char *selfPredictText[SPR_MAX] =
{
	"Off",
	"Manual",
	"Simple",
	"Less Simple",
	"L337"
};

const char *autoPointText[AUTOPT_MAX] =
{
	"Manual",
	"Auto, low fps",
	"Auto, max perfomance"
};

const char *coopTypeText[COOP_MAX] =
{
	"Off",
	"Friends",
	"All Users",
};

const char *traceTypeText[TRACE_MAX] =
{
	"Center",
	"Random Volume",
	"Random Surface",
	"Capsule Volume",
	"Capsule Surface",
 	"Contour",
//  	"Body Trace",
 	"Do Not Select!"
};

const char *hitboxText[HITBOX_MAX] =
{
	"Off",
	"etMain",
	"etPub",
	"etPro",
	"Generic",
	"Custom",
};

const char *priorityTypeText[AP_MAX] =
{
	"Body Only",
	"Head Only",
	"Body - Head",
	"Head - Body",
	"Head priority",
};

const char *aimTypeText[AIM_MAX] =
{
	"Off",
	"On Fire",
	"On Button",
	"Always",
};

const char *humanAimTypeText[HUMAN_AIM_MAX]=
{
	"Off",
	"Lucky headshot",
	"Full mode",
};
   
const char *aimModeText[AIMMODE_MAX] =
{
	"Aimbot off",
	"Normal aimbot",
	"Human aimbot",
};

const char *riflePredictText[RF_PREDICT_MAX] =
{
	"Off",
	"Linear",
	"Linear/2",
	"Average",
	"Adaptive",		
};
	
int SortCrosshair(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (Aimbot.CrosshairDistance(player1) < Aimbot.CrosshairDistance(player2))
		return -1;
	else
		return 1;
}

CAimbot::CAimbot(void)
{
	senslocked = GrenadeFireOK = false;
	numFrameTargets = 0;
	target = NULL;
	grenadeTarget = NULL;
	lastTarget = NULL;
	lastTargetValid = false;
	atkBtn = NULL;
	firing = false;
	stopAutoTargets = false;
}

void CAimbot::SetSelf( int clientNum ){ this->self = &eth32.cg.players[clientNum]; }

void CAimbot::PreFrame(void)
{
	numFrameTargets = 0;
	dbgPts = 0;
	target = NULL;
	lastTargetValid = false;

	if (lastTarget && (!eth32.settings.lockTarget || IS_DEAD(lastTarget->clientNum) || !IS_INFOVALID(lastTarget->clientNum)))
		lastTarget = NULL;
}

void CAimbot::selectGrenadeTarget(int direction)
{
	player_t *player;
	player_t *targets[MAX_CLIENTS];

	memset(targets, 0, MAX_CLIENTS*sizeof(player_t *));

	// get list of valid targets
	int n = 0;
	for(int i=0; i<numFrameTargets; i++){
		player = frameTargets[i];

		if (!player || player == grenadeTarget)
			continue;

		if (Draw.worldToScreen(player->lerpOrigin, &player->screenX, &player->screenY))
			targets[n++] = player;
	}

	// select
	float x, x0 = 1000;
	int m = -1;

	//set the one closest horizontally to crosshair
	for(int i=0; i<n; i++){
		if (!grenadeTarget)
			x = targets[i]->screenX < 0 ? -targets[i]->screenX : targets[i]->screenX;
		else if (direction > 0)
			x = targets[i]->screenX - grenadeTarget->screenX;
		else
			x = grenadeTarget->screenX - targets[i]->screenX;

		if (x < x0 && x >= 0){
			x0 = x;
			m = i;
		}
	}

	if (m < 0)
		grenadeTarget = NULL;
	else
		grenadeTarget = targets[m];
}

void CAimbot::PostFrame(void)
{
	// important default actions, assume neither are set unless explicitly told to do so (safer)
	// these actions are very fast anyway
	LockSensitivity(false);

	if (!this->attackPressed)
		Autofire(false);
		
	if (((eth32.settings.aimprotect == PROTECT_ALL) && (eth32.cg.nspectators || eth32.cg.limbo)) ||
		((eth32.settings.aimprotect == PROTECT_SPECS) && eth32.cg.nspectators) )
		return;
	
	// master aim mode
	if (eth32.settings.aimMode == AIMMODE_OFF)
		return;

	// No need for aimbot on those conditions
	if (IS_SPECTATOR(eth32.cg.snap->ps.clientNum) || IS_DEAD(eth32.cg.clientNum)){
		grenadeFireTime = 0;
		Autocrouch(false, true);
		return;
	}

	if (eth32.cg.currentWeapon->attribs & WA_USER_DEFINED){
		if (eth32.settings.aimMode == AIMMODE_NORMAL) {
			Aimbot.DoBulletBot();
			if (!target)
				Autocrouch(false, false);
		} else if (eth32.settings.aimMode == AIMMODE_HUMAN)
			Aimbot.DoHumanAimbot();
	} else
		Autocrouch(false, false);

	if ((eth32.cg.currentWeapon->attribs & WA_BALLISTIC) && eth32.settings.grenadeBot){
		if (eth32.settings.autoGrenTargets && !stopAutoTargets) {
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortCrosshair);
			if (numFrameTargets)
				grenadeTarget = frameTargets[0];
			else
				grenadeTarget = NULL;
		} else {
			if (!grenadeTarget)
				selectGrenadeTarget(0);
		}

		if (GetAsyncKeyState(XK_Left) & 1){				//		'<'
			selectGrenadeTarget(-1);
			stopAutoTargets = true;
		} else if (GetAsyncKeyState(XK_Right) & 1) {		//		'>'
			selectGrenadeTarget(1);
			stopAutoTargets = true;
		}

		if (grenadeTarget && (!IS_INFOVALID(grenadeTarget->clientNum) || IS_FRIENDLY(grenadeTarget->clientNum) || IS_DEAD(grenadeTarget->clientNum)))
			grenadeTarget = NULL;

		if (grenadeTarget) {
			bool solution = false;
			float flytime;
			vec3_t point;

			VectorCopy(grenadeTarget->lerpOrigin, point);
			Aimbot.grenadeAimbot(point, &flytime, &solution, !eth32.settings.ballisticPredict);
				
			// if prediction is enabled, need another pass because flytime isnt known beforehand
			if (!RifleMultiBounce && eth32.settings.ballisticPredict && solution){
				if (this->attackPressed) {
					int ii;
					ii = 1;
				}
				PredictPlayer(grenadeTarget, flytime, point, -1);
				Aimbot.grenadeAimbot(point, &flytime, &solution, true);
			}
		}
	}
}

// respawn so reset the aimbot
void CAimbot::Respawn(void)
{
#ifdef ETH32_DEBUG
	Debug.Log("Aimbot Respawn()");
#endif
	Autofire(false);
	LockSensitivity(false);
	Autocrouch(false, true);
	lastTarget = target = grenadeTarget = NULL;
	lastTargetValid = false;
	stopAutoTargets = false;
	GrenadeTicking = false;
	grenadeFireTime = 0;
	Engine.SetWeaponSpread();
	
	// reset histories
	for (int i=0; i<MAX_CLIENTS; i++) {
		player_t *p = &eth32.cg.players[i];
		memset(p->history, 0, sizeof(p->history));		
	}
}

void CAimbot::AddTarget(player_t *player)
{
	if (player->friendly || player->invulnerable)
		return;

	if (player->eth32user && eth32.settings.portalCoopMode == COOP_ALL)
		return;

	// - solcrushr: fov check added before adding to our target array
	if ((eth32.cg.currentWeapon->attribs & WA_USER_DEFINED) && player->distance < eth32.cg.currentWeapon->range && CheckFov(player->orHead.origin)) {
		// sol: if last target is valid, then don't add him to normal targetlist
		//      we can check him independently
		if (lastTarget && player == lastTarget) {
			lastTargetValid = true;
			return;
		}

		frameTargets[numFrameTargets] = player;
		numFrameTargets++;
		// sol: weapdefs look safe, but to prevent overwriting array
		return;
	}

	if (eth32.cg.currentWeapon->attribs & WA_BALLISTIC){
		frameTargets[numFrameTargets] = player;
		numFrameTargets++;
	}
}

// these are callbacks for qsort, not part of aimbot object
int distance_test(const void *elm1, const void *elm2)
{
	const trace_point *a = (trace_point *) elm1;
	const trace_point *b = (trace_point *) elm2;

	if( a->d > b->d )
		return 1;
	else if( a->d < b->d )
		return -1;
	else
		return 0;
}

int angle_test(const void *elm1, const void *elm2)
{
	const trace_point *a = (trace_point *) elm1;
	const trace_point *b = (trace_point *) elm2;

	// elements are dotproducts of normalized vecs so bigger value, smaller angle
	if( a->d > b->d )
		return -1;
	else if( a->d < b->d )
		return 1;
	else
		return 0;
}

void CAimbot::DrawGrenadelauncherTrace()
{
	vec3_t angles, forward, viewpos, lastPos;
	bool once = false;

	if (!eth32.settings.rifleTracer)
		return;

	if (!(eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE))
		return;

	if (eth32.cg.snap->ps.weaponTime != 0 )
		return;

	if (Engine.forward == NULL)
		return;

	trajectory_t rifleTrajectory;
	rifleTrajectory.trType = TR_GRAVITY;
	rifleTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, rifleTrajectory.trBase);
	VectorCopy(rifleTrajectory.trBase, viewpos);

	VectorCopy(eth32.cg.refdefViewAngles, angles);
	VectorCopy(Engine.forward, forward);
	viewpos[2] += eth32.cg.snap->ps.viewheight;
	VectorMA(viewpos, 32, forward, viewpos);
	VectorScale(forward, 2000, forward);

	VectorCopy(forward, rifleTrajectory.trDelta);
	SnapVector(rifleTrajectory.trDelta);

	// Calculate rifle impact
	int timeOffset = 0;
	trace_t rifleTrace;
	vec3_t rifleImpact;
	VectorCopy(rifleTrajectory.trBase, rifleImpact);
	#define TIME_STEPP 50
	int maxTime = 3250;
	int totalFly = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&rifleTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&rifleTrace, rifleImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

		// check for hit
		if (rifleTrace.startsolid || rifleTrace.fraction != 1) {
			// When a nade flies for over 750ms and hits somnexing, it'll explode
			if (totalFly > 750)
				break;

			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * rifleTrace.fraction;
			BG_EvaluateTrajectoryDelta(&rifleTrajectory, hitTime, rifleTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(rifleTrajectory.trDelta, rifleTrace.plane.normal);
			VectorMA(rifleTrajectory.trDelta, -2*dot, rifleTrace.plane.normal, rifleTrajectory.trDelta);

			VectorScale(rifleTrajectory.trDelta, 0.35, rifleTrajectory.trDelta);

			if (rifleTrace.surfaceFlags == 0)
				VectorScale(rifleTrajectory.trDelta, 0.5, rifleTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			rifleTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(rifleTrace.endpos, rifleTrajectory.trBase);

			SnapVector(rifleTrajectory.trDelta);
			SnapVector(rifleTrajectory.trBase);
		} else {
			VectorCopy(nextPos, rifleImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}
			if(Engine.IsVisible(eth32.cg.refdef->vieworg, nextPos, 0))
       			Engine.MakeRailTrail( lastPos, nextPos, false, colorYellow, eth32.cg.frametime*3 );
			else
				Engine.MakeRailTrail( lastPos, nextPos, false, colorRed, eth32.cg.frametime*4 );

			VectorCopy(nextPos, lastPos);
		}
	}

	// copy the results for the cams
	VectorCopy( lastPos, lastImpact );
	Aimbot.flyTime = totalFly;
}

void CAimbot::DrawGrenadeTrace()
{
	vec3_t forward, tosspos, viewpos, lastPos, right;

	float pitch, upangle;
	bool once = false;

	if (!eth32.settings.grenadeTracer)
		return;

	if (!(eth32.cg.currentWeapon->attribs & WA_GRENADE))
		return;

	if (Engine.forward == NULL)
		return;

	trajectory_t grenadeTrajectory;
	grenadeTrajectory.trType = TR_GRAVITY;
	grenadeTrajectory.trTime = eth32.cg.time;


	AngleVectors(eth32.cg.refdefViewAngles, NULL, right, NULL);
	VectorCopy(Engine.forward, forward);
	VectorMA(eth32.cg.muzzle, 8, forward, tosspos);
	VectorMA(tosspos, 20, right, tosspos);
	tosspos[2] -= 8;
	SnapVector( tosspos );

	pitch = eth32.cg.refdefViewAngles[0];
	if( pitch >= 0 ) {
		forward[2] += 0.5f;
		pitch = 1.3f;
	}
	else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		forward[2] += (pitch * 0.5f);
		pitch *= 0.3f;
		pitch += 1.f;
	}

	upangle = -eth32.cg.refdefViewAngles[0];
	upangle = min(upangle, 50);
	upangle = max(upangle, -50);
	upangle = upangle/100.0f;
	upangle += 0.5f;

	if(upangle < .1)
		upangle = .1;
	upangle *= 900.0f*pitch;

	VectorNormalizeFast( forward );

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( self->lerpOrigin, viewpos );
	viewpos[2] += eth32.cg.snap->ps.viewheight;

	trace_t tr;
	orig_CG_Trace(&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, self->currentState->number, MASK_MISSILESHOT);

	if( tr.startsolid ) {
		VectorCopy( forward, viewpos );
		VectorMA( self->lerpOrigin, -24.f, viewpos, viewpos );

		orig_CG_Trace(&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, self->currentState->number, MASK_MISSILESHOT);
		VectorCopy( tr.endpos, tosspos );
	} else if( tr.fraction < 1 ) {	// oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

	VectorScale(forward, upangle, forward);
	VectorCopy(tosspos, grenadeTrajectory.trBase);
	VectorCopy(forward, grenadeTrajectory.trDelta);
	SnapVector(grenadeTrajectory.trDelta);

	// Calculate grenade impact
	int timeOffset = 0;
	trace_t grenadeTrace;
	vec3_t grenadeImpact;
	VectorCopy(grenadeTrajectory.trBase, grenadeImpact);
	#define TIME_STEPP 50
	int maxTime = 4000;
	int totalFly = 0;
	int bounces = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&grenadeTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&grenadeTrace, grenadeImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

		// check for hit
		if (grenadeTrace.startsolid || grenadeTrace.fraction != 1) {
			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * grenadeTrace.fraction;
			BG_EvaluateTrajectoryDelta(&grenadeTrajectory, hitTime, grenadeTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(grenadeTrajectory.trDelta, grenadeTrace.plane.normal);
			VectorMA(grenadeTrajectory.trDelta, -2*dot, grenadeTrace.plane.normal, grenadeTrajectory.trDelta);

			bounces++;
			VectorScale(grenadeTrajectory.trDelta, powf(0.35, bounces), grenadeTrajectory.trDelta);

			if (VectorLength(grenadeTrajectory.trDelta) < 30.0f)
				return;

			if (grenadeTrace.surfaceFlags == 0)
				VectorScale(grenadeTrajectory.trDelta, 0.5, grenadeTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			grenadeTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(grenadeTrace.endpos, grenadeTrajectory.trBase);

			SnapVector(grenadeTrajectory.trDelta);
			SnapVector(grenadeTrajectory.trBase);
		} else {
			VectorCopy(nextPos, grenadeImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}
			if(Engine.IsVisible(eth32.cg.refdef->vieworg, nextPos, 0))
       			Engine.MakeRailTrail( lastPos, nextPos, false, colorYellow, eth32.cg.frametime*3 );
			else
				Engine.MakeRailTrail( lastPos, nextPos, false, colorRed, eth32.cg.frametime*4 );

			VectorCopy(nextPos, lastPos);
		}
	}

	// copy the results for the cams
	VectorCopy( lastPos, lastImpact );
	Aimbot.flyTime = totalFly;
}

void CAimbot::DrawBodyBox( int clientNum, bool axes )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		size;
	vec3_t		boxOrigin;
	
	// i suppose body box size does not vary much (if at all)
	size[0] = size[1] = 37.0;
	size[2] = 23.0;

	VectorCopy( player->lerpOrigin, boxOrigin );

	if (player->currentState->eFlags & EF_PRONE)
		size[2] += PRONE_VIEWHEIGHT+12.0;
	else if (player->currentState->eFlags & EF_CROUCHING)
		size[2] += CROUCH_VIEWHEIGHT+8.0;
	else
		size[2] += DEFAULT_VIEWHEIGHT-4.0;

	boxOrigin[2] += -24.0 + size[2]*0.5;

	vec3_t min,max;
	VectorCopy( boxOrigin, min );
	VectorCopy( boxOrigin, max );

	VectorMA( boxOrigin, -size[0]*0.5, xAxis, min );
	VectorMA( min, -size[1]*0.5, yAxis, min );
	VectorMA( min, -size[2]*0.5, zAxis, min );

	VectorMA( boxOrigin, size[0]*0.5, xAxis, max );
	VectorMA( max, size[1]*0.5, yAxis, max );
	VectorMA( max, size[2]*0.5, zAxis, max );

	Engine.MakeRailTrail( min, max, true, eth32.settings.colorBodyHitbox, eth32.settings.bodyRailTime );
}

void CAimbot::DrawHeadBox( int clientNum, bool axes )
{
	vec3_t		cv;
	vec3_t		p;
	hitbox_t 	*hbox;
	bool		moving;
	int 		eFlags;
	vec3_t		vel;
	vec3_t		size;
	float		speed;

	if( eth32.settings.hitboxType == HITBOX_CUSTOM )
		hbox = &customHitbox;
	else
		hbox = &head_hitboxes[eth32.settings.hitboxType];

	eFlags = eth32.cg.players[clientNum].currentState->eFlags;

	VectorCopy( eth32.cg.players[clientNum].currentState->pos.trDelta, vel );
	/* this is not really movement detector, but animation detector
		only use pos.trDelta since that handles user-intended velocities */
	if( VectorCompare(vel, vec3_origin) )
		moving = false;
	else
		moving = true;

	if( (eFlags & EF_PRONE) || (eFlags & EF_PRONE_MOVING) )
		VectorCopy( hbox->prone_offset, cv );
	else {
		if( !moving ){
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset, cv );
			else
				VectorCopy( hbox->stand_offset, cv );
		} else {
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset_moving, cv );
			else
				VectorCopy( hbox->stand_offset_moving, cv );
		}
	}

	VectorCopy( hbox->size, size );

	/* Dynamic Hitbox - adjust X,Y size based on speed perpendicular to our viewdirection
		This is gives the aimbot 'fastness' of aim when guy corners */
	if (eth32.settings.dynamicHitboxScale > 0){
		speed = VectorLength(vel) - fabs(DotProduct(vel,eth32.cg.refdef->viewaxis[0]));

		if( speed > 0 ){
			size[0] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->serverTime);		// ps.speed is g_speed
			size[1] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->serverTime);		// ps.speed is g_speed
		}
	}

	orientation_t *head = &eth32.cg.players[clientNum].orHead;
	// rotate hitbox offset vector with tag (hitboxes themselves dont rotate)
	VectorMA( head->origin, cv[2], head->axis[2], p );
	VectorMA( p, cv[1], head->axis[1], p );
	VectorMA( p, cv[0], head->axis[0], p );

	vec3_t min,max;

	VectorMA( p, -0.5* size[0], xAxis, min );
	VectorMA( min, -0.5* size[1], yAxis, min );
	VectorMA( min, -0.5* size[2], zAxis, min );

	VectorMA( p, 0.5* size[0], xAxis, max );
	VectorMA( max, 0.5* size[1], yAxis, max );
	VectorMA( max, 0.5* size[2], zAxis, max );

	//Engine.MakeRailTrail( min, max, true, 0, 1, 0, 40 );
	Engine.MakeRailTrail( min, max, true, eth32.settings.colorHeadHitbox, eth32.settings.headRailTime );
	if (axes){
		vec3_t ex1,ex2,ey1,ey2,ez1,ez2;
		VectorMA( p, 0, head->axis[0], ex1 );
		VectorMA( p, 25, head->axis[0], ex2 );

		VectorMA( p, 0, head->axis[1], ey1 );
		VectorMA( p, 25, head->axis[1], ey2 );

		VectorMA( p, 0, head->axis[2], ez1 );
		VectorMA( p, 25, head->axis[2], ez2 );

		Engine.MakeRailTrail( ex1, ex2, false, eth32.settings.colorXAxis, eth32.settings.headRailTime );
		Engine.MakeRailTrail( ey1, ey2, false, eth32.settings.colorYAxis, eth32.settings.headRailTime );
		Engine.MakeRailTrail( ez1, ez2, false, eth32.settings.colorZAxis, eth32.settings.headRailTime );
	}
}

/* trace a userdefined box, set the visible vector, and return visiblity
	size is world coordinates, X*Y*Z */
bool CAimbot::traceBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, boxtrace_t trType,  vec3_t visible, int maxTraces )
{
	trace_point *p;
	VectorCopy( vec3_origin, visible );
	float boxVolume = size[0]*size[1]*size[2];

	if (VectorCompare(boxOrigin, vec3_origin))
		return false;

     	VectorMA(boxOrigin, eth32.settings.predTarget, player->currentState->pos.trDelta, boxOrigin);

	if (eth32.settings.autoPoints != AUTOPT_OFF) {
		if (eth32.settings.autoPoints == AUTOPT_HIGH) {
			maxTraces = 1;
			float spread = 0.15+(float)eth32.cg.snap->ps.aimSpreadScale*0.003922;	// 1/255
			spread *= eth32.cg.spreadIndex*dist*0.0001221*0.65;						// 1/8192
			spreadd = spread;
			float avgSurfaceArea = pow(boxVolume,0.67);
 			float avgBone = 0.33*(size[0]+size[1]+size[2]);
			if (spread > 0.f)
				maxTraces = (int)avgSurfaceArea/spread;
			else
				maxTraces = 16;
			if (!maxTraces)
				maxTraces=1;
			if (maxTraces > 16)
				maxTraces = 16;
			dbgPts = maxTraces;
		}
	}

	// for now don't allow center trace for body in this fashion
	if (trType != TRACE_BODY){
		if (IsPointVisible(trOrigin, boxOrigin, skipEnt)){
			VectorCopy( boxOrigin, visible );
			return true;
		} else if( trType == TRACE_CENTER )
			return false;
	} else
		maxTraces += 16;

	// check for a too small volume
	if( boxVolume < 1.0 )
		return false;

	int k = 0;
	p = (trace_point *)malloc(sizeof(trace_point)*maxTraces);
	memset(p, 0, sizeof(trace_point)*maxTraces);

	switch( trType ){
		case TRACE_RANDOM_SURFACE: {
			/* get the 1, 2, or 3 plane(s) facing us */
			vec3_t dir,ndir, dr;
			vec3_t n, a, b;
			int i,j,N;
			float frac;
			VectorSubtract( eth32.cg.refdef->vieworg, boxOrigin, dir );
			VectorCopy( dir, ndir );
			VectorNormalizeFast( ndir );
			for( i=0; i<6; i++ ){
				n[(i+1)%3] = n[(i+2)%3] = 0;
				n[(i % 3)] = (i > 2) ? 1.0 : -1.0;
				frac = DotProduct( n, ndir );
				if( frac > 0 ){
					/* this plane is visible, fill it up with points, we have MAX_TR to distribute
					   so allocate by visible surface area. Note: max( sum(DotProduct(ndir, n)) ) = sqrt(3)
					   by axiom of spherical geometry */

					N = (int)((frac/sqrtf(3))*maxTraces);
					/* construct orthonormal plane vecs */
					a[i%3] = a[(i+2)%3] = 0;
					a[((i+1) % 3)] = 1.0;
					b[i%3] = b[(i+1)%3] = 0;
					b[((i+2) % 3)] = 1.0;
					for( j=0;j<N; j++ ){
						VectorMA( boxOrigin, size[i % 3]*0.5, n, p[k].pt );
						VectorMA( p[k].pt, size[(i+1) % 3]*crandom()*0.5, a, p[k].pt );
						VectorMA( p[k].pt, size[(i+2) % 3]*crandom()*0.5, b, p[k].pt );

						/* sort based on angular distance from center based on our viewdir (to maximize acc) */
						VectorSubtract( eth32.cg.refdef->vieworg, p[k].pt, dr );
						VectorNormalizeFast( dr );
						p[k].d = DotProduct( dr, ndir );
						k++;
					}
				}
			}
			qsort( p, maxTraces, sizeof(trace_point), angle_test );
			break;
		}
		case TRACE_RANDOM_VOLUME: {
			/* trace random points within hbox volume (faster but less accurate) */
			for( k=0; k < maxTraces; k++ ){
				VectorMA( p[k].pt, size[0]*crandom()*-0.5, xAxis, p[k].pt );
				VectorMA( p[k].pt, size[1]*crandom()*-0.5, yAxis, p[k].pt );
				VectorMA( p[k].pt, size[2]*crandom()*-0.5, zAxis, p[k].pt );
				p[k].d = VectorLengthSquared( p[k].pt );
			}
			qsort( p, maxTraces, sizeof(trace_point), distance_test );

			for( k=0; k < maxTraces; k++ )
				VectorAdd( boxOrigin, p[k].pt, p[k].pt );

			break;
		}

		// for capsules size[0] is the radious and size[2] the height
		case TRACE_CAPSULE_VOLUME: {
			float phi, r;
			// trace random points within capsule volume
			for( k=0; k < maxTraces; k++ ){
				phi = 2.0*M_PI*random();
				r = 0.5*random()*size[0];
				p[k].pt[0] = cosf(phi)*r;
				p[k].pt[1] = sinf(phi)*r;
				p[k].pt[2] = 0.5*crandom()*size[2];
				p[k].d = r*r+p[k].pt[2]*p[k].pt[2];
			}
			qsort( p, maxTraces, sizeof(trace_point), distance_test );

			for( k=0; k < maxTraces; k++ )
				VectorAdd( boxOrigin, p[k].pt, p[k].pt );

			break;
		}

		// for capsules size[0] is the radious and size[2] the height
		case TRACE_CAPSULE_SURFACE: {
			vec3_t dir, dr;
			float phi;

			VectorSubtract( eth32.cg.refdef->vieworg, boxOrigin, dir );
			VectorNormalizeFast( dir );

			for( k=0; k < maxTraces; k++ ){
				phi = 2.0*M_PI*random();
				p[k].pt[0] = cosf(phi)*0.5*size[0];
				p[k].pt[1] = sinf(phi)*0.5*size[0];
				p[k].pt[2] = 0.5*crandom()*size[2];

				VectorAdd( boxOrigin, p[k].pt, p[k].pt );

				VectorSubtract( eth32.cg.refdef->vieworg, p[k].pt, dr );
				VectorNormalizeFast( dr );
				p[k].d = DotProduct( dr, dir );

			}
			qsort( p, maxTraces, sizeof(trace_point), angle_test );

			break;
		}
		case TRACE_CONTOUR: {
			size[0] -= 0.7;
			size[1] -= 0.7;
			/* trace periphery - prioritize plane centers, more chance of hitting */
			VectorMA( boxOrigin, size[0]*0.5, xAxis, p[k].pt ); k++;
			VectorMA( boxOrigin, -size[0]*0.5, xAxis, p[k].pt ); k++;
			VectorMA( boxOrigin, size[1]*0.5, yAxis, p[k].pt ); k++;
			VectorMA( boxOrigin, -size[0]*0.5, yAxis, p[k].pt ); k++;
			VectorMA( boxOrigin, size[2]*0.5, zAxis, p[k].pt ); k++;

			/* corners */
			VectorMA( boxOrigin, size[0]*0.5, xAxis, p[k].pt );
			VectorMA( p[k].pt, size[2]*0.5, zAxis, p[k].pt );
			k++;

			VectorMA( boxOrigin, -size[0]*0.5, xAxis, p[k].pt );
			VectorMA( p[k].pt, size[2]*0.5, zAxis, p[k].pt );
			k++;

			VectorMA( boxOrigin, size[1]*0.5, yAxis, p[k].pt );
			VectorMA( p[k].pt, size[2]*0.5, zAxis, p[k].pt );
			k++;

			VectorMA( boxOrigin, -size[1]*0.5, yAxis, p[k].pt );
			VectorMA( p[k].pt, size[2]*0.5, zAxis, p[k].pt );
			k++;
			for( k=4; k < maxTraces; k++ ){
				VectorMA( boxOrigin, size[0]*crandom()*0.5, xAxis, p[k].pt );
				VectorMA( p[k].pt, size[1]*crandom()*0.5, yAxis, p[k].pt );
				VectorMA( p[k].pt, size[2]*crandom()*0.5, zAxis, p[k].pt );
			}

			break;
		}

		// First a few fixed points on the upper torso, when nothing is visible, do so random points
		case TRACE_BODY: {
			size[0] -= 0.7;
			size[1] -= 0.7;
			// create 4 fixed points on the vertices of the top square
			VectorMA( boxOrigin, 0.5*size[2]-5.0, zAxis, p[0].pt );
			VectorMA( p[0].pt, 0.5*size[0], xAxis, p[0].pt );
			VectorMA( p[0].pt, 0.5*size[1], yAxis, p[0].pt );

			VectorMA( boxOrigin, 0.5*size[2]-5.0, zAxis, p[1].pt );
			VectorMA( p[1].pt, -0.5*size[0], xAxis, p[1].pt );
			VectorMA( p[1].pt, 0.5*size[1], yAxis, p[1].pt );

			VectorMA( boxOrigin, 0.5*size[2]-5.0, zAxis, p[2].pt );
			VectorMA( p[2].pt, -0.5*size[0], xAxis, p[2].pt );
			VectorMA( p[2].pt, -0.5*size[1], yAxis, p[2].pt );

			VectorMA( boxOrigin, 0.5*size[2]-5.0, zAxis, p[3].pt );
			VectorMA( p[3].pt, 0.5*size[0], xAxis, p[3].pt );
			VectorMA( p[3].pt, -0.5*size[1], yAxis, p[3].pt );

			// if these fail to be visible (enemy behind complex object with cracks/holes
			// then create a user-requested amount of random points
			for( k=4; k < maxTraces; k++ ){
				VectorMA( boxOrigin, size[0]*crandom()*0.5, xAxis, p[k].pt );
				VectorMA( p[k].pt, size[1]*crandom()*0.5, yAxis, p[k].pt );
				VectorMA( p[k].pt, size[2]*crandom()*0.5, zAxis, p[k].pt );
			}
			break;
		}
		default:
			/* unknown trace type */
			free(p);
			return false;
	}

	/* trace them all */
	for( k=0; k<maxTraces; k++ ){
		if(IsPointVisible(trOrigin, p[k].pt, skipEnt)){
			VectorCopy( p[k].pt, visible );
			free(p);
			return true;
		}
	}

	free(p);
	return false;
}

/*					grenade trajectory explanation

given fixed endpos, startpos, and velocity, the pitch angle can be calculated
in closed form,

	g*s*tan(angle) = v^2 - sqrt(v^4 - 2*z*g*v^2 - s*s*g^2);

where g is gravity, v the velocity, z the vertical distance, and s the horizontal distance.
However, this solution is only valid in the case of fixed velocity. Grenades are
thrown with a velocity that depends on the pitch angle, which is the one we're
trying to find, with the result that the angle cannot be calculated in closed form anymore.

Since the velocity derivative is small compared to angular dependency and horizontal distance,
we can just iterate a few tries with sufficient accuracy

*/
bool CAimbot::grenadePitchAngle(vec3_t start, vec3_t endpos, float maxTime, float *flytime, float *angle, bool longOnly)
{
	float Di,s, z, zc, p, t, v, ez;
	float p0, p1, rpitch;
	const float g = 800.0f;
	vec3_t D;
	bool firstTry = true;

	// max error in end position
	ez = 10.0;

	// get the horizontal and vertical distance
	VectorSubtract(endpos, start, D);
	s = sqrtf(D[0]*D[0]+D[1]*D[1]);		// horizontal
	z = D[2];							// vertical

	// multiplication constant
	p = 1.0f/(g*s);

	// initial guess for pitch angle
	p0 = 0.78;

	for (int i=0;i <100; i++) {
		this->grenadePitchCorrection(p0, &rpitch);
		v = speedForPitch(rpitch);

		Di = v*v*v*v - 2.0f*z*g*v*v - g*g*s*s;

		if (Di < 0) {
			if (firstTry) {
				firstTry = false;
				i = -1;
				p0 = 0.78;
				continue;
			}
			return false;
		} else if (Di == 0) {
			*angle = atanf(v*v*p);
			return true;
		} else {
			if (firstTry && !longOnly)
				p1 = atanf(v*v*p-sqrtf(Di)*p);
			else
				p1 = atanf(v*v*p+sqrtf(Di)*p);
		}

		// smoothly converge to prevent premature jumping to other solution
		p0 += (p1-p0)*0.025;
	}

	t = s/(v*cosf(p0));
	rifleGrenadeTime = t;

	if (t > 0 && t < maxTime) {
		// make sure our speed & pitch do indeed solve the equation
		zc = v*sinf(p0)*t - 0.5*g*t*t;
		zc -= z;
		zc = zc < 0.0f ? -1.0f*zc : zc;
		if (zc < ez){
			*angle = p0;
			*flytime = t;
			return true;
		}
	}

	return false;
}

// Grenades aren't launched with a synced forward direction
// use a pre-calculated lookup table to translate grenade pitch angle to view pitch angle
void CAimbot::grenadePitchCorrection(float pitch, float *z)
{
	float gp;
	gp = -pitch*57.2957795131;

	if (gp < -30.0 || gp > 87.74824){
		*z = pitch;
		return;
	}

	float f = gp*2 + 60.0;
	int m = (int)f;
	f -= (float)m;

	*z = pitchLUT[m] + f*(pitchLUT[m+1]-pitchLUT[m]);
	*z *= -0.01745329252;
}

// pieced together from SDK - dont blame me for this mess!
float CAimbot::speedForPitch(float pitch)
{
	pitch *= -57.29578;

	if (pitch > 40.0)
		return 117.0;

	if (pitch < -50.0)
		return 900.0;

	float f = (pitch + 50.0)*2.0;
	int m = (int)f;
	f -= (float)m;

	float speed = speedLUT[m] + f*(speedLUT[m+1]-speedLUT[m]);
	return speed;
}

// calculates the pitch angle for non powered flight given startpos and endpos
bool CAimbot::ballisticPitchAngle(vec3_t start, vec3_t endpos, float v, float maxTime, float *flytime, float *angle)
{
	float pitch, Di, s, z, zc, p, t, ez;
	const float g = 800.0f;
	vec3_t D;

	// set tolerance in calculated endpos
	ez = 5.0f;

	VectorSubtract(endpos, start, D);
	s = sqrtf(D[0]*D[0]+D[1]*D[1]);
	z = D[2];

	Di = v*v*v*v - 2.0f*z*g*v*v - g*g*s*s;
	p = 1.0f/(g*s);
	if (Di < 0) {
		// no solution
		return false;

	} else if (Di == 0) {
		*angle = atanf(v*v*p);
		return true;
	} else {
		pitch = atanf(v*v*p-sqrtf(Di)*p);

		t = s/(v*cosf(pitch));
		rifleGrenadeTime = t;

		if (t > 0.1 && t < maxTime) {
			zc = v*sinf(pitch)*t - 0.5*g*t*t;
			zc -= z;
			zc = zc < 0.0f ? -1.0f*zc : zc;
			if (zc < ez){
				*angle = pitch;
				if (flytime)
					*flytime = t;
				return true;
			}
		}
	}
	return false;
}

// see if we dont hit anything along the way
bool CAimbot::ballisticTrajectoryValid(vec3_t start, vec3_t end, float pitch, float flytime, float v)
{
	float t, dt, vs, vz;
	const float g = 800.0;
	vec3_t d0, p0, p1;
	trace_t tr;

	vs = v*cosf(pitch);
	vz = v*sinf(pitch);

	VectorSubtract(end, start, d0);
	d0[2] = 0;
	VectorNormalizeFast(d0);

	// about 20 traces
	dt = flytime/20.0;

	VectorCopy(start, p0);
	for (t=dt; t<flytime; t+=dt) {
		VectorMA(start, t*vs, d0, p1);
		p1[2] += vz*t - 0.5*g*t*t;
		orig_CG_Trace(&tr, p0, 0, 0, p1, grenadeTarget->clientNum, MASK_MISSILESHOT);

		if (tr.fraction < 1.0f || tr.allsolid){
			// we hit something :(
			return false;
		}

		VectorCopy(p1, p0);
	}

	return true;
}

bool CAimbot::rifleEndPos(vec3_t forward, vec3_t impact, float *flytime)
{
	vec3_t viewpos, lastPos;
	bool once = false;

	trajectory_t rifleTrajectory;
	rifleTrajectory.trType = TR_GRAVITY;
	rifleTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, rifleTrajectory.trBase);
	VectorCopy(rifleTrajectory.trBase, viewpos);

	VectorMA(viewpos, 32, forward, viewpos);
	VectorScale(forward, 2000, forward);

	VectorCopy(forward, rifleTrajectory.trDelta);
	SnapVector(rifleTrajectory.trDelta);

	// Calculate rifle impact
	int timeOffset = 0;
	trace_t rifleTrace;
	vec3_t rifleImpact;
	VectorCopy(rifleTrajectory.trBase, rifleImpact);
	#define TIME_STEPP 50
	int maxTime = 3250;
	int totalFly = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&rifleTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		//orig_CG_Trace(&rifleTrace, rifleImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		orig_syscall(CG_CM_BOXTRACE, &rifleTrace, rifleImpact, nextPos, 0, 0, 0, MASK_MISSILESHOT);

		// check for hit
		if (rifleTrace.startsolid || rifleTrace.fraction != 1) {
			// When a nade flies for over 750ms and hits somnexing, it'll explode
			if (totalFly > 750)
				break;

			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * rifleTrace.fraction;
			BG_EvaluateTrajectoryDelta(&rifleTrajectory, hitTime, rifleTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(rifleTrajectory.trDelta, rifleTrace.plane.normal);
			VectorMA(rifleTrajectory.trDelta, -2*dot, rifleTrace.plane.normal, rifleTrajectory.trDelta);

			VectorScale(rifleTrajectory.trDelta, 0.35, rifleTrajectory.trDelta);

			if (rifleTrace.surfaceFlags == 0)
				VectorScale(rifleTrajectory.trDelta, 0.5, rifleTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			rifleTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(rifleTrace.endpos, rifleTrajectory.trBase);

			SnapVector(rifleTrajectory.trDelta);
			SnapVector(rifleTrajectory.trBase);
		} else {
			VectorCopy(nextPos, rifleImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}

			VectorCopy(nextPos, lastPos);
		}
	}

	VectorCopy( lastPos, impact );
	*flytime = totalFly;
}

bool CAimbot::traceHead( orientation_t *head, vec3_t hitPoint, int clientNum )
{
	return this->traceHead(head, hitPoint, eth32.cg.muzzle, clientNum);
}

bool CAimbot::traceHead( orientation_t *head, vec3_t hitPoint, vec3_t start, int clientNum )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		cv;
	vec3_t		p;
	hitbox_t 	*hbox;
	bool		moving;
	int 		eFlags;
	vec3_t		vel;
	vec3_t		size;
	float		speed;

	// allow users to disable head traces all together
	if (!eth32.cg.currentWeapon->headTraces)
		return false;

	if( eth32.settings.hitboxType == HITBOX_CUSTOM )
		hbox = &customHitbox;
	else
		hbox = &head_hitboxes[eth32.settings.hitboxType];

	eFlags = player->currentState->eFlags;

	VectorCopy( player->currentState->pos.trDelta, vel );
	/* this is not really movement detector, but animation detector
		only use pos.trDelta since that handles user-intended velocities */
	if( VectorCompare(vel, vec3_origin) )
		moving = false;
	else
		moving = true;

	if( (eFlags & EF_PRONE) || (eFlags & EF_PRONE_MOVING) )
		VectorCopy( hbox->prone_offset, cv );
	else {
		if( !moving ){
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset, cv );
			else
				VectorCopy( hbox->stand_offset, cv );
		} else {
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset_moving, cv );
			else
				VectorCopy( hbox->stand_offset_moving, cv );
		}
	}

	VectorCopy( hbox->size, size );


	/* Dynamic Hitbox - adjust X,Y size based on speed perpendicular to our viewdirection
		This is gives the aimbot 'fastness' of aim when guy corners */
	if (eth32.settings.dynamicHitboxScale > 0){
		speed = VectorLength(vel) - fabs(DotProduct(vel,eth32.cg.refdef->viewaxis[0]));

		if( speed > 0 ){
			size[0] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->serverTime);		// ps.speed is g_speed
			size[1] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->serverTime);		// ps.speed is g_speed
		}
	}

	// rotate hitbox offset vector with tag (hitboxes themselves dont rotate)
	VectorMA( head->origin, cv[2], head->axis[2], p );
	VectorMA( p, cv[1], head->axis[1], p );
	VectorMA( p, cv[0], head->axis[0], p );

	return traceBox(p, player->distance, size, start, clientNum, player, (boxtrace_t) eth32.settings.traceType, hitPoint, eth32.cg.currentWeapon->headTraces);
}

bool CAimbot::traceBody( vec3_t hitPoint, int clientNum )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		size;
	vec3_t		boxOrigin;

	// allow user to turn off body traces all together
	if (!eth32.cg.currentWeapon->bodyTraces)
		return false;

	// i suppose body box size does not vary much (if at all)
	size[0] = size[1] = 36.0;
	size[2] = 24.0;

	VectorCopy( player->lerpOrigin, boxOrigin );

	if (player->currentState->eFlags & EF_PRONE)
		size[2] += PRONE_VIEWHEIGHT+12.0;
	else if (player->currentState->eFlags & EF_CROUCHING)
		size[2] += CROUCH_VIEWHEIGHT+8.0;
	else
		size[2] += DEFAULT_VIEWHEIGHT-4.0;

	boxOrigin[2] += -24.0 + size[2]*0.5;
	// sol: testing with using user selected trace type on body
	return traceBox( boxOrigin, eth32.cg.players[clientNum].distance, size, eth32.cg.muzzle, clientNum, player, /*TRACE_BODY*/(boxtrace_t)eth32.settings.traceType, hitPoint, eth32.cg.currentWeapon->bodyTraces );
}

int SortDistance(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->distance < player2->distance)
		return -1;
	else
		return 1;
}

int SortKdRatio(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.kdRatio == player2->stats.kdRatio)
		return SortDistance( p1, p2 );

	if (player1->stats.kdRatio > player2->stats.kdRatio)
		return -1;
	else
		return 1;
}

int SortAccuracy(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.accuracy > player2->stats.accuracy || (player1->stats.accuracy == player2->stats.accuracy && player1->distance < player2->distance))
		return -1;
	else
		return 1;
}

// kobject: sort by whose is targeting us
// basic sort type is distance, but SortDistance can be replaced by any of the above sortfuncs as well
int SortTargeting(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->targetingUs && player2->targetingUs)
		return SortDistance( p1, p2 );

	if (player1->targetingUs)
		return -1;
	else if (player2->targetingUs)
		return 1;
	else
		return SortDistance( p1, p2 );
}

int SortThreat(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.threat == player2->stats.threat)
		return SortDistance( p1, p2 );

	if (player1->stats.threat > player2->stats.threat)
		return -1;
	else
		return 1;
}

bool CAimbot::TargetingUs( int clientNum )
{
	trace_t tr;
	vec3_t endpos, startpos;
	player_t *enemy = &eth32.cg.players[clientNum];

	if (!eth32.cg.currentWeapon)
		return false;

	VectorCopy(enemy->lerpOrigin, endpos);

	if (enemy->currentState->eFlags & EF_PRONE)
		endpos[2] += PRONE_VIEWHEIGHT;
	else if (enemy->currentState->eFlags & EF_CROUCHING)
		endpos[2] += CROUCH_VIEWHEIGHT;
	else
		endpos[2] += DEFAULT_VIEWHEIGHT;

	VectorCopy(endpos, startpos);

	// this provides a neat oppertunity to set the maximum distance we want to be responding to aggressors
	VectorMA( endpos, eth32.cg.currentWeapon->attribs & WA_SCOPED ? 8192.0f : 1800.0f, enemy->currentState->apos.trBase, endpos );

	orig_CG_Trace( &tr, startpos, NULL, NULL, endpos, enemy->clientNum, MASK_SHOT );
	if (tr.entityNum == self->clientNum)
		return true;

	return false;
}

void CAimbot::DoHumanAimbot(void)
{
	// Weapon parameters not known or weapon set to not aim
	if (!eth32.cg.currentWeapon || !(eth32.cg.currentWeapon->attribs & WA_USER_DEFINED))
		return;
	
	// nothing to do
	if (!target && !aimkeyPressed)
		return;
	
	player_t *player;
	int oldTracestyle = eth32.settings.traceType;
	eth32.settings.traceType = TRACE_CENTER;
	
	// sort based on FOV
	if (!target) {
		qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortCrosshair);
	
		for (int i=0 ; i<numFrameTargets ; i++)
		{
			player = frameTargets[i];
			if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
				break;
			}
		}
	}
	
	eth32.settings.traceType = oldTracestyle;
}

// normal bullet aimbot at full efficiency
void CAimbot::DoBulletBot(void)
{
	// autofire
	autoMode = (eth32.cg.currentWeapon->autofire && eth32.settings.autofire && eth32.settings.aimType != AIM_ON_FIRE);
	// validate target
	autoMode |= (eth32.settings.aimType == AIM_ON_FIRE && eth32.settings.atkValidate);

	// Weapon parameters not known or weapon set to not aim
	if (!eth32.cg.currentWeapon || !eth32.settings.aimSort || !(eth32.cg.currentWeapon->attribs & WA_USER_DEFINED))
		return;

	// global aim is off or set for on fire without fire button down
	if (eth32.settings.aimType == AIM_OFF ||
		(eth32.settings.aimType == AIM_ON_FIRE && !attackPressed) ||
		(eth32.settings.aimType == AIM_ON_BUTTON && !aimkeyPressed))
		return;

	player_t *player;

	// sol: lock target check here
	if (eth32.settings.lockTarget && lastTarget && lastTargetValid) { // redundant :P
		player = lastTarget;

		if (eth32.settings.headbody == HEAD_PRIORITY || eth32.settings.headbody == HEAD_BODY) {
			if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			} else if (traceBody(player->aimPt, player->clientNum ))
				target = player;
		} else if (eth32.settings.headbody == BODY_HEAD) {
			if (traceBody(player->aimPt, player->clientNum ))
				target = player;
			else if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			}
		} else if (eth32.settings.headbody == HEAD_ONLY) {
			if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			}
		} else if (eth32.settings.headbody == BODY_ONLY) {
			if (traceBody(player->aimPt, player->clientNum ))
				target = player;
		}
	}

	// sol: locked target wasn't valid or isn't visible, check normal target list
	if (!target) {
		// run sort based on selection type
		if (eth32.settings.aimSort == SORT_DISTANCE)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortDistance);
		else if (eth32.settings.aimSort == SORT_CROSSHAIR)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortCrosshair);
		else if (eth32.settings.aimSort == SORT_KDRATIO)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortKdRatio);
		else if (eth32.settings.aimSort == SORT_ACCURACY)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortAccuracy);
		else if (eth32.settings.aimSort == SORT_TARGETING)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortTargeting);
		else if (eth32.settings.aimSort == SORT_THREAT)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortThreat);
		else
			return;	// should never get here

		// sol: modified to allow user to choose priority, may need a seperate function to keep this clean
		if (eth32.settings.headbody == HEAD_PRIORITY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
			}

			if (!target){
				for (int i=0 ; i<numFrameTargets ; i++)
				{
					player = frameTargets[i];

					if (traceBody(player->aimPt, player->clientNum )) {
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == HEAD_BODY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
				if (!target) {
					if (traceBody(player->aimPt, player->clientNum )) {
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == BODY_HEAD) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceBody(player->aimPt, player->clientNum )) {
					target = player;
					break;
				}

				if (!target) {
					if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
						VectorCopy(player->headPt, player->aimPt);
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == HEAD_ONLY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
			}
		}
		else if (eth32.settings.headbody == BODY_ONLY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceBody(player->aimPt, player->clientNum )) {
					target = player;
					break;
				}
			}
		}
	}

	// set lastTarget here, we don't want preaim targets to lock
	lastTarget = (target) ? target : NULL;

	// do preaim and preshoot stuff if we do not have a valid target yet
	if (!target && (eth32.settings.preAim || eth32.settings.preShoot)) {
		vec3_t spos;
		vec3_t ppos;

		float preshoot;
		preshoot = eth32.settings.preShootTime*0.001;
		VectorMA(eth32.cg.muzzle, preshoot, eth32.cg.predictedps->velocity, spos);
		VectorMA(eth32.cg.muzzle, eth32.settings.preAimTime*0.001, eth32.cg.predictedps->velocity, ppos);

		if (eth32.settings.preShoot) {
			for (int i=0 ; i<numFrameTargets ; i++){
				player = frameTargets[i];

				VectorMA(player->orHead.origin, preshoot, player->cent->currentState.pos.trDelta, player->orHead.origin);
				if (traceHead(&player->orHead, player->headPt, spos, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}

				//restore (is this necessary?)
 				VectorMA(player->orHead.origin, -1.0f*preshoot, player->cent->currentState.pos.trDelta, player->orHead.origin);
			}
		}

		if (eth32.settings.preAim){
			for (int i=0 ; i<numFrameTargets ; i++){
				player = frameTargets[i];

				// use head "origin", somewhere in the backpack. This distance from this
				// point to a future aimpoint is smallest, thus minimizing spread
				VectorMA(player->orHead.origin, eth32.settings.preAimTime*0.001, player->cent->currentState.pos.trDelta, player->headPt);

				// point and lock, but do not fire!
				if (IsPointVisible(ppos, player->headPt, player->clientNum)){
					target = player;
					VectorCopy(target->orHead.origin, target->aimPt);
					LockSensitivity(true);
					Point(eth32.cg.muzzle);
					Cam.targetClientNum = player->clientNum;
					return;
				}
			}
		}
	}

	// we have a valid (i.e. real) target - fire away
	if (target) {
		applySelfPrediction();
		LockSensitivity(true);

		// sol: added check on target->lf, CG_SetLerpFrameAnimationRate not defined for all mods :P
		if (target->lf && (target->lf->frame == 1947 || target->lf->frame == 1998)){
			VectorMA(target->aimPt, eth32.settings.animCorrection, zAxis, target->aimPt);
		}

		Autofire(true);
		Autocrouch(true, false);
		Cam.targetClientNum = target->clientNum;

		// we can improve acc even more, when first bullet fires and AC is on,
		// locally we think we're standing but server already crouched, so adjust
		if (acFirstFrame){
			eth32.cg.muzzle[2] -= 24.0f;
			Point(eth32.cg.muzzle);
		} else
			Point(eth32.cg.muzzle);
	}
	else {
		Autofire(false);
		Cam.targetClientNum = -1;
	}
}

// multi bounce is somewhat different. The only way is to try a bunch 
// of guesses near the player and see where it ends up

bool CAimbot::multibounceAimbot(vec3_t point, float *fltime, float *ptch)
{
	// perpendicular vecs px, pz
	vec3_t p, px, pz, ds, dv, t, forward;
	VectorSubtract(point, eth32.cg.muzzle, ds);
	
	pz[0] = -ds[1];
	pz[1] = ds[0];
	pz[2] = 0.0;
	
	px[0] = -ds[2]*ds[0];
	px[1] = -ds[2]*ds[1];
	px[2] = ds[0]*ds[0]+ds[1]*ds[1];
	
	VectorNormalizeFast(pz);
	VectorNormalizeFast(px);
	
	float m = 0.785398; // 2*pi/8
	float pitch, bestPitch;
	float best = 40000.0;
	
	// check trajectories on expanding ring from target center
	for (int z=1; z<6; z++) {
		for (int a=0; a<8; a++) {
			VectorMA(point, 150.0*(float)z*sinf((float)a*m), px, p);
			VectorMA(p, 150.0*(float)z*cosf((float)a*m), pz, p);
			
			// get angle
			if (!ballisticPitchAngle(eth32.cg.muzzle, p, 2000.0, 3.0, NULL, &pitch))
				continue;
			
			// get launch dir
			VectorSubtract(p, eth32.cg.muzzle, ds);
			dv[0] = ds[0]; dv[1] = ds[1]; dv[2] = 0;
			VectorNormalizeFast(dv);
			dv[0] = dv[0]*cosf(pitch);
			dv[1] = dv[1]*cosf(pitch);
			dv[2] = sinf(pitch);
			
			// determine if we "hit" target
			rifleEndPos(dv, t, fltime);
			
			VectorSubtract(t, point, ds);
			if (DotProduct(ds, ds) < best) {
				best = DotProduct(ds, ds);
				bestPitch = pitch;
				VectorCopy(p, grenadeTarget->aimPt);
			}
		}
	}
	
	if (best < 40000.0) {
		*ptch = bestPitch;
		return true;
	} else	
		return false;
}

void CAimbot::grenadeAimbot(vec3_t point, float *fltime, bool *solution, bool pass)
{
	float pitch, flytime;
	bool doAim = false;

	RifleMultiBounce = false;
	
	// needed for prediction
	*solution = false;

	// grenades need senselock a *little* longer
	if (eth32.cg.time < (grenadeFireTime+300))
		LockSensitivity(true);

	if (eth32.cg.currentWeapon->attribs & WA_GRENADE) {
		flytime = 5.0;
		point[2] += eth32.settings.grenadeZ;

		// for validation of trajectory for grenades, we need an extra pass because more solutions are possible
		*solution = GrenadeFireOK = grenadePitchAngle(eth32.cg.muzzle, point, 5.0, &flytime, &pitch, false);
		if (eth32.settings.valGrenTrajectory && !ballisticTrajectoryValid(eth32.cg.muzzle, grenadeTarget->lerpOrigin, pitch, flytime, speedForPitch(pitch))){
				*solution = GrenadeFireOK = grenadePitchAngle(eth32.cg.muzzle, point, 5.0, &flytime, &pitch, true);
				if (!ballisticTrajectoryValid(eth32.cg.muzzle, grenadeTarget->lerpOrigin, pitch, flytime, speedForPitch(pitch)))
					GrenadeFireOK = false;
		}

		// fire control logic not needed for first prediction pass
		if (!pass) {
			*fltime = flytime;
			return;
		}

		// select conditions to base further actions on
		if (eth32.settings.grenadeAutoFire) {
			if (GrenadeTicking) {
				int trigger = eth32.settings.grenadeFireDelay+(int)(flytime*1000.0);

				if (GrenadeFireOK && eth32.cg.snap->ps.grenadeTimeLeft && eth32.cg.snap->ps.grenadeTimeLeft < trigger){
					// fire!
					atkBtn->active = 0;

					// need to save fire time - grenades arent launched right when we release it
					// so need to lock sensitivity a bit longer
					grenadeFireTime = eth32.cg.time;
					doAim = true;
					GrenadeTicking = false;
					stopAutoTargets = false;
				} else {
					// keep tickin'
					atkBtn->active = 1;
				}
			}

			// user requested to fire the nade in autofire mode, so activate the logic
			if (!GrenadeTicking && this->attackPressed) {
				GrenadeTicking = true;
				atkBtn->active = 1;
			}

			// make sure we drop the potato if its getting too hot
			if (eth32.cg.snap->ps.grenadeTimeLeft && eth32.cg.snap->ps.grenadeTimeLeft < 500){
				GrenadeTicking = false;
				stopAutoTargets = false;
				atkBtn->active = 0;

				if (GrenadeFireOK) {
					grenadeFireTime = eth32.cg.time;
					doAim = true;
				}
			}

		} else {
			// non-autofire mode
			if (!this->attackPressed)
				grenadeFireTime = 0;

			if (this->attackPressed)
				GrenadeTicking = true;
			else if (GrenadeTicking){
				// released attack, which means fire!
				grenadeFireTime = 0;
				GrenadeTicking = false;
				if (GrenadeFireOK) {
					grenadeFireTime = eth32.cg.time;
					doAim = true;
				}
			}
		}

		if (GrenadeFireOK && eth32.settings.grenadeSenslock)
			doAim = true;

	} else if (eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE) {
		point[2] += eth32.settings.riflenadeZ;
		GrenadeFireOK = ballisticPitchAngle(eth32.cg.muzzle, point, 2000.0f, 5.0f, &flytime, &pitch);
		if (GrenadeFireOK){
			*solution = true;
			Lastpitch = pitch;
			if (eth32.settings.valRifleTrajectory && !ballisticTrajectoryValid(eth32.cg.muzzle, point, pitch, flytime, 2000.0f))
				GrenadeFireOK = false;

			if ((this->attackPressed || eth32.settings.rifleAutoFire) && GrenadeFireOK){
				doAim = true;
				atkBtn->wasPressed = 1;
				stopAutoTargets = false;
			}			
		}
		
		if (!GrenadeFireOK){		
			if (eth32.settings.allowMultiBounce && multibounceAimbot(point, &flytime, &pitch)) {
				*solution = GrenadeFireOK = RifleMultiBounce = true;
				Lastpitch = pitch;
				
				if ((this->attackPressed || eth32.settings.rifleAutoFire)){					
					atkBtn->wasPressed = 1;
					stopAutoTargets = false;
					doAim = pass = true;
				}
			} else {			
				GrenadeTicking = false;
				GrenadeFireOK = false;
			}
		}
	}

	if (doAim && pass) {
		LockSensitivity(true);
		
		if (!RifleMultiBounce)
			VectorCopy(point, grenadeTarget->aimPt);
		
		if (eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE){
			Aimbot.PointGrenade(eth32.cg.muzzle, pitch);
		} else if ((eth32.cg.currentWeapon->attribs & WA_GRENADE)) {
			vec3_t newpt;
			vec3_t right;

			// correct for pitch angle ( forward != grenade launchdir )
			grenadePitchCorrection(pitch, &pitch);

			// set different muzzle ( eth32.cg.muzzle != grenade muzzle )
			AngleVectors(eth32.cg.refdefViewAngles, NULL, right, NULL);
			VectorMA(eth32.cg.muzzle, 20, right, newpt);
			newpt[2] -= 8;
			Aimbot.PointGrenade(newpt, pitch);
		}
	}

	// needed for prediction
	*fltime = flytime;
}
inline void CAimbot::applyTargPrediction()
{
 	if (eth32.settings.predTargzor)
	VectorMA(target->bodyPt,  (eth32.cg.ping/2) * 0.001f, target->currentState->pos.trDelta, target->bodyPt);

	VectorMA(target->headPt, (eth32.cg.ping/2)  * 0.001f, target->currentState->pos.trDelta, target->headPt);
 	}
inline void CAimbot::applySelfPrediction()
{
	vec3_t displ, v1, vp, p, vs;
	float dt;

	if (eth32.cg.predictedps)
		VectorCopy(eth32.cg.predictedps->velocity, v1);
	else
		VectorCopy(eth32.cg.snap->ps.velocity, v1);

	switch (eth32.settings.predSelfType)
	{
		case SPR_OFF:
			return;
		case SPR_MANUAL:
			VectorMA(target->aimPt, eth32.settings.predSelf, target->currentState->pos.trDelta, target->aimPt);
			return;
		case SPR_SIMPLE:
			dt = -1.0f*Engine.FrameCorrectionTime(2);
		case SPR_AVG:
			dt = -0.2f*Engine.FrameCorrectionTime(10);
		case SPR_STDERR:
			dt = -1.0f*Engine.FrameCorrectionTime(-1);
			break;

		default:
			return;
	}

	VectorSubtract(target->aimPt, eth32.cg.muzzle, p);
	VectorNormalizeFast(p);

	VectorScale(p, DotProduct(vs, p), v1);
	VectorSubtract(vs, v1, vp);
	VectorScale(vp, dt, displ);

	VectorAdd(target->aimPt, displ, target->aimPt);
}

bool CAimbot::IsPointVisible(vec3_t start, vec3_t pt, int skipEntity)
{
	trace_t t;

	orig_CG_Trace(&t, start, NULL, NULL, pt, skipEntity, MASK_SHOT);

	return (t.fraction == 1.f);
}

void CAimbot::Point(vec3_t vieworg)
{
	vec3_t org, ang;

	// sol: this check can probably go away, since box trace is checking for invalid box origins
	if (VectorCompare(target->aimPt, vec3_origin))
		return;

	if (eth32.settings.drawBulletRail && eth32.cg.snap->ps.weaponTime < 40)
		Engine.MakeRailTrail( eth32.cg.muzzle, target->aimPt, false, eth32.settings.colorBulletRail, 40);

	// Get the vector difference and calc the angle form it
	VectorSubtract(target->aimPt, vieworg, org);
	Tools.VectorAngles(org, ang);

	// Clip & normalize the angle
	ang[PITCH] = -ang[PITCH];
	ANG_CLIP(ang[YAW]);
	ANG_CLIP(ang[PITCH]);
	AnglesToAxis(ang, eth32.cg.refdef->viewaxis);
	ang[YAW] -= eth32.cg.refdefViewAngles[YAW];
	ANG_CLIP(ang[YAW]);
	ang[PITCH] -= eth32.cg.refdefViewAngles[PITCH];
	ANG_CLIP(ang[PITCH]);

	if (ang[YAW] < -180.0 || ang[YAW] > 180.0 || ang[PITCH] < -180.0 || ang[PITCH] > 180.0)
		return;
	// Add the angle difference to the view (i.e aim at the target)
	*(float *)eth32.game.viewAngleX += ang[YAW];
	*(float *)eth32.game.viewAngleY += ang[PITCH];
}

void CAimbot::PointGrenade(vec3_t vieworg, float pitch)
{
	vec3_t org, ang;

	// Get the vector difference and calc the angle form it
	VectorSubtract(grenadeTarget->aimPt, vieworg, org);
	Tools.VectorAngles(org, ang);

	ang[PITCH] = -pitch*180/M_PI;
	ANG_CLIP(ang[YAW]);
	ANG_CLIP(ang[PITCH]);
    AnglesToAxis(ang, eth32.cg.refdef->viewaxis);
    ang[YAW] -= eth32.cg.refdefViewAngles[YAW];
	ANG_CLIP(ang[YAW]);
    ang[PITCH] -= eth32.cg.refdefViewAngles[PITCH];
	ANG_CLIP(ang[PITCH]);

	// Add the angle difference to the view (i.e aim at the target)
	*eth32.game.viewAngleX += ang[YAW];
	*eth32.game.viewAngleY += ang[PITCH];
}

// best of both worlds ;)
void CAimbot::LockSensitivity(bool lock){ senslocked = lock; }
bool CAimbot::SensitivityLocked(){ return senslocked; }
void CAimbot::SetAttackButton( void *ptr ) { atkBtn = (kbutton_t *)ptr; }
void CAimbot::SetCrouchButton( void *ptr ) { crhBtn = (kbutton_t *)ptr; }

void CAimbot::Autocrouch(bool enable, bool force)
{
	static bool autoCrouching = false;
	acFirstFrame = false;

	// force release of crouch, else tapout wont work :(
	if (!enable && force) {
		crhBtn->active = crhBtn->down[0] = crhBtn->down[1] = 0;
		autoCrouching = false;
		return;
	}

	// dont bother if user is manually crouching
	if ((userCrouching && !autoCrouching) || !eth32.settings.autoCrouch){
		autoCrouching = false;
		return;
	}

	if (!autoCrouching && enable){
		// first check aimpt is still visible after crouch to prevent oscilation
		eth32.cg.muzzle[2] -= 24.0;
		if (IsPointVisible(eth32.cg.muzzle, target->aimPt, target->clientNum)){
			crhBtn->active = crhBtn->wasPressed = 1;
			crhBtn->down[0] = -1;
			crhBtn->downtime = *eth32.game.com_frameTime;
			autoCrouching = true;
			acFirstFrame = true;
		}
		eth32.cg.muzzle[2] += 24.0;
	} else if (autoCrouching && !enable) {
		crhBtn->active = crhBtn->down[0] = crhBtn->down[1] = 0;
		autoCrouching = false;
	}
}

void CAimbot::Autofire(bool enable)
{
	static int lastShotTime = 0;

	// don't mess with attack if not in control
	if (!autoMode)
		return;

	// check for heat
	if (enable && (eth32.cg.currentWeapon->attribs & WA_OVERHEAT) && eth32.cg.snap->ps.curWeapHeat > 200)
		enable = false;

	// User has selected a delay, so do a fire and release
	if (eth32.cg.currentWeapon && eth32.cg.currentWeapon->delay && enable) {
		atkBtn->active = 0;
		if ((eth32.cg.cgTime - lastShotTime) >= eth32.cg.currentWeapon->delay) {
			atkBtn->wasPressed = 1;
			lastShotTime = eth32.cg.cgTime;
		}
		return;
	}

	if (enable)
		atkBtn->active = 1;			// set fire
	else
		atkBtn->active = 0;
}

void CAimbot::SetUserCrouchStatus(bool crouched)  { userCrouching = crouched; }
void CAimbot::SetUserAttackStatus(bool atkPressed){ attackPressed = atkPressed; }

float CAimbot::CrosshairDistance(const player_t *player)
{
	int x_dif=0, y_dif=0;

	// Calculate obtuse & adjacent
	player->screenX > eth32.cg.centerX ? x_dif = player->screenX - eth32.cg.centerX : x_dif = eth32.cg.centerX - player->screenX;
	player->screenY > eth32.cg.centerY ? y_dif = player->screenY - eth32.cg.centerY : y_dif = eth32.cg.centerY - player->screenY;

	// Return hypotenuse
	float h2 = ((x_dif*x_dif)+(y_dif*y_dif));

	// kobject: no need, h2 is positive definite.
	//return sqrt(h2);
	return h2;
}

int CAimbot::CheckFov(vec3_t origin)
{
	if (AIM_FOV_VALUE > 359.f)
		return 1;

	vec3_t dir;
	float ang;
	VectorSubtract( origin, eth32.cg.refdef->vieworg, dir );
	VectorNormalizeFast( dir );
	ang = 57.3*acos(DotProduct( dir, eth32.cg.refdef->viewaxis[0] ));

	if (ang <= AIM_FOV_VALUE)
		return 1;
	else
		return 0;

}

void CAimbot::History(player_t *player)
{
	if (!player)
		return;
	
	if (eth32.settings.ballisticPredict == RF_PREDICT_OFF)
		return;
	
	if (!(eth32.cg.currentWeapon->attribs & WA_BALLISTIC) || !eth32.settings.grenadeBot)
		return;
	
	int cidx = player->history_idx - 1;
	if (cidx < 0)
		cidx = MAX_HISTORY;
	
	if (eth32.cg.time - (int)player->history[cidx].t < 15)
		return;
	
	VectorCopy(player->lerpOrigin, player->history[player->history_idx].p);
	player->history[player->history_idx].t = (float) eth32.cg.time;
	                
	if (player->history_idx == MAX_HISTORY)
		player->history_idx = 0;
	else
		player->history_idx++;	                
}

/* for now use the flytime as cutoff, approx 1 second */
void CAimbot::PredictPlayer(player_t *player, float time, vec3_t pos, int type)
{
	if (!player)
		return;
	
	float tf = time;					// predict ahead time
	float tc = (float)eth32.cg.time;	// current time
	
	if (type < 0)
		type = eth32.settings.ballisticPredict;
	
	switch (type)
	{
		case RF_PREDICT_OFF:
			VectorCopy(player->lerpOrigin, pos);
			return;
		case RF_PREDICT_LINEAR: {
			VectorMA(player->lerpOrigin, tf, player->currentState->pos.trDelta, pos);
			return;
		}
		case RF_PREDICT_LINEAR2: {
			VectorMA(player->lerpOrigin, 0.5*tf, player->currentState->pos.trDelta, pos);
			return;			
		}
		case RF_PREDICT_AVG: {
			int n = 0;
			double dx, dy, dz;
			dx = dy = dz = 0;
			
			for (int i=0; i<=MAX_HISTORY; i++) {
				if (tc - player->history[i].t > tf)
					continue;
				
				dx += (double)player->history[i].p[0];				
                dy += (double)player->history[i].p[1];	
                dz += (double)player->history[i].p[2];	
				n++;
			}
			
			double q = 1.0/(double)n;
			dx = dx * q;
			dy = dy * q;
			dz = dz * q;
		
			pos[0] = (float)dx;
			pos[1] = (float)dy;
			pos[2] = (float)dz;			
		
			return;
		}
		
		case RF_PREDICT_SMART: {
			int cidx = player->history_idx - 1;
			if (cidx < 0)
				cidx = MAX_HISTORY;	
			
			vec3_t p, ds;
			bool pass = true;
			
			// check for constancy in position
			VectorCopy(player->history[cidx].p, p);
			
			for (int i=cidx-1; i>=0; i--) {
				if (tc - player->history[i].t > tf)
					continue;				
				VectorSubtract(p, player->history[i].p, ds);
				if (DotProduct(ds, ds) > 22500.0) {
					pass = false;
					break;
				}
			}
			
			if (pass) {
				for (int i=MAX_HISTORY; i>cidx; i--) {
					if (tc - player->history[i].t > tf)
						continue;					
					VectorSubtract(p, player->history[i].p, ds);
					if (DotProduct(ds, ds) > 22500.0) {
						pass = false;
						break;
					}	
				}		
			}
			
			if (pass) {
				//printf("average\n");
				PredictPlayer(player, time, pos, RF_PREDICT_AVG);
				return;
			}
			
			// moving, so check for velocity constancy
			pass = true;
			
			vec3_t dv, v0;
			float dt;
			float v2;
			int c = cidx -1;
			if (c < 0)
				c = MAX_HISTORY;
			
			dt = (player->history[cidx].t - player->history[c].t)*0.001;
			VectorSubtract(player->history[cidx].p, player->history[c].p, ds);
			VectorScale(ds, 1.0/dt, v0);
			v2 = 0.8*DotProduct(v0, v0);
			
			for (int i=cidx-1; i>0; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);
				
				if (DotProduct(dv, v0) < v2) {
					pass = false;
					break;
				}
			}
			
			if (pass) {
				for (int i=MAX_HISTORY; i>cidx+1; i--) {
					if (tc - player->history[i].t > tf)
						continue;
					dt = (player->history[i].t - player->history[i-1].t)*0.001;
					VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
					VectorScale(ds, 1.0/dt, dv);
					
					if (DotProduct(dv, v0) < v2) {
						pass = false;
						break;
					}	
				}		
			}
			
			if (pass) {
				//printf("linear\n");
				PredictPlayer(player, time, pos, RF_PREDICT_LINEAR);
				return;								
			}
			
			// player is apparantly moving erraticly, so give rough extrap.
			int count = 0;
			int n;
			
			vec3_t v1, v3;			
			VectorCopy(vec3_origin, v1);
			VectorCopy(vec3_origin, v3);
			
			n = c = 0;
			
			for (int i=cidx; i>0; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);
				
				if (count < MAX_HISTORY/2) {
					VectorAdd(v1, dv, v1);
					n++;
				} else {
					VectorAdd(v3, dv, v3);
					c++;
				}
				count++;				
			}
			
			for (int i=MAX_HISTORY; i>cidx+1; i--) {
				if (tc - player->history[i].t > tf)
					continue;				
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);
				
				if (count < MAX_HISTORY/2) {
					VectorAdd(v1, dv, v1);
					n++;
				} else {
					VectorAdd(v3, dv, v3);
					c++;
				}
				count++;				
			}
						
			VectorScale(v1, 1.0/(float)n, v1);
			VectorScale(v3, 1.0/(float)c, v3);
			
			// make the latest velocities count more
			VectorMA(pos, 0.75*tf, v1, pos);
			VectorMA(pos, 0.25*tf, v3, pos);
		}
	}		
}
