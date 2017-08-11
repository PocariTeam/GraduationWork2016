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

cbuffer cbBone : register( b2 )
{
	matrix g_mtxBone[ 29 ];
}

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV : TEXCOORD;
	int4   vIndex  : BONEINDICES;
	float4 vWeight : WEIGHTS;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( g_mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	float3 vPos = ( float3 )0;
	float3 vNormal = ( float3 )0;
	float3 vTangent = ( float3 )0;

	int iIndex[ 4 ] = { 0, 0, 0, 0 };
	float fWeight[ 4 ] = { 0.f, 0.f, 0.f, 0.f };

	iIndex[ 0 ] = In.vIndex.x; iIndex[ 1 ] = In.vIndex.y; iIndex[ 2 ] = In.vIndex.z; iIndex[ 3 ] = In.vIndex.w;
	fWeight[ 0 ] = In.vWeight.x; fWeight[ 1 ] = In.vWeight.y; fWeight[ 2 ] = In.vWeight.z; fWeight[ 3 ] = In.vWeight.w;

	for( int i = 0; i < 4; ++i )
	{
		vPos += fWeight[ i ] * mul( float4( In.vPos, 1.f ), g_mtxBone[ iIndex[ i ] ] ).xyz;
	}

	Out.vPos = mul( float4( vPos, 1.f ), mtxWVP );
	Out.vUV = In.vUV;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float4	Out = ( float4 )0;

	Out = gTexture.Sample( gSamplerState, In.vUV );
	Out.w = g_vOption.w;

	return Out;
}