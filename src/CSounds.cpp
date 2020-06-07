// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include "puresounds.h"

CSounds Sounds;

// when replaced sounds are used, a lookup table is generated to quickly switch sounds on the fly.
// the index is a regular sfxHandle, and it points to a eth32 sound struct

int csnd_hash[8192];

void CSounds::Init()
{
	gameSounds = NULL;
	//ignoreSounds = NULL;
	nGameSounds = nIgnore = 0;
	memset(csnd_hash, 0, sizeof(csnd_hash));	
}

void CSounds::ShutDown()
{
	if (gameSounds) free(gameSounds);
	//if (*ignoreSounds) free(ignoreSounds);
	nGameSounds = nIgnore = 0;
}

void CSounds::Respawn()
{
	spree = -2;	
}

// all legit sounds pass trough here, therefore, 
// sounds that dont exist csnd_hash are eth32 sounds
int CSounds::HookSounds(char *name, int compressed)
{	
	if (!name)
		return 0; 
		
	gameSounds = (eth32Sounds_t *)realloc(gameSounds, (nGameSounds+1)*sizeof(eth32Sounds_t));
	strncpy(gameSounds[nGameSounds].name, name, 255);
			
	sfxHandle_t snd = Syscall.S_RegisterSound(name, (qboolean)compressed);
	gameSounds[nGameSounds].orig = snd;
	
	gameSounds[nGameSounds].replaced = false;
	
	if (existReplacement(name)){
		char csnd[256];
		char *base = (char*)strrchr(name, '/');
		char *snd = base+1;
		csnd[0] = '\0';
		if (base) {
			strncat(csnd, name, base-name);
			strcat(csnd, "/eth32_");
			strcat(csnd, snd);
		} else
			sprintf(csnd, "eth32_%s", name);
	
		gameSounds[nGameSounds].custom = Syscall.S_RegisterSound(csnd, qtrue);
		gameSounds[nGameSounds].replaced = true;
	} else
		gameSounds[nGameSounds].custom = -1; 
		
	int j = 0;
	gameSounds[nGameSounds].pure = false;
	while (pureSounds[j]) {
		if (!strcmp(pureSounds[j], name)){
			gameSounds[nGameSounds].pure = true;
			break;
		}
		j++;				
	}
	
	gameSounds[nGameSounds].hqsound = !strncmp(name, "hq_", 3) ? true : false;
	
	csnd_hash[(int)snd] = nGameSounds;
	nGameSounds++;	
	return snd;
}

// test wether a sound has a replacement :)
bool CSounds::existReplacement(char *sound)
{
	char csnd[256];
	char *base = (char*)strrchr(sound, '/');
	char *snd = base+1;
	csnd[0] = '\0';
	
	if (base) {
		strncat(csnd, sound, base-sound);
		strcat(csnd, "/eth32_");
		strcat(csnd, snd);
	} else
		sprintf(csnd, "eth32_%s", sound); 
	
	fileHandle_t f;
	int len = Syscall.FS_FOpenFile(csnd, &f, FS_READ);

	if (!f)
		return false;

	Syscall.FS_FCloseFile(f);
	return true;	
}

bool CSounds::Process(int type, int *args)
{
	if (!gameSounds)
		return true;
	
	int sndHnd = (type == CG_S_STARTLOCALSOUND) ? args[0] : args[3];
	eth32Sounds_t *snd = &gameSounds[csnd_hash[sndHnd]];
	
	if (!eth32.settings.hqSounds && snd->hqsound)
		return false;
		
	if (eth32.settings.pureSounds && !snd->pure)
		return false;		
		
	if (eth32.settings.customSounds && snd->replaced) {
		if (type == CG_S_STARTLOCALSOUND) args[0] = (int)snd->custom;
		else args[3] = (int)snd->custom;
	}
	
	return true;
}

// spree is actually the multikill amount
// a multikill sound gets played if spree works its way up to a value equal to or above 0
// meaning double kill or more
void CSounds::ProcessMultikillSounds( void )
{
	Syscall.S_FadeAllSound(-1, 0, qtrue); // stop sound 
	Syscall.S_FadeAllSound(1.0f, 0, qfalse); // fade sound back up after stop above 
	
	spree++;

	if( (eth32.cg.time - soundDelay) > eth32.settings.mkResetTime ){
		spree = 0;
		soundDelay = eth32.cg.time;
		return;
	}

	if (spree >= 0) {
		if(spree >= SOUND_LEVEL_MAX)
			spree = SOUND_LEVEL_MAX - 1;
		
		randNum = (rand() % (SOUND_LEVEL_SIZE_MAX));	
		Syscall.S_StartLocalSound(eth32.cg.media.sounds.playsound[spree][randNum], CHAN_LOCAL_SOUND);
	}
}
// still not the best but working :>
const char *defaultSounds[25] =
{
	// level 1
	"sounds/wowMan.wav",
	"sounds/haha.wav",
	"sounds/excellent.wav",
	"sounds/spreeLevelTwo/combowhore.wav",
	"sounds/boomheadshot.wav",
	
	"sounds/wowMan.wav",
	"sounds/haha.wav",
	"sounds/haha.wav",
	"sounds/spreeLevelTwo/combowhore.wav",
	"sounds/boomheadshot.wav",
	
	// level 3
	"sounds/hattrick.wav",
	"sounds/multikillw.wav",
	"sounds/multikill.wav",
	"sounds/wickedsick.wav",
	"sounds/triple.wav",
	
	// level 4
	"sounds/rampage.wav",
	"sounds/triple2.wav",
	"sounds/ultrakill.wav",
	"sounds/unstoppable.wav",
	"sounds/monsterkill.wav",
	
	// level 5
	"sounds/monsterkillw.wav",
	"sounds/holy.wav",
	"sounds/unreal.wav",
	"sounds/ludic.wav",
	"sounds/holy.wav"
};

void CSounds::RegisterSounds( void )
{
	sprintf(settingsFile, "%s/%s", eth32.path, ETH32_SETTINGS);

	eth32.cg.media.sounds.hitsound = Syscall.S_RegisterSound("sounds/etprohitsound.wav", qtrue);
	eth32.cg.media.sounds.hitsoundtm = Syscall.S_RegisterSound("sounds/etprohittm.wav", qtrue);
	
	char value[1024]; 
	char buf[64]; 
	int soundName = 0; 
	
	for(int t = 0; t < SOUND_LEVEL_MAX; t++){ 
		for(int s = 0; s < SOUND_LEVEL_SIZE_MAX; s++) { 
			sprintf(buf, "sound%i%i", t, s); 
			Tools.GetPrivateProfileString("Sound Settings", buf, (char*)defaultSounds[soundName], value, sizeof(value), settingsFile); 
	                    
			eth32.cg.media.sounds.playsound[t][s] = Syscall.S_RegisterSound(value, qtrue);			 
			soundName++; 
		} 
		
	} 

	this->spree = 0;

#ifdef ETH32_DEBUG
	Debug.Log("All Game Sounds Registered...");
#endif
}


