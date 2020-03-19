
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "items.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "player.hpp"

void initMetallicBeast(Entity* my, Stat* myStats)
{
	int c;
	node_t* node;

	my->initMonster(1438);

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 681;
		MONSTER_SPOTVAR = 2;
		MONSTER_IDLESND = 683;
		MONSTER_IDLEVAR = 3;
	}
	if ( multiplayer != CLIENT && !MONSTER_INIT )
	{
		if ( myStats != nullptr )
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
			if ( rand() % 50 || my->flags[USERFLAG2] )
			{
			}
			else
			{
				strcpy(myStats->name, "Titanium");
				myStats->LVL = 50;
				myStats->STR = 80;
				myStats->PER = 50;
				myStats->INT = 50;
				newItem(INQUISITOR_HAMMER, EXCELLENT, 0, 1, 0, false, nullptr);

				for ( c = 0; c < 1; c++ )
				{
					Entity* entity = summonMonster(COCKATRICE, my->x, my->y);
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
			int customItems = countCustomItems(myStats);
			//max limit of 6 custom items per entity.

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
					if (rand() % 10 == 0)
					{
						newItem(TOOL_MAGIC_SCRAP, EXCELLENT, 0, 5, 0, false, nullptr);
					}
				case 2:
					if (rand() % 10 == 0)
					{
						newItem(TOOL_METAL_SCRAP, EXCELLENT, 0, 5, 0, false, nullptr);
					}
				case 1:
					if ( rand() % 20 == 0 )
					{
						myStats->weapon = newItem(SPELLBOOK_LIGHTNING, EXCELLENT, 0, 1, 0, false, nullptr);
					}
					break;
				default:
					break;
			}
		}
	}

	// torso
	Entity* entity = newEntity(1444, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][1][0]; // 0
	entity->focaly = limbs[METALLICBEAST][1][1]; // 0
	entity->focalz = limbs[METALLICBEAST][1][2]; // 0
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// right leg
	entity = newEntity(1443, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][2][0]; // 1
	entity->focaly = limbs[METALLICBEAST][2][1]; // 0
	entity->focalz = limbs[METALLICBEAST][2][2]; // 5
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// left leg
	entity = newEntity(1441, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][3][0]; // 1
	entity->focaly = limbs[METALLICBEAST][3][1]; // 0
	entity->focalz = limbs[METALLICBEAST][3][2]; // 5
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// right arm
	entity = newEntity(1442, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][4][0]; // -.25
	entity->focaly = limbs[METALLICBEAST][4][1]; // 0
	entity->focalz = limbs[METALLICBEAST][4][2]; // 4
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// left arm
	entity = newEntity(1440, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][5][0]; // -.25
	entity->focaly = limbs[METALLICBEAST][5][1]; // 0
	entity->focalz = limbs[METALLICBEAST][5][2]; // 4
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// jaw
	entity = newEntity(1439, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[METALLICBEAST][6][0]; // 1.5
	entity->focaly = limbs[METALLICBEAST][6][1]; // 0
	entity->focalz = limbs[METALLICBEAST][6][2]; // 1
	entity->behavior = &actMetallicBeastLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);
}

void actMetallicBeastLimb(Entity* my)
{
	my->actMonsterLimb();
}

void metallicBeastDie(Entity* my)
{
	playSoundEntity(my, 686 + rand() % 3, 128);

	my->removeMonsterDeathNodes();

	list_RemoveNode(my->mynode);
	return;
}

#define METALLICBEASTWALKSPEED .135

