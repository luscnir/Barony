
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initWanRit(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1450; // wan-rit model

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 700;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 703;
		MONSTER_IDLEVAR = 3;
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
			if ( rand() % 50 == 0 && !my->flags[USERFLAG2] )
			{
				strcpy(myStats->name, "War-One");
				myStats->HP = 350;
				myStats->MAXHP = 350;
				myStats->OLDHP = myStats->HP;
				myStats->STR = 75;
				myStats->DEX = 10;
				myStats->CON = 29;
				myStats->INT = 20;
				myStats->LVL = 36;
				newItem(LOST_AXE, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
				customItemsToGenerate = customItemsToGenerate - 1;
				int c;
				for ( c = 0; c < 3; c++ )
				{
					Entity* entity = summonMonster(SKU_LIT, my->x, my->y);
					if ( entity )
					{
						entity->parent = my->getUID();
					}
				}
			}
			// random effects

			// generates equipment and weapons if available from editor
			createMonsterEquipment(myStats);

			// create any custom inventory items from editor if available
			createCustomInventory(myStats, customItemsToGenerate);

			// count if any custom inventory items from editor
			int customItems = countCustomItems(myStats); //max limit of 6 custom items per entity.

														 // count any inventory items set to default in edtior
			int defaultItems = countDefaultItems(myStats);

			my->setHardcoreStats(*myStats);

			// generate the default inventory items for the monster, provided the editor sprite allowed enough default slots
			switch ( defaultItems )
			{
				case 6:
				case 5:
				case 4:
				case 3:
					if (rand() % 5 == 0)
					{
						newItem(MARBLE_BREASTPIECE, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					}
					break;
				case 2:
					if (rand() % 5 == 0)
					{
						switch (rand() % 3)
						{
						case 0:
							newItem(ANTI_BLEED_SHIELD, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						case 1:
							newItem(ANTI_CHARM_SHIELD, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						case 2:
							newItem(ANTI_SLEEP_SHIELD, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						}
					}
					break;
				case 1:
					if (rand() % 20)
					{
						newItem(TOOL_UNIHORN, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
					}
					break;
				default:
					break;
			}
		}
	}
}

void wanRitAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move legs
	if ( (ticks % 5 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK) )
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if ( my->sprite == 1450)
		{
			my->sprite = 1451;
		}
		else if (my->sprite == 1451)
		{
			my->sprite = 1452;
		}
		else if (my->sprite == 1452)
		{
			my->sprite = 1453;
		}
		else
		{
			my->sprite = 1450;
		}
	}
}

void wanRitDie(Entity* my)
{
	int c = 0;
	for ( c = 0; c < 8; c++ )
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood();

	playSoundEntity(my, 706 + rand() % 3, 64);
	list_RemoveNode(my->mynode);
	return;
}
