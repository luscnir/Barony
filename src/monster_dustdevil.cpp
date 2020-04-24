

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "items.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "book.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "player.hpp"
#include "magic/magic.hpp"

void initDustDevil(Entity* my, Stat* myStats)
{
	node_t* node;

	//Sprite 1428 = dustDevil head model
	my->initMonster(1428);

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 667;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 670;
		MONSTER_IDLEVAR = 2;
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
			if (rand() % 50 || my->flags[USERFLAG2])
			{
			}
			else
			{
				strcpy(myStats->name, "Keeper of the sands");
				myStats->LVL += 5;
				myStats->STR += 5;
				for (int c = 0; c < 5; c++)
				{
					Entity* entity = summonMonster(COCKROACH, my->x, my->y);
					if (entity)
					{
						entity->parent = my->getUID();
					}
				}
				myStats->weapon = newItem(INQUISITOR_BOW, EXCELLENT, 0, 1, 0, false, nullptr);
				myStats->helmet = newItem(HAT_HOOD, static_cast<Status>(WORN + rand() % 3), 3, 1, 1, false, nullptr);
				myStats->cloak = newItem(CLOAK_BLACK, SERVICABLE, -3, 1, 0, true, nullptr);
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

			//give weapon
			if ( myStats->weapon == nullptr && myStats->EDITOR_ITEMS[ITEM_SLOT_WEAPON] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
						myStats->weapon = newItem(IRON_DAGGER, WORN, 0, 4, rand(), false, nullptr);
						break;
					case 1:
						myStats->weapon = newItem(SHORTBOW, EXCELLENT, 1, 1, rand(), false, nullptr);
						break;
					case 2:
						myStats->weapon = newItem(IRON_SWORD, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 3:
					case 4:
						myStats->weapon = newItem(WOOD_HAMMER, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 5:
					case 6:
					case 7:
					case 8:
						myStats->weapon = newItem(CROSSBOW, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 9:
						myStats->weapon = newItem(MAKESHIFT_BOW, static_cast<Status>(DECREPIT + rand() % 4), -2 + rand() % 5, 1, rand(), false, nullptr);
						break;
				}
			}

			// generate the default inventory items for the monster, provided the editor sprite allowed enough default slots
			switch ( defaultItems )
			{
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
					if ( rand() % 20 == 0 )
					{
						newItem(ENCHANTED_FEATHER, SERVICABLE, 0, 1, (2 * (ENCHANTED_FEATHER_MAX_DURABILITY - 1)) / 4, false, &myStats->inventory);
					}
					else if ( rand() % 5 == 0 ) // 20% chance
					{
						if ( rand() % 2 )
						{
							newItem(TOOL_ALEMBIC, WORN, -1 + rand() % 3, 1, rand(), false, &myStats->inventory);
						}
						else
						{
							newItem(TOOL_TINKERING_KIT, WORN, -1 + rand() % 3, 1, rand(), false, &myStats->inventory);
						}
					}
				case 1:
					if ( my->hasRangedWeapon() )
					{
						if ( rand() % 5 > 0 ) // 80% chance
						{
							newItem(SPELLBOOK_LIGHT, DECREPIT, 0, 1, MONSTER_ITEM_UNDROPPABLE_APPEARANCE, false, &myStats->inventory);
						}
					}
					break;
				default:
					break;
			}

			//give shield
			if ( myStats->shield == nullptr && myStats->EDITOR_ITEMS[ITEM_SLOT_SHIELD] == 1 )
			{
				if ( myStats->weapon && isRangedWeapon(*myStats->weapon) )
				{
					my->monsterGenerateQuiverItem(myStats);
				}
				else
				{
					switch ( rand() % 10 )
					{
						case 0:
							myStats->shield = newItem(IRON_SHIELD, static_cast<Status>(WORN + rand() % 2), -2 + rand() % 5, 1, rand(), false, nullptr);
							break;
						case 1:
						case 2:
						case 3:
							myStats->shield = newItem(TOOL_CANDLE, static_cast<Status>(DECREPIT + rand() % 4), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 4:
						case 5:
							myStats->shield = newItem(TOOL_LANTERN, EXCELLENT, 0, 1, rand(), false, nullptr);
							break;
						case 6:
							myStats->shield = newItem(STEEL_SHIELD, WORN, -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 7:
						case 8:
						case 9:
							// nothing
							break;
					}
				}
			}

			// give cloak
			if ( myStats->cloak == nullptr && myStats->EDITOR_ITEMS[ITEM_SLOT_CLOAK] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						break;
					case 8:
					case 9:
						myStats->cloak = newItem(CLOAK, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
				}
			}

			// give helm
			if ( myStats->helmet == nullptr && myStats->EDITOR_ITEMS[ITEM_SLOT_HELM] == 1 )
			{
				switch (rand() % 10)
				{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					break;
				case 7:
					myStats->helmet = newItem(TIN_HELM, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, 1, false, nullptr);
					break;
				case 8:
				case 9:
					myStats->helmet = newItem(HAT_HOOD, static_cast<Status>(WORN + rand() % 3), -1 + rand() % 3, 1, 1, false, nullptr);
					break;
				}
			}
		}
	}

	// torso
	Entity* entity = newEntity(1435, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[DUSTDEVIL][1][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][1][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][1][2]; // 0
	entity->behavior = &actDustDevilLimb;
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
	entity->focalx = limbs[DUSTDEVIL][2][0]; // .25
	entity->focaly = limbs[DUSTDEVIL][2][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][2][2]; // 1.5
	entity->behavior = &actDustDevilLimb;
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
	entity->focalx = limbs[DUSTDEVIL][3][0]; // .25
	entity->focaly = limbs[DUSTDEVIL][3][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][3][2]; // 1.5
	entity->behavior = &actDustDevilLimb;
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
	entity->focalx = limbs[DUSTDEVIL][4][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][4][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][4][2]; // 2
	entity->behavior = &actDustDevilLimb;
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
	entity->focalx = limbs[DUSTDEVIL][5][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][5][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][5][2]; // 2
	entity->behavior = &actDustDevilLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// world weapon
	entity = newEntity(-1, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[INVISIBLE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[DUSTDEVIL][6][0]; // 2
	entity->focaly = limbs[DUSTDEVIL][6][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][6][2]; // -.5
	entity->behavior = &actDustDevilLimb;
	entity->parent = my->getUID();
	entity->pitch = .25;
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// shield
	entity = newEntity(-1, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[INVISIBLE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[DUSTDEVIL][7][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][7][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][7][2]; // 1.5
	entity->behavior = &actDustDevilLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// cloak
	entity = newEntity(-1, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = 1.01;
	entity->scaley = 1.01;
	entity->scalez = 1.01;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[INVISIBLE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[DUSTDEVIL][8][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][8][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][8][2]; // 4
	entity->behavior = &actDustDevilLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// helmet
	entity = newEntity(-1, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = 1.1;
	entity->scaley = 1.1;
	entity->scalez = 1.1;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[DUSTDEVIL][9][0]; // 0
	entity->focaly = limbs[DUSTDEVIL][9][1]; // 0
	entity->focalz = limbs[DUSTDEVIL][9][2]; // -2
	entity->behavior = &actDustDevilLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	if ( multiplayer == CLIENT || MONSTER_INIT )
	{
		return;
	}
}

void actDustDevilLimb(Entity* my)
{
	my->actMonsterLimb(true);
}

void dustDevilDie(Entity* my)
{
	int c;
	for ( c = 0; c < 6; ++c )
	{
		Entity* entity = spawnGib(my);
		if ( entity )
		{
			serverSpawnGibForClient(entity);
		}
	}

	my->removeMonsterDeathNodes();

	playSoundEntity(my, 672 + rand() % 2, 128);
	list_RemoveNode(my->mynode);
	return;
}

#define DUSTDEVILWALKSPEED .13

void dustDevilMoveBodyparts(Entity* my, Stat* myStats, double dist)
{
	node_t* node;
	Entity* entity = nullptr, *entity2 = nullptr;
	Entity* rightbody = nullptr;
	Entity* weaponarm = nullptr;
	int bodypart;
	bool wearingring = false;

	// set invisibility //TODO: isInvisible()?
	if ( multiplayer != CLIENT )
	{
		if ( myStats->ring != nullptr )
			if ( myStats->ring->type == RING_INVISIBILITY )
			{
				wearingring = true;
			}
		if ( myStats->cloak != nullptr )
			if ( myStats->cloak->type == CLOAK_INVISIBILITY )
			{
				wearingring = true;
			}
		if (myStats->mask != nullptr)
		{
			if (myStats->mask->type == ABYSSAL_MASK)
			{
				wearingring = true;
			}
		}
		if ( myStats->EFFECTS[EFF_INVISIBLE] == true || wearingring == true )
		{
			my->flags[INVISIBLE] = true;
			my->flags[BLOCKSIGHT] = false;
			bodypart = 0;
			for (node = my->children.first; node != nullptr; node = node->next)
			{
				if ( bodypart < LIMB_HUMANOID_TORSO )
				{
					++bodypart;
					continue;
				}
				if ( bodypart >= LIMB_HUMANOID_WEAPON )
				{
					break;
				}
				entity = (Entity*)node->element;
				if ( !entity->flags[INVISIBLE] )
				{
					entity->flags[INVISIBLE] = true;
					serverUpdateEntityBodypart(my, bodypart);
				}
				++bodypart;
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
					++bodypart;
					continue;
				}
				if ( bodypart >= LIMB_HUMANOID_WEAPON )
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
				++bodypart;
			}
		}

		// sleeping
		if ( myStats->EFFECTS[EFF_ASLEEP] )
		{
			my->z = 4;
			my->pitch = PI / 4;
		}
		else
		{
			my->z = 2.25;
			my->pitch = 0;
		}
	}

	Entity* shieldarm = nullptr;

	//Move bodyparts
	for (bodypart = 0, node = my->children.first; node != nullptr; node = node->next, ++bodypart)
	{
		if ( bodypart < LIMB_HUMANOID_TORSO )
		{
			continue;
		}
		entity = (Entity*)node->element;
		entity->x = my->x;
		entity->y = my->y;
		entity->z = my->z;
		if ( MONSTER_ATTACK == MONSTER_POSE_MAGIC_WINDUP1 && bodypart == LIMB_HUMANOID_RIGHTARM )
		{
			// don't let the creatures's yaw move the casting arm
		}
		else
		{
			entity->yaw = my->yaw;
		}

		if ( bodypart == LIMB_HUMANOID_RIGHTLEG || bodypart == LIMB_HUMANOID_LEFTARM )
		{
			my->humanoidAnimateWalk(entity, node, bodypart, DUSTDEVILWALKSPEED, dist, 0.4);
		}
		else if ( bodypart == LIMB_HUMANOID_LEFTLEG || bodypart == LIMB_HUMANOID_RIGHTARM || bodypart == LIMB_HUMANOID_CLOAK )
		{
			// left leg, right arm, cloak.
			if ( bodypart == LIMB_HUMANOID_RIGHTARM )
			{
				weaponarm = entity;
				if ( my->monsterAttack > 0 )
				{
					my->handleWeaponArmAttack(entity);
				}
			}
			else if ( bodypart == LIMB_HUMANOID_CLOAK )
			{
				entity->pitch = entity->fskill[0];
			}

			my->humanoidAnimateWalk(entity, node, bodypart, DUSTDEVILWALKSPEED, dist, 0.4);
			
			if ( bodypart == LIMB_HUMANOID_CLOAK )
			{
				entity->fskill[0] = entity->pitch;
				entity->roll = my->roll - fabs(entity->pitch) / 2;
				entity->pitch = 0;
			}
		}
		switch ( bodypart )
		{
			// torso
			case LIMB_HUMANOID_TORSO:
				entity->x -= .25 * cos(my->yaw);
				entity->y -= .25 * sin(my->yaw);
				entity->z += 1.25;
				break;
			// right leg
			case LIMB_HUMANOID_RIGHTLEG:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == nullptr )
					{
						entity->sprite = 1434;
					}
					else
					{
						my->setBootSprite(entity, SPRITE_BOOT_RIGHT_OFFSET);
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x += 1.25 * cos(my->yaw + PI / 2);
				entity->y += 1.25 * sin(my->yaw + PI / 2);
				entity->z += 2.75;
				if ( my->z >= 3.9 && my->z <= 4.1 )
				{
					entity->yaw += PI / 8;
					entity->pitch = -PI / 2;
				}
				break;
			// left leg
			case LIMB_HUMANOID_LEFTLEG:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == nullptr )
					{
						entity->sprite = 1431;
					}
					else
					{
						my->setBootSprite(entity, SPRITE_BOOT_LEFT_OFFSET);
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= 1.25 * cos(my->yaw + PI / 2);
				entity->y -= 1.25 * sin(my->yaw + PI / 2);
				entity->z += 2.75;
				if ( my->z >= 3.9 && my->z <= 4.1 )
				{
					entity->yaw -= PI / 8;
					entity->pitch = -PI / 2;
				}
				break;
			// right arm
			case LIMB_HUMANOID_RIGHTARM:
			{
				node_t* weaponNode = list_Node(&my->children, 7);
				if ( weaponNode )
				{
					Entity* weapon = (Entity*)weaponNode->element;
					if ( my->monsterArmbended || (weapon->flags[INVISIBLE] && my->monsterState == MONSTER_STATE_WAIT) )
					{
						// if weapon invisible and I'm not moving, relax arm.
						entity->focalx = limbs[DUSTDEVIL][4][0]; // 0
						entity->focaly = limbs[DUSTDEVIL][4][1]; // 0
						entity->focalz = limbs[DUSTDEVIL][4][2]; // 2
						entity->sprite = 1432;
					}
					else
					{
						// else flex arm.
						entity->focalx = limbs[DUSTDEVIL][4][0] + 1; // 1
						entity->focaly = limbs[DUSTDEVIL][4][1]; // 0
						entity->focalz = limbs[DUSTDEVIL][4][2] - 1; // 1
						entity->sprite = 1433;
					}
				}
				entity->x += 2.5 * cos(my->yaw + PI / 2) - .75 * cos(my->yaw);
				entity->y += 2.5 * sin(my->yaw + PI / 2) - .75 * sin(my->yaw);
				entity->z -= .25;
				entity->yaw += MONSTER_WEAPONYAW;
				if ( my->z >= 3.9 && my->z <= 4.1 )
				{
					entity->pitch = 0;
				}
				break;
			}
			// left arm
			case LIMB_HUMANOID_LEFTARM:
			{
				shieldarm = entity;
				node_t* shieldNode = list_Node(&my->children, 8);
				if ( shieldNode )
				{
					Entity* shield = (Entity*)shieldNode->element;
					if ( shield->flags[INVISIBLE] && my->monsterState == MONSTER_STATE_WAIT )
					{
						// if shield invisible and I'm not moving, relax arm.
						entity->focalx = limbs[DUSTDEVIL][5][0]; // 0
						entity->focaly = limbs[DUSTDEVIL][5][1]; // 0
						entity->focalz = limbs[DUSTDEVIL][5][2]; // 2
						entity->sprite = 1429;
					}
					else
					{
						// else flex arm.
						entity->focalx = limbs[DUSTDEVIL][5][0] + 1; // 1
						entity->focaly = limbs[DUSTDEVIL][5][1]; // 0
						entity->focalz = limbs[DUSTDEVIL][5][2] - 1; // 1
						entity->sprite = 1430;
					}
				}
				entity->x -= 2.5 * cos(my->yaw + PI / 2) + .75 * cos(my->yaw);
				entity->y -= 2.5 * sin(my->yaw + PI / 2) + .75 * sin(my->yaw);
				entity->z -= .25;
				if ( my->z >= 3.9 && my->z <= 4.1 )
				{
					entity->pitch = 0;
				}
				if ( my->monsterDefend && my->monsterAttack == 0 )
				{
					MONSTER_SHIELDYAW = PI / 5;
				}
				else
				{
					MONSTER_SHIELDYAW = 0;
				}
				entity->yaw += MONSTER_SHIELDYAW;
				break;
			}
			// weapon
			case LIMB_HUMANOID_WEAPON:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->weapon == nullptr || myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->sprite = itemModel(myStats->weapon);
						if ( itemCategory(myStats->weapon) == SPELLBOOK )
						{
							entity->flags[INVISIBLE] = true;
						}
						else
						{
							entity->flags[INVISIBLE] = false;
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				else
				{
					if ( entity->sprite <= 0 )
					{
						entity->flags[INVISIBLE] = true;
					}
				}
				if ( weaponarm != nullptr )
				{
					my->handleHumanoidWeaponLimb(entity, weaponarm);
				}
				break;
			// shield
			case LIMB_HUMANOID_SHIELD:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shield == nullptr )
					{
						entity->flags[INVISIBLE] = true;
						entity->sprite = 0;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
						entity->sprite = itemModel(myStats->shield);
						if ( itemTypeIsQuiver(myStats->shield->type) )
						{
							entity->handleQuiverThirdPersonModel(*myStats);
						}
					}
					if ( myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
					{
						entity->flags[INVISIBLE] = true;
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				else
				{
					if ( entity->sprite <= 0 )
					{
						entity->flags[INVISIBLE] = true;
					}
				}
				entity->x -= 2.5 * cos(my->yaw + PI / 2) + .20 * cos(my->yaw);
				entity->y -= 2.5 * sin(my->yaw + PI / 2) + .20 * sin(my->yaw);
				entity->z += 1;
				entity->yaw = shieldarm->yaw;
				entity->roll = 0;
				entity->pitch = 0;
				if (entity->sprite == items[TOOL_TORCH].index || entity->sprite == items[TOOL_CANDLE].index || entity->sprite == items[TOOL_CANDLE_TIMELESS].index)
				{
					entity2 = spawnFlame(entity, SPRITE_FLAME);
					entity2->x += 2 * cos(entity->yaw);
					entity2->y += 2 * sin(entity->yaw);
					entity2->z -= 2;
				}
				else if ( entity->sprite == items[TOOL_CRYSTALSHARD].index )
				{
					entity2 = spawnFlame(entity, SPRITE_CRYSTALFLAME);
					entity2->x += 2 * cos(entity->yaw);
					entity2->y += 2 * sin(entity->yaw);
					entity2->z -= 2;
				}
				else if ( entity->sprite == items[TOOL_LANTERN].index )
				{
					entity->z += 2;
					entity2 = spawnFlame(entity, SPRITE_FLAME);
					entity2->x += 2 * cos(entity->yaw);
					entity2->y += 2 * sin(entity->yaw);
					entity2->z += 1;
				}
				else if (entity->sprite == items[TOOL_GREENTORCH].index)
				{
					entity2 = spawnFlame(entity, SPRITE_GREENFLAME);
					entity2->x += 2 * cos(entity->yaw);
					entity2->y += 2 * sin(entity->yaw);
					entity2->z -= 2;
				}
				else if (entity->sprite == items[INQUISITOR_LANTERN].index)
				{
					entity->z += 2;
					entity2 = spawnFlame(entity, SPRITE_ANGELFLAME);
					entity2->x += 2 * cos(entity->yaw);
					entity2->y += 2 * sin(entity->yaw);
					entity2->z += 1;
				}
				if (MONSTER_SHIELDYAW > PI / 32)
				{
					if (entity->sprite != items[TOOL_TORCH].index && entity->sprite != items[TOOL_LANTERN].index && entity->sprite != items[TOOL_CRYSTALSHARD].index
						&& entity->sprite != items[TOOL_GREENTORCH].index && entity->sprite != items[INQUISITOR_LANTERN].index && entity->sprite != items[TOOL_CANDLE].index
						&& entity->sprite != items[TOOL_CANDLE_TIMELESS].index)
					{
						// shield, so rotate a little.
						entity->roll += PI / 64;
					}
					else
					{
						entity->x += 0.25 * cos(my->yaw);
						entity->y += 0.25 * sin(my->yaw);
						entity->pitch += PI / 16;
						if ( entity2 )
						{
							entity2->x += 0.75 * cos(shieldarm->yaw);
							entity2->y += 0.75 * sin(shieldarm->yaw);
						}
					}
				}
				if ( itemSpriteIsQuiverThirdPersonModel(entity->sprite) )
				{
					/*shieldLimb->x -= -0.25 * cos(this->yaw + PI / 2) + 1.25 * cos(this->yaw);
					shieldLimb->y -= -0.25 * sin(this->yaw + PI / 2) + 1.25 * sin(this->yaw);*/
					entity->x -= 0.25 * cos(my->yaw + PI / 2);
					entity->y -= 0.25 * sin(my->yaw + PI / 2);
					entity->z += 1;
					entity->yaw += PI / 6;
				}
				break;
			// cloak
			case LIMB_HUMANOID_CLOAK:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->cloak == nullptr || myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
						entity->sprite = itemModel(myStats->cloak);
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				else
				{
					if ( entity->sprite <= 0 )
					{
						entity->flags[INVISIBLE] = true;
					}
				}
				entity->x -= cos(my->yaw) * 1.5;
				entity->y -= sin(my->yaw) * 1.5;
				entity->yaw += PI / 2;
				break;
			// helmet
			case LIMB_HUMANOID_HELMET:
				entity->focalx = limbs[DUSTDEVIL][9][0]; // 0
				entity->focaly = limbs[DUSTDEVIL][9][1]; // 0
				entity->focalz = limbs[DUSTDEVIL][9][2]; // -2
				entity->pitch = my->pitch;
				entity->roll = 0;
				if ( multiplayer != CLIENT )
				{
					entity->sprite = itemModel(myStats->helmet);
					if ( myStats->helmet == NULL || myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				else
				{
					if ( entity->sprite <= 0 )
					{
						entity->flags[INVISIBLE] = true;
					}
				}
				my->setHelmetLimbOffset(entity);
				break;
		}
	}
	// rotate shield a bit
	node_t* shieldNode = list_Node(&my->children, 8);
	if ( shieldNode )
	{
		Entity* shieldEntity = (Entity*)shieldNode->element;
		if (shieldEntity->sprite != items[TOOL_TORCH].index && shieldEntity->sprite != items[TOOL_LANTERN].index && shieldEntity->sprite != items[TOOL_CRYSTALSHARD].index
			&& shieldEntity->sprite != items[TOOL_GREENTORCH].index && shieldEntity->sprite != items[INQUISITOR_LANTERN].index && shieldEntity->sprite != items[TOOL_CANDLE].index
			&& shieldEntity->sprite != items[TOOL_CANDLE_TIMELESS].index)
		{
			shieldEntity->yaw -= PI / 6;
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
