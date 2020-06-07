// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#define IS_FRIENDLY(num)				(*eth32.cg.players[num].team == eth32.cg.team)
#define IS_SAMETEAM(num1,num2)			(*eth32.cg.players[num1].team == *eth32.cg.players[num2].team)
#define IS_DEAD(num)					(eth32.cg.players[num].currentState->eFlags & EF_DEAD)
#define IS_INVULNERABLE(num)			(eth32.cg.players[num].currentState->powerups & (1<<PW_INVULNERABLE))
#define IS_SPECTATOR(num)				(*eth32.cg.players[num].team == TEAM_SPECTATOR)
#define IS_INFOVALID(num)				(*eth32.cg.players[num].infoValid)
#define IS_WEAPATTRIB(num,attrib)		(eth32.cg.weaponList[num].attribs & attrib)

#define IS_LOCALHOST					(!(((et_netadr_t *)eth32.et->server_ip)->ip[0]))

