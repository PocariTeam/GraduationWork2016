#define TERRAINSIZE 16.f

/* GS */
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
SamplerState gSamplerState: register( s0 );

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float3 vPos : POSITION;
};

struct GS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV   : TEXCOORD0;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	Out.vPos = ( float3 )mul( float4( 0.f, 0.f, 0.f, 1.f ), g_mtxWorld );

	return Out;
}

[maxvertexcount( 4 )]
void GS( point VS_OUT In[1], inout TriangleStream<GS_OUT> TriStream )
{
	float4 pVertex[4];
	// Front
	pVertex[0] = float4( In[0].vPos.x - TERRAINSIZE, In[0].vPos.y, In[0].vPos.z + TERRAINSIZE, 1.f );
	pVertex[1] = float4( In[0].vPos.x + TERRAINSIZE, In[0].vPos.y, In[0].vPos.z + TERRAINSIZE, 1.f );
	pVertex[2] = float4( In[0].vPos.x - TERRAINSIZE, In[0].vPos.y, In[0].vPos.z - TERRAINSIZE, 1.f );
	pVertex[3] = float4( In[0].vPos.x + TERRAINSIZE, In[0].vPos.y, In[0].vPos.z - TERRAINSIZE, 1.f );

	float2 pUV[4];

	pUV[0] = float2( 0.f, 0.f );
	pUV[1] = float2( 1.f, 0.f );
	pUV[2] = float2( 0.f, 1.f );
	pUV[3] = float2( 1.f, 1.f );

	GS_OUT Out;

	for( int i = 0; i < 4; i++ )
	{
		Out.vPos = mul( pVertex[i], g_mtxView );
		Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );
		Out.vUV = pUV[i];
		TriStream.Append( Out );
	}
}

float4 PS( GS_OUT In ) : SV_Target
{
	float4 vColor = gTexture.Sample( gSamplerState, In.vUV );

	return vColor;
}