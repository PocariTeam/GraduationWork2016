cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWinSize;
	float4 g_vOption;
}

Texture2D g_AlbedoTexture : register( t0 );
Texture2D g_LightTexture : register( t1 );
Texture2D g_SpecularTexture : register( t2 );

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
	float4 vAlbedo = g_AlbedoTexture.Load( float3( In.vUV.x, In.vUV.y, 0.f ) );
	float4 vLight = g_LightTexture.Load( float3( In.vUV.x, In.vUV.y, 0.f ) );
	float4 vSpecular = g_SpecularTexture.Load( float3( In.vUV.x, In.vUV.y, 0.f ) );

	float4 vOut = float4( mad( vAlbedo, vLight, vSpecular ).xyz, 0.f );
	if( !any( vOut ) ) discard;
	vOut.a = 1.f;
	return vOut;
}