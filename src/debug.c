#if DEBUG
//CREDITS
//TheXaman:             https://github.com/TheXaman/pokeemerald/tree/tx_debug_system
//CODE USED FROM:
//ketsuban:             https://github.com/pret/pokeemerald/wiki/Add-a-debug-menu
//Pyredrid:             https://github.com/Pyredrid/pokeemerald/tree/debugmenu
//AsparagusEduardo:     https://github.com/AsparagusEduardo/pokeemerald/tree/InfusedEmerald_v2
//Ghoulslash:           https://github.com/ghoulslash/pokeemerald
#include "global.h"
#include "clock.h"
#include "credits.h"
#include "data.h"
#include "day_night.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_message_box.h"
#include "field_screen_effect.h"
#include "field_specials.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "list_menu.h"
#include "main.h"
#include "main_menu.h"
#include "malloc.h"
#include "map_name_popup.h"
#include "menu.h"
#include "naming_screen.h"
#include "new_game.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "random.h"
#include "region_map.h"
#include "rtc.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "wild_encounter.h"
#include "window.h"
#include "constants/abilities.h"
#include "constants/day_night.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_groups.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/vars.h"

// *******************************
// Enums
enum { // Main
    DEBUG_MENU_ITEM_UTILITIES,
    DEBUG_MENU_ITEM_FLAGS,
    DEBUG_MENU_ITEM_VARS,
    DEBUG_MENU_ITEM_GIVE,
    DEBUG_MENU_ITEM_MISC,
    DEBUG_MENU_ITEM_CANCEL
};
enum { // Util
    DEBUG_UTIL_MENU_ITEM_HEAL_PARTY,
    DEBUG_UTIL_MENU_ITEM_FLY,
    DEBUG_UTIL_MENU_ITEM_WARP,
    DEBUG_UTIL_MENU_ITEM_SAVEBLOCK,
    DEBUG_UTIL_MENU_ITEM_CHECKWALLCLOCK,
    DEBUG_UTIL_MENU_ITEM_SETWALLCLOCK,
    DEBUG_UTIL_MENU_ITEM_CHECKWEEKDAY,
    DEBUG_UTIL_MENU_ITEM_WATCHCREDITS,
    DEBUG_UTIL_MENU_ITEM_TRAINER_NAME,
    DEBUG_UTIL_MENU_ITEM_TRAINER_GENDER,
    DEBUG_UTIL_MENU_ITEM_TRAINER_ID,
    DEBUG_UTIL_MENU_ITEM_CHECKEVS,
    DEBUG_UTIL_MENU_ITEM_CHECKIVS,
    DEBUG_UTIL_MENU_ITEM_FORCEEGGHATCH,
    DEBUG_UTIL_MENU_ITEM_DIFF_CHECK,
};
enum { // Flags
    DEBUG_FLAG_MENU_ITEM_FLAGS,
    DEBUG_FLAG_MENU_ITEM_STARTMENU_ONOFF,
    DEBUG_FLAG_MENU_ITEM_POKEDEXFLAGS,
    DEBUG_FLAG_MENU_ITEM_POKEDEXONOFF,
    DEBUG_FLAG_MENU_ITEM_NATDEXONOFF,
    DEBUG_FLAG_MENU_ITEM_POKENAVONOFF,
    DEBUG_FLAG_MENU_ITEM_FLYANYWHERE,
    DEBUG_FLAG_MENU_ITEM_GETALLBADGES,
    DEBUG_FLAG_MENU_ITEM_COLISSION_ONOFF,
    DEBUG_FLAG_MENU_ITEM_ENCOUNTER_ONOFF,
    DEBUG_FLAG_MENU_ITEM_TRAINER_SEE_ONOFF,
    DEBUG_FLAG_MENU_ITEM_BAG_USE_ONOFF,
    DEBUG_FLAG_MENU_ITEM_CATCHING_ONOFF,
    DEBUG_FLAG_MENU_ITEM_BATTLESPEED_ONOFF,
};
enum { // Vars
    DEBUG_VARS_MENU_ITEM_VARS,
};
enum { // Give
    DEBUG_GIVE_MENU_ITEM_ITEM,
    DEBUG_MENU_ITEM_GIVE_ALLTMS,
    DEBUG_MENU_ITEM_GIVE_EGG,
    DEBUG_GIVE_MENU_ITEM_POKEMON_SIMPLE,
    DEBUG_GIVE_MENU_ITEM_POKEMON_COMPLEX,
    DEBUG_GIVE_MENU_ITEM_CHEAT,
    //DEBUG_MENU_ITEM_ACCESS_PC,
};
enum { // Misc
    DEBUG_MISC_MENU_ITEM_EDIT_TOD,
    DEBUG_MISC_MENU_ITEM_EDIT_TOD_PAL,
    DEBUG_MISC_MENU_ITEM_SETUP_BATTLE,
    DEBUG_MISC_MENU_ITEM_START_BATTLE,
    DEBUG_MENU_ITEM_ACCESS_PC,
};

// *******************************
// Constants
#define DEBUG_MAIN_MENU_WIDTH 13
#define DEBUG_MAIN_MENU_HEIGHT 8

#define DEBUG_NUMBER_DISPLAY_WIDTH 10
#define DEBUG_NUMBER_DISPLAY_HEIGHT 4

#define DEBUG_NUMBER_DIGITS_FLAGS 4
#define DEBUG_NUMBER_DIGITS_VARIABLES 5
#define DEBUG_NUMBER_DIGITS_VARIABLE_VALUE 5
#define DEBUG_NUMBER_DIGITS_ITEMS 4
#define DEBUG_NUMBER_DIGITS_ITEM_QUANTITY 2

#define DEBUG_NUMBER_ICON_X 210
#define DEBUG_NUMBER_ICON_Y 50

// EWRAM
static EWRAM_DATA struct DebugMonData *sDebugMonData = NULL;

// *******************************
struct DebugMonData
{
    u16 mon_speciesId;
    u8  mon_level;
    u8  isShiny;
    u16 mon_natureId;
    u16 mon_abilityNum;
    u8  mon_iv_hp;
    u8  mon_iv_atk;
    u8  mon_iv_def;
    u8  mon_iv_speed;
    u8  mon_iv_satk;
    u8  mon_iv_sdef;
    u16 mon_move_0;
    u16 mon_move_1;
    u16 mon_move_2;
    u16 mon_move_3;
};

// *******************************
// Define functions
static void Debug_ShowMenu(void (*HandleInput)(u8), struct ListMenuTemplate LMtemplate);
void Debug_ShowMainMenu(void);
static void Debug_DestroyMenu(u8);
static void DebugAction_Cancel(u8);
static void DebugAction_DestroyExtraWindow(u8 taskId);

static void DebugAction_OpenUtilitiesMenu(u8);
static void DebugAction_OpenFlagsMenu(u8);
static void DebugAction_OpenVariablesMenu(u8);
static void DebugAction_OpenGiveMenu(u8);
static void DebugAction_OpenMiscMenu(u8);
static void DebugTask_HandleMenuInput_Main(u8);
static void DebugTask_HandleMenuInput_Utilities(u8);
static void DebugTask_HandleMenuInput_Flags(u8);
static void DebugTask_HandleMenuInput_Vars(u8);
static void DebugTask_HandleMenuInput_Give(u8);
static void DebugTask_HandleMenuInput_Misc(u8);

static void DebugAction_Util_HealParty(u8 taskId);
static void DebugAction_Util_Fly(u8 taskId);
static void DebugAction_Util_Warp_Warp(u8 taskId);
static void DebugAction_Util_Warp_SelectMapGroup(u8 taskId);
static void DebugAction_Util_Warp_SelectMap(u8 taskId);
static void DebugAction_Util_Warp_SelectWarp(u8 taskId);
static void DebugAction_Util_CheckSaveBlock(u8);
static void DebugAction_Util_CheckWallClock(u8);
static void DebugAction_Util_SetWallClock(u8);
static void DebugAction_Util_CheckWeekDay(u8);
static void DebugAction_Util_WatchCredits(u8);
static void DebugAction_Util_Trainer_Name(u8);
static void DebugAction_Util_Trainer_Gender(u8);
static void DebugAction_Util_Trainer_Id(u8);
static void DebugAction_Util_CheckEVs(u8);
static void DebugAction_Util_CheckIVs(u8);
static void DebugAction_Util_ForceEggHatch(u8);

static void DebugAction_EditDayNight(u8 taskId);
static void DebugAction_EditDayNight_ProcessInput(u8 taskId);
static void DebugAction_EditDayNightPalette(u8 taskId);
static void DebugAction_EditDayNightPalette_ProcessInput(u8 taskId);

static void DebugAction_SetupWildBattle(u8 taskId);
static void DebugAction_StartWildBattle(u8 taskId);
static void DebugAction_Wild_Setup_Pokemon_SelectId(u8 taskId);
static void DebugAction_Wild_Setup_Pokemon_SelectLevel(u8 taskId);

static void DebugAction_Flags_Flags(u8 taskId);
static void DebugAction_Flags_FlagsSelect(u8 taskId);

static void DebugAction_Flags_SetPokedexFlags(u8);
static void DebugAction_Flags_SwitchDex(u8);
static void DebugAction_Flags_SwitchNatDex(u8);
static void DebugAction_Flags_SwitchPokeNav(u8);
static void DebugAction_Flags_ToggleFlyFlags(u8);
static void DebugAction_Flags_ToggleBadgeFlags(u8);
static void DebugAction_Flags_CollisionOnOff(u8);
static void DebugAction_Flags_EncounterOnOff(u8);
static void DebugAction_Flags_TrainerSeeOnOff(u8);
static void DebugAction_Flags_BagUseOnOff(u8);
static void DebugAction_Flags_CatchingOnOff(u8);
static void DebugAction_Flags_BattleSpeedOnOff(u8);
static void DebugAction_Vars_Vars(u8 taskId);
static void DebugAction_Vars_Select(u8 taskId);
static void DebugAction_Vars_SetValue(u8 taskId);

static void DebugAction_Give_Item(u8 taskId);
static void DebugAction_Give_Item_SelectId(u8 taskId);
static void DebugAction_Give_Item_SelectQuantity(u8 taskId);
static void DebugAction_Give_AllTMs(u8 taskId);
static void DebugAction_Give_Egg(u8 taskId);
static void DebugAction_Give_Egg_SelectId(u8 taskId);
static void DebugAction_Give_PokemonSimple(u8 taskId);
static void DebugAction_Give_PokemonComplex(u8 taskId);
static void DebugAction_Give_Pokemon_SelectId(u8 taskId);
static void DebugAction_Give_Pokemon_SelectLevel(u8 taskId);
static void DebugAction_Give_Pokemon_SelectShiny(u8 taskId);
static void DebugAction_Give_Pokemon_SelectNature(u8 taskId);
static void DebugAction_Give_Pokemon_SelectAbility(u8 taskId);
static void DebugAction_Give_Pokemon_SelectIVs(u8 taskId);
static void DebugAction_Give_Pokemon_ComplexCreateMon(u8 taskId);
static void DebugAction_Give_Pokemon_Move(u8 taskId);
static void DebugAction_Give_CHEAT(u8 taskId);
static void DebugAction_AccessPC(u8 taskId);
static void DebugAction_Util_Inclement_Difficulty_Check(u8 taskId);
static void DebugAction_Flags_StartMenuOnOff(u8 taskId);

static void DebugTask_HandleMenuInput(u8 taskId, void (*HandleInput)(u8));
static void DebugAction_OpenSubMenu(u8 taskId, struct ListMenuTemplate LMtemplate);

extern u8 EventScript_CheckSavefileSize[];
extern u8 EventScript_DebugPack[];
extern u8 PlayersHouse_2F_EventScript_SetWallClock[];
extern u8 PlayersHouse_2F_EventScript_CheckWallClock[];
//extern u8 EventScript_GetCurrentDay[];
extern u8 EventScript_CheckEVs[];
extern u8 EventScript_CheckIVs[];
extern u8 EventScript_ForceEggHatch[];
extern u8 EventScript_Inclement_Difficulty[];

extern bool8 gPaletteTintDisabled;
extern bool8 gPaletteOverrideDisabled;
extern s16 gDNPeriodOverride;
extern u16 gDNTintOverride[3];

