
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initEyeball(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 924;

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 443;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = 444;
		MONSTER_IDLEVAR = 1;
	}
	if ( multiplayer != CLIENT && !MONSTER_INIT )
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
				strcpy(myStats->name, "Baphomet's watcher");
				myStats->STR = 30;
				for (c = 0; c < 3; c++)
				{
					Entity* entity = summonMonster(EYEBALL, my->x, my->y);
					if (entity)
					{
						entity->parent = my->getUID();
					}
				}
				//newItem(ABYSSAL_MASK, EXCELLENT, 1, 1, rand(), false, &myStats->inventory);	//once abyssal_mask is created.
			}

			// random effects
			myStats->EFFECTS[EFF_LEVITATING] = true;
			myStats->EFFECTS_TIMERS[EFF_LEVITATING] = 0;

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
				if (rand() % 10)
				{
					newItem(FOOD_MEAT, WORN, -1, 1, rand(), false, &myStats->inventory);
				}
			case 2:
				if (rand() % 6)
				{
					newItem(TOOL_GLASSES, DECREPIT, -1, 1, rand(), false, &myStats->inventory);
				}
			case 1:
				if (rand() % 10)
				{
					newItem(TOOL_GLASSES, WORN, 0, 1, rand(), false, &myStats->inventory);
				}
				break;
			default:
				break;
			}
		}
	}
}

void eyeballAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move
	if ((ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK))
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if (my->sprite == 924)
		{
			my->sprite = 925;
		}
		else
		{
			my->sprite = 924;
		}
	}
}

void eyeballDie(Entity* my)
{
	int c = 0;
	for (c = 0; c < 3; c++)
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood(160);

	playSoundEntity(my, 445, 128);
	list_RemoveNode(my->mynode);
	return;
}
