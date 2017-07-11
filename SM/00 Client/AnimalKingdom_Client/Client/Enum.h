#pragma once

#ifndef Enum_h__
#define Enum_h__

enum eCollision_Group{ COL_STATIC = 0, COL_DYNAMIC = 1, COL_PLAYER1 = 2, COL_PLAYER2 = 4, COL_PLAYER3 = 8, COL_PLAYER4 = 16, COL_END };
using COL_GROUP = eCollision_Group;

enum eRendergroup { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_OUTCAVE, RENDER_LIGHT, RENDER_BLEND, RENDER_INCAVE, RENDER_ALPHA, RENDER_UI, RENDER_DEBUG, RENDER_END };
using RENDERGROUP = eRendergroup;

#endif // Enum_h__