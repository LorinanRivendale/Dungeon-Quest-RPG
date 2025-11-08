#include "inventory.h"
#include "party.h"
#include "game_state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Inventory* inventory_create(void) {
    Inventory* inv = (Inventory*)malloc(sizeof(Inventory));
    if (!inv) return NULL;
    
    memset(inv, 0, sizeof(Inventory));
    inv->item_count = 0;
    inv->equipment_count = 0;
    
    return inv;
}

void inventory_destroy(Inventory* inv) {
    if (inv) {
        free(inv);
    }
}

bool inventory_add_item(Inventory* inv, uint8_t item_id, uint8_t quantity) {
    if (!inv || inventory_is_full(inv)) return false;
    
    // Check if item already exists (for stackable items)
    for (uint8_t i = 0; i < inv->item_count; i++) {
        if (inv->items[i].item_id == item_id) {
            inv->items[i].quantity += quantity;
            printf("Added %d x %s\n", quantity, inv->items[i].name);
            return true;
        }
    }
    
    // Add new item
    Item new_item = item_create_consumable(item_id);
    new_item.quantity = quantity;
    inv->items[inv->item_count++] = new_item;
    
    printf("Obtained %s x%d\n", new_item.name, quantity);
    
    return true;
}

bool inventory_add_equipment(Inventory* inv, uint8_t equip_id) {
    if (!inv || inv->equipment_count >= MAX_EQUIPMENT_SLOTS) return false;
    
    Item equipment = item_create_equipment(equip_id);
    inv->equipment[inv->equipment_count++] = equipment;
    
    printf("Obtained %s\n", equipment.name);
    
    return true;
}

bool inventory_remove_item(Inventory* inv, uint8_t index, uint8_t quantity) {
    if (!inv || index >= inv->item_count) return false;
    
    if (inv->items[index].quantity <= quantity) {
        // Remove item completely
        for (uint8_t i = index; i < inv->item_count - 1; i++) {
            inv->items[i] = inv->items[i + 1];
        }
        inv->item_count--;
    } else {
        inv->items[index].quantity -= quantity;
    }
    
    return true;
}

bool inventory_remove_equipment(Inventory* inv, uint8_t index) {
    if (!inv || index >= inv->equipment_count) return false;
    
    for (uint8_t i = index; i < inv->equipment_count - 1; i++) {
        inv->equipment[i] = inv->equipment[i + 1];
    }
    inv->equipment_count--;
    
    return true;
}

Item* inventory_get_item(Inventory* inv, uint8_t index) {
    if (!inv || index >= inv->item_count) return NULL;
    return &inv->items[index];
}

Item* inventory_get_equipment(Inventory* inv, uint8_t index) {
    if (!inv || index >= inv->equipment_count) return NULL;
    return &inv->equipment[index];
}

bool inventory_use_item(Inventory* inv, uint8_t item_index, uint8_t party_member_index) {
    if (!inv || !g_game_state.party) return false;
    
    Item* item = inventory_get_item(inv, item_index);
    PartyMember* member = party_get_member(g_game_state.party, party_member_index);
    
    if (!item || !member) return false;
    
    printf("%s uses %s\n", member->name, item->name);
    
    // Apply item effects
    if (item->hp_restore > 0) {
        character_heal(member, item->hp_restore);
    }
    
    if (item->mp_restore > 0) {
        member->stats.current_mp += item->mp_restore;
        if (member->stats.current_mp > member->stats.max_mp) {
            member->stats.current_mp = member->stats.max_mp;
        }
        printf("%s recovers %d MP!\n", member->name, item->mp_restore);
    }
    
    if (item->status_cure != 0) {
        member->status_effects &= ~item->status_cure;
        printf("%s is cured!\n", member->name);
    }
    
    // Remove one from inventory
    inventory_remove_item(inv, item_index, 1);
    
    return true;
}

