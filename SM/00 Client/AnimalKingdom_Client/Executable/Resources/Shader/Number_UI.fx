Texture2D Texture : register( t0 );
SamplerState g_SamplerState : register( s0 );

cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWorld;
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

	if( iVertexNum == 0 ) { Out.vPos = float4( g_mtxWorld._11, g_mtxWorld._12, 0.5f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.z ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( g_mtxWorld._11 + g_mtxWorld._13, g_mtxWorld._12, 0.5f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.z ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( g_mtxWorld._11 + g_mtxWorld._13, g_mtxWorld._12 - g_mtxWorld._14, 0.5f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.w ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( g_mtxWorld._11, g_mtxWorld._12, 0.5f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.z ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( g_mtxWorld._11 + g_mtxWorld._13, g_mtxWorld._12 - g_mtxWorld._14, 0.5f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.w ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( g_mtxWorld._11, g_mtxWorld._12 - g_mtxWorld._14, 0.5f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.w ); }

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	return Texture.Sample( g_SamplerState, In.vUV );
}