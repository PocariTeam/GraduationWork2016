#define CUBESIZE	-400.f

TextureCube gTexture : register( t0 );
SamplerState gSamplerState : register( s0 );

matrix g_mtxWorld : register( b0 );

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

struct VS_OUT
{
	float3 vPos : POSITION;
};

struct GS_OUT
{
	float4 vPos : SV_POSITION;
	float3 vLocalPos : POSITION;
};

VS_OUT VS( VS_OUT In )
{
	return In;
}

[maxvertexcount( 24 )]
void GS( point VS_OUT In[ 1 ], inout TriangleStream<GS_OUT> TriStream )
{
	float4 pVertex[ 24 ];
	// Front
	pVertex[ 0 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 1 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 2 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 3 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	// Top
	pVertex[ 4 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 5 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 6 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 7 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	// Left
	pVertex[ 8 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 9 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 10 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 11 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	// Back
	pVertex[ 12 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 13 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 14 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 15 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	// Bottom
	pVertex[ 16 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 17 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 18 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 19 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	// Right
	pVertex[ 20 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 21 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
	pVertex[ 22 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
	pVertex[ 23 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );

	GS_OUT Out = ( GS_OUT )0;
	for( int j = 0; j < 6; ++j ) {
		for( int i = 0; i < 4; ++i ) {
			int iIndex = i + j * 4;
			Out.vLocalPos = pVertex[ iIndex ].xyz;
			Out.vPos = mul( mul( pVertex[ iIndex ], g_mtxView ), g_mtxProj );
			TriStream.Append( Out );
		}
		TriStream.RestartStrip();
	}
}

float4 PS( GS_OUT In ) : SV_Target
{
	return gTexture.Sample( gSamplerState, In.vLocalPos );
}