#define ABILITY_NAME_LENGTH 12
extern const u8 gAbilityNames[][ABILITY_NAME_LENGTH + 1];

// *******************************
//Maps per map group COPY FROM /include/constants/map_groups.h
 static const u8 MAP_GROUP_COUNT[] = {57, 5, 5, 6, 7, 8, 9, 7, 7, 14, 8, 17, 10, 23, 13, 15, 15, 2, 2, 2, 3, 1, 1, 1, 108, 61, 89, 2, 1, 13, 1, 1, 3, 1, 0};

 static const struct WindowTemplate sDebugMenu_Window_SetFlag =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 10,
     .height = 4,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_AddItem =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 10,
     .height = 4,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_SetVar =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 16,
     .height = 6,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_SetDexCount =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 8,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_EditDayNight =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 10,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_EditDayNightPalette =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 16,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_SetRespawn =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 10,
     .height = 4,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_LottoNum =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 7,
     .height = 4,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_BattleTerrain =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 1,
     .width = 11,
     .height = 4,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

 static const struct WindowTemplate sDebugMenu_Window_InfoControls =
 {
     .bg = 0,
     .tilemapLeft = 1,
     .tilemapTop = 18,
     .width = 28,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 0x120
 };

#define tWindowId data[0]
#define tWindowInfoId data[0]

