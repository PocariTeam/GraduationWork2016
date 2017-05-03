#pragma once

#ifndef Enum_h__
#define Enum_h__

enum eCollision_Group{ COL_STATIC = 1, COL_DYNAMIC = 2, COL_PLAYER1 = 4, COL_PLAYER2 = 8, COL_PLAYER3 = 16, COL_PLAYER4 = 32, COL_END };
using COL_GROUP = eCollision_Group;

enum eRendergroup { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_LIGHT, RENDER_BLEND, RENDER_ALPHA, RENDER_UI, RENDER_DEBUG, RENDER_END };
using RENDERGROUP = eRendergroup;

#endif // Enum_h__