#include "Player.h"
#include "GameAPI/Game.h"

ObjectPlayer *Player;
Hitbox Player_FallbackHitbox = { -10, -20, 10, 20 };
// Collision
Hitbox *Player_GetHitbox(EntityPlayer *player)
{
    Hitbox *playerHitbox = RSDK.GetHitbox(&player->animator, 0);
    return playerHitbox ? playerHitbox : &Player_FallbackHitbox;
}

void Player_JumpAbility_Mighty_Hook(void)
{
    RSDK_THIS(Player);
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
    if (self->right) {

        collidedHigh = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, 0xC0000, highY, true);
        highPos      = self->position.x;

        collidedLow      = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, 0xC0000, lowY, true);
        lowPos           = self->position.x;
    }
    if (self->left) {

        collidedHigh = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, -0xC0000, highY, true);
        highPos      = self->position.x;

        collidedLow      = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, -0xC0000, lowY, true);
        lowPos           = self->position.x;
    }
    if (self->jumpPress && (collidedHigh || collidedLow) && (self->left||self->right))
        {
        if (highPos == lowPos) {
            RSDK.StopSfx(Player->sfxRelease);
            RSDK.StopSfx(Player->sfxMightyDrill);
            RSDK.PlaySfx(Player->sfxGrab, false, 255);
            self->velocity.x = 0;
            self->velocity.y = 0;
            self->timer      = 0;
            self->direction  = self->direction != FLIP_X;
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_STICK, &self->animator, false, 0);
            self->state      = Player_WallStick_Mighty;
        }
    }
}

void Player_WallStick_Mighty(void)
{
    RSDK_THIS(Player);
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
    if (self->direction == FLIP_X) {

        collidedHigh = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, 0xC0000, highY, true);
        highPos      = self->position.x;

        collidedLow = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_LWALL, self->collisionPlane, 0xC0000, lowY, true);
        lowPos      = self->position.x;
    }
    if (self->direction == FLIP_NONE) {

        collidedHigh = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, -0xC0000, highY, true);
        highPos      = self->position.x;

        collidedLow = RSDK.ObjectTileCollision(self, self->collisionLayers, CMODE_RWALL, self->collisionPlane, -0xC0000, lowY, true);
        lowPos      = self->position.x;
    }
    self->velocity.y = 0x3000;
    if (self->onGround) {
        self->state = Player_State_Ground;
           }
    if (self->timer >= 32 || !collidedHigh) {
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
        self->applyJumpCap     = false;
        self->jumpAbilityState = 1;
        self->state            = Player_State_Air;
    }
    if (!self->jumpHold && self->up) {
        RSDK.PlaySfx(Player->sfxJump, false, 255);
        self->applyJumpCap     = false;
        self->jumpAbilityState = 1;
        self->velocity.y       = -0x80000;
        self->direction        = self->direction != FLIP_X;
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
        self->state = Player_State_Air;
    }
    if (!self->jumpHold && self->down) {
        RSDK.PlaySfx(Player->sfxJump, false, 255);
        self->applyJumpCap     = false;
        self->jumpAbilityState = 1;
        self->velocity.y       = 0x80000;
        self->direction        = self->direction != FLIP_X;
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
        self->state = Player_State_Air;
    }
    if (self->direction == FLIP_NONE && !self->down && !self->up) {
        if (!self->jumpHold && self->left) {
            RSDK.PlaySfx(Player->sfxJump, false, 255);
            self->applyJumpCap     = false;
            self->jumpAbilityState = 1;
            self->direction        = self->direction != FLIP_NONE;
            self->velocity.x       = 0x80000;
            self->velocity.y       = -0x50000;
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
            self->state = Player_State_Air;
        }
        if (!self->jumpHold && !self->left) {
            RSDK.PlaySfx(Player->sfxJump, false, 255);
            self->applyJumpCap     = false;
            self->jumpAbilityState = 1;
            self->direction        = self->direction != FLIP_NONE;
            self->velocity.x       = 0x80000;
            self->velocity.y       = -0x15000;
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
            self->state = Player_State_Air;
        }
    }
 if (self->direction == FLIP_X && !self->down && !self->up) {
            if (!self->jumpHold && self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = -0x80000;
                self->velocity.y       = -0x50000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
            if (!self->jumpHold && !self->right) {
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                self->applyJumpCap     = false;
                self->jumpAbilityState = 1;
                self->direction        = self->direction != FLIP_NONE;
                self->velocity.x       = -0x80000;
                self->velocity.y       = -0x15000;
                RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
                self->state = Player_State_Air;
            }
        }
    self->timer++;
    }
