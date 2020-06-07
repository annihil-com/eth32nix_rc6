// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#define MAX_PERL_SCRIPTS	16
#define MAX_PERL_WINDOWS	16

// for eth32 command
typedef struct perlInfoWindow_s {
	float 	x;
	float 	y;
	float 	scale;						// text scale
	bool 	active;
	char 	text[MAX_STRING_CHARS+1];
	int 	owner;						// owner script (id)
} perlInfoWindow_t;

typedef struct perlTask_s {
	uint32 			startTime;		// start of the process
	perlScript_t	*script;		// the running script (NULL means free slot)
	pthread_t		vid;			// thread id of script
	int				id;				// iterator id
	FILE			*pfd;			// process fd
	char 			command[256];	// command line 
	bool			wantexit;		// was exit requested
	perlInfoWindow_t	*win[MAX_PERL_WINDOWS];	// windows
} perlTask_t;

class CPerl
{
public:
	void Init();
	void Info();
	void PostFrame();
	void Kill(int id);
	void AddPerlScript(char *path, char *name);
	void GetPerlScripts(char *path);
	void ParsePerlCommand(perlScript_t *pl);	
	void closePipe(perlTask_t *pl, bool nice);
	void Print(const char *txtFmt, ...);	
	bool parseEth32Command(StringTokenizer &buf, std::string &cmd, perlTask_t *me);
	
private:		
	perlTask_t ptasks[MAX_PERL_SCRIPTS];
	pthread_mutex_t *mutex;
	pthread_mutex_t *mutex_fd;
	int	tasksLaunched;
};

extern CPerl Perl;
extern perlScript_t *perl_scripts;
extern perlInfoWindow_t perl_windows[MAX_PERL_WINDOWS];
