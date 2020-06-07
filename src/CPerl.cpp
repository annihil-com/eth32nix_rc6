// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

using namespace std;

CPerl Perl;
perlScript_t *perl_scripts = NULL;
perlInfoWindow_t perl_windows[MAX_PERL_WINDOWS];

void CPerl::PostFrame()
{
	for (int n=0; n<MAX_PERL_WINDOWS; n++) {
		if (perl_windows[n].active && perl_windows[n].text[0]){
			Draw.MultiText(perl_windows[n].x, perl_windows[n].y, GUI_FONTSCALE*perl_windows[n].scale, perl_windows[n].text, GUI_FONT);
		}
	}	
}

int perlScriptsOnly(const struct dirent *dir)
{
	char *s = strrchr(dir->d_name, '.');
	if (!s)
		return 0;

	if (!strcasecmp(s, ".pl"))
		return 1;
	else
		return 0;
}

void CPerl::AddPerlScript(char *path, char *name)
{
	int cnt = 0;
	perlScript_t *c = perl_scripts;
	while (c) {
		cnt++;
		c = c->next;
	}

	uint32 oldBase = (uint32)perl_scripts;
	perl_scripts = (perlScript_t *)realloc(perl_scripts, (cnt+1)*sizeof(perlScript_t));
	memset(&perl_scripts[cnt], 0, sizeof(perl_scripts[cnt])); 

	// realloc can also relocate the memory block, so we need to adjust the ptrs accordingly
	// (it does not fragment)
	c = perl_scripts;
	while (c) {
		if (c->next)
			c->next = (perlScript_t *)((uint32)c->next+(uint32)perl_scripts-oldBase);
		c = c->next;
	}

	sprintf(perl_scripts[cnt].path, "%s/%s", path, name);
	char *s = strstr(name, ".pl");
	if (!s) FATAL_ERROR("this shouldn't happen")
	*s = '\0';
	strcpy(perl_scripts[cnt].basename, name);

	if (cnt>0)
		perl_scripts[cnt-1].next = &perl_scripts[cnt];
}

void CPerl::Info()
{
	perlTask_t *n;
		
	uint32 t = *eth32.game.com_frameTime;
	Tools.Print("^d%2s   ^3%16s   ^2%s\n", "id", "task", "time");	
	Tools.Print("^9----------------------------\n");		
	for(int i=0; i<MAX_PERL_SCRIPTS; i++) {
		n = &ptasks[i];
		if (n->script) {			
			Tools.Print("^d%2i   ^3%16s   ^2%u\n", n->id, n->script->basename, t-n->startTime);	
		}
	}
}

void CPerl::Kill(int pId)
{
	perlTask_t *n;
	int i;
	for(i=0; i<MAX_PERL_SCRIPTS; i++) {
		n = &ptasks[i];
		if (n->id == pId && n->script) {
			n->wantexit=true;
			Tools.Print("^3script %i terminate scheduled\n", n->id);
			break;	
		}
	}
	
	if (i == MAX_PERL_SCRIPTS)
		Tools.Print("^3script %i invalid or not running\n", pId);		
}

void CPerl::closePipe(perlTask_t *pl, bool nice)
{	
	if (pthread_mutex_lock(mutex_fd)) FATAL_ERROR("could not lock mutex")
	if (pl && pl->pfd)
		pclose(pl->pfd);
	
	if (pl)
		pl->pfd = NULL;		
	if (pthread_mutex_unlock(mutex_fd)) FATAL_ERROR("could not unlock mutex")
}

void CPerl::Init()
{
	mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex,NULL);	
	
	mutex_fd = new pthread_mutex_t;
	pthread_mutex_init(mutex_fd,NULL);
	
	memset(ptasks, 0, sizeof(ptasks));
	memset(perl_windows, 0, sizeof(perl_windows));
}

void CPerl::GetPerlScripts(char *path)
{
	int n, j = 0;
	struct dirent **filelist;

	if (perl_scripts)
		free(perl_scripts);

	n = scandir(path, &filelist, perlScriptsOnly, alphasort);
	if (n < 0){
#ifdef ETH32_DEBUG
		Debug.Log("getPerlScripts: scandir error");
#endif
		return;
	}
	else {
		while(n--) {
#ifdef ETH32_DEBUG
			Debug.Log("Loaded perl script [%s/%s]", path, filelist[n]->d_name);
#endif
			AddPerlScript(path, filelist[n]->d_name);
			free(filelist[n]);
			j++;
		}
		free(filelist);
	}

#ifdef ETH32_DEBUG
	Debug.Log("done loading perl scripts");
#endif
}

