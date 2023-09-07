#ifndef NDEBUG

#include "global.h"
#include "list_menu.h"
#include "main.h"
#include "map_name_popup.h"
#include "menu.h"
#include "script.h"
#include "sound.h"
#include "strings.h"
#include "task.h"
#include "constants/songs.h"
#include "new_menu_helpers.h"
#include "region_map.h"
#include "event_object_movement.h"
#include "event_data.h"
#include "script_pokemon_util.h"

#define DEBUG_MAIN_MENU_HEIGHT 7
#define DEBUG_MAIN_MENU_WIDTH 12

void Debug_ShowMainMenu(void);
static void Debug_DestroyMainMenu(u8);
static void DebugAction_Cancel(u8);
static void DebugAction_Fly(u8);
static void DebugAction_Heal(u8);
static void DebugAction_ToggleCollision(u8);
static void DebugTask_HandleMainMenuInput(u8);

static const u8 gDebugText_Heal[] = _("Heal Party");
static const u8 gDebugText_Fly[] = _("Fly");
static const u8 gDebugText_ToggleCollision[] = _("Collision ON/OFF");
static const u8 gDebugText_Cancel[] = _("Cancel");

enum {
    DEBUG_MENU_ITEM_HEAL,
    DEBUG_MENU_ITEM_FLY,
    DEBUG_MENU_ITEM_TOGGLECOLLISION,
    DEBUG_MENU_ITEM_CANCEL,
};

static const struct ListMenuItem sDebugMenuItems[] =
{
    [DEBUG_MENU_ITEM_HEAL] = {gDebugText_Heal, DEBUG_MENU_ITEM_HEAL},
    [DEBUG_MENU_ITEM_FLY] = {gDebugText_Fly, DEBUG_MENU_ITEM_FLY},
    [DEBUG_MENU_ITEM_TOGGLECOLLISION] = {gDebugText_ToggleCollision, DEBUG_MENU_ITEM_TOGGLECOLLISION},
    [DEBUG_MENU_ITEM_CANCEL] = {gDebugText_Cancel, DEBUG_MENU_ITEM_CANCEL}
};

static void (*const sDebugMenuActions[])(u8) =
{
    [DEBUG_MENU_ITEM_HEAL] = DebugAction_Heal,
    [DEBUG_MENU_ITEM_FLY] = DebugAction_Fly,
    [DEBUG_MENU_ITEM_TOGGLECOLLISION] = DebugAction_ToggleCollision,
    [DEBUG_MENU_ITEM_CANCEL] = DebugAction_Cancel
};

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

static const struct ListMenuTemplate sDebugMenuListTemplate =
{
    .items = sDebugMenuItems,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .totalItems = ARRAY_COUNT(sDebugMenuItems),
    .maxShowed = DEBUG_MAIN_MENU_HEIGHT,
    .windowId = 0,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = 1,
    .cursorKind = 0
};

void Debug_ShowMainMenu(void) {
    struct ListMenuTemplate menuTemplate;
    u8 windowId;
    u8 menuTaskId;
    u8 inputTaskId;

    // create window
    DismissMapNamePopup();
    LoadStdWindowFrameGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    // create list menu
    menuTemplate = sDebugMenuListTemplate;
    menuTemplate.windowId = windowId;
    menuTaskId = ListMenuInit(&menuTemplate, 0, 0);

    // draw everything
    CopyWindowToVram(windowId, 3);

    // create input handler task
    inputTaskId = CreateTask(DebugTask_HandleMainMenuInput, 3);
    gTasks[inputTaskId].data[0] = menuTaskId;
    gTasks[inputTaskId].data[1] = windowId;
}

static void Debug_DestroyMainMenu(u8 taskId)
{
    DestroyListMenuTask(gTasks[taskId].data[0], NULL, NULL);
    ClearStdWindowAndFrame(gTasks[taskId].data[1], TRUE);
    RemoveWindow(gTasks[taskId].data[1]);
    DestroyTask(taskId);
    UnfreezeObjectEvents();
    // ScriptContext_Enable();
}

static void DebugTask_HandleMainMenuInput(u8 taskId)
{
    void (*func)(u8);
    u32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    if (gMain.newKeys & A_BUTTON)
    {
        PlaySE(SE_SELECT);
        if ((func = sDebugMenuActions[input]) != NULL)
            func(taskId);
    }
    else if (gMain.newKeys & B_BUTTON)
    {
        PlaySE(SE_SELECT);
        Debug_DestroyMainMenu(taskId);
        ScriptContext_Enable();
    }
}

static void DebugAction_Cancel(u8 taskId)
{
    Debug_DestroyMainMenu(taskId);
    ScriptContext_Enable();
}

static void DebugAction_Heal(u8 taskId)
{
    PlaySE(SE_USE_ITEM);
    HealPlayerParty();
    ScriptContext_Enable();
    Debug_DestroyMainMenu(taskId);
}

