/*-------------------------------------------------------------------------------

	BARONY
	File: monster_cockroach.cpp
	Desc: implements all of the cockroach monster's code

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initCockroach(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1095; // cockroach model 1095

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if (multiplayer != CLIENT)
	{
		MONSTER_SPOTSND = 588;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = 589;
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
				if (rand() % 4)
				{
					if (rand() % 2)
					{
						newItem(FOOD_BREAD, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
					}
					else
					{
						newItem(FOOD_BANANA, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void cockroachAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move legs
	if ((ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK))
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if (my->sprite == 1095)
		{
			my->sprite = 1096;
		}
		else
		{
			my->sprite = 1095;
		}
	}
}

void cockroachDie(Entity* my)
{
	int c = 0;
	for (c = 0; c < 3; c++)
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood(212);

	playSoundEntity(my, 590, 128);
	list_RemoveNode(my->mynode);
	return;
}