bool inventory_equip_item(Inventory* inv, uint8_t equip_index, uint8_t party_member_index) {
    if (!inv || !g_game_state.party) return false;
    
    Item* equipment = inventory_get_equipment(inv, equip_index);
    PartyMember* member = party_get_member(g_game_state.party, party_member_index);
    
    if (!equipment || !member) return false;
    
    // Check if already equipped
    if (equipment->is_equipped) {
        printf("%s is already equipped!\n", equipment->name);
        return false;
    }
    
    // Check job restriction (if any)
    if (equipment->usable_by_job != 0 && 
        !(equipment->usable_by_job & (1 << member->job))) {
        printf("%s cannot equip %s!\n", member->name, equipment->name);
        return false;
    }
    
    // Unequip current item in that slot
    EquipmentSlot slot = (EquipmentSlot)equipment->equip_type;
    if (member->equipped_items[slot] != 0xFF) {
        // Find and unequip the old item
        for (uint8_t i = 0; i < inv->equipment_count; i++) {
            if (inv->equipment[i].is_equipped && 
                inv->equipment[i].equip_type == equipment->equip_type) {
                inv->equipment[i].is_equipped = false;
                printf("%s unequipped.\n", inv->equipment[i].name);
                break;
            }
        }
    }
    
    // Equip new item
    equipment->is_equipped = true;
    member->equipped_items[slot] = equip_index;
    
    printf("%s equipped %s!\n", member->name, equipment->name);
    
    return true;
}

bool inventory_unequip_item(uint8_t party_member_index, EquipmentSlot slot) {
    if (!g_game_state.party || !g_game_state.inventory) return false;
    
    PartyMember* member = party_get_member(g_game_state.party, party_member_index);
    if (!member) return false;
    
    uint8_t equip_index = member->equipped_items[slot];
    if (equip_index == 0xFF) return false;
    
    Item* equipment = inventory_get_equipment(g_game_state.inventory, equip_index);
    if (equipment) {
        equipment->is_equipped = false;
        member->equipped_items[slot] = 0xFF;
        printf("%s unequipped.\n", equipment->name);
        return true;
    }
    
    return false;
}

bool inventory_is_full(Inventory* inv) {
    return inv && inv->item_count >= MAX_INVENTORY_ITEMS;
}

int8_t inventory_find_item(Inventory* inv, uint8_t item_id) {
    if (!inv) return -1;
    
    for (uint8_t i = 0; i < inv->item_count; i++) {
        if (inv->items[i].item_id == item_id) {
            return i;
        }
    }
    
    return -1;
}

bool inventory_has_item(Inventory* inv, uint8_t item_id) {
    return inventory_find_item(inv, item_id) >= 0;
}

Item item_create_consumable(uint8_t item_id) {
    Item item;
    memset(&item, 0, sizeof(Item));
    
    item.item_id = item_id;
    item.type = ITEM_TYPE_CONSUMABLE;
    item.quantity = 1;
    
    // Item database
    switch (item_id) {
        case ITEM_POTION:
            strcpy(item.name, "Potion");
            item.hp_restore = 50;
            break;
        case ITEM_HI_POTION:
            strcpy(item.name, "Hi-Potion");
            item.hp_restore = 150;
            break;
        case ITEM_ETHER:
            strcpy(item.name, "Ether");
            item.mp_restore = 30;
            break;
        case ITEM_ELIXIR:
            strcpy(item.name, "Elixir");
            item.hp_restore = 9999;
            item.mp_restore = 9999;
            break;
        case ITEM_ANTIDOTE:
            strcpy(item.name, "Antidote");
            item.status_cure = STATUS_POISON;
            break;
		case ITEM_TENT:
			strcpy(item.name, "Tent");
			item.hp_restore = 9999; // Full heal
			item.mp_restore = 9999; // Full MP restore
			break;
        default:
            strcpy(item.name, "Unknown");
            break;
    }
    
    return item;
}

