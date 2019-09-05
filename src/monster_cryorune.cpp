
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "player.hpp"

void initCryorune(Entity* my, Stat* myStats)
{
	int c;
	node_t* node;

	my->sprite = 908;

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 431;
		MONSTER_SPOTVAR = 2;
		MONSTER_IDLESND = 433;
		MONSTER_IDLEVAR = 2;
	}
	if ( multiplayer != CLIENT && !MONSTER_INIT )
	{
		if ( myStats != NULL )
		{
			if ( !myStats->leader_uid )
			{
				myStats->leader_uid = 0;
			}

			// apply random stat increases if set in stat_shared.cpp or editor
			setRandomMonsterStats(myStats);

			// generate 6 items max, less if there are any forced items from boss variants
			int customItemsToGenerate = ITEM_CUSTOM_SLOT_LIMIT;

			// boss variants
			bool isBoss = false;

			if ( rand() % 50 == 0 && !my->flags[USERFLAG2] )
			{
				isBoss = true;

				strcpy(myStats->name, "Zed");
				myStats->HP = 140;
				myStats->MAXHP = 140;
				myStats->OLDHP = myStats->HP;
				myStats->MP = 400;
				myStats->MAXMP = 400;
				myStats->STR = 1;
				myStats->DEX = 10;
				myStats->CON = 20;
				myStats->INT = 20;
				myStats->PER = 0;
				myStats->CHR = 5;
				myStats->LVL = 20;
				my->setEffect(EFF_VAMPIRICAURA, true, -1, true); //-1 duration, never expires.
				myStats->weapon = newItem(SPELLBOOK_MAGICMISSILE, EXCELLENT, 0, 1, 0, false, nullptr);
				newItem(BOOTS_LIGHTNESS, EXCELLENT, 0, 1, 0, false, nullptr);
				customItemsToGenerate = customItemsToGenerate - 1;
				int c;
				for ( c = 0; c < 2; ++c )
				{
					Entity* entity = summonMonster(CRYORUNE, my->x, my->y);
					if ( entity )
					{
						entity->parent = my->getUID();
					}
				}
			}
			// random effects
			myStats->EFFECTS[EFF_LEVITATING] = true;
			myStats->EFFECTS_TIMERS[EFF_LEVITATING] = 0;
			if ( !isBoss )
			{
				myStats->weapon = newItem(SPELLBOOK_COLD, EXCELLENT, 0, 1, 0, false, nullptr);
			}

			// generates equipment and weapons if available from editor
			createMonsterEquipment(myStats);

			// create any custom inventory items from editor if available
			createCustomInventory(myStats, customItemsToGenerate);

			// count if any custom inventory items from editor
			int customItems = countCustomItems(myStats); //max limit of 6 custom items per entity.

														 // count any inventory items set to default in edtior
			int defaultItems = countDefaultItems(myStats);

			my->setHardcoreStats(*myStats);

			int playerCount = 0;
			for ( c = 0; c < MAXPLAYERS; ++c )
			{
				if ( !client_disconnected[c] )
				{
					++playerCount;
				}
			}

			// generate the default inventory items for the monster, provided the editor sprite allowed enough default slots
			switch ( defaultItems )
			{
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
				case 1:
					if ( rand() % 2 || playerCount > 1 )
					{
						if ( rand() % 3 > 0 )
						{
							newItem(GEM_ROCK, static_cast<Status>(DECREPIT + rand() % 4), 0, 1, rand(), false, &myStats->inventory);
						}
						else
						{
							ItemType gem = GEM_GLASS;
							switch( rand() % 7 )
							{
								case 0:
									gem = GEM_OPAL;
									break;
								case 1:
									gem = GEM_JADE;
									break;
								case 2:
									gem = GEM_AMETHYST;
									break;
								case 3:
									gem = GEM_FLUORITE;
									break;
								case 4:
									gem = GEM_JETSTONE;
									break;
								case 5:
									gem = GEM_OBSIDIAN;
									break;
								default:
									gem = GEM_GLASS;
									break;
							}
							newItem(gem, static_cast<Status>(DECREPIT + rand()%2), (rand()%4 == 0), 1, rand(), false, &myStats->inventory);
						}
						if ( playerCount > 2 )
						{
							newItem(GEM_ROCK, static_cast<Status>(DECREPIT + rand() % 4), 0, 1 + rand() % 2, rand(), false, &myStats->inventory);
						}
					}
					break;
				default:
					break;
			}
		}
	}
}

void cryoruneAnimate(Entity* my, double dist)
{
	node_t* node;
	int bodypart;
	Entity* entity = nullptr;

	// set invisibility //TODO: isInvisible()?

	// move
	if ( (ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME == 0 && MONSTER_ATTACK == 1) )
	{
		//MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if ( my->sprite == 908 )
		{
			my->sprite = 909;
		}
		else if (my->sprite == 909)
		{
			my->sprite = 910;
		}
		else
		{
			my->sprite = 908;
		}
	}

	if ( MONSTER_ATTACK != 0 )
	{
		MONSTER_ATTACKTIME++;
	}
	else
	{
		MONSTER_ATTACKTIME = 0;
	}
}

void actCryoruneLimb(Entity* my)
{
	my->actMonsterLimb(true); //Can create light, but can't hold a lightsource.
}

void cryoruneDie(Entity* my)
{
	playSoundEntity(my, 435 + rand() % 2, 100);
	list_RemoveNode(my->mynode);
	return;
}
