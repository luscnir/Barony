
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

void initCholorosh(Entity* my, Stat* myStats)
{
	int c;
	node_t* node;
	int boss = 0;

	my->initMonster(942);// chlorosh head

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = 464;
		MONSTER_SPOTVAR = 3;
		MONSTER_IDLESND = 467;
		MONSTER_IDLEVAR = 3;
	}

	if ( multiplayer != CLIENT && !MONSTER_INIT )
	{
		if ( myStats != nullptr )
		{
			bool minion = false;
			if ( !myStats->leader_uid )
			{
				myStats->leader_uid = 0;
			}

			if ( my->parent != 0 )
			{
				minion = true;
			}

			/*if ( !strncmp(myStats->name, "chlorosh knight", strlen("chlorosh knight")) )
			{
				myStats->MAXHP = 200;
				myStats->HP = myStats->MAXHP;
				myStats->OLDHP = myStats->HP;
				myStats->RANDOM_MAXHP = 20;
				myStats->RANDOM_HP = myStats->RANDOM_MAXHP;
				myStats->STR = 25;
				myStats->DEX = 5;
				myStats->CON = 10;
				myStats->INT = -1;
				myStats->PER = 20;
				myStats->RANDOM_PER = 5;
				myStats->CHR = -10;
				myStats->weapon = newItem(STEEL_SWORD, EXCELLENT,-1, 1, rand(), false, &myStats->inventory);
				myStats->shield = newItem(STEEL_SHIELD, EXCELLENT,-1, 1, rand(), false, &myStats->inventory);
			}*/

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
					strcpy(myStats->name, "Twisted Vineweaver");
					myStats->STR += 10;
					myStats->DEX += 5;
					myStats->CHR += 1;
					myStats->INT += 1;
					myStats->PER += 1;
					myStats->CON += 10;
					myStats->MAXHP += 75;
					myStats->HP = myStats->MAXHP -1;
					myStats->OLDHP = myStats->MAXHP -1;		//Can not be full HP or abyssal_crossbow can stun-lock the player/target.
					myStats->weapon = newItem(ABYSSAL_CROSSBOW, EXCELLENT, 1 , 1, rand(), false, &myStats->inventory);
					myStats->helmet = newItem(STEEL_HELM, SERVICABLE, rand() % 5, 1, rand(), false, nullptr);
					myStats->breastplate = newItem(STEEL_BREASTPIECE, SERVICABLE, 0, 1, rand(), false, nullptr);
					myStats->shoes = newItem(STEEL_BOOTS, SERVICABLE, 0, 1, rand(), false, nullptr);
					break;
				case 1:
					strcpy(myStats->name, "Anti-mage Druid");
					myStats->STR += 10;
					myStats->DEX += 2;
					myStats->CHR += 5;
					myStats->INT += 10;
					myStats->PER += 1;
					myStats->CON += 10;
					myStats->MAXHP += 50;
					myStats->HP = myStats->MAXHP;
					myStats->OLDHP = myStats->MAXHP;
					my->setEffect(EFF_MAGICREFLECT, true, -1, true);
					myStats->weapon = newItem(POTION_SICKNESS, EXCELLENT, -1, 4, rand(), true, &myStats->inventory);
					myStats->helmet = newItem(HAT_WIZARD, SERVICABLE, 3 + rand() % 3, 1, rand(), false, nullptr);
					myStats->shoes = newItem(GLOVES_DEXTERITY, WORN, 7, 1, rand(), false, nullptr);
					newItem(SHORTBOW, EXCELLENT, -1, 1, rand(), true, &myStats->inventory);
					break;
				}
			}

			// random effects
			if ( rand() % 20 == 0 )
			{
				my->setEffect(EFF_FAST, true, 500 + rand() % 1000, false);
			}

			// generates equipment and weapons if available from editor
			createMonsterEquipment(myStats);

			// create any custom inventory items from editor if available
			createCustomInventory(myStats, customItemsToGenerate);

			// count if any custom inventory items from editor
			int customItems = countCustomItems(myStats); //max limit of 6 custom items per entity.

			// count any inventory items set to default in edtior
			int defaultItems = countDefaultItems(myStats);

			my->setHardcoreStats(*myStats);

			bool isShaman = false;
			if ( rand() % 2 && boss == 0 && !minion )
			{
				isShaman = true;
			}
			else
			{
				myStats->DEX += 1;
				myStats->STR += 1;
			}
			
			// generate the default inventory items for the monster, provided the editor sprite allowed enough default slots
			switch ( defaultItems )
			{
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
				case 1:
					if ( isShaman && rand() % 10 == 0 )
					{
						switch ( rand() % 3 )
						{
							case 0:
								newItem(SPELLBOOK_ACID_SPRAY, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, &myStats->inventory);
								break;
							case 1:
								newItem(SPELLBOOK_LIGHTNING, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, &myStats->inventory);
								break;
							case 2:
								newItem(SPELLBOOK_COLD, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, &myStats->inventory);
								break;
						}
					}
					break;
				default:
					break;
			}


			//Give weapons.
			if ( !boss )
			{
				if ( !isShaman && rand() % 3 > 0 )
				{
					newItem(STEEL_CHAKRAM, static_cast<Status>(rand() % 3 + DECREPIT), 0, rand() % 1, rand(), false, &myStats->inventory);
				}
				if ( rand() % 4 > 0 )
				{
					// undroppable
					newItem(POTION_ACID, static_cast<Status>(rand() % 3 + DECREPIT), 0, 1, MONSTER_ITEM_UNDROPPABLE_APPEARANCE, false, &myStats->inventory);
				}
			}

			if ( isShaman )
			{
				//give shield
				if ( myStats->shield == nullptr )
				{
					// give shield
					switch ( rand() % 20 )
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							myStats->shield = newItem(STEEL_SHIELD_RESISTANCE, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 5:
						case 6:
						case 7:
							myStats->shield = newItem(IRON_SHIELD, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 8:
							myStats->shield = newItem(MIRROR_SHIELD, static_cast<Status>(rand() % 4 + DECREPIT), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						default:
							break;
					}
				}
				// give cloak
				if ( myStats->cloak == nullptr )
				{
					switch ( rand() % 10 )
					{
						case 0:
							myStats->cloak = newItem(CLOAK_MAGICREFLECTION, WORN, -1 + rand() % 3, 1, rand(), false, nullptr);
						case 1:
							break;
						default:
							myStats->cloak = newItem(CLOAK, WORN, -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
					}
				}
				// give helmet
				if ( myStats->helmet == nullptr  )
				{
					switch ( rand() % 10 )
					{
						case 0:
						case 1:
							break;
						default:
							myStats->helmet = newItem(HAT_HOOD, WORN, -1 + rand() % 3, 1, 1, false, nullptr);
							break;
					}
				}
				// give armor
				if ( myStats->breastplate == nullptr )
				{
					switch ( rand() % 10 )
					{
						case 0:
						case 1:
							myStats->breastplate = newItem(IRON_BREASTPIECE, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 2:
							myStats->breastplate = newItem(LEATHER_BREASTPIECE, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						default:
							break;
					}
				}
				// give booties
				if ( myStats->shoes == nullptr )
				{
					switch ( rand() % 20 )
					{
						case 0:
						case 1:
						case 2:
							break;
						case 3:
							myStats->shoes = newItem(IRON_BOOTS, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 19:
							myStats->shoes = newItem(LEATHER_BOOTS_SPEED, static_cast<Status>(rand() % 4 + DECREPIT), -6 + rand() % 3, 1, rand(), false, nullptr);
							break;
						default:
							myStats->shoes = newItem(LEATHER_BOOTS, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
					}
				}
				// give weapon
				if ( myStats->weapon == nullptr )
				{
					switch ( rand() % 15 )
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							myStats->weapon = newItem(MAGICSTAFF_COLD, static_cast<Status>(rand() % 2 + SERVICABLE), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 5:
						case 6:
						case 7:
						case 8:
							myStats->weapon = newItem(MAGICSTAFF_LIGHTNING, static_cast<Status>(rand() % 2 + SERVICABLE), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
						case 9:
							switch ( rand() % 4 )
							{
								case 0:
									myStats->weapon = newItem(SPELLBOOK_ACID_SPRAY, static_cast<Status>(rand() % 2 + DECREPIT), -1 + rand() % 3, 1, MONSTER_ITEM_UNDROPPABLE_APPEARANCE, false, nullptr);
									break;
								case 1:
									myStats->weapon = newItem(SPELLBOOK_LIGHTNING, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, MONSTER_ITEM_UNDROPPABLE_APPEARANCE, false, nullptr);
									break;
								case 2:
									myStats->weapon = newItem(SPELLBOOK_COLD, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, MONSTER_ITEM_UNDROPPABLE_APPEARANCE, false, nullptr);
									break;
							}
							break;
						default:
							myStats->weapon = newItem(CROSSBOW, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
							break;
					}
				}
			}
			else
			{
				// give armor
				if ( myStats->breastplate == nullptr && myStats->EDITOR_ITEMS[ITEM_SLOT_ARMOR] == 1 )
				{
					switch ( rand() % 10 )
					{
					case 0:
					case 1:
					case 2:
						myStats->breastplate = newItem(STEEL_BREASTPIECE, static_cast<Status>(rand() % 4 + DECREPIT), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 3:
					case 4:
						break;
					case 5:
					case 6:
						myStats->breastplate = newItem(IRON_BREASTPIECE, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					default:
						myStats->breastplate = newItem(LEATHER_BREASTPIECE, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					}
				}
				// give booties
				if ( myStats->shoes == nullptr )
				{
					switch ( rand() % 10 )
					{
					case 0:
					case 1:
					case 2:
					case 3:
						myStats->shoes = newItem(IRON_BOOTS, static_cast<Status>(rand() % 3 + DECREPIT), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					default:
						myStats->shoes = newItem(STEEL_BOOTS, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					}
				}
				// give weapon
				if ( myStats->weapon == nullptr )
				{
					switch ( rand() % 20 )
					{
					case 0:
						myStats->weapon = newItem(RAPIER, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 1:
						myStats->weapon = newItem(TRIDENT, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 2:
						myStats->weapon = newItem(GRANITE_MAUL, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 3:
						myStats->weapon = newItem(EXECUTIONER_AXE, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					case 4:
					case 5:
					case 6:
					case 7:
						myStats->weapon = newItem(STEEL_SWORD, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					default:
						myStats->weapon = newItem(STEEL_HALBERD, static_cast<Status>(rand() % 3 + WORN), -1 + rand() % 3, 1, rand(), false, nullptr);
						break;
					}
				}
			}
		}
	}

	// torso
	Entity* entity = newEntity(949, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = 1.01;
	entity->scaley = 1.01;
	entity->scalez = 1.01;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][1][0]; // 0
	entity->focaly = limbs[CHOLOROSH][1][1]; // 0
	entity->focalz = limbs[CHOLOROSH][1][2]; // 0
	entity->behavior = &actCholoroshLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// right leg
	entity = newEntity(948, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][2][0]; // 0
	entity->focaly = limbs[CHOLOROSH][2][1]; // 0
	entity->focalz = limbs[CHOLOROSH][2][2]; // 2
	entity->behavior = &actCholoroshLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// left leg
	entity = newEntity(947, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][3][0]; // 0
	entity->focaly = limbs[CHOLOROSH][3][1]; // 0
	entity->focalz = limbs[CHOLOROSH][3][2]; // 2
	entity->behavior = &actCholoroshLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// right arm
	entity = newEntity(945, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][4][0]; // 0
	entity->focaly = limbs[CHOLOROSH][4][1]; // 0
	entity->focalz = limbs[CHOLOROSH][4][2]; // 1.5
	entity->behavior = &actCholoroshLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// left arm
	entity = newEntity(943, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][5][0]; // 0
	entity->focaly = limbs[CHOLOROSH][5][1]; // 0
	entity->focalz = limbs[CHOLOROSH][5][2]; // 1.5
	entity->behavior = &actCholoroshLimb;
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
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][6][0]; // 1.5
	entity->focaly = limbs[CHOLOROSH][6][1]; // 0
	entity->focalz = limbs[CHOLOROSH][6][2]; // -.5
	entity->behavior = &actCholoroshLimb;
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
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][7][0]; // 2
	entity->focaly = limbs[CHOLOROSH][7][1]; // 0
	entity->focalz = limbs[CHOLOROSH][7][2]; // 0
	entity->behavior = &actCholoroshLimb;
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
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][8][0]; // 0
	entity->focaly = limbs[CHOLOROSH][8][1]; // 0
	entity->focalz = limbs[CHOLOROSH][8][2]; // 4
	entity->behavior = &actCholoroshLimb;
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
	entity->scalex = 1.01;
	entity->scaley = 1.01;
	entity->scalez = 1.01;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][9][0]; // 0
	entity->focaly = limbs[CHOLOROSH][9][1]; // 0
	entity->focalz = limbs[CHOLOROSH][9][2]; // -2
	entity->behavior = &actCholoroshLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);
	my->bodyparts.push_back(entity);

	// mask
	entity = newEntity(-1, 0, map.entities, nullptr); //Limb entity.
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[CHOLOROSH][10][0]; // 0
	entity->focaly = limbs[CHOLOROSH][10][1]; // 0
	entity->focalz = limbs[CHOLOROSH][10][2]; // .25
	entity->behavior = &actCholoroshLimb;
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

void actCholoroshLimb(Entity* my)
{
	my->actMonsterLimb(true);
}

void choloroshDie(Entity* my)
{
	int c;
	for ( c = 0; c < 5; c++ )
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}

	my->spawnBlood();

	playSoundEntity(my, 470 + rand() % 3, 128);

	my->removeMonsterDeathNodes();

	list_RemoveNode(my->mynode);
	return;
}

#define CHOLOROSHWALKSPEED .13

void choloroshMoveBodyparts(Entity* my, Stat* myStats, double dist)
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
		if (myStats->mask != NULL)
			if (myStats->mask->type == ABYSSAL_AMULET)
			{
				wearingring = true;
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

		// sleeping
		if ( myStats->EFFECTS[EFF_ASLEEP] )
		{
			my->z = 2.5;
			my->pitch = PI / 4;
		}
		else
		{
			my->z = 0;
			if ( my->monsterAttack == 0 )
			{
				my->pitch = 0;
			}
		}
	}

	Entity* shieldarm = nullptr;

	//Move bodyparts
	for (bodypart = 0, node = my->children.first; node != nullptr; node = node->next, bodypart++)
	{
		if ( bodypart < LIMB_HUMANOID_TORSO )
		{
			// post-swing head animation. client doesn't need to adjust the entity pitch, server will handle.
			if ( my->monsterAttack != MONSTER_POSE_RANGED_WINDUP3 && bodypart == 1 && multiplayer != CLIENT )
			{
				limbAnimateToLimit(my, ANIMATE_PITCH, 0.1, 0, false, 0.0);
			}
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
			my->humanoidAnimateWalk(entity, node, bodypart, CHOLOROSHWALKSPEED, dist, 0.4);
		}
		else if ( bodypart == LIMB_HUMANOID_LEFTLEG || bodypart == LIMB_HUMANOID_RIGHTARM || bodypart == LIMB_HUMANOID_CLOAK )
		{
			// left leg, right arm, cloak.
			if ( bodypart == LIMB_HUMANOID_RIGHTARM )
			{
				weaponarm = entity;
				if ( MONSTER_ATTACK > 0 )
				{
					if ( my->monsterAttack == MONSTER_POSE_RANGED_WINDUP3 )
					{
						Entity* rightbody = nullptr;
						// set rightbody to left leg.
						node_t* rightbodyNode = list_Node(&my->children, LIMB_HUMANOID_LEFTLEG);
						if ( rightbodyNode )
						{
							rightbody = (Entity*)rightbodyNode->element;
						}
						else
						{
							return;
						}

						if ( my->monsterAttackTime == 0 )
						{
							// init rotations
							weaponarm->pitch = 0;
							my->monsterArmbended = 0;
							my->monsterWeaponYaw = 0;
							weaponarm->roll = 0;
							weaponarm->skill[1] = 0;
							if ( multiplayer != CLIENT )
							{
								myStats->EFFECTS[EFF_PARALYZED] = true;
								myStats->EFFECTS_TIMERS[EFF_PARALYZED] = 40;
							}
						}
						if ( multiplayer != CLIENT )
						{
							// move the head and weapon yaw
							limbAnimateToLimit(my, ANIMATE_PITCH, -0.1, 11 * PI / 6, true, 0.05);
							limbAnimateToLimit(my, ANIMATE_WEAPON_YAW, -0.25, 14 * PI / 8, false, 0.0);
						}
						limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.25, 7 * PI / 4, true, 0.0);
						//limbAnimateToLimit(weaponarm, ANIMATE_ROLL, -0.25, 7 * PI / 4, false, 0.0);

						if ( my->monsterAttackTime >= 4 * ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
						{
							if ( multiplayer != CLIENT )
							{
								my->attack(MONSTER_POSE_MELEE_WINDUP1, 0, nullptr);
							}
						}
					}
					else
					{
						my->handleWeaponArmAttack(entity);
					}
				}
			}
			else if ( bodypart == LIMB_HUMANOID_CLOAK )
			{
				entity->pitch = entity->fskill[0];
			}

			my->humanoidAnimateWalk(entity, node, bodypart, CHOLOROSHWALKSPEED, dist, 0.4);

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
				if ( multiplayer != CLIENT )
				{
					if ( myStats->breastplate == nullptr )
					{
						entity->sprite = 949;
					}
					else
					{
						entity->sprite = itemModel(myStats->breastplate);
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
				my->setHumanoidLimbOffset(entity, CHOLOROSH, LIMB_HUMANOID_TORSO);
				break;
			// right leg
			case LIMB_HUMANOID_RIGHTLEG:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == nullptr )
					{
						entity->sprite = 948;
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
				my->setHumanoidLimbOffset(entity, CHOLOROSH, LIMB_HUMANOID_RIGHTLEG);
				break;
			// left leg
			case LIMB_HUMANOID_LEFTLEG:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == nullptr )
					{
						entity->sprite = 947;
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
				my->setHumanoidLimbOffset(entity, CHOLOROSH, LIMB_HUMANOID_LEFTLEG);
				break;
			// right arm
			case LIMB_HUMANOID_RIGHTARM:
			{
				node_t* weaponNode = list_Node(&my->children, 7);
				if ( weaponNode )
				{
					Entity* weapon = (Entity*)weaponNode->element;
					if ( MONSTER_ARMBENDED || (weapon->flags[INVISIBLE] && my->monsterState == MONSTER_STATE_WAIT) )
					{
						// if weapon invisible and I'm not attacking, relax arm.
						entity->focalx = limbs[CHOLOROSH][4][0]; // 0
						entity->focaly = limbs[CHOLOROSH][4][1]; // 0
						entity->focalz = limbs[CHOLOROSH][4][2]; // 2
						entity->sprite = 945;
					}
					else
					{
						// else flex arm.
						entity->focalx = limbs[CHOLOROSH][4][0] + 0.75;
						entity->focaly = limbs[CHOLOROSH][4][1];
						entity->focalz = limbs[CHOLOROSH][4][2] - 0.75;
						entity->sprite = 946;
					}
				}
				my->setHumanoidLimbOffset(entity, CHOLOROSH, LIMB_HUMANOID_RIGHTARM);
				entity->yaw += MONSTER_WEAPONYAW;
				break;
				// left arm
			}
			case LIMB_HUMANOID_LEFTARM:
			{
				shieldarm = entity;
				node_t* shieldNode = list_Node(&my->children, 8);
				if ( shieldNode )
				{
					Entity* shield = (Entity*)shieldNode->element;
					if ( shield->flags[INVISIBLE] && my->monsterState == MONSTER_STATE_WAIT )
					{
						entity->focalx = limbs[CHOLOROSH][5][0]; // 0
						entity->focaly = limbs[CHOLOROSH][5][1]; // 0
						entity->focalz = limbs[CHOLOROSH][5][2]; // 2
						entity->sprite = 943;
					}
					else
					{
						entity->focalx = limbs[CHOLOROSH][5][0] + 0.75;
						entity->focaly = limbs[CHOLOROSH][5][1];
						entity->focalz = limbs[CHOLOROSH][5][2] - 0.75;
						entity->sprite = 944;
					}
				}
				my->setHumanoidLimbOffset(entity, CHOLOROSH, LIMB_HUMANOID_LEFTARM);
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
				my->handleHumanoidShieldLimb(entity, shieldarm);
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
				entity->x -= cos(my->yaw);
				entity->y -= sin(my->yaw);
				entity->yaw += PI / 2;
				break;
			// helm
			case LIMB_HUMANOID_HELMET:
				entity->focalx = limbs[CHOLOROSH][9][0]; // 0
				entity->focaly = limbs[CHOLOROSH][9][1]; // 0
				entity->focalz = limbs[CHOLOROSH][9][2]; // -2
				entity->pitch = my->pitch;
				entity->roll = 0;
				if ( multiplayer != CLIENT )
				{
					entity->sprite = itemModel(myStats->helmet);
					if ( myStats->helmet == nullptr || myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
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
			// mask
			case LIMB_HUMANOID_MASK:
				entity->focalx = limbs[CHOLOROSH][10][0]; // 0
				entity->focaly = limbs[CHOLOROSH][10][1]; // 0
				entity->focalz = limbs[CHOLOROSH][10][2]; // .25
				entity->pitch = my->pitch;
				entity->roll = PI / 2;
				if ( multiplayer != CLIENT )
				{
					if ( myStats->mask == nullptr || myStats->EFFECTS[EFF_INVISIBLE] || wearingring ) //TODO: isInvisible()?
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
					}
					if ( myStats->mask != nullptr )
					{
						if ( myStats->mask->type == TOOL_GLASSES )
						{
							entity->sprite = 165; // GlassesWorn.vox
						}
						else
						{
							entity->sprite = itemModel(myStats->mask);
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
				if ( entity->sprite != 165 )
				{
					entity->focalx = limbs[CHOLOROSH][10][0] + .35; // .35
					entity->focaly = limbs[CHOLOROSH][10][1] - 2; // -2
					entity->focalz = limbs[CHOLOROSH][10][2]; // .25
				}
				else
				{
					entity->focalx = limbs[CHOLOROSH][10][0] + .25; // .25
					entity->focaly = limbs[CHOLOROSH][10][1] - 2.25; // -2.25
					entity->focalz = limbs[CHOLOROSH][10][2]; // .25
				}
				break;
		}
	}
	// rotate shield a bit
	node_t* shieldNode = list_Node(&my->children, 8);
	if ( shieldNode )
	{
		Entity* shieldEntity = (Entity*)shieldNode->element;
		if ( shieldEntity->sprite != items[TOOL_TORCH].index && shieldEntity->sprite != items[TOOL_LANTERN].index && shieldEntity->sprite != items[TOOL_CRYSTALSHARD].index && shieldEntity->sprite != items[TOOL_GREENTORCH].index)
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

void Entity::choloroshChooseWeapon(const Entity* target, double dist)
{
	if ( monsterSpecialState != 0 )
	{
		//Holding a weapon assigned from the special attack. Don't switch weapons.
		//messagePlayer()
		return;
	}

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( myStats->weapon && (itemCategory(myStats->weapon) == SPELLBOOK) )
	{
		return;
	}

	int specialRoll = -1;
	bool usePotionSpecial = false;

	/*
	 * TODO: For the cholorosh's special
	 */

	node_t* hasPotion = nullptr;
	bool isHealingPotion = false;
	/*
	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		specialRoll = rand()%10;

		if ( specialRoll == 0 )
		{
			if ( myStats->HP <= myStats->MAXHP / 3 * 2 )
			{
				//Try to get a health potion.
				hasPotion = itemNodeInInventory(myStats, POTION_EXTRAHEALING, static_cast<Category>(-1));
				if ( !hasPotion )
				{
					hasPotion = itemNodeInInventory(myStats, POTION_HEALING, static_cast<Category>(-1));
					if ( hasPotion )
					{
						//Equip and chuck it now.
						bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
						if ( !swapped )
						{
							printlog("Error in Entity::goatmanChooseWeapon(): failed to swap healing potion into hand!");
							//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
						}
						else
						{
							monsterSpecialState = GOATMAN_POTION;
							//monsterHitTime = 2 * HITRATE;
							return;
						}
					}
				}
				else
				{
					//Equip and chuck it now.
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
					if ( !swapped )
					{
						printlog("Error in Entity::goatmanChooseWeapon(): failed to swap healing potion into hand!");
						//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
					}
					else
					{
						monsterSpecialState = GOATMAN_POTION;
						//monsterHitTime = 2 * HITRATE;
						return;
					}
				}
			}

			if ( !hasPotion )
			{
				//Couldn't find a healing potion? Try for a potion of booze.
				hasPotion = itemNodeInInventory(myStats, POTION_BOOZE, static_cast<Category>(-1));
				if ( hasPotion )
				{
					//Equip and chuck it now.
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
					if ( !swapped )
					{
						printlog("Error in Entity::goatmanChooseWeapon(): failed to swap healing potion into hand!");
						//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
					}
					else
					{
						monsterSpecialState = GOATMAN_POTION;
						//monsterHitTime = 2 * HITRATE;
						return;
					}
				}
			}
		}
	}*/

	bool inMeleeRange = monsterInMeleeRange(target, dist);

	if ( inMeleeRange )
	{
		if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 && specialRoll == 0 )
		{
			bool tryChakram = true;
			if ( hasPotion && rand()%10 )
			{
				tryChakram = false;
			}

			if ( tryChakram )
			{
				//Grab a chakram instead.
				node_t* thrownNode = itemNodeInInventory(myStats, static_cast<ItemType>(-1), THROWN);
				if ( thrownNode )
				{
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, thrownNode, false, false);
					if ( !swapped )
					{
						printlog("Error in Entity::choloroshChooseWeapon(): failed to swap THROWN into hand! Cursed? (%d)", myStats->weapon->beatitude);
						//Don't return, make sure holding a melee weapon at least.
					}
					else
					{
						//monsterSpecialState = GOATMAN_THROW;
						return;
					}
				}
			}
		}

		//Switch to a melee weapon if not already wielding one. Unless monster special state is overriding the AI.
		if ( !myStats->weapon || !isMeleeWeapon(*myStats->weapon) )
		{
			node_t* weaponNode = getMeleeWeaponItemNodeInInventory(myStats);
			if ( !weaponNode )
			{
				if ( myStats->weapon && myStats->weapon->type == MAGICSTAFF_SLOW )
				{
					monsterUnequipSlotFromCategory(myStats, &myStats->weapon, MAGICSTAFF);
				}
				return; //Resort to fists.
			}

			bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
			if ( !swapped )
			{
				printlog("Error in Entity::choloroshChooseWeapon(): failed to swap melee weapon into hand! Cursed? (%d)", myStats->weapon->beatitude);
				//Don't return so that monsters will at least equip ranged weapons in melee range if they don't have anything else.
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	//Switch to a thrown weapon or a ranged weapon. Potions are reserved as a special attack.
	if ( !myStats->weapon || isMeleeWeapon(*myStats->weapon) )
	{
		//First search the inventory for a THROWN weapon.
		node_t *weaponNode = nullptr;
		if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 && rand() % 10 == 0 )
		{
			weaponNode = itemNodeInInventory(myStats, static_cast<ItemType>(-1), THROWN);
			if ( weaponNode )
			{
				if ( swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false) )
				{
					//monsterSpecialState = GOATMAN_THROW;
					return;
				}
			}
		}
		if ( !weaponNode )
		{
			//If couldn't find any, search the inventory for a ranged weapon.
			weaponNode = getRangedWeaponItemNodeInInventory(myStats, true);
		}

		bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
		return;
	}

	return;
}

bool Entity::choloroshCanWieldItem(const Item& item) const
{

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case POTION:
			switch ( item.type )
			{
				case POTION_HEALING:
					return true;
				default:
					return false;
			}
		case THROWN:
			return true;
		case ARMOR:
			return true;
		default:
			return false;
	}

	return false;
}



