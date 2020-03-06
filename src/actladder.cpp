/*-------------------------------------------------------------------------------

	BARONY
	File: actladder.cpp
	Desc: behavior function for ladders

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "sound.hpp"
#include "entity.hpp"
#include "scores.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "player.hpp"
#include "magic/magic.hpp"
#include "menu.hpp"

/*-------------------------------------------------------------------------------

	act*

	The following function describes an entity behavior. The function
	takes a pointer to the entity that uses it as an argument.

-------------------------------------------------------------------------------*/

#define LADDER_AMBIENCE my->skill[1]
#define LADDER_SECRET my->skill[3]

void actLadder(Entity* my)
{
	int playercount = 0;
	double dist;
	int i, c;

	LADDER_AMBIENCE--;
	if (LADDER_AMBIENCE <= 0)
	{
		LADDER_AMBIENCE = TICKS_PER_SECOND * 30;
		playSoundEntityLocal(my, 149, 64);
	}

	// use ladder (climb)
	if (multiplayer != CLIENT)
	{
		for (i = 0; i < MAXPLAYERS; i++)
		{
			if ((i == 0 && selectedEntity == my) || (client_selected[i] == my))
			{
				if (inrange[i])
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
						{
							continue;
						}
						else
						{
							playercount++;
						}
						dist = sqrt(pow(my->x - players[c]->entity->x, 2) + pow(my->y - players[c]->entity->y, 2));
						if (dist > TOUCHRANGE)
						{
							messagePlayer(i, language[505]);
							return;
						}
					}
					if (playercount == 1)
					{
						messagePlayer(i, language[506]);
					}
					else
					{
						messagePlayer(i, language[507]);
					}
					if (secretlevel)
					{
						switch (currentlevel)
						{
							case 3:
								for (c = 0; c < MAXPLAYERS; c++)
								{
									steamAchievementClient(c, "BARONY_ACH_THUNDERGNOME");
								}
								break;
						}
						if ( strncmp(map.name, "Underworld", 10) )
						{
							skipLevelsOnLoad = -1; // don't skip a regular level anymore. still skip if in underworld.
						}
					}
					if (LADDER_SECRET)
					{
						secretlevel = (secretlevel == false);    // toggle level lists
					}
					if ( !strncmp(map.name, "Ant Nest", 16) )
					{
						currentlevel += 1; // These level still work as normal even as secrets levels.
					}
					loadnextlevel = true;
					return;
				}
			}
		}
	}
}

void actLadderUp(Entity* my)
{
	int i;

	LADDER_AMBIENCE--;
	if ( LADDER_AMBIENCE <= 0 )
	{
		LADDER_AMBIENCE = TICKS_PER_SECOND * 30;
		playSoundEntityLocal( my, 149, 64 );
	}

	// use ladder
	if ( multiplayer != CLIENT )
	{
		for (i = 0; i < MAXPLAYERS; i++)
		{
			if ( (i == 0 && selectedEntity == my) || (client_selected[i] == my) )
			{
				if (inrange[i])
				{
					messagePlayer(i, language[508]);
					return;
				}
			}
		}
	}
}

