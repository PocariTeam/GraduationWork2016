#pragma once
#ifndef Define_h__
#define Define_h__

#define MIN_STR 64

#define WIN_WIDTH	1024
#define WIN_HEIGHT  768

#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

/* VS */
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX		0x01
#define VS_SLOT_LIGHT				0x02
#define VS_SLOT_PARTICLE			0x03

#define MAX_THREAD_CNT				2
#define MAX_PLAYER_CNT				4

#endif // Define_h__