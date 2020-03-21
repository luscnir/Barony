/*-------------------------------------------------------------------------------

	BARONY
	File: monster_shared.cpp
	Desc: contains shared monster implementation and helper functions

	Copyright 2013-2017 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "collision.hpp"
#include "player.hpp"
#include "entity.hpp"

void Entity::initMonster(int mySprite)
{
	sprite = mySprite;

	//Common flags.
	flags[UPDATENEEDED] = true;
	flags[BLOCKSIGHT] = true;
	flags[INVISIBLE] = false;

	int monsterType = this->getMonsterTypeFromSprite();

	switch ( monsterType )
	{
		case GNOME:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_SMALL_HUMANOID;
			break;
		case KOBOLD:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_SMALL_HUMANOID;
			break;
		case HUMAN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case RAT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case GOBLIN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SLIME:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case TROLL:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case OCTOPUS:
			// unused
			break;
		case SPIDER:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case GHOUL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SKELETON:
			monsterFootstepType = MONSTER_FOOTSTEP_SKELETON;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SCORPION:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CREATURE_IMP:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case BUGBEAR:
			// unused
			break;
		case DEMON:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SUCCUBUS:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case MIMIC:
			// unused
			break;
		case LICH:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case MINOTAUR:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case DEVIL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SHOPKEEPER:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SCARAB:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CRYSTALGOLEM:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case INCUBUS:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case VAMPIRE:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SHADOW:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case COCKATRICE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case INSECTOID:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case GOATMAN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case AUTOMATON:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case LICH_ICE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case LICH_FIRE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SENTRYBOT:
		case SPELLBOT:
		case GYROBOT:
		case DUMMYBOT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case COCKROACH:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case BURGGUARD:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case GARGOYLE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case MATILDA:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CRYORUNE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case YETI:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case ICEDEMON:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case EYEBALL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case FLESHLING:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case ABOMINATION:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CHOLOROSH:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case LICH_FALLEN:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case ANT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case DUSTDEVIL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case PARASITE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case METALLICBEAST:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SKU_LIT:
		case RAN_GIC:
		case WAN_RIT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CLONE_FALLEN:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;

		default:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
	}
	return;
}

int Entity::getMonsterTypeFromSprite()
{
	int mySprite = this->sprite;

	if ( (mySprite >= 113 && mySprite < 118) ||
		(mySprite >= 125 && mySprite < 130) ||
		(mySprite >= 332 && mySprite < 334) ||
		(mySprite >= 341 && mySprite < 347) ||
		(mySprite >= 354 && mySprite < 360) ||
		(mySprite >= 367 && mySprite < 373) ||
		(mySprite >= 380 && mySprite < 386) )   // human heads
	{
		return HUMAN;
	}
	else if ( mySprite == 131 || mySprite == 265 || mySprite == 814 )     // rat
	{
		return RAT;
	}
	else if ( mySprite == 180 || mySprite == 694 || mySprite == 752 )     // goblin head
	{
		return GOBLIN;
	}
	else if ( mySprite == 196 || mySprite == 266 )     // scorpion body
	{
		return SCORPION;
	}
	else if ( mySprite == 190 || mySprite == 710 )     // succubus head
	{
		return SUCCUBUS;
	}
	else if ( mySprite == 204 || mySprite == 817 )     // troll head
	{
		return TROLL;
	}
	else if ( mySprite == 217 )     // shopkeeper head
	{
		return SHOPKEEPER;
	}
	else if ( mySprite == 229 || mySprite == 686 )     // skeleton head
	{
		return SKELETON;
	}
	else if ( mySprite == 239 )     // minotaur waist
	{
		return MINOTAUR;
	}
	else if ( mySprite == 246 )     // ghoul head
	{
		return GHOUL;
	}
	else if ( mySprite == 258 )     // demon head
	{
		return DEMON;
	}
	else if ( mySprite == 267 || mySprite == 823 )     // spider body
	{
		return SPIDER;
	}
	else if ( mySprite == 274 )     // lich body
	{
		return LICH;
	}
	else if ( mySprite == 289 || mySprite == 827 )     // imp head
	{
		return CREATURE_IMP;
	}
	else if ( mySprite == 295 )     // gnome head
	{
		return GNOME;
	}
	else if ( mySprite == 304 )     // devil torso
	{
		return DEVIL;
	}
	else if ( mySprite == 475 )     // crystal golem head
	{
		return CRYSTALGOLEM;
	}
	else if ( mySprite == 413 )     // cockatrice head
	{
		return COCKATRICE;
	}
	else if ( mySprite == 467 || mySprite == 742 || mySprite == 770 )     // automaton head
	{
		return AUTOMATON;
	}
	else if ( mySprite == 429 || mySprite == 430 )     // scarab
	{
		return SCARAB;
	}
	else if ( mySprite == 421 )     // kobold head
	{
		return KOBOLD;
	}
	else if ( mySprite == 481 )     // shadow head
	{
		return SHADOW;
	}
	else if ( mySprite == 437 || mySprite == 718 || mySprite == 756 )     // vampire head
	{
		return VAMPIRE;
	}
	else if ( mySprite == 445 || mySprite == 702 )     // incubus head
	{
		return INCUBUS;
	}
	else if ( mySprite == 455 || mySprite == 726 || mySprite == 760 )     // insectoid head
	{
		return INSECTOID;
	}
	else if ( mySprite == 463 || mySprite == 734 || mySprite == 768 )     // goatman head
	{
		return GOATMAN;
	}
	else if ( mySprite == 646 )     // lich body
	{
		return LICH_FIRE;
	}
	else if ( mySprite == 650 )     // lich body
	{
		return LICH_ICE;
	}
	else if ( mySprite == 189 || mySprite == 210 )
	{
		return SLIME;
	}
	else if ( mySprite == 872 )     // sentrybot
	{
		return SENTRYBOT;
	}
	else if ( mySprite == 885 )
	{
		return SPELLBOT;
	}
	else if ( mySprite == 886 )
	{
		return GYROBOT;
	}
	else if ( mySprite == 889 )
	{
		return DUMMYBOT;
	}
	else if ( mySprite == 1095 || mySprite == 1096 )	// cockroach
	{
		return COCKROACH;
	}
	else if ( mySprite == 1142 )     // burg guard head
	{
		return BURGGUARD;
	}
	else if ( mySprite == 1150 )     // gargoyle head
	{
		return GARGOYLE;
	}
	else if ( mySprite == 1158 )     // Matilda body
	{
		return MATILDA;
	}
	else if ( mySprite == 1185 || mySprite == 1186 || mySprite == 1187 )	// cryorune
	{
		return CRYORUNE;
	}
	else if ( mySprite == 1188 )     // yeti head
	{
		return YETI;
	}
	else if ( mySprite == 1194 )     // ice demon head
	{
		return ICEDEMON;
	}
	else if ( mySprite == 1201 || mySprite == 1202 )	// eyeball
	{
		return EYEBALL;
	}
	else if ( mySprite == 1203 )     // fleshling head
	{
		return FLESHLING;
	}
	else if ( mySprite == 1211 )     // abomination head
	{
		return ABOMINATION;
	}
	else if ( mySprite == 1219 )     // cholorosh head
	{
		return CHOLOROSH;
	}
	else if ( mySprite == 1228 )     // fallen lich body
	{
		return LICH_FALLEN;
	}
	else if ( mySprite == 1426 || mySprite == 1427 )	// ant
	{
		return ANT;
	}
	else if ( mySprite == 1428 )     // dust devil head
	{
		return DUSTDEVIL;
	}
	else if ( mySprite == 1436 || mySprite == 1437 )	// eyeball
	{
		return PARASITE;
	}
	else if ( mySprite == 1438 )     // metallic beast head
	{
		return METALLICBEAST;
	}
	else if ( mySprite == 1445 || mySprite == 1446 || mySprite == 1447 )	// sku-lit
	{
		return SKU_LIT;
	}
	else if ( mySprite == 1448 || mySprite == 1449 )	// ran-gic
	{
		return RAN_GIC;
	}
	else if (mySprite == 1450 || mySprite == 1451 || mySprite == 1452 || mySprite == 1453)	// wan-rit
	{
		return WAN_RIT;
	}
	else if (mySprite == 1456)     // fallen clone body
	{
		return CLONE_FALLEN;
	}

	return NOTHING;
}

void Entity::actMonsterLimb(bool processLight)
{
	//If no longer part of a monster, delete the limb.
	Entity *parentEnt = nullptr;
	if ( (parentEnt = uidToEntity(skill[2])) == nullptr )
	{
		list_RemoveNode(mynode);
		return;
	}

	//Do something magical beyond my comprehension.
	if ( multiplayer != CLIENT )
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( inrange[i] )
			{
				if ( i == 0 && selectedEntity == this )
				{
					parentEnt->skill[13] = i + 1;
				}
				else if ( client_selected[i] == this )
				{
					parentEnt->skill[13] = i + 1;
				}
			}
		}
	}

	if ( processLight )
	{
		//Only run by monsters who can carry stuff (like torches). Sorry, rats.
		if ( light != nullptr )
		{
			list_RemoveNode(light->node);
			light = nullptr;
		}

		int carryingLightSource = 0;
		if ( flags[INVISIBLE] == false )
		{
			if ( sprite == 93 || sprite == 1337 )   // torch and candle
			{
				carryingLightSource = 6;
			}
			else if ( sprite == 94 )     // lantern
			{
				carryingLightSource = 9;
			}
			else if ( sprite == 529 )	// crystal shard
			{
				carryingLightSource = 4;
			}
			else if ( sprite == 814 )	// green torch
			{
				carryingLightSource = 8;
			}
			else if ( sprite == 836 )	// abyssal shield
			{
				carryingLightSource = 5;
			}
			else if ( sprite == 1285 )	// inquisitor lantern
			{
				carryingLightSource = 35;
			}
		}

		if ( carryingLightSource != 0 )
		{
			light = lightSphereShadow(x / 16, y / 16, carryingLightSource, 50 + 15 * carryingLightSource);
		}
	}

	if ( parentEnt && parentEnt->behavior == &actMonster && parentEnt->monsterEntityRenderAsTelepath == 1 )
	{
		monsterEntityRenderAsTelepath = 1;
	}
	else
	{
		monsterEntityRenderAsTelepath = 0;
	}
}

void Entity::removeMonsterDeathNodes()
{
	int i = 0;
	node_t *nextnode = nullptr;
	for ( node_t *node = children.first; node != nullptr; node = nextnode )
	{
		nextnode = node->next;
		if ( node->element != nullptr && i >= 2 )
		{
			Entity* entity = (Entity*)node->element;
			if ( entity->light != nullptr )
			{
				list_RemoveNode(entity->light->node);
			}
			entity->light = nullptr;
			entity->flags[UPDATENEEDED] = false; //TODO: Do only demon & baphy need this?
			list_RemoveNode(entity->mynode);
		}
		list_RemoveNode(node);
		++i;
	}
}

void Entity::spawnBlood(int bloodSprite)
{
	if ( spawn_blood )
	{
		int tileX = std::min<unsigned int>(std::max<int>(0, this->x / 16), map.width - 1);
		int tileY = std::min<unsigned int>(std::max<int>(0, this->y / 16), map.height - 1);
		if ( map.tiles[tileY * MAPLAYERS + tileX * MAPLAYERS * map.height] )
		{
			if ( !checkObstacle(this->x, this->y, this, nullptr) )
			{
				Entity* entity = newEntity(bloodSprite, 1, map.entities, nullptr); //Blood/gib entity.
				entity->x = this->x;
				entity->y = this->y;
				entity->z = 8 + (rand() % 20) / 100.0;
				entity->parent = getUID();
				entity->sizex = 2;
				entity->sizey = 2;
				entity->yaw = (rand() % 360) * PI / 180.0;
				entity->flags[UPDATENEEDED] = true;
				entity->flags[PASSABLE] = true;
			}
		}
	}
}
