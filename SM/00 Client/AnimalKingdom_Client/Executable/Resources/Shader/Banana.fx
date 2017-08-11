Texture2D gTexture : register( t0 );
SamplerState gSamplerState : register( s0 );

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
	float3 vWorldPos : POSITION;
	float2 vUV : TEXCOORD0;
	float  fAlpha : TEXCOORD1;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( g_mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	Out.vPos = mul( float4( In.vPos, 1.f ), mtxWVP );
	Out.vWorldPos = mul( float4( In.vPos, 1.f ), g_mtxWorld ).xyz;
	Out.vUV = In.vUV;
	Out.fAlpha = g_vOption.w;

	// Out.vNormal = normalize( mul( float4( In.vNormal, 0.f ), g_mtxWorld ) ).xyz;
	// Out.vTangent = normalize( mul( float4( In.vTangent, 0.f ), g_mtxWorld ) ).xyz;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float4	vColor = float4( gTexture.Sample( gSamplerState, In.vUV ).xyz, In.fAlpha );

	return vColor;
}