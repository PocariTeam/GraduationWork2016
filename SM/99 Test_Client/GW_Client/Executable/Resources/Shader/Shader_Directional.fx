/* VS */
cbuffer CbMtxViewProj : register( b0 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

cbuffer CbMtxWorld : register( b1 )
{
	matrix g_mtxWorld;
	matrix g_mtxWorldInv;
};

/* PS */
Texture2D gTexture : register( t0 );
SamplerState gSamplerState : register( s0 );

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV	   : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV	: TEXCOORD1;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;
	Out.vPos = mul( float4( In.vPos.xyz, 1.f ), g_mtxWorld );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxView );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );

	Out.vUV = In.vUV;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	 float4 vColor = saturate( gTexture.Sample( gSamplerState, In.vUV ) );

	return vColor;
}