// Equipment IDs
#define EQUIP_DAGGER 0
#define EQUIP_SHORT_SWORD 1
#define EQUIP_LONG_SWORD 2
#define EQUIP_GREAT_SWORD 3
#define EQUIP_STAFF 4
#define EQUIP_IRON_STAFF 5
#define EQUIP_WOODEN_ROD 6
#define EQUIP_IRON_ROD 7
#define EQUIP_NUNCHAKU 8
#define EQUIP_IRON_NUNCHAKU 9

#define EQUIP_CLOTH_ARMOR 20
#define EQUIP_LEATHER_ARMOR 21
#define EQUIP_CHAIN_MAIL 22
#define EQUIP_PLATE_MAIL 23
#define EQUIP_ROBE 24
#define EQUIP_SILK_ROBE 25

#define EQUIP_LEATHER_CAP 40
#define EQUIP_IRON_HELM 41
#define EQUIP_WIZARD_HAT 42

#define EQUIP_POWER_RING 60
#define EQUIP_DEFENSE_RING 61
#define EQUIP_LUCK_RING 62

Item item_create_equipment(uint8_t equip_id) {
    Item item;
    memset(&item, 0, sizeof(Item));
    
    item.item_id = equip_id;
    item.type = ITEM_TYPE_EQUIPMENT;
    item.is_equipped = false;
    item.usable_by_job = 0; // 0 = all jobs can use
    
    // Weapon database
    if (equip_id < 20) {
        item.equip_type = EQUIP_TYPE_WEAPON;
        
        switch (equip_id) {
            case EQUIP_DAGGER:
                strcpy(item.name, "Dagger");
                item.attack_bonus = 5;
                break;
            case EQUIP_SHORT_SWORD:
                strcpy(item.name, "Short Sword");
                item.attack_bonus = 8;
                break;
            case EQUIP_LONG_SWORD:
                strcpy(item.name, "Long Sword");
                item.attack_bonus = 12;
                item.usable_by_job = (1 << JOB_KNIGHT);
                break;
            case EQUIP_GREAT_SWORD:
                strcpy(item.name, "Great Sword");
                item.attack_bonus = 18;
                item.usable_by_job = (1 << JOB_KNIGHT);
                break;
            case EQUIP_STAFF:
                strcpy(item.name, "Staff");
                item.attack_bonus = 4;
                item.intelligence_bonus = 3;
                break;
            case EQUIP_IRON_STAFF:
                strcpy(item.name, "Iron Staff");
                item.attack_bonus = 7;
                item.intelligence_bonus = 5;
                break;
            case EQUIP_WOODEN_ROD:
                strcpy(item.name, "Wooden Rod");
                item.attack_bonus = 3;
                item.intelligence_bonus = 5;
                item.usable_by_job = (1 << JOB_MAGE) | (1 << JOB_SAGE);
                break;
            case EQUIP_IRON_ROD:
                strcpy(item.name, "Iron Rod");
                item.attack_bonus = 5;
                item.intelligence_bonus = 8;
                item.usable_by_job = (1 << JOB_MAGE) | (1 << JOB_SAGE);
                break;
            case EQUIP_NUNCHAKU:
                strcpy(item.name, "Nunchaku");
                item.attack_bonus = 10;
                item.usable_by_job = (1 << JOB_BLACK_BELT);
                break;
            case EQUIP_IRON_NUNCHAKU:
                strcpy(item.name, "Iron Nunchaku");
                item.attack_bonus = 15;
                item.usable_by_job = (1 << JOB_BLACK_BELT);
                break;
            default:
                strcpy(item.name, "Unknown Weapon");
                item.attack_bonus = 1;
                break;
        }
    }
    // Armor database
    else if (equip_id < 40) {
        item.equip_type = EQUIP_TYPE_ARMOR;
        
        switch (equip_id) {
            case EQUIP_CLOTH_ARMOR:
                strcpy(item.name, "Cloth Armor");
                item.defense_bonus = 3;
                break;
            case EQUIP_LEATHER_ARMOR:
                strcpy(item.name, "Leather Armor");
                item.defense_bonus = 6;
                break;
            case EQUIP_CHAIN_MAIL:
                strcpy(item.name, "Chain Mail");
                item.defense_bonus = 10;
                item.usable_by_job = (1 << JOB_KNIGHT);
                break;
            case EQUIP_PLATE_MAIL:
                strcpy(item.name, "Plate Mail");
                item.defense_bonus = 15;
                item.usable_by_job = (1 << JOB_KNIGHT);
                break;
            case EQUIP_ROBE:
                strcpy(item.name, "Robe");
                item.defense_bonus = 4;
                item.intelligence_bonus = 2;
                item.usable_by_job = (1 << JOB_MAGE) | (1 << JOB_SAGE) | (1 << JOB_PRIEST);
                break;
            case EQUIP_SILK_ROBE:
                strcpy(item.name, "Silk Robe");
                item.defense_bonus = 7;
                item.intelligence_bonus = 4;
                item.usable_by_job = (1 << JOB_MAGE) | (1 << JOB_SAGE) | (1 << JOB_PRIEST);
                break;
            default:
                strcpy(item.name, "Unknown Armor");
                item.defense_bonus = 1;
                break;
        }
    }
    // Helmet database
    else if (equip_id < 60) {
        item.equip_type = EQUIP_TYPE_HELMET;
        
        switch (equip_id) {
            case EQUIP_LEATHER_CAP:
                strcpy(item.name, "Leather Cap");
                item.defense_bonus = 2;
                break;
            case EQUIP_IRON_HELM:
                strcpy(item.name, "Iron Helm");
                item.defense_bonus = 5;
                item.usable_by_job = (1 << JOB_KNIGHT);
                break;
            case EQUIP_WIZARD_HAT:
                strcpy(item.name, "Wizard Hat");
                item.defense_bonus = 2;
                item.intelligence_bonus = 3;
                item.usable_by_job = (1 << JOB_MAGE) | (1 << JOB_SAGE);
                break;
            default:
                strcpy(item.name, "Unknown Helm");
                item.defense_bonus = 1;
                break;
        }
    }
    // Accessory database
    else {
        item.equip_type = EQUIP_TYPE_ACCESSORY;
        
        switch (equip_id) {
            case EQUIP_POWER_RING:
                strcpy(item.name, "Power Ring");
                item.attack_bonus = 3;
                break;
            case EQUIP_DEFENSE_RING:
                strcpy(item.name, "Defense Ring");
                item.defense_bonus = 3;
                break;
            case EQUIP_LUCK_RING:
                strcpy(item.name, "Luck Ring");
                item.agility_bonus = 3;
                break;
            default:
                strcpy(item.name, "Unknown Ring");
                break;
        }
    }
    
    return item;
}

