#define TRAIL_LENGTH	31

struct PARTICLE_INFO
{
	matrix mtxWorld;
	float4 vOption;
	float4 vVelocity;
};

cbuffer cbParticle : register( b0 )
{
	PARTICLE_INFO tParticle[ TRAIL_LENGTH ];
};

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	matrix g_mtxOrtho;
	matrix g_mtxViewInv;
	matrix g_mtxProjInv;
	float4 g_vCameraPos;
};

struct VS_OUT
{
	float4  vPos : POSITION;
	uint	iVertexID : VERTEXID;
};

struct GS_OUT
{
	float4 vPos	 : SV_POSITION;
	float  fAge  : AGE;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;
	
	Out.vPos = float4( tParticle[ iVertexNum ].vVelocity.x, tParticle[ iVertexNum ].vVelocity.y, tParticle[ iVertexNum ].vVelocity.z, 1.f );
	Out.iVertexID = iVertexNum;

	/*if( iVertexNum == 0 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 1 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 2 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 3 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 4 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 5 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	*/
	/*matrix mtxBill = g_mtxView;
	mtxBill._41 = 0.f;
	mtxBill._42 = 0.f;
	mtxBill._43 = 0.f;

	Out.vPos = mul( mtxBill, Out.vPos );*/

	/*matrix	mtxWV, mtxWVP;
	mtxWV = mul( In.mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	Out.vPos = mul( Out.vPos, mtxWVP );
	Out.fAge = In.vOption.y;*/

	return Out;
}

[maxvertexcount( 4 )]
void GS( point VS_OUT In[ 1 ], inout TriangleStream<GS_OUT> TriStream )
{
	uint iVertexNum = In[ 0 ].iVertexID;

	if( iVertexNum == TRAIL_LENGTH ) return;

	float4 pVertex[ 4 ];
	pVertex[ 0 ] = float4( In[ 0 ].vPos.x - tParticle[ iVertexNum ].vOption.z * 0.5f, In[ 0 ].vPos.y, In[ 0 ].vPos.z, 1.f );
	pVertex[ 1 ] = float4( In[ 0 ].vPos.x + tParticle[ iVertexNum ].vOption.z * 0.5f, In[ 0 ].vPos.y, In[ 0 ].vPos.z, 1.f );
	pVertex[ 2 ] = float4( In[ 0 ].vPos.x - tParticle[ iVertexNum ].vOption.z * 0.5f, In[ 0 ].vPos.y, In[ 0 ].vPos.z, 1.f );
	pVertex[ 3 ] = float4( In[ 0 ].vPos.x + tParticle[ iVertexNum ].vOption.z * 0.5f, In[ 0 ].vPos.y, In[ 0 ].vPos.z, 1.f );
	
	float  pAge[ 4 ];
	pAge[ 0 ] = tParticle[ iVertexNum ].vOption.y;
	pAge[ 1 ] = tParticle[ iVertexNum ].vOption.y;
	pAge[ 2 ] = tParticle[ iVertexNum + 1 ].vOption.y;
	pAge[ 3 ] = tParticle[ iVertexNum + 1 ].vOption.y;

	matrix mtxBill = g_mtxView;
	mtxBill._41 = 0.f;
	mtxBill._42 = 0.f;
	mtxBill._43 = 0.f;

	

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( tParticle[ iVertexNum ].mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );
	
	GS_OUT Out;
	for( int i = 0; i < 2; ++i ) {
		Out.vPos = mul( mtxBill, pVertex[ i ] );
		Out.vPos = mul( Out.vPos, mtxWVP );
		Out.fAge = pAge[ i ];
		TriStream.Append( Out );
	}

	mtxWV = mul( tParticle[ iVertexNum + 1 ].mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	for( int j = 2; j < 4; ++j ) {
		Out.vPos = mul( mtxBill, pVertex[ j ] );
		Out.vPos = mul( Out.vPos, mtxWVP );
		Out.fAge = pAge[ j ];
		TriStream.Append( Out );
	}
}

//[maxvertexcount( 24 )]
//void GS( point VS_OUT In[ 1 ], inout TriangleStream<GS_OUT> TriStream )
//{
//	uint	iVertexNum = In[ 0 ].iVertexID;
//	if( iVertexNum == TRAIL_LENGTH ) return;
//	float3	vHalfSize = float3( tParticle[ iVertexNum ].vOption.z * 0.5f, tParticle[ iVertexNum ].vOption.w * 0.5f, tParticle[ iVertexNum ].vOption.w * 0.5f );
//
//	float4 pVertex[ 24 ];
//	// Front
//	pVertex[ 0 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 1 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 2 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 3 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	// Top
//	pVertex[ 4 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 5 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 6 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 7 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	// Left
//	pVertex[ 8 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 9 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 10 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 11 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	// Back
//	pVertex[ 12 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 13 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 14 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 15 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	// Bottom
//	pVertex[ 16 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 17 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 18 ] = float4( In[ 0 ].vPos.x - vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 19 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	// Right
//	pVertex[ 20 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 21 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y + vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//	pVertex[ 22 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z - vHalfSize.z, 1.f );
//	pVertex[ 23 ] = float4( In[ 0 ].vPos.x + vHalfSize.x, In[ 0 ].vPos.y - vHalfSize.y, In[ 0 ].vPos.z + vHalfSize.z, 1.f );
//
//	matrix	mtxVP = mul( g_mtxView, g_mtxProj );
//
//	GS_OUT Out;
//	for( int j = 0; j < 6; ++j )
//	{
//		for( int i = 0; i < 4; ++i ) {
//			if( pVertex[ j * 4 + i ].y > In[ 0 ].vPos.y )
//			{
//				Out.vPos = mul( pVertex[ j * 4 + i ], tParticle[ iVertexNum ].mtxWorld );
//				Out.fAge = tParticle[ iVertexNum ].vOption.y;
//			}
//			else
//			{
//				Out.vPos = mul( pVertex[ j * 4 + i ], tParticle[ iVertexNum + 1 ].mtxWorld );
//				Out.fAge = tParticle[ iVertexNum + 1 ].vOption.y;
//			}
//			Out.vPos = mul( Out.vPos, g_mtxView );
//			Out.vPos = mul( Out.vPos, g_mtxProj );
//			TriStream.Append( Out );
//		}
//		TriStream.RestartStrip();
//	}
//}

float4 PS( GS_OUT In ) : SV_Target
{
	return float4( In.fAge, 0.1f, 0.1f, In.fAge );
}