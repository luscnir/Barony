
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initParasite(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1436;

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 674;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 677;
		MONSTER_IDLEVAR = 2;
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
				strcpy(myStats->name, "Eater of Minds");
				myStats->INT = 35;
				myStats->PER = 30;
				myStats->DEX = 16;
				myStats->weapon = newItem(SPELLBOOK_BLEED, EXCELLENT, 0, 1, 0, false, nullptr);
				newItem (INQUISITOR_AXE, EXCELLENT, 0, 1, 0, false, nullptr);

				for (c = 0; c < 2; c++)
				{
					Entity* entity = summonMonster(DEMON, my->x, my->y);
					if (entity)
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
			switch (defaultItems)
			{
			case 6:
			case 5:
			case 4:
				if (rand() % 10)
				{
					newItem(MANA_GLOVES, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
				}
			case 3:
				if (rand() % 10)
				{
					newItem(FOOD_MEAT, WORN, -1, 1, rand(), false, &myStats->inventory);
				}
			case 2:
				if (rand() % 10)
				{
					newItem(MASK_ANGRY, DECREPIT, -1, 1, rand(), false, &myStats->inventory);
				}
			case 1:
				if (rand() % 10)
				{
					newItem(TOOL_MIRROR, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
				}
				break;
			default:
				break;
			}
		}
	}
}

void parasiteAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move
	if ((ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK))
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if (my->sprite == 1436)
		{
			my->sprite = 1437;
		}
		else
		{
			my->sprite = 1436;
		}
	}
}

void parasiteDie(Entity* my)
{
	int c = 0;
	for (c = 0; c < 3; c++)
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood(160);

	playSoundEntity(my, 679 + rand() % 2, 128);
	list_RemoveNode(my->mynode);
	return;
}