// Helper function to give starting equipment based on job
void inventory_give_starting_equipment(Inventory* inv, Party* party) {
    if (!inv || !party) return;
    
    printf("\n=== Distributing Starting Equipment ===\n");
    
    for (uint8_t i = 0; i < party->member_count; i++) {
        PartyMember* member = &party->members[i];
        
        switch (member->job) {
            case JOB_KNIGHT:
                inventory_add_equipment(inv, EQUIP_SHORT_SWORD);
                inventory_add_equipment(inv, EQUIP_CHAIN_MAIL);
                inventory_add_equipment(inv, EQUIP_IRON_HELM);
                inventory_equip_item(inv, inv->equipment_count - 3, i);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            case JOB_BLACK_BELT:
                inventory_add_equipment(inv, EQUIP_NUNCHAKU);
                inventory_add_equipment(inv, EQUIP_CLOTH_ARMOR);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            case JOB_THIEF:
                inventory_add_equipment(inv, EQUIP_DAGGER);
                inventory_add_equipment(inv, EQUIP_LEATHER_ARMOR);
                inventory_add_equipment(inv, EQUIP_LEATHER_CAP);
                inventory_equip_item(inv, inv->equipment_count - 3, i);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            case JOB_SAGE:
                inventory_add_equipment(inv, EQUIP_STAFF);
                inventory_add_equipment(inv, EQUIP_ROBE);
                inventory_add_equipment(inv, EQUIP_WIZARD_HAT);
                inventory_equip_item(inv, inv->equipment_count - 3, i);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            case JOB_PRIEST:
                inventory_add_equipment(inv, EQUIP_STAFF);
                inventory_add_equipment(inv, EQUIP_ROBE);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            case JOB_MAGE:
                inventory_add_equipment(inv, EQUIP_WOODEN_ROD);
                inventory_add_equipment(inv, EQUIP_SILK_ROBE);
                inventory_add_equipment(inv, EQUIP_WIZARD_HAT);
                inventory_equip_item(inv, inv->equipment_count - 3, i);
                inventory_equip_item(inv, inv->equipment_count - 2, i);
                inventory_equip_item(inv, inv->equipment_count - 1, i);
                break;
                
            default:
                break;
        }
    }
    
    printf("=== Equipment Distribution Complete ===\n\n");
}
	
	// Equipment bonus calculation functions