void actPortal(Entity* my)
{
	int playercount = 0;
	double dist;
	int i, c;

	if ( !my->portalInit )
	{
		my->portalInit = 1;
		my->light = lightSphereShadow(my->x / 16, my->y / 16, 3, 255);
		if ( !strncmp(map.name, "Cockatrice Lair", 15) )
		{
			my->flags[INVISIBLE] = true;
		}
		else if ( !strncmp(map.name, "Bram's Castle", 13) )
		{
			my->flags[INVISIBLE] = true;
		}
	}

	my->portalAmbience--;
	if ( my->portalAmbience <= 0 )
	{
		my->portalAmbience = TICKS_PER_SECOND * 2;
		if ( !my->flags[INVISIBLE] )
		{
			playSoundEntityLocal( my, 154, 128 );
		}
	}

	my->yaw += 0.01; // rotate slowly on my axis
	my->sprite = 254 + (my->ticks / 20) % 4; // animate

	if ( multiplayer == CLIENT )
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == my) || (client_selected[i] == my))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(my->x - players[c]->entity->x, 2) + pow(my->y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[505]);
						return;
					}
				}
				if (playercount == 1)
				{
					messagePlayer(i, language[510]);
				}
				else
				{
					messagePlayer(i, language[511]);
				}
				loadnextlevel = true;
				if ( secretlevel )
				{
					switch ( currentlevel )
					{
						case 9:
						{
							; //lol
							bool visiblegrave = false;
							node_t* node;
							for ( node = map.entities->first; node != nullptr; node = node->next )
							{
								Entity* entity = (Entity*)node->element;
								if ( entity->sprite == 224 && !entity->flags[INVISIBLE] )
								{
									visiblegrave = true;
									break;
								}
							}
							if ( visiblegrave )
							{
								for ( c = 0; c < MAXPLAYERS; ++c )
								{
									steamAchievementClient(c, "BARONY_ACH_ROBBING_THE_CRADLE");
								}
							}
							break;
						}
						case 14:
							for ( c = 0; c < MAXPLAYERS; ++c )
							{
								steamAchievementClient(c, "BARONY_ACH_THESEUS_LEGACY");
							}
							break;
						case 29:
							for ( c = 0; c < MAXPLAYERS; c++ )
							{
								steamAchievementClient(c, "BARONY_ACH_CULT_FOLLOWING");
							}
							break;
						case 34:
							for ( c = 0; c < MAXPLAYERS; c++ )
							{
								steamAchievementClient(c, "BARONY_ACH_DESPAIR_CALMS");
							}
							break;
						default:
							break;
					}
					if ( strncmp(map.name, "Underworld", 10) )
					{
						skipLevelsOnLoad = -1; // don't skip a regular level anymore. still skip if in underworld.
					}
					else
					{
						// underworld - don't skip on the early sections.
						if ( currentlevel == 6 || currentlevel == 7 )
						{
							skipLevelsOnLoad = -1;
						}
					}
				}
				if ( !my->portalNotSecret )
				{
					secretlevel = (secretlevel == false);  // toggle level lists
				}
				return;
			}
		}
	}
}

void actWinningPortal(Entity* my)
{
	int playercount = 0;
	double dist;
	int i, c;

	if ( multiplayer != CLIENT )
	{
		if ( my->flags[INVISIBLE] )
		{
			if ( !strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9) )
			{
				if ( !(svFlags & SV_FLAG_CLASSIC) )
				{
					return; // classic mode disabled.
				}
			}
			node_t* node;
			for ( node = map.creatures->first; node != nullptr; node = node->next )
			{
				Entity* entity = (Entity*)node->element;
				if ( entity->behavior == &actMonster )
				{
					Stat* stats = entity->getStats();
					if ( stats )
					{
						if ( stats->type == LICH || stats->type == DEVIL )
						{
							return;
						}
					}
				}
			}
			if ( my->skill[28] != 0 )
			{
				if ( my->skill[28] == 2 )
				{
					// powered on.
					if ( !my->portalFireAnimation )
					{
						Entity* timer = createParticleTimer(my, 100, 174);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(my, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						my->portalFireAnimation = 1;
					}
				}
			}
		}
		else
		{
			if ( !strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9) )
			{
				if ( !(svFlags & SV_FLAG_CLASSIC) )
				{
					my->flags[INVISIBLE] = true; // classic mode disabled, hide.
					serverUpdateEntityFlag(my, INVISIBLE);
					my->portalFireAnimation = 0;
				}
			}
		}
	}
	else
	{
		if ( my->flags[INVISIBLE] )
		{
			return;
		}
	}

	if ( !my->portalInit )
	{
		my->portalInit = 1;
		my->light = lightSphereShadow(my->x / 16, my->y / 16, 3, 255);
	}

	my->portalAmbience--;
	if ( my->portalAmbience <= 0 )
	{
		my->portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal( my, 154, 128 );
	}

	my->yaw += 0.01; // rotate slowly on my axis
	my->sprite = 278 + (my->ticks / 20) % 4; // animate

	if ( multiplayer == CLIENT )
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == my) || (client_selected[i] == my))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt( pow(my->x - players[c]->entity->x, 2) + pow(my->y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				victory = my->portalVictoryType;
				if ( multiplayer == SERVER )
				{
					for ( c = 0; c < MAXPLAYERS; c++ )
					{
						if ( client_disconnected[c] == true )
						{
							continue;
						}
						strcpy((char*)net_packet->data, "WING");
						net_packet->data[4] = victory;
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 8;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				subwindow = 0;
				introstage = 5; // prepares win game sequence
				fadeout = true;
				if ( !intro )
				{
					pauseGame(2, false);
				}
				return;
			}
		}
	}
}

void actExpansionEndGamePortal(Entity* my)
{
	if ( !my )
	{
		return;
	}

	my->actExpansionEndGamePortal();
}