// Text
// Main Menu
static const u8 gDebugText_Utilities[] =        _("Utilities");
static const u8 gDebugText_Flags[] =            _("Flags");
static const u8 gDebugText_Vars[] =             _("Variables");
static const u8 gDebugText_Give[] =             _("Give X");
static const u8 gDebugText_Misc[] =             _("Misc");
static const u8 gDebugText_Cancel[] =           _("Cancel");
// Util Menu
static const u8 gDebugText_Util_HealParty[] =               _("Heal Party");
static const u8 gDebugText_Util_Fly[] =                     _("Fly to map");
static const u8 gDebugText_Util_WarpToMap[] =               _("Warp to map warp");
static const u8 gDebugText_Util_WarpToMap_SelectMapGroup[] =_("Group: {STR_VAR_1}          \n                 \n\n{STR_VAR_3}     ");
static const u8 gDebugText_Util_WarpToMap_SelectMap[] =     _("Map: {STR_VAR_1}            \nMapSec:          \n{STR_VAR_2}                       \n{STR_VAR_3}     ");
static const u8 gDebugText_Util_WarpToMap_SelectWarp[] =    _("Warp:             \n{STR_VAR_1}                \n                                  \n{STR_VAR_3}     ");
static const u8 gDebugText_Util_WarpToMap_SelMax[] =        _("{STR_VAR_1} / {STR_VAR_2}");
static const u8 gDebugText_Util_SaveBlockSpace[] =          _("SaveBlock Space");
static const u8 gDebugText_Util_CheckWallClock[] =          _("Check Wall Clock");
static const u8 gDebugText_Util_SetWallClock[] =            _("Set Wall Clock");
static const u8 gDebugText_Util_CheckWeekDay[] =            _("Check Week Day");
static const u8 gDebugText_Util_WatchCredits[] =            _("Watch Credits");
static const u8 gDebugText_Util_Trainer_Name[] =            _("Trainer name");
static const u8 gDebugText_Util_Trainer_Gender[] =          _("Toggle T. Gender");
static const u8 gDebugText_Util_Trainer_Id[] =              _("New Trainer Id");
static const u8 gDebugText_Util_Difficulty[] =              _("Difficulty: {STR_VAR_1}          \nLevel Cap: {STR_VAR_2}");
static const u8 gDebugText_Util_CheckEVs[] =                _("Check EVs");
static const u8 gDebugText_Util_CheckIVs[] =                _("Check IVs");
static const u8 gDebugText_Util_ForceEggHatch[] =           _("Force Egg Hatch");
// Flags Menu
static const u8 gDebugText_Flags_Flags[] =                _("Set Flag XXXX");
static const u8 gDebugText_Flags_SetPokedexFlags[] =      _("All Pokédex Flags");
static const u8 gDebugText_Flags_SwitchDex[] =            _("Pokédex ON/OFF");
static const u8 gDebugText_Flags_SwitchNationalDex[] =    _("NatDex ON/OFF");
static const u8 gDebugText_Flags_SwitchPokeNav[] =        _("PokéNav ON/OFF");
static const u8 gDebugText_Flags_ToggleFlyFlags[] =       _("Fly Flags ON/OFF");
static const u8 gDebugText_Flags_ToggleAllBadges[] =      _("All badges ON/OFF");
static const u8 gDebugText_Flags_SwitchCollision[] =      _("Collision ON/OFF");
static const u8 gDebugText_Flags_SwitchEncounter[] =      _("Encounter ON/OFF");
static const u8 gDebugText_Flags_SwitchTrainerSee[] =     _("TrainerSee ON/OFF");
static const u8 gDebugText_Flags_SwitchBagUse[] =         _("BagUse ON/OFF");
static const u8 gDebugText_Flags_SwitchCatching[] =       _("Catching ON/OFF");
static const u8 gDebugText_Flags_StartMenuDebug[] =       _("In Start ON/OFF");
static const u8 gDebugText_Flags_BattleSpeed[] =          _("Battle Speed ON/OFF");
static const u8 gDebugText_Flag[] =                       _("Flag: {STR_VAR_1}   \n{STR_VAR_2}                   \n{STR_VAR_3}");
static const u8 gDebugText_FlagHex[] =                    _("{STR_VAR_1}           \n0x{STR_VAR_2}             ");
static const u8 gDebugText_FlagSet[] =                    _("TRUE");
static const u8 gDebugText_FlagUnset[] =                  _("FALSE");
// Variables Menu
static const u8 gDebugText_Vars_Vars[] =             _("Set Vars XXXX");
static const u8 gDebugText_VariableHex[] =           _("{STR_VAR_1}           \n0x{STR_VAR_2}             ");
static const u8 gDebugText_Variable[] =              _("Var: {STR_VAR_1}             \nVal: {STR_VAR_3}             \n{STR_VAR_2}");
static const u8 gDebugText_VariableValueSet[] =      _("Var: {STR_VAR_1}             \nVal: {STR_VAR_3}             \n{STR_VAR_2}");
// Give Menu
static const u8 gDebugText_Give_GiveItem[] =            _("Give item XXXX");
static const u8 gDebugText_ItemQuantity[] =             _("Quantity:       \n{STR_VAR_1}    \n\n{STR_VAR_2}");
static const u8 gDebugText_ItemID[] =                   _("{STR_VAR_3}\n{STR_VAR_1}    \n\n{STR_VAR_2}");
static const u8 gDebugText_Give_AllTMs[] =              _("Give all TMs");
static const u8 gDebugText_Give_Egg[] =                 _("Give Egg");
static const u8 gDebugText_Give_GivePokemonSimple[] =   _("Pkm(lvl)");
static const u8 gDebugText_Give_GivePokemonComplex[] =  _("Pkm(l,s,n,a,IV,mov)");
static const u8 gDebugText_PokemonID[] =                _("Species: {STR_VAR_3}\n{STR_VAR_1}    \n\n{STR_VAR_2}");
static const u8 gDebugText_PokemonLevel[] =             _("Level:                   \n{STR_VAR_1}           \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonShiny[] =             _("Shiny:                   \n   {STR_VAR_2}             \n              \n                ");
static const u8 gDebugText_PokemonNature[] =            _("NatureId: {STR_VAR_3}          \n{STR_VAR_1}          \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonAbility[] =           _("AbilityNum: {STR_VAR_3}          \n{STR_VAR_1}          \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonIVs[] =               _("All IVs:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_0[] =              _("IV HP:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_1[] =              _("IV Attack:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_2[] =              _("IV Defense:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_3[] =              _("IV Speed:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_4[] =              _("IV Sp. Attack:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonIV_5[] =              _("IV Sp. Defense:               \n    {STR_VAR_3}            \n             \n{STR_VAR_2}          ");
static const u8 gDebugText_PokemonMove_0[] =            _("Move 0: {STR_VAR_3}                   \n{STR_VAR_1}           \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonMove_1[] =            _("Move 1: {STR_VAR_3}                   \n{STR_VAR_1}           \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonMove_2[] =            _("Move 2: {STR_VAR_3}                   \n{STR_VAR_1}           \n          \n{STR_VAR_2}");
static const u8 gDebugText_PokemonMove_3[] =            _("Move 3: {STR_VAR_3}                   \n{STR_VAR_1}           \n          \n{STR_VAR_2}");
static const u8 gDebugText_Give_GiveCHEAT[] =           _("CHEAT start");
static const u8 gDebugText_AccessPC[] =                 _("Access PC");
static const u8 gDebugText_Util_CheckDiff[] =           _("Check Difficulty");

// Misc Menu
static const u8 gDebugText_Util_Edit_TOD[] =                _("Edit Time Of Day");
static const u8 gDebugText_Util_Edit_TOD_Palette[] =        _("Edit Time Of Day Palette");
static const u8 gDebugText_Util_Setup_Wild_Battle[] =       _("Setup Wild Battle");
static const u8 gDebugText_Util_Start_Wild_Battle[] =       _("Start Wild Battle");

static const u8 sText_On[] = _("{COLOR GREEN}ON");
static const u8 sText_Off[] = _("{COLOR RED}OFF");
static const u8 sText_ClockStatus[] = _("Time: {STR_VAR_1}");
static const u8 sText_RGBValues[] = _("{COLOR RED}{STR_VAR_1}\n{COLOR GREEN}{STR_VAR_2}\n{COLOR BLUE}{STR_VAR_3}");


static const u8 sText_Controls_Select_Reset[] = _("{SELECT_BUTTON} = Reset");
static const u8 sText_Controls[] = _("{STR_VAR_1}  {STR_VAR_2}  {STR_VAR_3}");

static const u8 sText_RGBValuesRed[] =    _("{COLOR RED}{SHADOW LIGHT_GRAY} ▶{STR_VAR_1}{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR GREEN}{SHADOW LIGHT_GRAY}{STR_VAR_2}{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR BLUE}{SHADOW LIGHT_GRAY}{STR_VAR_3}");
static const u8 sText_RGBValuesGreen[] =  _("{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR RED}{SHADOW LIGHT_GRAY}{STR_VAR_1}{COLOR GREEN}{SHADOW LIGHT_GRAY} ▶{STR_VAR_2}{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR BLUE}{SHADOW LIGHT_GRAY}{STR_VAR_3}");
static const u8 sText_RGBValuesBlue[] =   _("{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR RED}{SHADOW LIGHT_GRAY}{STR_VAR_1}{COLOR TRANSPARENT}{SHADOW TRANSPARENT} ▶{COLOR GREEN}{SHADOW LIGHT_GRAY}{STR_VAR_2}{COLOR BLUE}{SHADOW LIGHT_GRAY} ▶{STR_VAR_3}");


static const u8 digitInidicator_1[] =               _("{LEFT_ARROW}{RIGHT_ARROW} +1       ");
static const u8 digitInidicator_10[] =              _("{LEFT_ARROW}{RIGHT_ARROW} +10      ");
static const u8 digitInidicator_100[] =             _("{LEFT_ARROW}{RIGHT_ARROW} +100     ");
static const u8 digitInidicator_1000[] =            _("{LEFT_ARROW}{RIGHT_ARROW} +1000    ");
static const u8 digitInidicator_10000[] =           _("{LEFT_ARROW}{RIGHT_ARROW} +10000   ");
static const u8 digitInidicator_100000[] =          _("{LEFT_ARROW}{RIGHT_ARROW} +100000  ");
static const u8 digitInidicator_1000000[] =         _("{LEFT_ARROW}{RIGHT_ARROW} +1000000 ");
static const u8 digitInidicator_10000000[] =        _("{LEFT_ARROW}{RIGHT_ARROW} +10000000");
const u8 * const gText_DigitIndicator[] =
{
    digitInidicator_1,
    digitInidicator_10,
    digitInidicator_100,
    digitInidicator_1000,
    digitInidicator_10000,
    digitInidicator_100000,
    digitInidicator_1000000,
    digitInidicator_10000000
};
static const s32 sPowersOfTen[] =
{
             1,
            10,
           100,
          1000,
         10000,
        100000,
       1000000,
      10000000,
     100000000,
    1000000000,
};

// *******************************
// List Menu Items

static const struct ListMenuItem sDebugMenu_Items_Main[] =
{
    [DEBUG_MENU_ITEM_UTILITIES]     = {gDebugText_Utilities,    DEBUG_MENU_ITEM_UTILITIES},
    [DEBUG_MENU_ITEM_FLAGS]         = {gDebugText_Flags,        DEBUG_MENU_ITEM_FLAGS},
    [DEBUG_MENU_ITEM_VARS]          = {gDebugText_Vars,         DEBUG_MENU_ITEM_VARS},
    [DEBUG_MENU_ITEM_GIVE]          = {gDebugText_Give,         DEBUG_MENU_ITEM_GIVE},
    [DEBUG_MENU_ITEM_MISC]          = {gDebugText_Misc,         DEBUG_MENU_ITEM_MISC},
    [DEBUG_MENU_ITEM_CANCEL]        = {gDebugText_Cancel,       DEBUG_MENU_ITEM_CANCEL}
};

static const struct ListMenuItem sDebugMenu_Items_Utilities[] =
{
    [DEBUG_UTIL_MENU_ITEM_HEAL_PARTY]       = {gDebugText_Util_HealParty,        DEBUG_UTIL_MENU_ITEM_HEAL_PARTY},
    [DEBUG_UTIL_MENU_ITEM_FLY]              = {gDebugText_Util_Fly,              DEBUG_UTIL_MENU_ITEM_FLY},
    [DEBUG_UTIL_MENU_ITEM_WARP]             = {gDebugText_Util_WarpToMap,        DEBUG_UTIL_MENU_ITEM_WARP},
    [DEBUG_UTIL_MENU_ITEM_SAVEBLOCK]        = {gDebugText_Util_SaveBlockSpace,   DEBUG_UTIL_MENU_ITEM_SAVEBLOCK},
    [DEBUG_UTIL_MENU_ITEM_CHECKWALLCLOCK]   = {gDebugText_Util_CheckWallClock,   DEBUG_UTIL_MENU_ITEM_CHECKWALLCLOCK},
    [DEBUG_UTIL_MENU_ITEM_SETWALLCLOCK]     = {gDebugText_Util_SetWallClock,     DEBUG_UTIL_MENU_ITEM_SETWALLCLOCK},
    [DEBUG_UTIL_MENU_ITEM_CHECKWEEKDAY]     = {gDebugText_Util_CheckWeekDay,     DEBUG_UTIL_MENU_ITEM_CHECKWEEKDAY},
    [DEBUG_UTIL_MENU_ITEM_WATCHCREDITS]     = {gDebugText_Util_WatchCredits,     DEBUG_UTIL_MENU_ITEM_WATCHCREDITS},
    [DEBUG_UTIL_MENU_ITEM_TRAINER_NAME]     = {gDebugText_Util_Trainer_Name,     DEBUG_UTIL_MENU_ITEM_TRAINER_NAME},
    [DEBUG_UTIL_MENU_ITEM_TRAINER_GENDER]   = {gDebugText_Util_Trainer_Gender,   DEBUG_UTIL_MENU_ITEM_TRAINER_GENDER},
    [DEBUG_UTIL_MENU_ITEM_TRAINER_ID]       = {gDebugText_Util_Trainer_Id,       DEBUG_UTIL_MENU_ITEM_TRAINER_ID},
    [DEBUG_UTIL_MENU_ITEM_CHECKEVS]         = {gDebugText_Util_CheckEVs,         DEBUG_UTIL_MENU_ITEM_CHECKEVS},
    [DEBUG_UTIL_MENU_ITEM_CHECKIVS]         = {gDebugText_Util_CheckIVs,         DEBUG_UTIL_MENU_ITEM_CHECKIVS},
    [DEBUG_UTIL_MENU_ITEM_FORCEEGGHATCH]    = {gDebugText_Util_ForceEggHatch,    DEBUG_UTIL_MENU_ITEM_FORCEEGGHATCH},
    [DEBUG_UTIL_MENU_ITEM_DIFF_CHECK]       = {gDebugText_Util_CheckDiff,        DEBUG_UTIL_MENU_ITEM_DIFF_CHECK},
};
static const struct ListMenuItem sDebugMenu_Items_Flags[] =
{
    [DEBUG_FLAG_MENU_ITEM_FLAGS]            = {gDebugText_Flags_Flags,               DEBUG_FLAG_MENU_ITEM_FLAGS},
    [DEBUG_FLAG_MENU_ITEM_POKEDEXFLAGS]     = {gDebugText_Flags_SetPokedexFlags,     DEBUG_FLAG_MENU_ITEM_POKEDEXFLAGS},
    [DEBUG_FLAG_MENU_ITEM_POKEDEXONOFF]     = {gDebugText_Flags_SwitchDex,           DEBUG_FLAG_MENU_ITEM_POKEDEXONOFF},
    [DEBUG_FLAG_MENU_ITEM_NATDEXONOFF]      = {gDebugText_Flags_SwitchNationalDex,   DEBUG_FLAG_MENU_ITEM_NATDEXONOFF},
    [DEBUG_FLAG_MENU_ITEM_POKENAVONOFF]     = {gDebugText_Flags_SwitchPokeNav,       DEBUG_FLAG_MENU_ITEM_POKENAVONOFF},
    [DEBUG_FLAG_MENU_ITEM_FLYANYWHERE]      = {gDebugText_Flags_ToggleFlyFlags,      DEBUG_FLAG_MENU_ITEM_FLYANYWHERE},
    [DEBUG_FLAG_MENU_ITEM_GETALLBADGES]     = {gDebugText_Flags_ToggleAllBadges,     DEBUG_FLAG_MENU_ITEM_GETALLBADGES},
    [DEBUG_FLAG_MENU_ITEM_COLISSION_ONOFF]  = {gDebugText_Flags_SwitchCollision,     DEBUG_FLAG_MENU_ITEM_COLISSION_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_ENCOUNTER_ONOFF]  = {gDebugText_Flags_SwitchEncounter,     DEBUG_FLAG_MENU_ITEM_ENCOUNTER_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_TRAINER_SEE_ONOFF]= {gDebugText_Flags_SwitchTrainerSee,    DEBUG_FLAG_MENU_ITEM_TRAINER_SEE_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_BAG_USE_ONOFF]    = {gDebugText_Flags_SwitchBagUse,        DEBUG_FLAG_MENU_ITEM_BAG_USE_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_CATCHING_ONOFF]   = {gDebugText_Flags_SwitchCatching,      DEBUG_FLAG_MENU_ITEM_CATCHING_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_STARTMENU_ONOFF]  = {gDebugText_Flags_StartMenuDebug,      DEBUG_FLAG_MENU_ITEM_STARTMENU_ONOFF},
    [DEBUG_FLAG_MENU_ITEM_BATTLESPEED_ONOFF]= {gDebugText_Flags_BattleSpeed,         DEBUG_FLAG_MENU_ITEM_BATTLESPEED_ONOFF},
};
static const struct ListMenuItem sDebugMenu_Items_Vars[] =
{
    [DEBUG_VARS_MENU_ITEM_VARS]   = {gDebugText_Vars_Vars,       DEBUG_FLAG_MENU_ITEM_FLAGS},
};
static const struct ListMenuItem sDebugMenu_Items_Give[] =
{
    [DEBUG_GIVE_MENU_ITEM_ITEM]             = {gDebugText_Give_GiveItem,            DEBUG_GIVE_MENU_ITEM_ITEM},
    [DEBUG_MENU_ITEM_GIVE_ALLTMS]           = {gDebugText_Give_AllTMs,              DEBUG_MENU_ITEM_GIVE_ALLTMS},
    [DEBUG_MENU_ITEM_GIVE_EGG]              = {gDebugText_Give_Egg,                 DEBUG_MENU_ITEM_GIVE_EGG},
    [DEBUG_GIVE_MENU_ITEM_POKEMON_SIMPLE]   = {gDebugText_Give_GivePokemonSimple,   DEBUG_GIVE_MENU_ITEM_POKEMON_SIMPLE},
    [DEBUG_GIVE_MENU_ITEM_POKEMON_COMPLEX]  = {gDebugText_Give_GivePokemonComplex,  DEBUG_GIVE_MENU_ITEM_POKEMON_COMPLEX},
    [DEBUG_GIVE_MENU_ITEM_CHEAT]            = {gDebugText_Give_GiveCHEAT,           DEBUG_GIVE_MENU_ITEM_CHEAT},
    //[DEBUG_MENU_ITEM_ACCESS_PC] = {gDebugText_AccessPC, DEBUG_MENU_ITEM_ACCESS_PC},
};
static const struct ListMenuItem sDebugMenu_Items_Misc[] =
{
    [DEBUG_MISC_MENU_ITEM_EDIT_TOD]         = {gDebugText_Util_Edit_TOD, DEBUG_MISC_MENU_ITEM_EDIT_TOD},
    [DEBUG_MISC_MENU_ITEM_EDIT_TOD_PAL]     = {gDebugText_Util_Edit_TOD_Palette, DEBUG_MISC_MENU_ITEM_EDIT_TOD_PAL},
    [DEBUG_MISC_MENU_ITEM_SETUP_BATTLE]     = {gDebugText_Util_Setup_Wild_Battle, DEBUG_MISC_MENU_ITEM_SETUP_BATTLE},
    [DEBUG_MISC_MENU_ITEM_START_BATTLE]     = {gDebugText_Util_Start_Wild_Battle, DEBUG_MISC_MENU_ITEM_START_BATTLE},
    [DEBUG_MENU_ITEM_ACCESS_PC]             = {gDebugText_AccessPC, DEBUG_MENU_ITEM_ACCESS_PC},
};

// *******************************
// Menu Actions
static void (*const sDebugMenu_Actions_Main[])(u8) =
{
    [DEBUG_MENU_ITEM_UTILITIES]     = DebugAction_OpenUtilitiesMenu,
    [DEBUG_MENU_ITEM_FLAGS]         = DebugAction_OpenFlagsMenu,
    [DEBUG_MENU_ITEM_VARS]          = DebugAction_OpenVariablesMenu,
    [DEBUG_MENU_ITEM_GIVE]          = DebugAction_OpenGiveMenu,
    [DEBUG_MENU_ITEM_MISC]          = DebugAction_OpenMiscMenu,
    [DEBUG_MENU_ITEM_CANCEL]        = DebugAction_Cancel
};
static void (*const sDebugMenu_Actions_Utilities[])(u8) =
{
    [DEBUG_UTIL_MENU_ITEM_HEAL_PARTY]       = DebugAction_Util_HealParty,
    [DEBUG_UTIL_MENU_ITEM_FLY]              = DebugAction_Util_Fly,
    [DEBUG_UTIL_MENU_ITEM_WARP]             = DebugAction_Util_Warp_Warp,
    [DEBUG_UTIL_MENU_ITEM_SAVEBLOCK]        = DebugAction_Util_CheckSaveBlock,
    [DEBUG_UTIL_MENU_ITEM_CHECKWALLCLOCK]   = DebugAction_Util_CheckWallClock,
    [DEBUG_UTIL_MENU_ITEM_SETWALLCLOCK]     = DebugAction_Util_SetWallClock,
    [DEBUG_UTIL_MENU_ITEM_CHECKWEEKDAY]     = DebugAction_Util_CheckWeekDay,
    [DEBUG_UTIL_MENU_ITEM_WATCHCREDITS]     = DebugAction_Util_WatchCredits,
    [DEBUG_UTIL_MENU_ITEM_TRAINER_NAME]     = DebugAction_Util_Trainer_Name,
    [DEBUG_UTIL_MENU_ITEM_TRAINER_GENDER]   = DebugAction_Util_Trainer_Gender,
    [DEBUG_UTIL_MENU_ITEM_TRAINER_ID]       = DebugAction_Util_Trainer_Id,
    [DEBUG_UTIL_MENU_ITEM_CHECKEVS]         = DebugAction_Util_CheckEVs,
    [DEBUG_UTIL_MENU_ITEM_CHECKIVS]         = DebugAction_Util_CheckIVs,
    [DEBUG_UTIL_MENU_ITEM_FORCEEGGHATCH]    = DebugAction_Util_ForceEggHatch,
    [DEBUG_UTIL_MENU_ITEM_DIFF_CHECK]       = DebugAction_Util_Inclement_Difficulty_Check,
};
static void (*const sDebugMenu_Actions_Flags[])(u8) =
{
    [DEBUG_FLAG_MENU_ITEM_FLAGS]            = DebugAction_Flags_Flags,
    [DEBUG_FLAG_MENU_ITEM_POKEDEXFLAGS]     = DebugAction_Flags_SetPokedexFlags,
    [DEBUG_FLAG_MENU_ITEM_POKEDEXONOFF]     = DebugAction_Flags_SwitchDex,
    [DEBUG_FLAG_MENU_ITEM_NATDEXONOFF]      = DebugAction_Flags_SwitchNatDex,
    [DEBUG_FLAG_MENU_ITEM_POKENAVONOFF]     = DebugAction_Flags_SwitchPokeNav,
    [DEBUG_FLAG_MENU_ITEM_FLYANYWHERE]      = DebugAction_Flags_ToggleFlyFlags,
    [DEBUG_FLAG_MENU_ITEM_GETALLBADGES]     = DebugAction_Flags_ToggleBadgeFlags,
    [DEBUG_FLAG_MENU_ITEM_COLISSION_ONOFF]  = DebugAction_Flags_CollisionOnOff,
    [DEBUG_FLAG_MENU_ITEM_ENCOUNTER_ONOFF]  = DebugAction_Flags_EncounterOnOff,
    [DEBUG_FLAG_MENU_ITEM_TRAINER_SEE_ONOFF]= DebugAction_Flags_TrainerSeeOnOff,
    [DEBUG_FLAG_MENU_ITEM_BAG_USE_ONOFF]    = DebugAction_Flags_BagUseOnOff,
    [DEBUG_FLAG_MENU_ITEM_CATCHING_ONOFF]   = DebugAction_Flags_CatchingOnOff,
    [DEBUG_FLAG_MENU_ITEM_STARTMENU_ONOFF]  = DebugAction_Flags_StartMenuOnOff,
    [DEBUG_FLAG_MENU_ITEM_BATTLESPEED_ONOFF]= DebugAction_Flags_BattleSpeedOnOff,
};
static void (*const sDebugMenu_Actions_Vars[])(u8) =
{
    [DEBUG_VARS_MENU_ITEM_VARS]     = DebugAction_Vars_Vars,
};
static void (*const sDebugMenu_Actions_Give[])(u8) =
{
    [DEBUG_GIVE_MENU_ITEM_ITEM]             = DebugAction_Give_Item,
    [DEBUG_MENU_ITEM_GIVE_ALLTMS]           = DebugAction_Give_AllTMs,
    [DEBUG_MENU_ITEM_GIVE_EGG]              = DebugAction_Give_Egg,
    [DEBUG_GIVE_MENU_ITEM_POKEMON_SIMPLE]   = DebugAction_Give_PokemonSimple,
    [DEBUG_GIVE_MENU_ITEM_POKEMON_COMPLEX]  = DebugAction_Give_PokemonComplex,
    [DEBUG_GIVE_MENU_ITEM_CHEAT]            = DebugAction_Give_CHEAT,
    //[DEBUG_MENU_ITEM_ACCESS_PC] = DebugAction_AccessPC,
};

static void (*const sDebugMenu_Actions_Misc[])(u8) =
{

    [DEBUG_MISC_MENU_ITEM_EDIT_TOD]         = DebugAction_EditDayNight,
    [DEBUG_MISC_MENU_ITEM_EDIT_TOD_PAL]     = DebugAction_EditDayNightPalette,
    [DEBUG_MISC_MENU_ITEM_SETUP_BATTLE]     = DebugAction_SetupWildBattle,
    [DEBUG_MISC_MENU_ITEM_START_BATTLE]     = DebugAction_StartWildBattle,
    [DEBUG_MENU_ITEM_ACCESS_PC]             = DebugAction_AccessPC,
};

// *******************************
// Windows
static const struct WindowTemplate sDebugMenuWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = DEBUG_MAIN_MENU_WIDTH,
    .height = 2 * DEBUG_MAIN_MENU_HEIGHT,
    .paletteNum = 15,
    .baseBlock = 1,
};
static const struct WindowTemplate sDebugNumberDisplayWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 6 + DEBUG_MAIN_MENU_WIDTH,
    .tilemapTop = 1,
    .width = DEBUG_NUMBER_DISPLAY_WIDTH,
    .height = 2 * DEBUG_NUMBER_DISPLAY_HEIGHT,
    .paletteNum = 15,
    .baseBlock = 1,
};

// *******************************
// List Menu Templates
static const struct ListMenuTemplate sDebugMenu_ListTemplate_Main =
{
    .items = sDebugMenu_Items_Main,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Main),
};
static const struct ListMenuTemplate sDebugMenu_ListTemplate_Utilities =
{
    .items = sDebugMenu_Items_Utilities,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Utilities),
};
static const struct ListMenuTemplate sDebugMenu_ListTemplate_Flags =
{
    .items = sDebugMenu_Items_Flags,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Flags),
};
static const struct ListMenuTemplate sDebugMenu_ListTemplate_Vars =
{
    .items = sDebugMenu_Items_Vars,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Vars),
};
static const struct ListMenuTemplate sDebugMenu_ListTemplate_Give =
{
    .items = sDebugMenu_Items_Give,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Give),
};

