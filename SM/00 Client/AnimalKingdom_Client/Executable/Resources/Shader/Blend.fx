cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWinSize;
	float4 g_vOption;
}

Texture2D/*MS<float4, 4>*/ g_AlbedoTexture : register( t0 );
Texture2D/*MS<float4, 4>*/ g_LightTexture : register( t1 );
Texture2D/*MS<float4, 4>*/ g_SpecularTexture : register( t2 );

static float	Filter[ 9 ] = { 1.f / 16.f, 1.f / 8.f, 1.f / 16.f, 1.f / 8.f, 1.f / 4.f, 1.f / 8.f, 1.f / 16.f, 1.f / 8.f, 1.f / 16.f };		// 가우시안 필터
static float2	TextureOffsetUV[ 9 ] = { { -1.f, -1.f },{ 0.f, -1.f },{ 1.f, -1.f },{ -1.f, 0.f },{ 0.f, 0.f },{ 0.f, 1.f },{ -1.f, 1.f },{ 0.f, 1.f },{ 1.f, 1.f } };

float4 Blur( float2 vUV );

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;

	if( iVertexNum == 0 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( +1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( g_mtxWinSize._11, 0.f ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( g_mtxWinSize._11, g_mtxWinSize._21 ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( g_mtxWinSize._11, g_mtxWinSize._21 ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( -1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, g_mtxWinSize._21 ); }

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float4 vAlbedo = g_AlbedoTexture.Load( float3( In.vUV, 0.f )/*int2( In.vUV.x, In.vUV.y ), 0*/ );
	float4 vLight = g_LightTexture.Load( float3( In.vUV, 0.f )/*int2( In.vUV.x, In.vUV.y ), 0*/ );
	float4 vSpecular = g_SpecularTexture.Load( float3( In.vUV, 0.f )/*int2( In.vUV.x, In.vUV.y ), 0*/ );

	if( !dot( vAlbedo.xyz, float3( 1.f, 1.f, 1.f ) ) ) discard;

	float4 vOut = /*float4( mad( vAlbedo, vLight, vSpecular ).xyz, 1.f )*/Blur( In.vUV.xy );
	return vOut;
}

float4 Blur( float2 vUV )
{
	float3 vAlbedo = ( float3 )0;
	float3 vLight = ( float3 )0;
	float3 vSpecular = ( float3 )0;

	for( int j = 0; j < 9; ++j )
	{
		vAlbedo += Filter[ j ] * g_AlbedoTexture.Load( float3( vUV.xy + TextureOffsetUV[ j ], 0.f )/*int2( vUV.xy + TextureOffsetUV[ j ] ), 0*/ ).xyz;
		vLight += Filter[ j ] * g_LightTexture.Load( float3( vUV.xy + TextureOffsetUV[ j ], 0.f )/*int2( vUV.xy + TextureOffsetUV[ j ] ), 0*/ ).xyz;
		vSpecular += Filter[ j ] * g_SpecularTexture.Load( float3( vUV.xy + TextureOffsetUV[ j ], 0.f )/*int2( vUV.xy + TextureOffsetUV[ j ] ), 0*/ ).xyz;
	}

	return float4( mad( vAlbedo, vLight, vSpecular ).xyz, 1.f );
}