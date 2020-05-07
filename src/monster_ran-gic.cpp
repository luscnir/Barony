
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initRanGic(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1448; // ran-gic model

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 695;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = 696;
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
			if ( rand() % 50 == 0 && !my->flags[USERFLAG2] )
			{
				switch (rand() % 2)
				{
				case 0:
					strcpy(myStats->name, "Sni-Per");
					myStats->HP = 170;
					myStats->MAXHP = 170;
					myStats->OLDHP = myStats->HP;
					myStats->DEX = 10;
					myStats->CON = 9;
					myStats->INT = 20;
					myStats->PER = 50;
					newItem(LOST_BOW, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					customItemsToGenerate = customItemsToGenerate - 1;
					int c;
					for (c = 0; c < 2; c++)
					{
						Entity* entity = summonMonster(RAN_GIC, my->x, my->y);
						if (entity)
						{
							entity->parent = my->getUID();
						}
					}
					break;
				case 1:
					strcpy(myStats->name, "Blo-Odd");
					myStats->HP = 135;
					myStats->MAXHP = 135;
					myStats->OLDHP = myStats->HP;
					myStats->DEX = 0;
					myStats->CON = 0;
					myStats->INT = 20;
					myStats->PER = 25;
					myStats->LVL = 11;
					myStats->weapon = newItem(SPELLBOOK_DRAIN_SOUL, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					newItem(LOST_MACE, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					customItemsToGenerate = customItemsToGenerate - 1;
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
					if (rand() % 30 == 0)
					{
						newItem(MARBLE_BREASTPIECE, WORN, 0, 1, rand(), false, &myStats->inventory);
					}
					break;
				case 2:
					if (rand() % 5 == 0)
					{
						switch (rand() % 3)
						{
						case 0:
							newItem(SPELLBOOK_LIGHT, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						case 1:
							newItem(SPELLBOOK_CONFUSE, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						case 2:
							newItem(SPELLBOOK_FORCEBOLT, SERVICABLE, 0, 1, rand(), false, &myStats->inventory);
							break;
						}
					}
					break;
				case 1:
					if ( rand() % 20 == 0 )
					{
						newItem(AMULET_MAGICREFLECTION, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
					}
					break;
				default:
					break;
			}
			myStats->weapon = newItem(SPELLBOOK_FORCEBOLT, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
		}
	}
}

void ranGicAnimate(Entity* my, double dist)
{
	// move legs
	if ( (ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK) )
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if ( my->sprite == 1448 )
		{
			my->sprite = 1449;
		}
		else
		{
			my->sprite = 1448;
		}
	}
}

void ranGicDie(Entity* my)
{
	int c = 0;
	for ( c = 0; c < 5; c++ )
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	playSoundEntity(my, 698 + rand() % 2, 64);
	list_RemoveNode(my->mynode);
	return;
}
