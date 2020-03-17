
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "net.hpp"
#include "collision.hpp"

void initSkuLit(Entity* my, Stat* myStats)
{
	int c;

	my->sprite = 1445; // sku-lit model

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 679;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 692;
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
					strcpy(myStats->name, "Epy-Lif");
					myStats->HP = 160;
					myStats->MAXHP = 160;
					myStats->OLDHP = myStats->HP;
					myStats->STR = 5;
					myStats->DEX = 10;
					myStats->CON = 4;
					myStats->INT = 20;
					myStats->PER = -1;
					myStats->LVL = 5;
					newItem(LOST_SWORD, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					customItemsToGenerate = customItemsToGenerate - 1;
					int c;
					for (c = 0; c < 4; c++)
					{
						Entity* entity = summonMonster(SKU_LIT, my->x, my->y);
						if (entity)
						{
							entity->parent = my->getUID();
						}
					}
					break;
				case 1:
					strcpy(myStats->name, "Fra-Cor");
					myStats->HP = 100;
					myStats->MAXHP = 100;
					myStats->OLDHP = myStats->HP;
					myStats->STR = 10;
					myStats->DEX = 5;
					myStats->CON = 6;
					myStats->INT = 20;
					myStats->PER = 0;
					myStats->LVL = 6;
					newItem(LOST_POLEARM, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
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
				case 2:
					if (rand() % 50 == 0)
					{
						newItem(MARBLE_BREASTPIECE, DECREPIT, 0, 1, rand(), false, &myStats->inventory);
					}
				case 1:
					if ( rand() % 2 )
					{
						newItem(TOOL_CANDLE, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);
					}
					else
					{
						newItem(TOOL_CANDLE, WORN, 0, 2, rand(), false, &myStats->inventory);
					}
					break;
				default:
					break;
			}
		}
	}
}

void skuLitAnimate(Entity* my, double dist)
{
	//TODO: Invisibility check.

	// move legs
	if ( (ticks % 10 == 0 && dist > 0.1) || (MONSTER_ATTACKTIME != MONSTER_ATTACK) )
	{
		MONSTER_ATTACKTIME = MONSTER_ATTACK;
		if ( my->sprite == 1445 )
		{
			my->sprite = 1446;
		}
		else if (my->sprite == 1446)
		{
			my->sprite = 1447;
		}
		else
		{
			my->sprite = 1445;
		}
	}
}

void skuLitDie(Entity* my)
{
	int c = 0;
	for ( c = 0; c < 5; c++ )
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood();

	playSoundEntity(my, 694, 64);
	list_RemoveNode(my->mynode);
	return;
}