static const struct ListMenuTemplate sDebugMenu_ListTemplate_Misc =
{
    .items = sDebugMenu_Items_Misc,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenu_Items_Misc),
};

// *******************************
// Functions universal
void Debug_ShowMainMenu(void)
{
    Debug_ShowMenu(DebugTask_HandleMenuInput_Main, sDebugMenu_ListTemplate_Main);
}
static void Debug_ShowMenu(void (*HandleInput)(u8), struct ListMenuTemplate LMtemplate)
{
    struct ListMenuTemplate menuTemplate;
    u8 windowId;
    u8 menuTaskId;
    u8 inputTaskId;

    // create window
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    // create list menu
    menuTemplate = LMtemplate;
    menuTemplate.maxShowed = DEBUG_MAIN_MENU_HEIGHT;
    menuTemplate.windowId = windowId;
    menuTemplate.header_X = 0;
    menuTemplate.item_X = 8;
    menuTemplate.cursor_X = 0;
    menuTemplate.upText_Y = 1;
    menuTemplate.cursorPal = 2;
    menuTemplate.fillValue = 1;
    menuTemplate.cursorShadowPal = 3;
    menuTemplate.lettersSpacing = 1;
    menuTemplate.itemVerticalPadding = 0;
    menuTemplate.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    menuTemplate.fontId = 1;
    menuTemplate.cursorKind = 0;
    menuTaskId = ListMenuInit(&menuTemplate, 0, 0);

    // draw everything
    CopyWindowToVram(windowId, 3);

    // create input handler task
    inputTaskId = CreateTask(HandleInput, 3);
    gTasks[inputTaskId].data[0] = menuTaskId;
    gTasks[inputTaskId].data[1] = windowId;
}
static void Debug_DestroyMenu(u8 taskId)
{
    DestroyListMenuTask(gTasks[taskId].data[0], NULL, NULL);
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);
    DestroyTask(taskId);
}
static void DebugAction_Cancel(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    EnableBothScriptContexts();
}
static void DebugAction_DestroyExtraWindow(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    ClearStdWindowAndFrame(gTasks[taskId].data[2], TRUE);
    RemoveWindow(gTasks[taskId].data[2]);

    DestroyTask(taskId);
    EnableBothScriptContexts();
}

// *******************************
// Handle Inputs
static void DebugTask_HandleMenuInput_Main(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Main[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        EnableBothScriptContexts();
    }
}
static void DebugTask_HandleMenuInput_Utilities(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Utilities[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}
static void DebugTask_HandleMenuInput_Flags(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Flags[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}
static void DebugTask_HandleMenuInput_Vars(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Vars[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}
static void DebugTask_HandleMenuInput_Give(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Give[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}
static void DebugTask_HandleMenuInput_Misc(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenu_Actions_Misc[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}

// *******************************
// Open sub-menus
static void DebugAction_OpenUtilitiesMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput_Utilities, sDebugMenu_ListTemplate_Utilities);
}
static void DebugAction_OpenFlagsMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput_Flags, sDebugMenu_ListTemplate_Flags);
}
static void DebugAction_OpenVariablesMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput_Vars, sDebugMenu_ListTemplate_Vars);
}
static void DebugAction_OpenGiveMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput_Give, sDebugMenu_ListTemplate_Give);
}

static void DebugAction_OpenMiscMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput_Misc, sDebugMenu_ListTemplate_Misc);
}


// *******************************
// Actions Utilities
static void DebugAction_Util_HealParty(u8 taskId)
{
    PlaySE(SE_USE_ITEM);
    HealPlayerParty();
    Debug_DestroyMenu(taskId);
    EnableBothScriptContexts();
}

static void DebugAction_Util_Fly(u8 taskId)
{
    FlagSet(FLAG_VISITED_LITTLEROOT_TOWN);
    FlagSet(FLAG_VISITED_OLDALE_TOWN);
    FlagSet(FLAG_VISITED_DEWFORD_TOWN);
    FlagSet(FLAG_VISITED_LAVARIDGE_TOWN);
    FlagSet(FLAG_VISITED_FALLARBOR_TOWN);
    FlagSet(FLAG_VISITED_VERDANTURF_TOWN);
    FlagSet(FLAG_VISITED_PACIFIDLOG_TOWN);
    FlagSet(FLAG_VISITED_PETALBURG_CITY);
    FlagSet(FLAG_VISITED_SLATEPORT_CITY);
    FlagSet(FLAG_VISITED_MAUVILLE_CITY);
    FlagSet(FLAG_VISITED_RUSTBORO_CITY);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    FlagSet(FLAG_VISITED_LILYCOVE_CITY);
    FlagSet(FLAG_VISITED_MOSSDEEP_CITY);
    FlagSet(FLAG_VISITED_SOOTOPOLIS_CITY);
    FlagSet(FLAG_VISITED_EVER_GRANDE_CITY);
    FlagSet(FLAG_LANDMARK_POKEMON_LEAGUE);
    FlagSet(FLAG_LANDMARK_BATTLE_FRONTIER);
    Debug_DestroyMenu(taskId);
    SetMainCallback2(CB2_OpenFlyMap);
}

