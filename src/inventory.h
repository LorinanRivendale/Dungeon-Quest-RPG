#ifndef INVENTORY_H
#define INVENTORY_H

#include "party.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_INVENTORY_ITEMS 20
#define MAX_EQUIPMENT_SLOTS 20
#define MAX_ITEM_NAME 16

// Item types
typedef enum {
    ITEM_TYPE_CONSUMABLE = 0,
    ITEM_TYPE_KEY_ITEM,
    ITEM_TYPE_EQUIPMENT
} ItemType;

// Item categories (consumables)
typedef enum {
    ITEM_POTION = 0,
    ITEM_HI_POTION,
    ITEM_ETHER,
    ITEM_ELIXIR,
    ITEM_ANTIDOTE,
    ITEM_TENT,
    ITEM_CONSUMABLE_COUNT
} ConsumableItem;

// Equipment types
typedef enum {
    EQUIP_TYPE_WEAPON = 0,
    EQUIP_TYPE_ARMOR,
    EQUIP_TYPE_HELMET,
    EQUIP_TYPE_ACCESSORY
} EquipmentType;

// Item structure
typedef struct {
    char name[MAX_ITEM_NAME];
    uint8_t item_id;
    ItemType type;
    uint8_t quantity; // For stackable items
    bool is_equipped;
    
    // Consumable effects
    uint16_t hp_restore;
    uint16_t mp_restore;
    uint8_t status_cure; // Bitfield
    
    // Equipment stats
    EquipmentType equip_type;
    uint8_t attack_bonus;
    uint8_t defense_bonus;
    uint8_t intelligence_bonus;
    uint8_t agility_bonus;
    
    // Requirements
    uint8_t required_level;
    uint8_t usable_by_job; // Bitfield for jobs
} Item;

// Inventory structure
typedef struct Inventory {
    Item items[MAX_INVENTORY_ITEMS];
    Item equipment[MAX_EQUIPMENT_SLOTS];
    uint8_t item_count;
    uint8_t equipment_count;
} Inventory;

// Inventory management
Inventory* inventory_create(void);
void inventory_destroy(Inventory* inv);
bool inventory_add_item(Inventory* inv, uint8_t item_id, uint8_t quantity);
bool inventory_add_equipment(Inventory* inv, uint8_t equip_id);
bool inventory_remove_item(Inventory* inv, uint8_t index, uint8_t quantity);
bool inventory_remove_equipment(Inventory* inv, uint8_t index);
Item* inventory_get_item(Inventory* inv, uint8_t index);
Item* inventory_get_equipment(Inventory* inv, uint8_t index);

// Item usage
bool inventory_use_item(Inventory* inv, uint8_t item_index, uint8_t party_member_index);
bool inventory_equip_item(Inventory* inv, uint8_t equip_index, uint8_t party_member_index);
bool inventory_unequip_item(uint8_t party_member_index, EquipmentSlot slot);

// Item queries
bool inventory_has_item(Inventory* inv, uint8_t item_id);
int8_t inventory_find_item(Inventory* inv, uint8_t item_id);
bool inventory_is_full(Inventory* inv);

// Item initialization
void item_init_database(void);
Item item_create_consumable(uint8_t item_id);
Item item_create_equipment(uint8_t equip_id);

// Equipment bonus calculation
extern uint8_t character_get_total_attack(PartyMember* member);
extern uint8_t character_get_total_defense(PartyMember* member);
extern uint8_t character_get_total_intelligence(PartyMember* member);
extern uint8_t character_get_total_agility(PartyMember* member);
extern uint8_t character_get_total_luck(PartyMember* member);

// Helper function to give starting equipment
extern void inventory_give_starting_equipment(Inventory* inv, Party* party);

#endif // INVENTORY_H