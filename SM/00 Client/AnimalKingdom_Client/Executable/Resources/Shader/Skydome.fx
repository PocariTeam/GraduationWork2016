Texture2D gTexture0 : register( t0 );
Texture2D gTexture1 : register( t1 );
Texture2D gTexture2 : register( t2 );
Texture2D gTexture3 : register( t3 );
Texture2D gTexture4 : register( t4 );

SamplerState gSamplerState0 : register( s0 );
SamplerState gSamplerState1 : register( s1 );
SamplerState gSamplerState2 : register( s2 );
SamplerState gSamplerState3 : register( s3 );
SamplerState gSamplerState4 : register( s4 );

cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWorld;
	float4 g_vOption;
}

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	matrix g_mtxOrtho;
	matrix g_mtxViewInv;
	matrix g_mtxProjInv;
	float4 g_vCameraPos;
};

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

//struct GS_OUT
//{
//	float4 vPos : SV_POSITION;
//	float3 vLocalPos : POSITION;
//};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	matrix	mtxWP;
	mtxWP = mul( g_mtxWorld, g_mtxProj );
	Out.vPos = mul( float4( In.vPos, 1.f ), mtxWP );
	Out.vUV = In.vUV;

	return Out;
}

// Box
//[maxvertexcount( 24 )]
//void GS( point VS_OUT In[ 1 ], inout TriangleStream<GS_OUT> TriStream )
//{
//	float4 pVertex[ 24 ];
//	// Front
//	pVertex[ 0 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 1 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 2 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 3 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	// Top
//	pVertex[ 4 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 5 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 6 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 7 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	// Left
//	pVertex[ 8 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 9 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 10 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 11 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	// Back
//	pVertex[ 12 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 13 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 14 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 15 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	// Bottom
//	pVertex[ 16 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 17 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 18 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 19 ] = float4( In[ 0 ].vPos.x - CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	// Right
//	pVertex[ 20 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 21 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y + CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//	pVertex[ 22 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z - CUBESIZE, 1.f );
//	pVertex[ 23 ] = float4( In[ 0 ].vPos.x + CUBESIZE, In[ 0 ].vPos.y - CUBESIZE, In[ 0 ].vPos.z + CUBESIZE, 1.f );
//
//	GS_OUT Out = ( GS_OUT )0;
//	for( int j = 0; j < 6; ++j ) {
//		for( int i = 0; i < 4; ++i ) {
//			int iIndex = i + j * 4;
//			Out.vLocalPos = pVertex[ iIndex ].xyz;
//			Out.vPos = mul( /*mul( */pVertex[ iIndex ]/*, g_mtxView )*/, g_mtxProj );
//			TriStream.Append( Out );
//		}
//		TriStream.RestartStrip();
//	}
//}

float4 PS( VS_OUT In ) : SV_Target
{
	int	  iSelect = g_vOption.y;
	float fLerp = frac( g_vOption.y );
	switch( iSelect )
	{
	case 0:
		return lerp( gTexture0.Sample( gSamplerState0, In.vUV ), gTexture1.Sample( gSamplerState1, In.vUV ), fLerp );
	case 1:
		return lerp( gTexture1.Sample( gSamplerState1, In.vUV ), gTexture2.Sample( gSamplerState2, In.vUV ), fLerp );
	case 2:
		return lerp( gTexture2.Sample( gSamplerState2, In.vUV ), gTexture3.Sample( gSamplerState3, In.vUV ), fLerp );
	case 3:
		return lerp( gTexture3.Sample( gSamplerState3, In.vUV ), gTexture4.Sample( gSamplerState4, In.vUV ), fLerp );
	case 4:
		return lerp( gTexture4.Sample( gSamplerState4, In.vUV ), gTexture0.Sample( gSamplerState0, In.vUV ), fLerp );
	}

	return gTexture0.Sample( gSamplerState0, In.vUV );
}