static void DebugAction_Util_Warp_Warp(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);


    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
    ConvertIntToDecimalStringN(gStringVar2, MAP_GROUPS_COUNT-1, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringExpandPlaceholders(gStringVar1, gDebugText_Util_WarpToMap_SelMax);
    StringCopy(gStringVar3, gText_DigitIndicator[0]);
    StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectMapGroup);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    gTasks[taskId].func = DebugAction_Util_Warp_SelectMapGroup;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 0;            //Current Flag
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[5] = 0; //Map Group
    gTasks[taskId].data[6] = 0; //Map
    gTasks[taskId].data[7] = 0; //warp
}
static void DebugAction_Util_Warp_SelectMapGroup(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > MAP_GROUPS_COUNT-1)
                gTasks[taskId].data[3] = MAP_GROUPS_COUNT-1;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        ConvertIntToDecimalStringN(gStringVar2, MAP_GROUPS_COUNT-1, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar1, gDebugText_Util_WarpToMap_SelMax);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectMapGroup);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[5] = gTasks[taskId].data[3];
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        ConvertIntToDecimalStringN(gStringVar2, MAP_GROUP_COUNT[gTasks[taskId].data[5]]-1, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar1, gDebugText_Util_WarpToMap_SelMax);
        GetMapName(gStringVar2, Overworld_GetMapHeaderByGroupAndId(gTasks[taskId].data[5], gTasks[taskId].data[3])->regionMapSectionId, 0);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectMap);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Util_Warp_SelectMap;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Util_Warp_SelectMap(u8 taskId)
{
    u8 max_value = MAP_GROUP_COUNT[gTasks[taskId].data[5]]; //maps in the selected map group

    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > max_value-1)
                gTasks[taskId].data[3] = max_value-1;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        ConvertIntToDecimalStringN(gStringVar2, MAP_GROUP_COUNT[gTasks[taskId].data[5]]-1, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar1, gDebugText_Util_WarpToMap_SelMax);
        GetMapName(gStringVar2, Overworld_GetMapHeaderByGroupAndId(gTasks[taskId].data[5], gTasks[taskId].data[3])->regionMapSectionId, 0);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectMap);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[6] = gTasks[taskId].data[3];
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectWarp);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
        gTasks[taskId].func = DebugAction_Util_Warp_SelectWarp;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Util_Warp_SelectWarp(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > 10)
                gTasks[taskId].data[3] = 10;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }

        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar4, gDebugText_Util_WarpToMap_SelectWarp);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[7] = gTasks[taskId].data[3];
        //WARP
        SetWarpDestinationToMapWarp(gTasks[taskId].data[5], gTasks[taskId].data[6], gTasks[taskId].data[7]); //If not warp with the number available -> center of map
        DoWarp();
        ResetInitialPlayerAvatarState();
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Util_CheckSaveBlock(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_CheckSavefileSize);
}
static void DebugAction_Util_CheckWallClock(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(PlayersHouse_2F_EventScript_CheckWallClock);
}
static void DebugAction_Util_SetWallClock(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(PlayersHouse_2F_EventScript_SetWallClock);
}
static void DebugAction_Util_CheckWeekDay(u8 taskId)
{
    //GetCurrentDayString();
    //Debug_DestroyMenu(taskId);
    //ScriptContext2_Enable();
    //ScriptContext1_SetupScript(EventScript_GetCurrentDay);
    //EnableBothScriptContexts();
}
static void DebugAction_Util_WatchCredits(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    Debug_DestroyMenu(taskId);
    SetMainCallback2(CB2_StartCreditsSequence);
}
static void DebugAction_Util_Trainer_Name(u8 taskId)
{
    NewGameBirchSpeech_SetDefaultPlayerName(Random() % 20);
    DoNamingScreen(0, gSaveBlock2Ptr->playerName, gSaveBlock2Ptr->playerGender, 0, 0, CB2_ReturnToFieldContinueScript);
}
static void DebugAction_Util_Trainer_Gender(u8 taskId)
{
    if(gSaveBlock2Ptr->playerGender == 0) // 0 Male, 1 Female
        gSaveBlock2Ptr->playerGender = 1;
    else
        gSaveBlock2Ptr->playerGender = 0;
    EnableBothScriptContexts();
    Debug_DestroyMenu(taskId);
}
static void DebugAction_Util_Trainer_Id(u8 taskId)
{
    u32 trainerId = (Random() << 0x10) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
    Debug_DestroyMenu(taskId);
    EnableBothScriptContexts();
}
static void DebugAction_Util_CheckEVs(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_CheckEVs);
}
static void DebugAction_Util_CheckIVs(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_CheckIVs);
}
static void DebugAction_Util_ForceEggHatch(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_ForceEggHatch);
}

// *******************************
// Actions Flags
static void DebugAction_Flags_Flags(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial Flag
    ConvertIntToDecimalStringN(gStringVar1, 0, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_FLAGS);
    ConvertIntToHexStringN(gStringVar2, 0, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar1, gDebugText_FlagHex);
    if(FlagGet(0) == TRUE)
        StringCopyPadded(gStringVar2, gDebugText_FlagSet, CHAR_SPACE, 15);
    else
        StringCopyPadded(gStringVar2, gDebugText_FlagUnset, CHAR_SPACE, 15);
    StringCopy(gStringVar3, gText_DigitIndicator[0]);
    StringExpandPlaceholders(gStringVar4, gDebugText_Flag);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    gTasks[taskId].func = DebugAction_Flags_FlagsSelect;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 0;            //Current Flag
    gTasks[taskId].data[4] = 0;            //Digit Selected
}
static void DebugAction_Flags_FlagsSelect(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
        FlagToggle(gTasks[taskId].data[3]);
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }

    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
        if(gTasks[taskId].data[3] >= FLAGS_COUNT){
            gTasks[taskId].data[3] = FLAGS_COUNT - 1;
        }
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
        if(gTasks[taskId].data[3] < 0){
            gTasks[taskId].data[3] = 0;
        }
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[4] -= 1;
        if(gTasks[taskId].data[4] < 0)
        {
            gTasks[taskId].data[4] = 0;
        }
    }
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[4] += 1;
        if(gTasks[taskId].data[4] > DEBUG_NUMBER_DIGITS_FLAGS-1)
        {
            gTasks[taskId].data[4] = DEBUG_NUMBER_DIGITS_FLAGS-1;
        }
    }

    if (JOY_REPEAT(DPAD_ANY) || JOY_NEW(A_BUTTON))
    {
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_FLAGS);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].data[3], STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar1, gDebugText_FlagHex);
        if(FlagGet(gTasks[taskId].data[3]) == TRUE)
            StringCopyPadded(gStringVar2, gDebugText_FlagSet, CHAR_SPACE, 15);
        else
            StringCopyPadded(gStringVar2, gDebugText_FlagUnset, CHAR_SPACE, 15);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringExpandPlaceholders(gStringVar4, gDebugText_Flag);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }
}

static void DebugAction_Flags_SetPokedexFlags(u8 taskId)
{
    u16 i;
    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        GetSetPokedexFlag(i + 1, FLAG_SET_CAUGHT);
        GetSetPokedexFlag(i + 1, FLAG_SET_SEEN);
    }
    Debug_DestroyMenu(taskId);
    EnableBothScriptContexts();
}
static void DebugAction_Flags_SwitchDex(u8 taskId)
{
    if(FlagGet(FLAG_SYS_POKEDEX_GET))
    {
        FlagClear(FLAG_SYS_POKEDEX_GET);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_SYS_POKEDEX_GET);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_SwitchNatDex(u8 taskId)
{
    if(IsNationalPokedexEnabled())
    {
        DisableNationalPokedex();
        PlaySE(SE_PC_OFF);
    }else{
        EnableNationalPokedex();
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_SwitchPokeNav(u8 taskId)
{
    if(FlagGet(FLAG_SYS_POKENAV_GET))
    {
        FlagClear(FLAG_SYS_POKENAV_GET);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_SYS_POKENAV_GET);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_ToggleFlyFlags(u8 taskId)
{
    // Sound effect
    if(FlagGet(FLAG_LANDMARK_BATTLE_FRONTIER))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_VISITED_LITTLEROOT_TOWN);
    FlagToggle(FLAG_VISITED_OLDALE_TOWN);
    FlagToggle(FLAG_VISITED_DEWFORD_TOWN);
    FlagToggle(FLAG_VISITED_LAVARIDGE_TOWN);
    FlagToggle(FLAG_VISITED_FALLARBOR_TOWN);
    FlagToggle(FLAG_VISITED_VERDANTURF_TOWN);
    FlagToggle(FLAG_VISITED_PACIFIDLOG_TOWN);
    FlagToggle(FLAG_VISITED_PETALBURG_CITY);
    FlagToggle(FLAG_VISITED_SLATEPORT_CITY);
    FlagToggle(FLAG_VISITED_MAUVILLE_CITY);
    FlagToggle(FLAG_VISITED_RUSTBORO_CITY);
    FlagToggle(FLAG_VISITED_FORTREE_CITY);
    FlagToggle(FLAG_VISITED_LILYCOVE_CITY);
    FlagToggle(FLAG_VISITED_MOSSDEEP_CITY);
    FlagToggle(FLAG_VISITED_SOOTOPOLIS_CITY);
    FlagToggle(FLAG_VISITED_EVER_GRANDE_CITY);
    FlagToggle(FLAG_LANDMARK_POKEMON_LEAGUE);
    FlagToggle(FLAG_LANDMARK_BATTLE_FRONTIER);
}
static void DebugAction_Flags_ToggleBadgeFlags(u8 taskId)
{
    // Sound effect
    if(FlagGet(FLAG_BADGE08_GET))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_BADGE01_GET);
    FlagToggle(FLAG_BADGE02_GET);
    FlagToggle(FLAG_BADGE03_GET);
    FlagToggle(FLAG_BADGE04_GET);
    FlagToggle(FLAG_BADGE05_GET);
    FlagToggle(FLAG_BADGE06_GET);
    FlagToggle(FLAG_BADGE07_GET);
    FlagToggle(FLAG_BADGE08_GET);
}
static void DebugAction_Flags_CollisionOnOff(u8 taskId)
{
    if(FlagGet(FLAG_DISABLE_COLLISIONS))
    {
        FlagClear(FLAG_DISABLE_COLLISIONS);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_DISABLE_COLLISIONS);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_EncounterOnOff(u8 taskId)
{
    if(FlagGet(FLAG_DISABLE_WILD_ENCOUNTERS))
    {
        FlagClear(FLAG_DISABLE_WILD_ENCOUNTERS);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_DISABLE_WILD_ENCOUNTERS);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_TrainerSeeOnOff(u8 taskId)
{
    if(FlagGet(FLAG_DISABLE_TRAINER_ENCOUNTERS))
    {
        FlagClear(FLAG_DISABLE_TRAINER_ENCOUNTERS);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_DISABLE_TRAINER_ENCOUNTERS);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_BagUseOnOff(u8 taskId)
{
    if(FlagGet(FLAG_DISABLE_BATTLE_BAG_ACCESS))
    {
        FlagClear(FLAG_DISABLE_BATTLE_BAG_ACCESS);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_DISABLE_BATTLE_BAG_ACCESS);
        PlaySE(SE_PC_LOGIN);
    }
}
static void DebugAction_Flags_CatchingOnOff(u8 taskId)
{
    if(FlagGet(FLAG_DISABLE_BALL_THROWS))
    {
        FlagClear(FLAG_DISABLE_BALL_THROWS);
        PlaySE(SE_PC_OFF);
    }else{
        FlagSet(FLAG_DISABLE_BALL_THROWS);
        PlaySE(SE_PC_LOGIN);
    }
}

static void DebugAction_Flags_BattleSpeedOnOff(u8 taskId)
{
  // Applies directly on save data rather than a flag. TODO?
    if(gSaveBlock2Ptr->battleAnimSpeed > 0)
    {
        gSaveBlock2Ptr->battleAnimSpeed = 0;
        PlaySE(SE_PC_OFF);
    }else{
        gSaveBlock2Ptr->battleAnimSpeed = 1;
        PlaySE(SE_PC_LOGIN);
    }
}
// *******************************
// Actions Variables
static void DebugAction_Vars_Vars(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial Variable
    ConvertIntToDecimalStringN(gStringVar1, VARS_START, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
    ConvertIntToHexStringN(gStringVar2, VARS_START, STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar1, gDebugText_VariableHex);
    ConvertIntToDecimalStringN(gStringVar3, 0, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    StringExpandPlaceholders(gStringVar4, gDebugText_Variable);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    gTasks[taskId].func = DebugAction_Vars_Select;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = VARS_START;            //Current Variable
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[5] = 0;            //Current Variable VALUE
}

static void DebugAction_Vars_Select(u8 taskId)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
        if(gTasks[taskId].data[3] > VARS_END){
            gTasks[taskId].data[3] = VARS_END;
        }
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
        if(gTasks[taskId].data[3] < VARS_START){
            gTasks[taskId].data[3] = VARS_START;
        }
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        gTasks[taskId].data[4] -= 1;
        if(gTasks[taskId].data[4] < 0)
        {
            gTasks[taskId].data[4] = 0;
        }
    }
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        gTasks[taskId].data[4] += 1;
        if(gTasks[taskId].data[4] > DEBUG_NUMBER_DIGITS_VARIABLES-1)
        {
            gTasks[taskId].data[4] = DEBUG_NUMBER_DIGITS_VARIABLES-1;
        }
    }

    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].data[3], STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, gDebugText_VariableHex);
        if (VarGetIfExist(gTasks[taskId].data[3]) == 65535) //Current value, if 65535 the value hasnt been set
            gTasks[taskId].data[5] = 0;
        else
            gTasks[taskId].data[5] = VarGet(gTasks[taskId].data[3]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[5], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]); //Current digit

        //Combine str's to full window string
        StringExpandPlaceholders(gStringVar4, gDebugText_Variable);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[4] = 0;

        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].data[3], STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, gDebugText_VariableHex);
        if (VarGetIfExist(gTasks[taskId].data[3]) == 65535) //Current value if 65535 the value hasnt been set
            gTasks[taskId].data[5] = 0;
        else
            gTasks[taskId].data[5] = VarGet(gTasks[taskId].data[3]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[5], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]); //Current digit
        StringExpandPlaceholders(gStringVar4, gDebugText_VariableValueSet);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].data[6] = gTasks[taskId].data[5]; //New value selector
        gTasks[taskId].func = DebugAction_Vars_SetValue;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }
}
static void DebugAction_Vars_SetValue(u8 taskId)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        if (gTasks[taskId].data[6] + sPowersOfTen[gTasks[taskId].data[4]] <= 32000)
            gTasks[taskId].data[6] += sPowersOfTen[gTasks[taskId].data[4]];
        else
            gTasks[taskId].data[6] = 32000-1;
        if(gTasks[taskId].data[6] >= 32000){
            gTasks[taskId].data[6] = 32000-1;
        }
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        gTasks[taskId].data[6] -= sPowersOfTen[gTasks[taskId].data[4]];
        if(gTasks[taskId].data[6] < 0){
            gTasks[taskId].data[6] = 0;
        }
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        gTasks[taskId].data[4] -= 1;
        if(gTasks[taskId].data[4] < 0)
        {
            gTasks[taskId].data[4] = 0;
        }
    }
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        gTasks[taskId].data[4] += 1;
        if(gTasks[taskId].data[4] > 4)
        {
            gTasks[taskId].data[4] = 4;
        }
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        VarSet(gTasks[taskId].data[3], gTasks[taskId].data[6]);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }

    if (JOY_REPEAT(DPAD_ANY) || JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].data[3], STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, gDebugText_VariableHex);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[6], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]); //Current digit
        StringExpandPlaceholders(gStringVar4, gDebugText_VariableValueSet);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }
}

