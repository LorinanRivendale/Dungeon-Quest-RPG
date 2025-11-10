# Dungeon Quest RPG - TODO List

## Future Features & Enhancements

### Combat & Skills

#### Status Effect Skills (High Priority)
Expand the buff/debuff system to include traditional RPG status effects with skill-based application:

**Debilitating Status Effects:**
- **Poison** - Damage over time (5-10% max HP per turn)
  - Skills: Poison Blade (Thief), Toxic Cloud (Mage)
- **Stone/Petrify** - Character cannot act (temporary paralysis variant)
  - Skills: Stone Gaze (Mage), Petrify (Sage)
- **Blind** - Reduces accuracy/increases miss chance
  - Skills: Flash (Thief), Blinding Light (Priest)
- **Slow** - Reduces agility/turn order
  - Skills: Slow (Mage), Time Warp (Sage)
- **Silence** - Cannot use magic/skills
  - Skills: Silence (Mage), Mute (Sage)
- **Berserk** - Forced attack actions with increased ATK but no control
  - Skills: Provoke (Knight), Rage (Black Belt)

**Positive Status Effects:**
- **Haste** - Increases agility/turn frequency
  - Skills: Haste (Mage), Quick (Sage)
- **Regen** - HP restoration over time
  - Skills: Regen (Priest), Life Spring (Sage)
- **Shell** - Reduces magic damage taken
  - Skills: Shell (Priest), Magic Shield (Mage)
- **Barrier** - Reduces physical damage taken
  - Skills: Barrier (Knight), Iron Skin (Black Belt)

**Implementation Notes:**
- Status effects already defined in party.h (STATUS_POISON, STATUS_PARALYSIS, etc.)
- Need to create status-applying skills and handle duration/effects in battle system
- Consider cure spells: Esuna already exists for status removal
- Balance status effect durations (2-5 turns typical)
- Some statuses should be boss-resistant or have reduced duration

### Gameplay Systems

#### GameBoy Port Preparation
- Convert party selection to cursor-based input (currently uses scanf)
- Convert save/load menus to cursor-based input
- Convert inventory menu to cursor-based input
- Remove all scanf() usage for full GameBoy compatibility

#### Save/Load System
- Verify save/load functionality works with new buff system
- Test that active buffs are properly saved/restored

### Content Additions

#### New Job Classes
- Consider adding more job variety (Ranger, Paladin, Dark Knight, etc.)
- Balance starting stats and skill progressions

#### Additional Dungeons
- More optional side dungeons
- Secret areas with rare equipment
- Challenge dungeons for high-level parties

#### Equipment Variety
- More weapon types with unique effects
- Armor sets with set bonuses
- Accessories with status immunity

### Quality of Life

#### Battle Improvements
- Show active buffs/debuffs in battle UI
- Display buff duration counters
- More detailed combat log/damage breakdown

#### UI Enhancements
- Party status screen showing active buffs
- Skill tooltips with detailed effect descriptions
- Enemy HP bars or percentage indicators

### Polish & Balance

#### Playtesting Feedback
- Continue gathering feedback on buff/debuff system
- Monitor MP regeneration skill balance
- Adjust status effect resistance for bosses
- Fine-tune skill costs and power levels

---

## Completed Features

- ✅ Full buff/debuff system with stat modifiers
- ✅ MP regeneration "burn a turn" skills for all classes
- ✅ Turn-based buff duration system
- ✅ Equipment system with stat bonuses
- ✅ Town system (Inn, Item Shop, Equipment Shop, Tavern)
- ✅ Comprehensive economy and balance improvements
- ✅ Skill damage rebalancing (physical vs magic)
- ✅ Battle system fully cursor-based (GameBoy-ready)