uint8_t character_get_total_attack(PartyMember* member) {
    if (!member || !g_game_state.inventory) return member ? member->stats.strength : 0;
    
    uint8_t total = member->stats.strength;
    
    // Add weapon bonus
    uint8_t weapon_index = member->equipped_items[EQUIP_WEAPON];
    if (weapon_index != 0xFF && weapon_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[weapon_index].attack_bonus;
    }
    
    // Add accessory bonus
    uint8_t accessory_index = member->equipped_items[EQUIP_ACCESSORY];
    if (accessory_index != 0xFF && accessory_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[accessory_index].attack_bonus;
    }
    
    return total;
}

uint8_t character_get_total_defense(PartyMember* member) {
    if (!member || !g_game_state.inventory) return member ? member->stats.defense : 0;
    
    uint8_t total = member->stats.defense;
    
    // Add armor bonus
    uint8_t armor_index = member->equipped_items[EQUIP_ARMOR];
    if (armor_index != 0xFF && armor_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[armor_index].defense_bonus;
    }
    
    // Add helmet bonus
    uint8_t helmet_index = member->equipped_items[EQUIP_HELMET];
    if (helmet_index != 0xFF && helmet_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[helmet_index].defense_bonus;
    }
    
    // Add accessory bonus
    uint8_t accessory_index = member->equipped_items[EQUIP_ACCESSORY];
    if (accessory_index != 0xFF && accessory_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[accessory_index].defense_bonus;
    }
    
    return total;
}

uint8_t character_get_total_intelligence(PartyMember* member) {
    if (!member || !g_game_state.inventory) return member ? member->stats.intelligence : 0;
    
    uint8_t total = member->stats.intelligence;
    
    // Add weapon bonus (some weapons boost intelligence)
    uint8_t weapon_index = member->equipped_items[EQUIP_WEAPON];
    if (weapon_index != 0xFF && weapon_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[weapon_index].intelligence_bonus;
    }
    
    // Add armor bonus
    uint8_t armor_index = member->equipped_items[EQUIP_ARMOR];
    if (armor_index != 0xFF && armor_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[armor_index].intelligence_bonus;
    }
    
    // Add helmet bonus
    uint8_t helmet_index = member->equipped_items[EQUIP_HELMET];
    if (helmet_index != 0xFF && helmet_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[helmet_index].intelligence_bonus;
    }
    
    return total;
}

uint8_t character_get_total_agility(PartyMember* member) {
    if (!member || !g_game_state.inventory) return member ? member->stats.agility : 0;

    uint8_t total = member->stats.agility;

    // Add accessory bonus
    uint8_t accessory_index = member->equipped_items[EQUIP_ACCESSORY];
    if (accessory_index != 0xFF && accessory_index < g_game_state.inventory->equipment_count) {
        total += g_game_state.inventory->equipment[accessory_index].agility_bonus;
    }

    return total;
}

uint8_t character_get_total_luck(PartyMember* member) {
    if (!member) return 0;

    // Luck currently has no equipment bonuses, just return base stat
    return member->stats.luck;
}	