bool CPerl::parseEth32Command(StringTokenizer &buf, std::string &cmd, perlTask_t *me)
{
	if (cmd == std::string("cwin")) {
		int winId = buf.nextIntToken();
		
		for (int n=0; n<MAX_PERL_WINDOWS; n++) {
			if (!perl_windows[n].active) {
				me->win[winId] = &perl_windows[n];
				me->win[winId]->owner = me->id;
				me->win[winId]->active = true;
				me->win[winId]->x = buf.nextFloatToken();
				me->win[winId]->y = buf.nextFloatToken();
				me->win[winId]->scale = buf.nextFloatToken();	
				break;
			}	
		}
		return false;		
	} else if (cmd == std::string("dwin")) {
		int winId = buf.nextIntToken();
		me->win[winId]->active = false;
		return false;
	} else if (cmd == std::string("uwin")) {
		int winId = buf.nextIntToken();
		if (me->win[winId]->active) {
			memset(me->win[winId]->text, 0, MAX_STRING_CHARS+1);
			strncpy(me->win[winId]->text, buf.remainingString().c_str(), MAX_STRING_CHARS);
		}
		return false;		
	}
	
	return true;
}

// called when task completes
void perl_cleanup(void *arg)
{
	perlTask_t *n = (perlTask_t *)arg;	
	Perl.closePipe(n, true);
	if (n)
		n->script = NULL;
		
	// close any active windows owned by this script
	for (int i=0; i<MAX_PERL_WINDOWS; i++) {
		if (n->win[i])
			n->win[i]->active = false;
	}		
}

void *perl_thread(void *arg)
{	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);		
	pthread_cleanup_push(perl_cleanup, arg);
		
	char output[10000];			// program output
	char output_con[10000];		// output meant for ET console
	char command[256];
	int args, i, c, count;

	char *s, *e, *f;
	char etcmd[MAX_STRING_CHARS+1];
	
	perlTask_t *n = (perlTask_t *)arg;
	n->pfd = popen(n->command, "r");

	fd_set readfds; 
	FD_ZERO (&readfds);	
		
	while( 1 )
	{
		if (n->wantexit)
			pthread_exit(NULL);	
		
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 500000;	
		FD_SET (fileno(n->pfd), &readfds);	
		select (fileno(n->pfd)+1, &readfds, NULL, NULL, &tv);	
			
		if (FD_ISSET(fileno(n->pfd), &readfds)) {
			if (!fgets(output, sizeof(output), n->pfd))
				pthread_exit(NULL);	
				
			count = strlen(output);
			
			s = output;
			output_con[0] = '\0';
	
			while (1){
				if ((s-output) > count)
					break;
				e = strstr(s, "et_str<");
				if (e) {
					*e = '\0';
					strcat(output_con, s);
		
					f = strstr(e+1, ">et_end");
					if (!f){
						Tools.Print("^3Warning: script(%i): unterminated et command\n", n->id);
						break;
					}
					memset(etcmd, 0, sizeof(etcmd));
					strncpy(etcmd, e+7, f-e-7);
		
					// parse the console command and its arguments correctly
					std::string buf;
					std::string concatcmd = etcmd;
					StringTokenizer strtok(concatcmd, " ");
					
					buf += strtok.nextToken();	
					// if its not an internal eth32 command, buffer it for cgame			
					if (Perl.parseEth32Command(strtok, buf, n)) {			
						buf += std::string(" \"") + strtok.remainingString() + std::string("\"\n");
						Tools.BufferConsoleCommand(buf.c_str());
					}
		
					s = f+7;
				} else {
					if ((s-output) > count)
						break;
					strcat(output_con, s);
					break;
				}
			}
		
			// avoid printing empty strings
			strcpy(output, output_con);
			s = Q_CleanStr(output_con);
			for (; *s; s++){
				if (*s == ' ' || *s == '\n')
					continue;
				break;
			}
		
			// 800x600 max length seems to be 97 chars
			if (*s)
				Tools.Print("^3%s\n", output_con);			 
		}		
		clearerr(n->pfd);
	}	
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

void CPerl::ParsePerlCommand(perlScript_t *pl)
{
	perlTask_t *n;
	int args, i;

	for(int i=0; i<MAX_PERL_SCRIPTS; i++) {
		n = &ptasks[i];
		if (!n->script) {
			memset(n, 0, sizeof(perlTask_t));
			n->script = pl;
			n->startTime = *eth32.game.com_frameTime;
			n->wantexit = false;
			
			tasksLaunched++;
			n->id = tasksLaunched;
			break;
		}
	}

	if ( i==MAX_PERL_SCRIPTS ){
		Tools.Print("^1Error: ^2too many scripts running, try to free one\n");
		return;
	}

	sprintf(n->command, "perl %s", pl->path);

	if (UI_VM) args = Syscall.UI_Argc();
	else args = Syscall.CG_Argc();
	for (i=1; i<args; i++){
		strcat(n->command, " ");
		if (UI_VM) strcat(n->command, Syscall.UI_Argv(i));
		else strcat(n->command, Syscall.CG_Argv(i));
	}
	
	pthread_create(&n->vid, NULL, perl_thread, (void *)n);

	return;
}