// *******************************
// Actions Give
#define ITEM_TAG 0xFDF3
static void DebugAction_Give_Item(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial ID
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    CopyItemName(1, gStringVar1);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, gDebugText_ItemID);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    gTasks[taskId].func = DebugAction_Give_Item_SelectId;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 1;            //Current ID
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[6] = AddItemIconSprite(ITEM_TAG, ITEM_TAG, gTasks[taskId].data[3]);
    gSprites[gTasks[taskId].data[6]].x = DEBUG_NUMBER_ICON_X+10;
    gSprites[gTasks[taskId].data[6]].y = DEBUG_NUMBER_ICON_Y+10;
    gSprites[gTasks[taskId].data[6]].oam.priority = 0;
}
static void DebugAction_Give_Item_SelectId(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] >= ITEMS_COUNT)
                gTasks[taskId].data[3] = ITEMS_COUNT - 1;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < DEBUG_NUMBER_DIGITS_ITEMS-1)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        CopyItemName(gTasks[taskId].data[3], gStringVar1);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
        StringExpandPlaceholders(gStringVar4, gDebugText_ItemID);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        FreeSpriteTilesByTag(ITEM_TAG);                         //Destroy item icon
        FreeSpritePaletteByTag(ITEM_TAG);                       //Destroy item icon
        FreeSpriteOamMatrix(&gSprites[gTasks[taskId].data[6]]); //Destroy item icon
        DestroySprite(&gSprites[gTasks[taskId].data[6]]);       //Destroy item icon
        gTasks[taskId].data[6] = AddItemIconSprite(ITEM_TAG, ITEM_TAG, gTasks[taskId].data[3]);
        gSprites[gTasks[taskId].data[6]].x = DEBUG_NUMBER_ICON_X+10;
        gSprites[gTasks[taskId].data[6]].y = DEBUG_NUMBER_ICON_Y+10;
        gSprites[gTasks[taskId].data[6]].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[5] = gTasks[taskId].data[3];
        gTasks[taskId].data[3] = 1;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEM_QUANTITY);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_ItemQuantity);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Give_Item_SelectQuantity;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        FreeSpriteTilesByTag(ITEM_TAG);                         //Destroy item icon
        FreeSpritePaletteByTag(ITEM_TAG);                       //Destroy item icon
        FreeSpriteOamMatrix(&gSprites[gTasks[taskId].data[6]]); //Destroy item icon
        DestroySprite(&gSprites[gTasks[taskId].data[6]]);       //Destroy item icon

        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Item_SelectQuantity(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] >= 100)
                gTasks[taskId].data[3] = 99;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEM_QUANTITY);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_ItemQuantity);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        FreeSpriteTilesByTag(ITEM_TAG);                         //Destroy item icon
        FreeSpritePaletteByTag(ITEM_TAG);                       //Destroy item icon
        FreeSpriteOamMatrix(&gSprites[gTasks[taskId].data[6]]); //Destroy item icon
        DestroySprite(&gSprites[gTasks[taskId].data[6]]);       //Destroy item icon

        PlaySE(MUS_OBTAIN_ITEM);
        AddBagItem(gTasks[taskId].data[5], gTasks[taskId].data[3]);
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        FreeSpriteTilesByTag(ITEM_TAG);                         //Destroy item icon
        FreeSpritePaletteByTag(ITEM_TAG);                       //Destroy item icon
        FreeSpriteOamMatrix(&gSprites[gTasks[taskId].data[6]]); //Destroy item icon
        DestroySprite(&gSprites[gTasks[taskId].data[6]]);       //Destroy item icon

        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

//TMs
static void DebugAction_Give_AllTMs(u8 taskId)
{
    u16 i;
    PlayFanfare(MUS_OBTAIN_TMHM);
    for (i = ITEM_TM01_FOCUS_PUNCH; i <= ITEM_HM08_DIVE; i++)
        if(!CheckBagHasItem(i, 1))
            AddBagItem(i, 1);
    Debug_DestroyMenu(taskId);
    EnableBothScriptContexts();
}

//Pokemon
static void ResetMonDataStruct(struct DebugMonData *sDebugMonData)
{
    sDebugMonData->mon_speciesId    = 1;
    sDebugMonData->mon_level        = 1;
    sDebugMonData->isShiny          = 0;
    sDebugMonData->mon_natureId     = 0;
    sDebugMonData->mon_abilityNum   = 0;
    sDebugMonData->mon_iv_hp        = 0;
    sDebugMonData->mon_iv_atk       = 0;
    sDebugMonData->mon_iv_def       = 0;
    sDebugMonData->mon_iv_speed     = 0;
    sDebugMonData->mon_iv_satk      = 0;
    sDebugMonData->mon_iv_sdef      = 0;
}

//Egg
static void DebugAction_Give_Egg(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial ID
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringCopy(gStringVar1, gSpeciesNames[1]);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    //Set task data
    gTasks[taskId].func = DebugAction_Give_Egg_SelectId;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 1;            //Current ID
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[5] = 0;            //Complex?
    FreeMonIconPalettes();                 //Free space for new pallete
    LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
    #ifndef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
    #endif
    #ifdef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
    #endif
    gSprites[gTasks[taskId].data[6]].oam.priority = 0; //Mon Icon ID
}
static void DebugAction_Give_Egg_SelectId(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > SPECIES_CELEBI && gTasks[taskId].data[3] < SPECIES_TREECKO)
                gTasks[taskId].data[3] = SPECIES_TREECKO;
            if(gTasks[taskId].data[3] >= NUM_SPECIES - 1)
                gTasks[taskId].data[3] = NUM_SPECIES - 2;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < SPECIES_TREECKO && gTasks[taskId].data[3] > SPECIES_CELEBI)
                gTasks[taskId].data[3] = SPECIES_CELEBI;
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < DEBUG_NUMBER_DIGITS_ITEMS-1)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringCopy(gStringVar1, gSpeciesNames[gTasks[taskId].data[3]]); //CopyItemName(gTasks[taskId].data[3], gStringVar1);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 4);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]);
        FreeMonIconPalettes(); //Free space for new pallete
        LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
        #ifndef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
        #endif
        #ifdef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
        #endif
        gSprites[gTasks[taskId].data[6]].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->mon_speciesId = gTasks[taskId].data[3]; //Species ID
        gTasks[taskId].data[3] = 1;
        gTasks[taskId].data[4] = 0;

        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        if (gTasks[taskId].data[5] == 0)
        {
            PlaySE(MUS_LEVEL_UP);
            ScriptGiveEgg(sDebugMonData->mon_speciesId);
            Free(sDebugMonData); //Frees EWRAM of MonData Struct
            DebugAction_DestroyExtraWindow(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_PokemonSimple(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial ID
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringCopy(gStringVar1, gSpeciesNames[1]);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

    //Set task data
    gTasks[taskId].func = DebugAction_Give_Pokemon_SelectId;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 1;            //Current ID
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[5] = 0;            //Complex?
    FreeMonIconPalettes();                 //Free space for new pallete
    LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
    #ifndef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
    #endif
    #ifdef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
    #endif
    gSprites[gTasks[taskId].data[6]].oam.priority = 0; //Mon Icon ID
}
static void DebugAction_Give_PokemonComplex(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, 3);

    //Display initial ID
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, 4);
    StringCopy(gStringVar1, gSpeciesNames[1]);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
    AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);


    gTasks[taskId].func = DebugAction_Give_Pokemon_SelectId;
    gTasks[taskId].data[2] = windowId;
    gTasks[taskId].data[3] = 1;            //Current ID
    gTasks[taskId].data[4] = 0;            //Digit Selected
    gTasks[taskId].data[5] = 1;            //Complex?
    FreeMonIconPalettes();                 //Free space for new palletes
    LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
    #ifndef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
    #endif
    #ifdef POKEMON_EXPANSION
        gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
    #endif
    gSprites[gTasks[taskId].data[6]].oam.priority = 0; //Mon Icon ID
    gTasks[taskId].data[7] = 0;             //iterator
}