void Entity::actExpansionEndGamePortal()
{
	int playercount = 0;
	double dist;
	int i, c;

	if ( multiplayer != CLIENT )
	{
		if ( flags[INVISIBLE] )
		{
			node_t* node;
			for ( node = map.creatures->first; node != nullptr; node = node->next )
			{
				Entity* entity = (Entity*)node->element;
				if ( entity )
				{
					if ( entity->behavior == &actMonster )
					{
						Stat* stats = entity->getStats();
						if ( stats )
						{
							if ( stats->type == LICH_FIRE || stats->type == LICH_ICE )
							{
								return;
							}
						}
					}
				}
			}
			if ( circuit_status != 0 )
			{
				if ( circuit_status == CIRCUIT_ON )
				{
					// powered on.
					if ( !portalFireAnimation )
					{
						Entity* timer = createParticleTimer(this, 100, 174);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
	}
	else
	{
		if ( flags[INVISIBLE] )
		{
			return;
		}
	}

	if ( !portalInit )
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if ( portalAmbience <= 0 )
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 614 + (this->ticks / 20) % 4; // animate

	if ( multiplayer == CLIENT )
	{
		return;
	}

	// step through portal
	for ( i = 0; i < MAXPLAYERS; i++ )
	{
		if ( (i == 0 && selectedEntity == this) || (client_selected[i] == this) )
		{
			if ( inrange[i] )
			{
				for ( c = 0; c < MAXPLAYERS; c++ )
				{
					if ( client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr )
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if ( dist > TOUCHRANGE )
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				victory = portalVictoryType;
				if ( multiplayer == SERVER )
				{
					for ( c = 0; c < MAXPLAYERS; c++ )
					{
						if ( client_disconnected[c] == true )
						{
							continue;
						}
						strcpy((char*)net_packet->data, "WING");
						net_packet->data[4] = victory;
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 8;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				subwindow = 0;
				introstage = 5; // prepares win game sequence
				fadeout = true;
				if ( !intro )
				{
					pauseGame(2, false);
				}
				return;
			}
		}
	}
}

void actMidGamePortal(Entity* my)
{
	if ( !my )
	{
		return;
	}

	my->actMidGamePortal();
}

void Entity::actMidGamePortal()
{
	int playercount = 0;
	double dist;
	int i, c;

	if ( multiplayer != CLIENT )
	{
		if ( flags[INVISIBLE] )
		{
			if ( !strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9) )
			{
				if ( (svFlags & SV_FLAG_CLASSIC) )
				{
					return; // classic mode enabled, don't process.
				}
			}
			node_t* node;
			for ( node = map.creatures->first; node != nullptr; node = node->next )
			{
				Entity* entity = (Entity*)node->element;
				if ( entity )
				{
					if ( entity->behavior == &actMonster )
					{
						Stat* stats = entity->getStats();
						if ( stats )
						{
							if ( stats->type == LICH || stats->type == DEVIL )
							{
								return;
							}
						}
					}
				}
			}
			if ( circuit_status != 0 )
			{
				if ( circuit_status == CIRCUIT_ON )
				{
					// powered on.
					if ( !portalFireAnimation )
					{
						Entity* timer = createParticleTimer(this, 100, 174);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
		else
		{
			if ( !strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9) )
			{
				if ( (svFlags & SV_FLAG_CLASSIC) )
				{
					flags[INVISIBLE] = true; // classic mode enabled, hide.
					serverUpdateEntityFlag(this, INVISIBLE);
					portalFireAnimation = 0;
				}
			}
		}
	}
	else
	{
		if ( flags[INVISIBLE] )
		{
			return;
		}
	}

	if ( !portalInit )
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if ( portalAmbience <= 0 )
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 614 + (this->ticks / 20) % 4; // animate

	if ( multiplayer == CLIENT )
	{
		return;
	}

	// step through portal
	for ( i = 0; i < MAXPLAYERS; i++ )
	{
		if ( (i == 0 && selectedEntity == this) || (client_selected[i] == this) )
		{
			if ( inrange[i] )
			{
				for ( c = 0; c < MAXPLAYERS; c++ )
				{
					if ( client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr )
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if ( dist > TOUCHRANGE )
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				//victory = portalVictoryType;
				int movieCrawlType = -1;
				if ( !strncmp(map.name, "Hell Boss", 9) )
				{
					movieCrawlType = MOVIE_MIDGAME_BAPHOMET_HUMAN_AUTOMATON;
					if ( stats[0] && stats[0]->playerRace > 0 && stats[0]->playerRace != RACE_AUTOMATON )
					{
						movieCrawlType = MOVIE_MIDGAME_BAPHOMET_MONSTERS;
					}
				}
				else if ( !strncmp(map.name, "Boss", 4) )
				{
					if ( stats[0] && stats[0]->playerRace > 0 && stats[0]->playerRace != RACE_AUTOMATON )
					{
						movieCrawlType = MOVIE_MIDGAME_HERX_MONSTERS;
					}
				}
				int introstageToChangeTo = 9;
				if ( movieCrawlType >= 0 )
				{
					introstageToChangeTo = 11 + movieCrawlType;
				}

				if ( multiplayer == SERVER )
				{
					for ( c = 0; c < MAXPLAYERS; c++ )
					{
						if ( client_disconnected[c] == true )
						{
							continue;
						}
						strcpy((char*)net_packet->data, "MIDG");
						net_packet->data[4] = introstageToChangeTo;
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 8;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				subwindow = 0;
				fadeout = true;
				currentlevel += 6;
				if ( !intro )
				{
					pauseGame(2, false);
				}
				introstage = introstageToChangeTo; // prepares mid game sequence
				return;
			}
		}
	}
}

// modded portals TODO: This is a lot of duplicated code, make only one ladder/portal that can choose how many levels to skip.
void actPortalJump2(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actPortalJump2();
}

void Entity::actPortalJump2()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer != CLIENT)
	{
		if (flags[INVISIBLE])
		{
			if (circuit_status != 0)
			{
				if (circuit_status == CIRCUIT_ON)
				{
					// powered on.
					if (!portalFireAnimation)
					{
						Entity* timer = createParticleTimer(this, 100, 169);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
	}
	else
	{
		if (flags[INVISIBLE])
		{
			return;
		}
	}

	if (!portalInit)
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if (portalAmbience <= 0)
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 1181 + (this->ticks / 20) % 4; // animate

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				
				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				++currentlevel;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actPortalJump8(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actPortalJump8();
}

void Entity::actPortalJump8()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer != CLIENT)
	{
		if (flags[INVISIBLE])
		{
			if (circuit_status != 0)
			{
				if (circuit_status == CIRCUIT_ON)
				{
					// powered on.
					if (!portalFireAnimation)
					{
						Entity* timer = createParticleTimer(this, 100, 169);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
	}
	else
	{
		if (flags[INVISIBLE])
		{
			return;
		}
	}

	if (!portalInit)
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if (portalAmbience <= 0)
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 1181 + (this->ticks / 20) % 4; // animate

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 7;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump2(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump2();
}

void Entity::actLadderJump2()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				++currentlevel;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump7(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump7();
}

void Entity::actLadderJump7()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 6;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump8(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump8();
}

void Entity::actLadderJump8()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 7;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump9(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump9();
}

void Entity::actLadderJump9()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 8;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump10(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump10();
}

void Entity::actLadderJump10()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 9;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump16(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump16();
}

void Entity::actLadderJump16()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 15;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actLadderJump14(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actLadderJump14();
}

void Entity::actLadderJump14()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through ladder
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				currentlevel += 13;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actMidPortalJump13(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actMidPortalJump13();
}

void Entity::actMidPortalJump13()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer != CLIENT)
	{
		if (flags[INVISIBLE])
		{
			if (circuit_status != 0)
			{
				if (circuit_status == CIRCUIT_ON)
				{
					// powered on.
					if (!portalFireAnimation)
					{
						Entity* timer = createParticleTimer(this, 100, 169);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
	}
	else
	{
		if (flags[INVISIBLE])
		{
			return;
		}
	}

	if (!portalInit)
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if (portalAmbience <= 0)
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 1181 + (this->ticks / 20) % 4; // animate

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}

				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
	/*			subwindow = 0;
				introstage = 9; // prepares win game sequence //use introstage = 11(?) or 9?
				fadeout = true;
				if (!intro)
				{
					pauseGame(2, false);
				}*/
				currentlevel += 12;
				loadnextlevel = true;

				return;
			}
		}
	}
}

void actMidGamePortalBoss2(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actMidGamePortalBoss2();
}

void Entity::actMidGamePortalBoss2()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer != CLIENT)
	{
		if (flags[INVISIBLE])
		{
			if (!strncmp(map.name, "Boss2", 4))
			{
				if ((svFlags & SV_FLAG_CLASSIC))
				{
					return; // classic mode enabled, don't process.
				}
			}
			node_t* node;
			for (node = map.creatures->first; node != nullptr; node = node->next)
			{
				Entity* entity = (Entity*)node->element;
				if (entity)
				{
					if (entity->behavior == &actMonster)
					{
						Stat* stats = entity->getStats();
						if (stats)
						{
							if (stats->type == LICH)
							{
								return;
							}
						}
					}
				}
			}
			if (circuit_status != 0)
			{
				if (circuit_status == CIRCUIT_ON)
				{
					// powered on.
					if (!portalFireAnimation)
					{
						Entity* timer = createParticleTimer(this, 100, 174);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
		else
		{
			if (!strncmp(map.name, "Boss2", 4))
			{
				if ((svFlags & SV_FLAG_CLASSIC))
				{
					flags[INVISIBLE] = true; // classic mode enabled, hide.
					serverUpdateEntityFlag(this, INVISIBLE);
					portalFireAnimation = 0;
				}
			}
		}
	}
	else
	{
		if (flags[INVISIBLE])
		{
			return;
		}
	}

	if (!portalInit)
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if (portalAmbience <= 0)
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 614 + (this->ticks / 20) % 4; // animate

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				//victory = portalVictoryType;
				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						strcpy((char*)net_packet->data, "MIDG");
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 7;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				subwindow = 0;
				fadeout = true;
				currentlevel -= 4;
				if (!intro)
				{
					pauseGame(2, false);
				}
				introstage = 9; // prepares mid game sequence
				return;
			}
		}
	}
}

void actEndGamePortalAbyss(Entity* my)
{
	if (!my)
	{
		return;
	}

	my->actEndGamePortalAbyss();
}

void Entity::actEndGamePortalAbyss()
{
	int playercount = 0;
	double dist;
	int i, c;

	if (multiplayer != CLIENT)
	{
		if (flags[INVISIBLE])
		{
			node_t* node;
			for (node = map.creatures->first; node != nullptr; node = node->next)
			{
				Entity* entity = (Entity*)node->element;
				if (entity)
				{
					if (entity->behavior == &actMonster)
					{
						Stat* stats = entity->getStats();
						if (stats)
						{
							if (stats->type == LICH_FIRE || stats->type == LICH_ICE)
							{
								return;
							}
						}
					}
				}
			}
			if (circuit_status != 0)
			{
				if (circuit_status == CIRCUIT_ON)
				{
					// powered on.
					if (!portalFireAnimation)
					{
						Entity* timer = createParticleTimer(this, 100, 174);
						timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SPAWN_PORTAL;
						timer->particleTimerCountdownSprite = 174;
						timer->particleTimerEndAction = PARTICLE_EFFECT_PORTAL_SPAWN;
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_PORTAL_SPAWN, 174);
						portalFireAnimation = 1;
					}
				}
			}
		}
	}
	else
	{
		if (flags[INVISIBLE])
		{
			return;
		}
	}

	if (!portalInit)
	{
		portalInit = 1;
		light = lightSphereShadow(x / 16, y / 16, 3, 255);
	}

	portalAmbience--;
	if (portalAmbience <= 0)
	{
		portalAmbience = TICKS_PER_SECOND * 2;
		playSoundEntityLocal(this, 154, 128);
	}

	yaw += 0.01; // rotate slowly on my axis
	sprite = 614 + (this->ticks / 20) % 4; // animate

	if (multiplayer == CLIENT)
	{
		return;
	}

	// step through portal
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if ((i == 0 && selectedEntity == this) || (client_selected[i] == this))
		{
			if (inrange[i])
			{
				for (c = 0; c < MAXPLAYERS; c++)
				{
					if (client_disconnected[c] || players[c] == nullptr || players[c]->entity == nullptr)
					{
						continue;
					}
					else
					{
						playercount++;
					}
					dist = sqrt(pow(x - players[c]->entity->x, 2) + pow(y - players[c]->entity->y, 2));
					if (dist > TOUCHRANGE)
					{
						messagePlayer(i, language[509]);
						return;
					}
				}
				victory = portalVictoryType;
				if (multiplayer == SERVER)
				{
					for (c = 0; c < MAXPLAYERS; c++)
					{
						if (client_disconnected[c] == true)
						{
							continue;
						}
						strcpy((char*)net_packet->data, "WING");
						net_packet->data[4] = victory;
						net_packet->address.host = net_clients[c - 1].host;
						net_packet->address.port = net_clients[c - 1].port;
						net_packet->len = 8;
						sendPacketSafe(net_sock, -1, net_packet, c - 1);
					}
				}
				subwindow = 0;
				introstage = 10; // prepares win game sequence //use introstage = 10 for normal ending, 12 for custom
				fadeout = true;
				if (!intro)
				{
					pauseGame(2, false);
				}
				return;
			}
		}
	}
}