void metallicBeastMoveBodyparts(Entity* my, Stat* myStats, double dist)
{
	node_t* node;
	Entity* entity = nullptr;
	Entity* rightbody = nullptr;
	int bodypart;

	// set invisibility //TODO: isInvisible()?
	if ( multiplayer != CLIENT )
	{
		if ( myStats->EFFECTS[EFF_INVISIBLE] == true )
		{
			my->flags[INVISIBLE] = true;
			my->flags[BLOCKSIGHT] = false;
			bodypart = 0;
			for (node = my->children.first; node != nullptr; node = node->next)
			{
				if ( bodypart < LIMB_HUMANOID_TORSO )
				{
					bodypart++;
					continue;
				}
				if ( bodypart >= 7 )
				{
					break;
				}
				entity = (Entity*)node->element;
				if ( !entity->flags[INVISIBLE] )
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
			for (node = my->children.first; node != nullptr; node = node->next)
			{
				if ( bodypart < LIMB_HUMANOID_TORSO )
				{
					bodypart++;
					continue;
				}
				if ( bodypart >= 7 )
				{
					break;
				}
				entity = (Entity*)node->element;
				if ( entity->flags[INVISIBLE] )
				{
					entity->flags[INVISIBLE] = false;
					serverUpdateEntityBodypart(my, bodypart);
					serverUpdateEntityFlag(my, INVISIBLE);
				}
				bodypart++;
			}
		}
	}

	//Move bodyparts
	for (bodypart = 0, node = my->children.first; node != nullptr; node = node->next, bodypart++)
	{
		if ( bodypart < LIMB_HUMANOID_TORSO )
		{
			continue;
		}
		entity = (Entity*)node->element;
		entity->x = my->x;
		entity->y = my->y;
		entity->z = my->z;
		entity->yaw = my->yaw;
		if ( bodypart == LIMB_HUMANOID_RIGHTLEG || bodypart == LIMB_HUMANOID_LEFTARM )
		{
			if ( bodypart == LIMB_HUMANOID_RIGHTLEG )
			{
				rightbody = (Entity*)node->next->element;
			}
			if ( bodypart == LIMB_HUMANOID_RIGHTLEG || my->monsterAttack == 0 )
			{
				// swing right leg, left arm in sync.
				if ( dist > 0.1 )
				{
					if ( !rightbody->skill[0] )
					{
						entity->pitch -= dist * METALLICBEASTWALKSPEED;
						if ( entity->pitch < -PI / 4.0 )
						{
							entity->pitch = -PI / 4.0;
							if (bodypart == 3)
							{
								entity->skill[0] = 1;
							}
						}
					}
					else
					{
						entity->pitch += dist * METALLICBEASTWALKSPEED;
						if ( entity->pitch > PI / 4.0 )
						{
							entity->pitch = PI / 4.0;
							if (bodypart == 3)
							{
								entity->skill[0] = 0;
							}
						}
					}
				}
				else
				{
					// if not moving, reset position of the leg/arm.
					if ( entity->pitch < 0 )
					{
						entity->pitch += 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch > 0 )
						{
							entity->pitch = 0;
						}
					}
					else if ( entity->pitch > 0 )
					{
						entity->pitch -= 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch < 0 )
						{
							entity->pitch = 0;
						}
					}
				}
			}
			else
			{
				// vertical chop windup
				if ( my->monsterAttack == MONSTER_POSE_MELEE_WINDUP1 )
				{
					if ( my->monsterAttackTime == 0 )
					{
						// init rotations
						entity->pitch = 0;
						my->monsterArmbended = 0;
						my->monsterWeaponYaw = 0;
						entity->roll = 0;
						entity->skill[1] = 0;
					}

					limbAnimateToLimit(entity, ANIMATE_PITCH, -0.25, 5 * PI / 4, false, 0.0);

					if ( my->monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
					{
						if ( multiplayer != CLIENT )
						{
							my->attack(1, 0, nullptr);
						}
					}
				}
				// ceiling buster chop windup
				if ( my->monsterAttack == MONSTER_POSE_MELEE_WINDUP2 )
				{
					if ( my->monsterAttackTime == 0 )
					{
						// init rotations
						entity->pitch = 0;
						my->monsterArmbended = 0;
						my->monsterWeaponYaw = 0;
						entity->roll = 0;
						entity->skill[1] = 0;
					}

					limbAnimateToLimit(entity, ANIMATE_PITCH, -0.25, 5 * PI / 4, false, 0.0);

					if ( my->monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
					{
						my->monsterAttack = 1;
					}
				}
				// vertical chop attack
				else if ( my->monsterAttack == 1 )
				{
					if ( entity->pitch >= 3 * PI / 2 )
					{
						my->monsterArmbended = 1;
					}

					if ( entity->skill[1] == 0 )
					{
						// chop forwards
						if ( limbAnimateToLimit(entity, ANIMATE_PITCH, 0.4, PI / 3, false, 0.0) )
						{
							entity->skill[1] = 1;
						}
					}
					else if ( entity->skill[1] == 1 )
					{
						// return to neutral
						if ( limbAnimateToLimit(entity, ANIMATE_PITCH, -0.25, 0, false, 0.0) )
						{
							entity->skill[0] = rightbody->skill[0];
							my->monsterWeaponYaw = 0;
							entity->pitch = rightbody->pitch;
							entity->roll = 0;
							my->monsterArmbended = 0;
							my->monsterAttack = 0;
						}
					}
				}
			}
		}
		else if ( bodypart == LIMB_HUMANOID_LEFTLEG || bodypart == LIMB_HUMANOID_RIGHTARM )
		{
			if ( bodypart == LIMB_HUMANOID_RIGHTARM )
			{
				if ( my->monsterAttack > 0 )
				{
					// vertical chop
					// get leftarm from bodypart 6 element if ready to attack
					Entity* leftarm = (Entity*)node->next->element;

					if ( my->monsterAttack == 1 || my->monsterAttack == MONSTER_POSE_MELEE_WINDUP1
						|| my->monsterAttack == MONSTER_POSE_MELEE_WINDUP2 )
					{
						if ( leftarm != nullptr )
						{
							// follow the right arm animation.
							entity->pitch = leftarm->pitch;
							entity->roll = -leftarm->roll;
						}
					}
				}
			}

			if ( bodypart != LIMB_HUMANOID_RIGHTARM || (my->monsterAttack == 0) )
			{
				// swing right arm/ left leg in sync
				if ( dist > 0.1 )
				{
					if ( entity->skill[0] )
					{
						entity->pitch -= dist * METALLICBEASTWALKSPEED;
						if ( entity->pitch < -PI / 4.0 )
						{
							entity->skill[0] = 0;
							entity->pitch = -PI / 4.0;
						}
					}
					else
					{
						entity->pitch += dist * METALLICBEASTWALKSPEED;
						if ( entity->pitch > PI / 4.0 )
						{
							entity->skill[0] = 1;
							entity->pitch = PI / 4.0;
						}
					}
				}
				else
				{
					// if not moving, reset position of the leg/arm.
					if ( entity->pitch < 0 )
					{
						entity->pitch += 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch > 0 )
						{
							entity->pitch = 0;
						}
					}
					else if ( entity->pitch > 0 )
					{
						entity->pitch -= 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch < 0 )
						{
							entity->pitch = 0;
						}
					}
				}
			}
		}
		else if ( bodypart == 7 )
		{
			// jaw
			if ( MONSTER_ATTACK == 1 )
			{
				limbAnimateToLimit(entity, ANIMATE_PITCH, 0.04, 0.16, false, 0.0);
			}
			else
			{
				entity->pitch = 0;
			}
		}
		switch ( bodypart )
		{
			// torso
			case 2:
				entity->x -= .5 * cos(my->yaw);
				entity->y -= .5 * sin(my->yaw);
				entity->z += 5;
				break;
			// right leg
			case 3:
				entity->x += 2.25 * cos(my->yaw + PI / 2) - 1.25 * cos(my->yaw);
				entity->y += 2.25 * sin(my->yaw + PI / 2) - 1.25 * sin(my->yaw);
				entity->z += 7.5;
				break;
			// left leg
			case 4:
				entity->x -= 2.25 * cos(my->yaw + PI / 2) + 1.25 * cos(my->yaw);
				entity->y -= 2.25 * sin(my->yaw + PI / 2) + 1.25 * sin(my->yaw);
				entity->z += 7.5;
				break;
			// right arm
			case 5:
				entity->x += 5 * cos(my->yaw + PI / 2) - 1 * cos(my->yaw);
				entity->y += 5 * sin(my->yaw + PI / 2) - 1 * sin(my->yaw);
				entity->z += 2.75;
				entity->yaw += MONSTER_WEAPONYAW;
				break;
			// left arm
			case 6:
				entity->x -= 5 * cos(my->yaw + PI / 2) + 1 * cos(my->yaw);
				entity->y -= 5 * sin(my->yaw + PI / 2) + 1 * sin(my->yaw);
				entity->z += 2.75;
				break;
			default:
				break;
		}
	}
	if ( MONSTER_ATTACK > 0 && MONSTER_ATTACK <= MONSTER_POSE_MAGIC_CAST3 )
	{
		MONSTER_ATTACKTIME++;
	}
	else if ( MONSTER_ATTACK == 0 )
	{
		MONSTER_ATTACKTIME = 0;
	}
	else
	{
		// do nothing, don't reset attacktime or increment it.
	}
}
