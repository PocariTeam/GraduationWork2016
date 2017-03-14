#pragma once

#ifndef Enum_h__
#define Enum_h__

enum eCharacter_Type { CHARACTER_CHM, CHARACTER_MON, CHARACTER_END };
using CHARACTER = eCharacter_Type;

enum eState_Animation { STATE_IDLE, STATE_ATT1, STATE_ATT2, STATE_RUN, STATE_JUMP, STATE_DEFEND, STATE_BEATEN1, STATE_BEATEN2, /*STATE_SPECIAL, */STATE_DOWN, /*STATE_STANDUP, STATE_DIE, */STATE_END };
using STATE = eState_Animation;

enum eCollision_Group{ COL_STATIC, COL_DYNAMIC, COL_MINE, COL_OTHERS, COL_END };
using COL_GROUP = eCollision_Group;

enum eRendergroup { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_LIGHT, RENDER_BLEND, RENDER_ALPHA, RENDER_UI, RENDER_DEBUG, RENDER_END };
using RENDERGROUP = eRendergroup;

#endif // Enum_h__