static void DebugAction_Fly(u8 taskId)
{
    FlagSet(FLAG_WORLD_MAP_PALLET_TOWN);                                 
    FlagSet(FLAG_WORLD_MAP_VIRIDIAN_CITY);                               
    FlagSet(FLAG_WORLD_MAP_PEWTER_CITY);                                 
    FlagSet(FLAG_WORLD_MAP_CERULEAN_CITY);                               
    FlagSet(FLAG_WORLD_MAP_LAVENDER_TOWN);                               
    FlagSet(FLAG_WORLD_MAP_VERMILION_CITY);                              
    FlagSet(FLAG_WORLD_MAP_CELADON_CITY);                                
    FlagSet(FLAG_WORLD_MAP_FUCHSIA_CITY);                                
    FlagSet(FLAG_WORLD_MAP_CINNABAR_ISLAND);                             
    FlagSet(FLAG_WORLD_MAP_INDIGO_PLATEAU_EXTERIOR);                     
    FlagSet(FLAG_WORLD_MAP_SAFFRON_CITY);                                
    FlagSet(FLAG_WORLD_MAP_ONE_ISLAND);                                  
    FlagSet(FLAG_WORLD_MAP_TWO_ISLAND);                                  
    FlagSet(FLAG_WORLD_MAP_THREE_ISLAND);                                
    FlagSet(FLAG_WORLD_MAP_FOUR_ISLAND);                                 
    FlagSet(FLAG_WORLD_MAP_FIVE_ISLAND);                                 
    FlagSet(FLAG_WORLD_MAP_SEVEN_ISLAND);                                
    FlagSet(FLAG_WORLD_MAP_SIX_ISLAND);                                  
    FlagSet(FLAG_WORLD_MAP_ROUTE4_POKEMON_CENTER_1F);                    
    FlagSet(FLAG_WORLD_MAP_ROUTE10_POKEMON_CENTER_1F);                   
    FlagSet(FLAG_WORLD_MAP_VIRIDIAN_FOREST);                             
    FlagSet(FLAG_WORLD_MAP_MT_MOON_1F);                                  
    FlagSet(FLAG_WORLD_MAP_SSANNE_EXTERIOR);                             
    FlagSet(FLAG_WORLD_MAP_UNDERGROUND_PATH_NORTH_SOUTH_TUNNEL);         
    FlagSet(FLAG_WORLD_MAP_UNDERGROUND_PATH_EAST_WEST_TUNNEL);           
    FlagSet(FLAG_WORLD_MAP_DIGLETTS_CAVE_B1F);                           
    FlagSet(FLAG_WORLD_MAP_VICTORY_ROAD_1F);                             
    FlagSet(FLAG_WORLD_MAP_ROCKET_HIDEOUT_B1F);                          
    FlagSet(FLAG_WORLD_MAP_SILPH_CO_1F);                                 
    FlagSet(FLAG_WORLD_MAP_POKEMON_MANSION_1F);                          
    FlagSet(FLAG_WORLD_MAP_SAFARI_ZONE_CENTER);                          
    FlagSet(FLAG_WORLD_MAP_POKEMON_LEAGUE_LORELEIS_ROOM);                
    FlagSet(FLAG_WORLD_MAP_ROCK_TUNNEL_1F);                              
    FlagSet(FLAG_WORLD_MAP_SEAFOAM_ISLANDS_1F);                          
    FlagSet(FLAG_WORLD_MAP_POKEMON_TOWER_1F);                            
    FlagSet(FLAG_WORLD_MAP_CERULEAN_CAVE_1F);                            
    FlagSet(FLAG_WORLD_MAP_POWER_PLANT);                                 
    FlagSet(FLAG_WORLD_MAP_NAVEL_ROCK_EXTERIOR);                         
    FlagSet(FLAG_WORLD_MAP_MT_EMBER_EXTERIOR);                           
    FlagSet(FLAG_WORLD_MAP_THREE_ISLAND_BERRY_FOREST);                   
    FlagSet(FLAG_WORLD_MAP_FOUR_ISLAND_ICEFALL_CAVE_ENTRANCE);           
    FlagSet(FLAG_WORLD_MAP_FIVE_ISLAND_ROCKET_WAREHOUSE);                
    FlagSet(FLAG_WORLD_MAP_TRAINER_TOWER_LOBBY);                         
    FlagSet(FLAG_WORLD_MAP_SIX_ISLAND_DOTTED_HOLE_1F);                   
    FlagSet(FLAG_WORLD_MAP_FIVE_ISLAND_LOST_CAVE_ENTRANCE);              
    FlagSet(FLAG_WORLD_MAP_SIX_ISLAND_PATTERN_BUSH);                     
    FlagSet(FLAG_WORLD_MAP_SIX_ISLAND_ALTERING_CAVE);                    
    FlagSet(FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER);    
    FlagSet(FLAG_WORLD_MAP_THREE_ISLAND_DUNSPARCE_TUNNEL);               
    FlagSet(FLAG_WORLD_MAP_SEVEN_ISLAND_SEVAULT_CANYON_TANOBY_KEY);      
    FlagSet(FLAG_WORLD_MAP_BIRTH_ISLAND_EXTERIOR);                       
    FlagSet(FLAG_WORLD_MAP_CERULEAN_CITY);
    Debug_DestroyMainMenu(taskId);
    SetMainCallback2(CB2_OpenFlyMap);
}

static void DebugAction_ToggleCollision(u8 taskId)
{
    if (FlagGet(FLAG_SYS_NO_COLLISION)) {
        PlaySE(SE_PC_OFF);
        FlagClear(FLAG_SYS_NO_COLLISION);
    } else {
        PlaySE(SE_PC_LOGIN);
        FlagSet(FLAG_SYS_NO_COLLISION);
    }
        
}

#endif