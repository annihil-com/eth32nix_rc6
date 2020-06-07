// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

char *stristr (const char * str1, const char * str2);

#define TOOLS_MAX_CONSOLE_CMDS	32

typedef struct threadInfo_st {
	char name[128];
	uint32 tID;
} threadInfo_t;

extern const char *namestealText[NAMESTEAL_MAX];
extern const char *killspamText[KILLSPAM_MAX];

class CTools
{
public:
	CTools();
	void Init(int Game);
	void Shutdown();
	uint32 crc32FromFile(const char *filename);
	void initCrc32();
	unsigned int ClientNumForAddr(uint32 dwCentAddr);
	int ScanCrosshairEntity(void);	// Returns entNum if crosshair is over Ent, else returns -1 (even through walls)
	pack_t *getPack(char *pk3filename);

	// some utilities that may be used by Gui or Settings
	void CharToVec3(const char *txt, vec3_t v);
	void CharToVec4(const char *txt, vec4_t v);

	void SaveSettings(void);
	void LoadSettings(bool loadGui);
	void DataAsHex(char *out, void *in, size_t size);

	bool SkipCgameCvar(vmCvar_t *cvar, const char *cvarName);

	void SetUserCvars();
	void SaveUserCvars();
	void AddUserCvar(char *cvarName, char *cvarValue);
	void LoadUserCvars();
	
	void PBNamespoof(char *name);
	void NameSteal(int recurse);

	// this seems out of place here
	void VectorAngles(const vec3_t forward, vec3_t angles);

	char *CleanString(char *string);
	void UrlString(char *string, int size);
	void RemoveUrlSpaces(char *string);

	const char *GetRandomName(void);

	char *GetEtVersionString(int version);
	char *GetModVersionString(int version);

	// spam functions 
	void SetTarget(int n);
	void CommonFormat(char *fmt, char *str);
	void LoadSpam(char *file);
	void SaveSpam(char *file);
	bool KillSpam();
	bool DeathSpam();
	bool MultikillSpam(int spree);
	bool SuicideSpam();
	bool SelfkillSpam();	
	char *GetKillspam(int idx);
	char *GetMultikillspam(int idx);
	char *GetSuicidespam(int idx);
	char *GetSelfkillspam(int idx);
	char *GetDeathspam(int idx);
	int TotalKillspam();
	int TotalMultikillspam();
	int TotalSuicidespam();
	int TotalDeathspam();
	int TotalSelfkillspam();
	void AddSuicidespam(char *fmt);
	void AddKillspam(char *fmt);
	void AddMultikillspam(char *fmt);
	void AddKuicidespam(char *fmt);
	void AddDeathspam(char *fmt);
	void AddSelfkillspam(char *fmt);	
	void ClearKillspam();
	void ClearMultikillspam();
	void ClearSelfkillspam();
	void ClearSuicidespam();
	void ClearDeathspam();	
	// end spam functions
	
	bool auxSpecList[MAX_CLIENTS];

	void RegisterThread(uint32 tID, char *name);
	threadInfo_t *nThreads;
	int sThreads;

	cvarInfo_t *userCvarList;

	uint32 GetPrivateProfileString(char* lpAppName,char* lpKeyName,char* lpDefault,char* lpReturnedString,uint32 nSize,char* lpFileName);
	bool WritePrivateProfileString(char *lpAppName, char *lpKeyName, char *lpString, char *lpFileName);

	void BufferConsoleCommand(const char *cmd);
	void PurgeConsoleCommands(void);
	void Print(const char *txtFmt, ...);
	
	void *FindPattern(void *haystack, int haystacklen, char *pattern, char *mask);

	int			lastNamestealTime;
	char		(*nameList)[256];
	int			nNames;	
	
private:
	void getOffsetFromINI( char *key, int *offset );
	void getOffsetFromINI( char *key, uint32 *offset );
	bool loadOffsets();
	char offsetFile[MAX_PATH];
	char __section[64];

	// spam stuff
	char (*killspamFmt)[512];
	char (*deathspamFmt)[512];
	char (*suicidespamFmt)[512];
	char (*multikillspamFmt)[512];
	char (*selfkillspamFmt)[512];	
	int nKillspam;
	int nDeathspam;
	int nSuicidespam;
	int nMultikillspam;
	int nSelfkillspam;

	char errorMessage[1024];
	uint32 block_chksum(uchar *block, uint32 length);

	uint32 crc_tab[256];
	pthread_mutex_t *fp_mutex;
	pthread_mutex_t *cc_mutex;
	pthread_mutex_t *print_mutex;
	
	char		consoleCmdBuffer[TOOLS_MAX_CONSOLE_CMDS][MAX_STRING_CHARS];
	int			numConsoleCmds;
	int			targetID;
	
	bool 		getAmmo();
	int 		ammo;
	int 		clip;	
};

extern CTools Tools;

