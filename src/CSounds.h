#pragma once

typedef struct eth32Sounds_s {
	char name[256];
	sfxHandle_t	orig;
	sfxHandle_t	custom;
	bool replaced;
	bool pure;
	bool hqsound;
	bool disabled;
} eth32Sounds_t;

class CSounds
{
public:
	void	Init();
	void	ShutDown();
	void	ProcessMultikillSounds();
	void	RegisterSounds();
	void	Respawn();
	bool	Process(int type, int *args);
	int		HookSounds(char *name, int compressed);

private:
	bool	existReplacement(char *sound);

	int		randNum;
	int		soundDelay;
	int		spree;
	char	settingsFile[MAX_PATH];

	char	buf[64];
	char	value[1024];
	char	soundFile[MAX_PATH];
	
	int		nIgnore;					// sounds to be ignored
	char	*ignoreSounds[MAX_PATH];	// sounds to be ignored
	
	int				nGameSounds;		// registered gamesounds
	eth32Sounds_t	*gameSounds;		// associated structure
};
extern CSounds Sounds;

extern const char *defaultSounds[25];
