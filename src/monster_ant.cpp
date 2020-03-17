
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initAnt(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1426; // ant model 1426

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if (multiplayer != CLIENT)
	{
		MONSTER_SPOTSND = 664;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = 665;
		MONSTER_IDLEVAR = 1;
	}
	if (multiplayer != CLIENT && !MONSTER_INIT)
	{
		if (myStats != NULL)
		{
			if (!myStats->leader_uid)
			{
				myStats->leader_uid = 0;
			}

			// apply random stat increases if set in stat_shared.cpp or editor
			setRandomMonsterStats(myStats);

			// generate 6 items max, less if there are any forced items from boss variants
			int customItemsToGenerate = ITEM_CUSTOM_SLOT_LIMIT;

			// boss variants
			if (rand() % 50 || my->flags[USERFLAG2])
			{
			}
			else
			{
				switch (rand() % 2)
				{
				case 0:
					strcpy(myStats->name, "Matriarch");
					myStats->LVL = 5;
					myStats->STR = 10;
					for (c = 0; c < 13; c++)
					{
						Entity* entity = summonMonster(ANT, my->x, my->y);
						if (entity)
						{
							entity->parent = my->getUID();
						}
					}
					newItem(INQUISITOR_SPEAR, EXCELLENT, 0, 1, 0, false, nullptr);
					break;
				case 1:
					strcpy(myStats->name, "Soldier Ant");
					myStats->MAXHP = 100;
					myStats->HP = 100;
					myStats->CON = 8;
					newItem(FOOD_TOMATO, EXCELLENT, 0, 1, 0, true, nullptr);
					break;
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
			switch (defaultItems)
			{
			case 6:
			case 5:
			case 4:
			case 3:
			case 2:
			case 1:
				if (rand() % 10)
				{
					if (rand() % 2)
					{
						newItem(FOOD_APPLE, WORN, 0, 1, rand(), false, &myStats->inventory);
					}
					else
					{
						newItem(FOOD_PEARS, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void antAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move legs
	if ((ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK))
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if (my->sprite == 1426)
		{
			my->sprite = 1427;
		}
		else
		{
			my->sprite = 1426;
		}
	}
}

void antDie(Entity* my)
{
	int c = 0;
	for (c = 0; c < 3; c++)
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood(213);

	playSoundEntity(my, 666, 128);
	list_RemoveNode(my->mynode);
	return;
}
