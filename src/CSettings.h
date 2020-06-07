// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "settings.h"

class CSettings
{
public:
	void Load(const char *filename);
	void Save(const char *filename);
	bool SaveUserSettings(const char *filename);

	void ConsoleCommand(void);
	void Toggle(void);
private:
	void PrintAll(void);
	void PrintVarLine(const settingdef_t *var, char color);
	void PrintVarDescription(const settingdef_t *var);
	void PrintValue(const settingdef_t *var, char *val);
	bool symbolicDiscription(char *buf, const settingdef_t *setting);
};

extern CSettings Settings;
