#include "../GameAPI/C/GameAPI/Game.h"
#include "Objects/Player.h"

#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);
#endif

void InitModAPI(void) {
Player_State_Ground = Mod.GetPublicFunction(NULL, "Player_State_Ground");
Player_State_Air        = Mod.GetPublicFunction(NULL, "Player_State_Air");
Player_JumpAbility_Mighty = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Mighty");
Player_HandleAirMovement = Mod.GetPublicFunction(NULL, "Player_HandleAirMovement");
Player_HandleAirFriction  = Mod.GetPublicFunction(NULL, "Player_HandleAirFriction");
Mod.RegisterStateHook(Player_JumpAbility_Mighty, Player_JumpAbility_Mighty_Hook, false);
Mod.RegisterStateHook(Player_State_Air, Player_State_Air_Hook, false);
MOD_REGISTER_OBJECT_HOOK(Player);
}

#if RETRO_USE_MOD_LOADER
#define ADD_PUBLIC_FUNC(func) Mod.AddPublicFunction(#func, (void *)(func))

void InitModAPI(void);

bool32 LinkModLogic(EngineInfo *info, const char *id)
{
#if MANIA_USE_PLUS
    LinkGameLogicDLL(info);
#else
    LinkGameLogicDLL(*info);
#endif

    globals = Mod.GetGlobals();

    modID = id;

    InitModAPI();

    return true;
}
#endif