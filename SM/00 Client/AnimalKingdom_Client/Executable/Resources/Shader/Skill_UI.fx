Texture2D g_Texture : register( t0 );
SamplerState g_SamplerState : register( s0 );

#define PI 3.141592f

cbuffer cbWorld : register( b0 )
{
	matrix g_mtxPosSize;
	float4 g_vOption;
}

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;

	if( iVertexNum == 0 ) { Out.vPos = float4( g_mtxPosSize._11, g_mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( g_mtxPosSize._11 + g_mtxPosSize._13, g_mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( 1.f, 0.f ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( g_mtxPosSize._11 + g_mtxPosSize._13, g_mtxPosSize._12 - g_mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( g_mtxPosSize._11, g_mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( g_mtxPosSize._11 + g_mtxPosSize._13, g_mtxPosSize._12 - g_mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( g_mtxPosSize._11, g_mtxPosSize._12 - g_mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( 0.f, 1.f ); }

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	if( g_vOption.w < In.vUV.y )
		return g_Texture.Sample( g_SamplerState, In.vUV ) * float4( 0.3f, 0.3f, 0.3f, 1.f );
	return g_Texture.Sample( g_SamplerState, In.vUV );
}