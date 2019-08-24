/*-------------------------------------------------------------------------------

	BARONY
	File: monster_matilda.cpp
	Desc: implements all of the matilda monster's code

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "net.hpp"
#include "items.hpp"
#include "collision.hpp"
#include "player.hpp"

void initMatilda(Entity* my, Stat* myStats)
{
	int c;

	my->flags[UPDATENEEDED] = true;
	my->flags[INVISIBLE] = false;

	my->sprite = 881;
	if (multiplayer != CLIENT)
	{
		MONSTER_SPOTSND = 422;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 425;
		MONSTER_IDLEVAR = 4;
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
			//This IS a boss

			// random effects

			// generates equipment and weapons if available from editor
			createMonsterEquipment(myStats);

			// create any custom inventory items from editor if available
			createCustomInventory(myStats, customItemsToGenerate);

			// count if any custom inventory items from editor
			int customItems = countCustomItems(myStats);
			//max limit of 6 custom items per entity.

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
				switch( rand()% 10 ) //10% chance for rare drop
				{
				case 0:
					//newItem(ABYSSAL_MASK, EXCELLENT, 0, 1, rand(), false, &myStats->inventory);	//once abyssal_mask is created.
					break;
				}
				break;
			default:
				break;
			}
		}
	}

	// right pedipalp
	Entity* entity = newEntity(882, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[MATILDA][1][0]; // 1
	entity->focaly = limbs[MATILDA][1][1]; // 0
	entity->focalz = limbs[MATILDA][1][2]; // 1
	entity->behavior = &actMatildaLimb;
	entity->parent = my->getUID();
	node_t* node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// left pedipalp
	entity = newEntity(882, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[MATILDA][2][0]; // 1
	entity->focaly = limbs[MATILDA][2][1]; // 0
	entity->focalz = limbs[MATILDA][2][2]; // 1
	entity->behavior = &actMatildaLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// eight legs :)
	for (c = 0; c < 8; c++)
	{
		// "thigh"
		entity = newEntity(884, 0, map.entities, nullptr); //Limb entity.
		entity->sizex = 4;
		entity->sizey = 4;
		entity->skill[2] = my->getUID();
		entity->fskill[2] = (c / 8.f);
		entity->flags[PASSABLE] = true;
		entity->flags[NOUPDATE] = true;
		entity->flags[USERFLAG2] = my->flags[USERFLAG2];
		entity->focalx = limbs[MATILDA][3][0]; // 1
		entity->focaly = limbs[MATILDA][3][1]; // 0
		entity->focalz = limbs[MATILDA][3][2]; // -1
		entity->behavior = &actMatildaLimb;
		entity->parent = my->getUID();
		node = list_AddNodeLast(&my->children);
		node->element = entity;
		node->deconstructor = &emptyDeconstructor;
		node->size = sizeof(Entity*);
		my->bodyparts.push_back(entity);

		// "shin"
		entity = newEntity(883, 0, map.entities, nullptr); //Limb entity.
		entity->sizex = 4;
		entity->sizey = 4;
		entity->skill[2] = my->getUID();
		entity->flags[PASSABLE] = true;
		entity->flags[NOUPDATE] = true;
		entity->flags[USERFLAG2] = my->flags[USERFLAG2];
		entity->focalx = limbs[MATILDA][4][0]; // 3
		entity->focaly = limbs[MATILDA][4][1]; // 0
		entity->focalz = limbs[MATILDA][4][2]; // 0
		entity->behavior = &actMatildaLimb;
		entity->parent = my->getUID();
		node = list_AddNodeLast(&my->children);
		node->element = entity;
		node->deconstructor = &emptyDeconstructor;
		node->size = sizeof(Entity*);
		my->bodyparts.push_back(entity);
	}
}

void matildaDie(Entity* my)
{
	int c = 0;
	for (c = 0; c < 5; c++)
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood(212);

	my->removeMonsterDeathNodes();

	playSoundEntity(my, 429 + rand() % 2, 128);
	list_RemoveNode(my->mynode);
	return;
}

void actMatildaLimb(Entity* my)
{
	my->actMonsterLimb();
}

void matildaMoveBodyparts(Entity* my, Stat* myStats, double dist)
{
	node_t* node;
	Entity* entity;
	int bodypart;

	// set invisibility //TODO: isInvisible()?
	if (multiplayer != CLIENT)
	{
		if (myStats->EFFECTS[EFF_INVISIBLE] == true)
		{
			my->flags[INVISIBLE] = true;
			my->flags[BLOCKSIGHT] = false;
			bodypart = 0;
			for (node = my->children.first; node != NULL; node = node->next)
			{
				if (bodypart < 2)
				{
					bodypart++;
					continue;
				}
				entity = (Entity*)node->element;
				if (!entity->flags[INVISIBLE])
				{
					entity->flags[INVISIBLE] = true;
					serverUpdateEntityBodypart(my, bodypart);
				}
				bodypart++;
			}
		}
		else
		{
			my->flags[INVISIBLE] = false;
			my->flags[BLOCKSIGHT] = true;
			bodypart = 0;
			for (node = my->children.first; node != NULL; node = node->next)
			{
				if (bodypart < 2)
				{
					bodypart++;
					continue;
				}
				entity = (Entity*)node->element;
				if (entity->flags[INVISIBLE])
				{
					entity->flags[INVISIBLE] = false;
					serverUpdateEntityBodypart(my, bodypart);
					serverUpdateEntityFlag(my, INVISIBLE);
				}
				bodypart++;
			}
		}
	}

	// animate limbs
	for (bodypart = 0, node = my->children.first; node != NULL; node = node->next, bodypart++)
	{
		if (bodypart < 2)
		{
			continue;
		}
		entity = (Entity*)node->element;
		Entity* previous = NULL; // previous part
		if (bodypart > 2)
		{
			previous = (Entity*)node->prev->element;
		}
		entity->x = my->x;
		entity->y = my->y;
		entity->z = my->z;
		entity->yaw = my->yaw;
		entity->pitch = my->pitch;
		entity->roll = my->roll;

		switch (bodypart)
		{
			// right pedipalp
		case 2:
			entity->x += cos(my->yaw) * 2 + cos(my->yaw + PI / 2) * 2;
			entity->y += sin(my->yaw) * 2 + sin(my->yaw + PI / 2) * 2;
			entity->yaw += PI / 10;
			entity->pitch -= PI / 8;
			break;
			// left pedipalp
		case 3:
			entity->x += cos(my->yaw) * 2 - cos(my->yaw + PI / 2) * 2;
			entity->y += sin(my->yaw) * 2 - sin(my->yaw + PI / 2) * 2;
			entity->yaw -= PI / 10;
			entity->pitch -= PI / 8;
			break;

			// 1st/5th leg:
			// thigh
		case 4:
		case 12:
			entity->x += cos(my->yaw) * 1 + cos(my->yaw + PI / 2) * 2.5 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * 1 + sin(my->yaw + PI / 2) * 2.5 * (1 - 2 * (bodypart > 11));
			if (dist > 0.1)
			{
				if (!entity->skill[4])
				{
					entity->fskill[2] += .1;
					if (entity->fskill[2] >= 1)
					{
						entity->fskill[2] = 1;
						entity->skill[4] = 1;
					}
				}
				else
				{
					entity->fskill[2] -= .1;
					if (entity->fskill[2] <= 0)
					{
						entity->fskill[2] = 0;
						entity->skill[4] = 0;
					}
				}
			}
			entity->z += entity->fskill[2];
			entity->yaw += PI / 6 * (1 - 2 * (bodypart > 11));
			entity->pitch += PI / 4;
			break;
			// shin
		case 5:
		case 13:
			entity->x = previous->x;
			entity->y = previous->y;
			entity->z = previous->z;
			entity->yaw = previous->yaw;
			entity->pitch = previous->pitch;
			entity->x += cos(my->yaw) * 3 + cos(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * 3 + sin(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			entity->z += .5;
			entity->pitch += PI / 6 - PI / 4;
			entity->pitch -= (PI / 10) * (previous->z - my->z);
			break;

			// 2nd/6th leg:
			// thigh
		case 6:
		case 14:
			entity->x += cos(my->yaw + PI / 2) * 3 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw + PI / 2) * 3 * (1 - 2 * (bodypart > 11));
			if (dist > 0.1)
			{
				if (!entity->skill[4])
				{
					entity->fskill[2] += .1;
					if (entity->fskill[2] >= 1)
					{
						entity->fskill[2] = 1;
						entity->skill[4] = 1;
					}
				}
				else
				{
					entity->fskill[2] -= .1;
					if (entity->fskill[2] <= 0)
					{
						entity->fskill[2] = 0;
						entity->skill[4] = 0;
					}
				}
			}
			entity->z += entity->fskill[2];
			entity->yaw += PI / 3 * (1 - 2 * (bodypart > 11));
			entity->pitch += PI / 4;
			break;
			// shin
		case 7:
		case 15:
			entity->x = previous->x;
			entity->y = previous->y;
			entity->z = previous->z;
			entity->yaw = previous->yaw;
			entity->pitch = previous->pitch;
			entity->x += cos(my->yaw) * 1.75 + cos(my->yaw + PI / 2) * 3 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * 1.75 + sin(my->yaw + PI / 2) * 3 * (1 - 2 * (bodypart > 11));
			entity->z += .5;
			entity->pitch += PI / 6 - PI / 4;
			entity->pitch -= (PI / 10) * (previous->z - my->z);
			break;

			// 3rd/7th leg:
			// thigh
		case 8:
		case 16:
			entity->x += cos(my->yaw) * -.5 + cos(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * -.5 + sin(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			if (dist > 0.1)
			{
				if (!entity->skill[4])
				{
					entity->fskill[2] += .1;
					if (entity->fskill[2] >= 1)
					{
						entity->fskill[2] = 1;
						entity->skill[4] = 1;
					}
				}
				else
				{
					entity->fskill[2] -= .1;
					if (entity->fskill[2] <= 0)
					{
						entity->fskill[2] = 0;
						entity->skill[4] = 0;
					}
				}
			}
			entity->z += entity->fskill[2];
			entity->yaw += (PI / 2 + PI / 8) * (1 - 2 * (bodypart > 11));
			entity->pitch += PI / 4;
			break;
			// shin
		case 9:
		case 17:
			entity->x = previous->x;
			entity->y = previous->y;
			entity->z = previous->z;
			entity->yaw = previous->yaw;
			entity->pitch = previous->pitch;
			entity->x += cos(my->yaw) * -1.25 + cos(my->yaw + PI / 2) * 3.25 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * -1.25 + sin(my->yaw + PI / 2) * 3.25 * (1 - 2 * (bodypart > 11));
			entity->z += .5;
			entity->pitch += PI / 6 - PI / 4;
			entity->pitch -= (PI / 10) * (previous->z - my->z);
			break;

			// 4th/8th leg:
			// thigh
		case 10:
		case 18:
			entity->x += cos(my->yaw) * -.5 + cos(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * -.5 + sin(my->yaw + PI / 2) * 2 * (1 - 2 * (bodypart > 11));
			if (dist > 0.1)
			{
				if (!entity->skill[4])
				{
					entity->fskill[2] += .1;
					if (entity->fskill[2] >= 1)
					{
						entity->fskill[2] = 1;
						entity->skill[4] = 1;
					}
				}
				else
				{
					entity->fskill[2] -= .1;
					if (entity->fskill[2] <= 0)
					{
						entity->fskill[2] = 0;
						entity->skill[4] = 0;
					}
				}
			}
			entity->z += entity->fskill[2];
			entity->yaw += (PI / 2 + PI / 3) * (1 - 2 * (bodypart > 11));
			entity->pitch += PI / 4;
			break;
			// shin
		case 11:
		case 19:
			entity->x = previous->x;
			entity->y = previous->y;
			entity->z = previous->z;
			entity->yaw = previous->yaw;
			entity->pitch = previous->pitch;
			entity->x += cos(my->yaw) * -3 + cos(my->yaw + PI / 2) * 1.75 * (1 - 2 * (bodypart > 11));
			entity->y += sin(my->yaw) * -3 + sin(my->yaw + PI / 2) * 1.75 * (1 - 2 * (bodypart > 11));
			entity->z += .5;
			entity->pitch += PI / 6 - PI / 4;
			entity->pitch += (PI / 10) * (previous->z - my->z);
			break;
		default:
			entity->flags[INVISIBLE] = true; // for debugging
			break;
		}
	}
}