static void DebugAction_Give_Pokemon_SelectId(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > SPECIES_CELEBI && gTasks[taskId].data[3] < SPECIES_TREECKO)
                gTasks[taskId].data[3] = SPECIES_TREECKO;
            if(gTasks[taskId].data[3] >= NUM_SPECIES - 1)
                gTasks[taskId].data[3] = NUM_SPECIES - 2;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < SPECIES_TREECKO && gTasks[taskId].data[3] > SPECIES_CELEBI)
                gTasks[taskId].data[3] = SPECIES_CELEBI;
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < DEBUG_NUMBER_DIGITS_ITEMS-1)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringCopy(gStringVar1, gSpeciesNames[gTasks[taskId].data[3]]); //CopyItemName(gTasks[taskId].data[3], gStringVar1);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 4);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]);
        FreeMonIconPalettes(); //Free space for new pallete
        LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
        #ifndef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
        #endif
        #ifdef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
        #endif
        gSprites[gTasks[taskId].data[6]].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->mon_speciesId = gTasks[taskId].data[3]; //Species ID
        gTasks[taskId].data[3] = 1;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonLevel);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectLevel;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_SelectLevel(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > 100)
                gTasks[taskId].data[3] = 100;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonLevel);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        if (gTasks[taskId].data[5] == 0)
        {
            PlaySE(MUS_LEVEL_UP);
            ScriptGiveMon(sDebugMonData->mon_speciesId, gTasks[taskId].data[3], ITEM_NONE, 0,0,0);
            Free(sDebugMonData); //Frees EWRAM of MonData Struct
            DebugAction_DestroyExtraWindow(taskId);
        }
        else
        {
            sDebugMonData->mon_level = gTasks[taskId].data[3]; //Level
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;

            ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 0);
            StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
            StringCopyPadded(gStringVar2, gDebugText_FlagUnset, CHAR_SPACE, 15);
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonShiny);
            AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectShiny;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
    }
}
//If complex
static void DebugAction_Give_Pokemon_SelectShiny(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }

        if(gTasks[taskId].data[3] == 1)
            StringCopyPadded(gStringVar2, gDebugText_FlagSet, CHAR_SPACE, 15);
        else
            StringCopyPadded(gStringVar2, gDebugText_FlagUnset, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 0);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonShiny);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->isShiny = gTasks[taskId].data[3]; //isShiny
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar1, gNatureNamePointers[0]);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonNature);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectNature;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_SelectNature(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > NUM_NATURES-1)
                gTasks[taskId].data[3] = NUM_NATURES-1;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar1, gNatureNamePointers[gTasks[taskId].data[3]]);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonNature);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        u8 abilityId;
        sDebugMonData->mon_natureId = gTasks[taskId].data[3]; //NatureId
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        abilityId = GetAbilityBySpecies(sDebugMonData->mon_speciesId, 0);
        StringCopy(gStringVar1, gAbilityNames[abilityId]);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonAbility);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectAbility;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_SelectAbility(u8 taskId)
{
    u8 abilityId;
    u8 abilityCount = 0;
    if (gBaseStats[sDebugMonData->mon_speciesId].abilities[1] != ABILITY_NONE)
        abilityCount++;
    #ifdef POKEMON_EXPANSION
        if (gBaseStats[sDebugMonData->mon_speciesId].abilities[2] != ABILITY_NONE)
            abilityCount++;
    #endif
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > abilityCount)
                gTasks[taskId].data[3] = abilityCount;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }

        abilityId = GetAbilityBySpecies(sDebugMonData->mon_speciesId, gTasks[taskId].data[3]);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar1, gAbilityNames[abilityId]);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonAbility);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->mon_abilityNum = gTasks[taskId].data[3]; //AbilityNum
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_0);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectIVs;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_SelectIVs(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > 31)
                gTasks[taskId].data[3] = 31;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        switch (gTasks[taskId].data[7])
        {
        case 0:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_0);
            break;
        case 1:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_1);
            break;
        case 2:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_2);
            break;
        case 3:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_3);
            break;
        case 4:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_4);
            break;
        case 5:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_5);
            break;
        }
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    //If A or B button
    if (JOY_NEW(A_BUTTON))
    {
        switch (gTasks[taskId].data[7])
        {
        case 0:
            sDebugMonData->mon_iv_hp = gTasks[taskId].data[3];
            break;
        case 1:
            sDebugMonData->mon_iv_atk = gTasks[taskId].data[3];
            break;
        case 2:
            sDebugMonData->mon_iv_def = gTasks[taskId].data[3];
            break;
        case 3:
            sDebugMonData->mon_iv_speed = gTasks[taskId].data[3];
            break;
        case 4:
            sDebugMonData->mon_iv_satk = gTasks[taskId].data[3];
            break;
        case 5:
            sDebugMonData->mon_iv_sdef = gTasks[taskId].data[3];
            break;
        }

        //Check if all IVs set
        if (gTasks[taskId].data[7] != 5)
        {
            gTasks[taskId].data[7] += 1;
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;

            StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
            ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
            StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
            switch (gTasks[taskId].data[7])
            {
            case 0:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_0);
                break;
            case 1:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_1);
                break;
            case 2:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_2);
                break;
            case 3:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_3);
                break;
            case 4:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_4);
                break;
            case 5:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonIV_5);
                break;
            }
            AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectIVs;
        }
        else
        {
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;
            gTasks[taskId].data[7] = 0; //Reset iterator

            StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
            StringCopy(gStringVar1, gMoveNames[gTasks[taskId].data[3]]);
            StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
            ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_0);
            AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

            gTasks[taskId].func = DebugAction_Give_Pokemon_Move;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_Move(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > MOVES_COUNT)
                gTasks[taskId].data[3] = MOVES_COUNT;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 0)
                gTasks[taskId].data[3] = 0;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 3)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringCopy(gStringVar1, gMoveNames[gTasks[taskId].data[3]]);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
        switch (gTasks[taskId].data[7])
        {
        case 0:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_0);
            break;
        case 1:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_1);
            break;
        case 2:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_2);
            break;
        case 3:
            StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_3);
            break;
        }
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        //If MOVE_NONE selected, stop asking for additional moves
        if (gTasks[taskId].data[3] == 0)
            gTasks[taskId].data[7] = 4;

        //Set current value
        switch (gTasks[taskId].data[7])
        {
        case 0:
            sDebugMonData->mon_move_0 = gTasks[taskId].data[3];
            break;
        case 1:
            sDebugMonData->mon_move_1 = gTasks[taskId].data[3];
            break;
        case 2:
            sDebugMonData->mon_move_2 = gTasks[taskId].data[3];
            break;
        case 3:
            sDebugMonData->mon_move_3 = gTasks[taskId].data[3];
            break;
        }

        //If NOT last move or selected MOVE_NONE ask for next move, else make mon
        if (gTasks[taskId].data[7] < 3)
        {
            gTasks[taskId].data[7] += 1;
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;

            StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
            StringCopy(gStringVar1, gMoveNames[gTasks[taskId].data[3]]);
            StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
            ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
            switch (gTasks[taskId].data[7])
            {
            case 0:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_0);
                break;
            case 1:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_1);
                break;
            case 2:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_2);
                break;
            case 3:
                StringExpandPlaceholders(gStringVar4, gDebugText_PokemonMove_3);
                break;
            }
            AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

            gTasks[taskId].func = DebugAction_Give_Pokemon_Move;
        }
        else
        {
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;

            PlaySE(MUS_LEVEL_UP);
            gTasks[taskId].func = DebugAction_Give_Pokemon_ComplexCreateMon;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Give_Pokemon_ComplexCreateMon(u8 taskId) //https://github.com/ghoulslash/pokeemerald/tree/custom-givemon
{
    u16 nationalDexNum;
    int sentToPc;
    struct Pokemon mon;
    u8 i;
    u16 moves[4];
    u8 IVs[6];
    u8 iv_val;
    u16 species     = sDebugMonData->mon_speciesId;
    u8 level        = sDebugMonData->mon_level;
    u8 isShiny      = sDebugMonData->isShiny; //Shiny: no 0, yes 1
    u8 nature       = sDebugMonData->mon_natureId;
    u8 abilityNum   = sDebugMonData->mon_abilityNum;
    moves[0]        = sDebugMonData->mon_move_0;
    moves[1]        = sDebugMonData->mon_move_1;
    moves[2]        = sDebugMonData->mon_move_2;
    moves[3]        = sDebugMonData->mon_move_3;
    IVs[0]          = sDebugMonData->mon_iv_hp;
    IVs[1]          = sDebugMonData->mon_iv_atk;
    IVs[2]          = sDebugMonData->mon_iv_def;
    IVs[3]          = sDebugMonData->mon_iv_speed;
    IVs[4]          = sDebugMonData->mon_iv_satk;
    IVs[5]          = sDebugMonData->mon_iv_sdef;


    //Nature
    if (nature == NUM_NATURES || nature == 0xFF)
        nature = Random() % NUM_NATURES;

    //Shinyness
    if (isShiny == 1)
    {
        u32 personality;
        u32 otid = gSaveBlock2Ptr->playerTrainerId[0]
            | (gSaveBlock2Ptr->playerTrainerId[1] << 8)
            | (gSaveBlock2Ptr->playerTrainerId[2] << 16)
            | (gSaveBlock2Ptr->playerTrainerId[3] << 24);

        do
        {
            personality = Random32();
            personality = ((((Random() % 8) ^ (HIHALF(otid) ^ LOHALF(otid))) ^ LOHALF(personality)) << 16) | LOHALF(personality);
        } while (nature != GetNatureFromPersonality(personality));

        CreateMon(&mon, species, level, 32, 1, personality, OT_ID_PRESET, otid);
    }
    else
        CreateMonWithNature(&mon, species, level, 32, nature);

    //EVs/IVs
    for (i = 0; i < NUM_STATS; i++)
    {
        // ev
        // if (evs[i] != 0xFF && evTotal < 510)
        // {
        //     // only up to 510 evs
        //     if ((evTotal + evs[i]) > 510)
        //         evs[i] = (510 - evTotal);

        //     evTotal += evs[i];
        //     SetMonData(&mon, MON_DATA_HP_EV + i, &evs[i]);
        // }

        // iv
        iv_val = IVs[i];
        if (iv_val != 32 && iv_val != 0xFF)
            SetMonData(&mon, MON_DATA_HP_IV + i, &iv_val);
    }
    CalculateMonStats(&mon);

    //Moves
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (moves[i] == 0 || moves[i] == 0xFF || moves[i] > MOVES_COUNT)
            continue;

        SetMonMoveSlot(&mon, moves[i], i);
    }

    //Ability
    if (abilityNum == 0xFF || GetAbilityBySpecies(species, abilityNum) == 0)
    {
        do {
            abilityNum = Random() % 3;  // includes hidden abilities
        } while (GetAbilityBySpecies(species, abilityNum) == 0);
    }

    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);

    //ball
    // if (ball <= POKEBALL_COUNT)
    //     SetMonData(&mon, MON_DATA_POKEBALL, &ball);

    //item
    // heldItem[0] = item;
    // heldItem[1] = item >> 8;
    // SetMonData(&mon, MON_DATA_HELD_ITEM, heldItem);

    // give player the mon
    //sentToPc = GiveMonToPlayer(&mon);
    SetMonData(&mon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
    SetMonData(&mon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            break;
    }

    if (i >= PARTY_SIZE)
        sentToPc = SendMonToPC(&mon);
    else
    {
        sentToPc = MON_GIVEN_TO_PARTY;
        CopyMon(&gPlayerParty[i], &mon, sizeof(mon));
        gPlayerPartyCount = i + 1;
    }

    //Pokedex entry
    nationalDexNum = SpeciesToNationalPokedexNum(species);
    switch(sentToPc)
    {
    case MON_GIVEN_TO_PARTY:
    case MON_GIVEN_TO_PC:
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_SEEN);
        GetSetPokedexFlag(nationalDexNum, FLAG_SET_CAUGHT);
        break;
    case MON_CANT_GIVE:
        break;
    }

    Free(sDebugMonData); //Frees EWRAM of MonData Struct
    DebugAction_DestroyExtraWindow(taskId); //return sentToPc;
}

static void DebugAction_Give_CHEAT(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_DebugPack);
}

static void DebugAction_AccessPC(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    PlaySE(SE_PC_ON);
    ScriptContext1_SetupScript(EventScript_PC);
}

static void DebugAction_Flags_StartMenuOnOff(u8 taskId)
{
  if(FlagGet(FLAG_SYS_ENABLE_DEBUG_MENU))
  {
      FlagClear(FLAG_SYS_ENABLE_DEBUG_MENU);
      PlaySE(SE_PC_OFF);
  }else{
      FlagSet(FLAG_SYS_ENABLE_DEBUG_MENU);
      PlaySE(SE_PC_LOGIN);
  }
}

static void DebugAction_Util_Inclement_Difficulty_Check(u8 taskId)
{
    switch(gSaveBlock2Ptr->gameDifficulty)
    {
      case 0:
        StringExpandPlaceholders(gStringVar1,gText_SavingNormalMode);
        break;
      case 1:
        StringExpandPlaceholders(gStringVar1,gText_SavingHardMode);
        break;
      case 2:
        StringExpandPlaceholders(gStringVar1,gText_SavingChallengeMode);
        break;
      case 3:
        StringExpandPlaceholders(gStringVar1,gText_SavingInsanityMode);
    }
    switch(gSaveBlock2Ptr->levelCaps)
    {
      case 0:
        StringExpandPlaceholders(gStringVar2,gText_BirchDefaultCaps);
        break;
      case 1:
        StringExpandPlaceholders(gStringVar2,gText_BirchMoreCaps);
        break;
      case 2:
        StringExpandPlaceholders(gStringVar2,gText_BirchStrictCaps);
        break;
    }
    StringExpandPlaceholders(gStringVar4,gDebugText_Util_Difficulty);
    Debug_DestroyMenu(taskId);
    ScriptContext2_Enable();
    ScriptContext1_SetupScript(EventScript_Inclement_Difficulty);
}

static void DebugAction_StartWildBattle(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_StartWildBattle(SPECIES_ABRA, 100, 0);
}

