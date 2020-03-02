
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

	my->sprite = 1201;

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 627;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = 628;
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
				switch (rand() % 2)
				{
				case 0:
					strcpy(myStats->name, "Baphomet's watcher");
					myStats->INT += 30;
					myStats->weapon = newItem(SPELLBOOK_FIREBALL, EXCELLENT, 0, 1, 0, false, nullptr);
					newItem (BOOTS_LIGHTNESS, EXCELLENT, 0, 1, 0, false, nullptr);
					newItem (TOOL_GLASSES, EXCELLENT, 5, 1, 0, false, nullptr);

					for (c = 0; c < 3; c++)
					{
						Entity* entity = summonMonster(EYEBALL, my->x, my->y);
						if (entity)
						{
							entity->parent = my->getUID();
						}
					}
					break;
				case 1:
					strcpy(myStats->name, "Blind Soul Seeker");
					myStats->MAXHP -= 40;
					myStats->HP -= 40;
					myStats->CON += 30;
					myStats->GOLD += 1000;
					myStats->RANDOM_GOLD += 1000;
					myStats->weapon = newItem(SPELLBOOK_TELEPORTATION, EXCELLENT, 0, 1, 0, false, nullptr); //runs away
					newItem (TOOL_UNIHORN, EXCELLENT, 0, 1, 0, true, nullptr);
					myStats->EFFECTS[EFF_BLIND] = true;
					myStats->EFFECTS_TIMERS[EFF_BLIND] = 0;
					break;
				}
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
		if (my->sprite == 1201)
		{
			my->sprite = 1202;
		}
		else
		{
			my->sprite = 1201;
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

	playSoundEntity(my, 629, 128);
	list_RemoveNode(my->mynode);
	return;
}
