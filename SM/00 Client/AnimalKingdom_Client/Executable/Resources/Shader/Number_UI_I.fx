Texture2D Texture : register( t0 );
SamplerState g_SamplerState : register( s0 );

struct VS_IN
{
	matrix mtxPosSize : WORLD;
	float4 vOption : OPTION;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( VS_IN In, uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;

	if( iVertexNum == 0 ) { Out.vPos = float4( In.mtxPosSize._11, In.mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( In.vOption.x, In.vOption.z ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( In.mtxPosSize._11 + In.mtxPosSize._13, In.mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( In.vOption.y, In.vOption.z ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( In.mtxPosSize._11 + In.mtxPosSize._13, In.mtxPosSize._12 - In.mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( In.vOption.y, In.vOption.w ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( In.mtxPosSize._11, In.mtxPosSize._12, 0.5f, 1.f ); Out.vUV = float2( In.vOption.x, In.vOption.z ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( In.mtxPosSize._11 + In.mtxPosSize._13, In.mtxPosSize._12 - In.mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( In.vOption.y, In.vOption.w ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( In.mtxPosSize._11, In.mtxPosSize._12 - In.mtxPosSize._14, 0.5f, 1.f ); Out.vUV = float2( In.vOption.x, In.vOption.w ); }

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	return Texture.Sample( g_SamplerState, In.vUV );
}