static void DebugAction_SetupWildBattle(u8 taskId)
{
      u8 windowId;

      //Mon data struct
      sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
      ResetMonDataStruct(sDebugMonData);

      //Window initialization
      ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
      RemoveWindow(gTasks[taskId].data[1]);

      HideMapNamePopUpWindow();
      LoadMessageBoxAndBorderGfx();
      windowId = AddWindow(&sDebugNumberDisplayWindowTemplate);
      DrawStdWindowFrame(windowId, FALSE);

      CopyWindowToVram(windowId, 3);

      //Display initial ID
      StringCopy(gStringVar2, gText_DigitIndicator[0]);
      ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, 3);
      StringCopy(gStringVar1, gSpeciesNames[1]);
      StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
      StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
      AddTextPrinterParameterized(windowId, 1, gStringVar4, 1, 1, 0, NULL);

      //Set task data
      gTasks[taskId].func = DebugAction_Wild_Setup_Pokemon_SelectId;
      gTasks[taskId].data[2] = windowId;
      gTasks[taskId].data[3] = 1;            //Current ID
      gTasks[taskId].data[4] = 0;            //Digit Selected
      gTasks[taskId].data[5] = 0;            //Complex?
      FreeMonIconPalettes();                 //Free space for new pallete
      LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
      #ifndef POKEMON_EXPANSION
          gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
      #endif
      #ifdef POKEMON_EXPANSION
          gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
      #endif
      gSprites[gTasks[taskId].data[6]].oam.priority = 0; //Mon Icon ID
}

static void DebugAction_Wild_Setup_Pokemon_SelectId(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > SPECIES_CELEBI && gTasks[taskId].data[3] < SPECIES_TREECKO)
                gTasks[taskId].data[3] = SPECIES_TREECKO;
            if(gTasks[taskId].data[3] >= NUM_SPECIES - 1)
                gTasks[taskId].data[3] = NUM_SPECIES - 2;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < SPECIES_TREECKO && gTasks[taskId].data[3] > SPECIES_CELEBI)
                gTasks[taskId].data[3] = SPECIES_CELEBI;
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < DEBUG_NUMBER_DIGITS_ITEMS-1)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        StringCopy(gStringVar1, gSpeciesNames[gTasks[taskId].data[3]]); //CopyItemName(gTasks[taskId].data[3], gStringVar1);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 4);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonID);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]);
        FreeMonIconPalettes(); //Free space for new pallete
        LoadMonIconPalette(gTasks[taskId].data[3]); //Loads pallete for current mon
        #ifndef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0, TRUE); //Create pokemon sprite
        #endif
        #ifdef POKEMON_EXPANSION
            gTasks[taskId].data[6] = CreateMonIcon(gTasks[taskId].data[3], SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0); //Create pokemon sprite
        #endif
        gSprites[gTasks[taskId].data[6]].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->mon_speciesId = gTasks[taskId].data[3]; //Species ID
        gTasks[taskId].data[3] = 1;
        gTasks[taskId].data[4] = 0;

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonLevel);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);

        gTasks[taskId].func = DebugAction_Wild_Setup_Pokemon_SelectLevel;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
    }
}
static void DebugAction_Wild_Setup_Pokemon_SelectLevel(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_REPEAT(DPAD_UP))
        {
            gTasks[taskId].data[3] += sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] > 100)
                gTasks[taskId].data[3] = 100;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            gTasks[taskId].data[3] -= sPowersOfTen[gTasks[taskId].data[4]];
            if(gTasks[taskId].data[3] < 1)
                gTasks[taskId].data[3] = 1;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if(gTasks[taskId].data[4] > 0)
                gTasks[taskId].data[4] -= 1;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if(gTasks[taskId].data[4] < 2)
                gTasks[taskId].data[4] += 1;
        }

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].data[4]]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[3], STR_CONV_MODE_LEADING_ZEROS, 3);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        StringExpandPlaceholders(gStringVar4, gDebugText_PokemonLevel);
        AddTextPrinterParameterized(gTasks[taskId].data[2], 1, gStringVar4, 1, 1, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
        Debug_StartWildBattle(sDebugMonData->mon_speciesId, gTasks[taskId].data[3], 0);
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData); //Frees EWRAM of MonData Struct
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].data[6]]); //Destroy pokemon sprite
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_EditDayNight_PrintStatus(u8 windowId, s16 timePeriod)
{
    FillWindowPixelBuffer(windowId, 0x11);
    WriteTimeString(gStringVar1,
                    timePeriod / TINT_PERIODS_PER_HOUR,
                    (timePeriod % TINT_PERIODS_PER_HOUR) * MINUTES_PER_TINT_PERIOD,
                    FALSE,
                    TRUE);
    StringExpandPlaceholders(gStringVar4, sText_ClockStatus);
    AddTextPrinterParameterized5(windowId, 2, gStringVar4, 0, 1, 0, NULL, 0, 2);
}

#define tDeltaIndex data[1]

static void DebugAction_EditDayNight(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tDeltaIndex = 0;

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    tWindowId = AddWindow(&sDebugMenu_Window_EditDayNight);
    DrawStdWindowFrame(tWindowId, FALSE);

    CopyWindowToVram(tWindowId, 3);

    SetStandardWindowBorderStyle(tWindowId, FALSE);
    gDNPeriodOverride = (gLocalTime.hours * TINT_PERIODS_PER_HOUR) + (((gLocalTime.minutes / MINUTES_PER_TINT_PERIOD) / 5) * 5) + 1;
    DebugAction_EditDayNight_PrintStatus(tWindowId, gDNPeriodOverride - 1);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = DebugAction_EditDayNight_ProcessInput;
}

static void DebugAction_EditDayNight_ProcessInput(u8 taskId)
{
    const u8 deltas[] = { 60, 30, 10, 5, 2, 1 };

    s16 *data = gTasks[taskId].data;

    if (JOY_REPEAT(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        gDNPeriodOverride += 1;

        if (gDNPeriodOverride > TINT_PERIODS_COUNT)
            gDNPeriodOverride -= TINT_PERIODS_COUNT;

    }

    if (JOY_REPEAT(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        gDNPeriodOverride -= 1;

        if (gDNPeriodOverride < 1)
            gDNPeriodOverride += TINT_PERIODS_COUNT;

    }

    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gDNPeriodOverride += 10;

        if (gDNPeriodOverride > TINT_PERIODS_COUNT)
            gDNPeriodOverride -= TINT_PERIODS_COUNT;

    }

    if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gDNPeriodOverride -= 10;

        if (gDNPeriodOverride < 1)
            gDNPeriodOverride += TINT_PERIODS_COUNT;

    }

    if (JOY_REPEAT(L_BUTTON))
    {
      PlaySE(SE_SELECT);
      gDNPeriodOverride -= 60;

      if (gDNPeriodOverride < 1)
          gDNPeriodOverride += TINT_PERIODS_COUNT;
    }

    if (JOY_REPEAT(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        gDNPeriodOverride += 60;

        if (gDNPeriodOverride > TINT_PERIODS_COUNT)
            gDNPeriodOverride -= TINT_PERIODS_COUNT;
    }

    if (JOY_REPEAT(DPAD_LEFT) || JOY_REPEAT(DPAD_DOWN)|| JOY_REPEAT(DPAD_RIGHT) || JOY_REPEAT(DPAD_UP) || JOY_REPEAT(L_BUTTON) || JOY_REPEAT(R_BUTTON))
    {
      ProcessImmediateTimeEvents();
      DebugAction_EditDayNight_PrintStatus(tWindowId, gDNPeriodOverride - 1);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }

    if (JOY_NEW(B_BUTTON))
    {
        gDNPeriodOverride = -1;
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}

#undef tDeltaIndex

static void DebugAction_EditDayNightPalette_PrintStatus(u8 windowId, u16 tintR, u16 tintG, u16 tintB)
{
    FillWindowPixelBuffer(windowId, 0x11);
    ConvertQ88ToDecimalStringN(gStringVar1, tintR, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertQ88ToDecimalStringN(gStringVar2, tintG, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertQ88ToDecimalStringN(gStringVar3, tintB, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized5(windowId, 1, gStringVar4, 0, 1, 0, NULL, 0, 2);
}

static void DebugAction_InfoControls_PrintStatus(u8 windowInfoId, u8 info)
{
    FillWindowPixelBuffer(windowInfoId, 0x11);
    AddTextPrinterParameterized5(windowInfoId, 1, gStringVar4, 0, 1, 0, NULL, 0, 2);
}


#define TINT_R gDNTintOverride[0]
#define TINT_G gDNTintOverride[1]
#define TINT_B gDNTintOverride[2]

#define tWhichComponent data[1]

static void DebugAction_EditDayNightPalette(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    TINT_R = Q_8_8(1.0);
    TINT_G = Q_8_8(1.0);
    TINT_B = Q_8_8(1.0);

    gPaletteOverrideDisabled = TRUE;

    PlaySE(SE_PC_ON);

    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);

    tWindowId = AddWindow(&sDebugMenu_Window_EditDayNightPalette);
//    tWindowInfoId = AddWindow(&sDebugMenu_Window_InfoControls);
//    CopyWindowToVram(tWindowInfoId, 3);

    SetStandardWindowBorderStyle(tWindowId, FALSE);
//    DebugAction_InfoControls_PrintStatus(tWindowInfoId,0);
    DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
    StringExpandPlaceholders(gStringVar4, sText_RGBValuesGreen);
    DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = DebugAction_EditDayNightPalette_ProcessInput;
}

static void DebugAction_EditDayNightPalette_ProcessInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_NEW(SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        TINT_R = Q_8_8(1.0);
        TINT_G = Q_8_8(1.0);
        TINT_B = Q_8_8(1.0);
        DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
    }

    if (JOY_NEW(L_BUTTON) && tWhichComponent != 0)
    {
        PlaySE(SE_SELECT);
        tWhichComponent--;
        switch(tWhichComponent)
        {
          case 0:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesRed);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
          case 1:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesGreen);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
          case 2:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesBlue);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
        }
    }
    if (JOY_NEW(R_BUTTON) && tWhichComponent != 2)
    {
        PlaySE(SE_SELECT);
        tWhichComponent++;
        switch(tWhichComponent)
        {
          case 0:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesRed);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
          case 1:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesGreen);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
          case 2:
          StringExpandPlaceholders(gStringVar4, sText_RGBValuesBlue);
          DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
          break;
        }
    }

    if (JOY_REPEAT(DPAD_LEFT))
    {
        gDNTintOverride[tWhichComponent] -= Q_8_8(0.005);
    }

    if (JOY_REPEAT(DPAD_DOWN))
    {
        gDNTintOverride[tWhichComponent] -= Q_8_8(0.025);
    }

    if (JOY_REPEAT(DPAD_RIGHT))
    {
        gDNTintOverride[tWhichComponent] += Q_8_8(0.005);
    }

    if (JOY_REPEAT(DPAD_UP))
    {
        gDNTintOverride[tWhichComponent] += Q_8_8(0.025);
    }

    if (JOY_REPEAT(DPAD_LEFT) || JOY_REPEAT(DPAD_DOWN)|| JOY_REPEAT(DPAD_RIGHT) || JOY_REPEAT(DPAD_UP))
    switch(tWhichComponent)
    {
      case 0:
      StringExpandPlaceholders(gStringVar4, sText_RGBValuesRed);
      DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
      break;
      case 1:
      StringExpandPlaceholders(gStringVar4, sText_RGBValuesGreen);
      DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
      break;
      case 2:
      StringExpandPlaceholders(gStringVar4, sText_RGBValuesBlue);
      DebugAction_EditDayNightPalette_PrintStatus(tWindowId, TINT_R, TINT_G, TINT_B);
      break;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }

    if (JOY_NEW(B_BUTTON))
    {
        TINT_R = 0xFFFF;
        TINT_G = 0;
        TINT_B = 0;

        gPaletteOverrideDisabled = FALSE;

        PlaySE(SE_PC_OFF);
        Debug_DestroyMenu(taskId);
        Debug_ShowMainMenu();
    }
}

// Additional functions
/*
static void DebugAction_OpenSubMenu(u8 taskId, struct ListMenuTemplate LMtemplate)
{
    Debug_DestroyMenu(taskId);
    Debug_ShowMenu(DebugTask_HandleMenuInput, LMtemplate);
}
static void DebugTask_HandleMenuInput(u8 taskId, void (*HandleInput)(u8))
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if ((func = HandleInput[input]) != NULL)
            func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMenu(taskId);
        EnableBothScriptContexts();
    }
}
*/

#endif
