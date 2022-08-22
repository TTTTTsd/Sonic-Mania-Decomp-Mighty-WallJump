#include "Player.h"
#include "GameAPI/Game.h"
#include "../ModConfig.h"

ObjectPlayer *Player;
Hitbox Player_FallbackHitbox = { -10, -20, 10, 20 };
// Collision
Hitbox *Player_GetHitbox(EntityPlayer *player)
{
    Hitbox *playerHitbox = RSDK.GetHitbox(&player->animator, 0);
    return playerHitbox ? playerHitbox : &Player_FallbackHitbox;
}

void Player_State_Air_Hook(void)
{
    RSDK_THIS(Player);
    if ((ControllerInfo[self->controllerID].keyA.press && config.ButtonOption || self->jumpPress && !config.ButtonOption) && self->timer >= 1 && self->animator.animationID == ANI_JUMP) {
        StateMachine_Run(self->stateAbility);
    }
}

void Player_JumpAbility_Mighty_Hook(void)
{
    RSDK_THIS(Player);
    Hitbox *hitbox = Player_GetHitbox(self);
    int32 highY = 0, lowY = 0;
    if (self->isChibi) {
        highY = -0x10000;
        lowY  = 0x30000;
    }
    else {
        highY = -0x20000;
        lowY  = 0xB0000;
    }

    bool32 collidedHigh = false, collidedLow = false;
    int32 highPos = 0, lowPos = 0;
    if (self->right && self->animator.animationID == ANI_HAMMERDROP) {

        collidedHigh = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, hitbox->right << 16, highY, 8);
        highPos      = self->position.x;

        collidedLow = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, hitbox->right << 16, lowY, 8);
        lowPos      = self->position.x;
    }
    if (self->left && self->animator.animationID == ANI_HAMMERDROP) {

        collidedHigh = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, hitbox->left << 16, highY, 8);
        highPos      = self->position.x;

        collidedLow = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, hitbox->left << 16, lowY, 8);
        lowPos      = self->position.x;
    }
    if ((ControllerInfo[self->controllerID].keyA.press && config.ButtonOption || self->jumpPress && !config.ButtonOption)
        && (collidedHigh || collidedLow) && (self->left || self->right))
        {
            RSDK.StopSfx(Player->sfxRelease);
            RSDK.StopSfx(Player->sfxMightyDrill);
            RSDK.PlaySfx(Player->sfxGrab, false, 255);
            self->velocity.x = 0;
            self->rotation   = 0;
            self->velocity.y = 0;
            self->timer      = 0;
            
            if (self->isChibi) {
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_SKID, &self->animator, false, 0);
            }
            else {
                self->direction = self->direction != FLIP_X;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_STICK, &self->animator, false, 0);
            }
            self->state      = Player_WallStick_Mighty;
        }
    }

void Player_WallStick_Mighty(void)
{
    RSDK_THIS(Player);
    Hitbox *hitbox = Player_GetHitbox(self);
    int32 storeX   = self->position.x;

    int32 highY = 0;
    int32 lowY  = 0;

    int32 roofX = 0;
    int32 roofY = 0;
    if (self->isChibi) {
        highY = -0x10000;
        lowY  = 0x30000;
    }
    else {
        highY = -0x20000;
        lowY  = 0xB0000;
    }

    bool32 collidedHigh = false, collidedLow = false;
        if (!self->direction && !self->isChibi || self->direction && self->isChibi) {
            collidedHigh = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, hitbox->left << 16, highY, 8);
            int32 highX  = self->position.x;

            self->position.x = storeX;
            collidedLow      = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, hitbox->left << 16, lowY, 8);
        }
        else {
            collidedHigh = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, hitbox->right << 16, highY, 8);
            int32 highY  = self->position.x;

            self->position.x = storeX;
            collidedLow      = RSDK.ObjectTileGrip(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, hitbox->right << 16, lowY, 8);
        }
    self->velocity.y = 0x3000;
    if (self->onGround) {
        self->state = Player_State_Ground;
           }
    if (self->timer >= 32 || !collidedHigh) {
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
        self->applyJumpCap     = true;
        self->jumpAbilityState = 0;
        self->direction        = self->direction != FLIP_NONE;
        self->state            = Player_State_Air;
    }
    if (!self->jumpHold) {         /// HERE WE GO
        if (self->direction == FLIP_NONE && !self->isChibi
            || self->direction == FLIP_X && self->isChibi) {
            if (self->down & !self->left & !self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.y       = 0x80000;
                self->velocity.x       = 0;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->down && self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = 0x70000;
                self->velocity.y       = 0x50000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (!self->left && !self->down) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = 0x70000;
                self->velocity.y       = -0x5000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->left || self->up && self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = 0x70000;
                self->velocity.y       = -0x80000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->up && !self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->timer            = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.y       = -0x80000;
                self->velocity.x       = 0;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
        }
        if (self->direction == FLIP_X && !self->isChibi || self->direction == FLIP_NONE && self->isChibi) {
            if (self->down & !self->left & !self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.y       = 0x80000;
                self->velocity.x       = 0;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->down && self->left) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = -0x70000;
                self->velocity.y       = 0x50000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (!self->right && !self->down) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = -0x70000;
                self->velocity.y       = -0x5000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->right || self->up && self->left) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = -0x70000;
                self->velocity.y       = -0x80000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (self->up && !self->left) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->timer            = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.y       = -0x80000;
                self->velocity.x       = 0;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
        }
    } 
    self->timer++;
    }
