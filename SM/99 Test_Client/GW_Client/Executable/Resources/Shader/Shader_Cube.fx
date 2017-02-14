#define CUBESIZE 1.f

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

	Out.vPos = (float3)mul( float4(0.f, 0.f, 0.f, 1.f), g_mtxWorld );

	return Out;
}

[maxvertexcount(24)]
void GS( point VS_OUT In[1], inout TriangleStream<GS_OUT> TriStream )
{
	float4 pVertex[24];
	// Front
	pVertex[0] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[1] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[2] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[3] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	// Top
	pVertex[4] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[5] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[6] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[7] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	// Left
	pVertex[8] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[9] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[10] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[11] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	// Back
	pVertex[12] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[13] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[14] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[15] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );
	// Bottom
	pVertex[16] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[17] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[18] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[19] = float4( In[0].vPos.x - CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	// Right
	pVertex[20] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[21] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y + 2 * CUBESIZE, In[0].vPos.z + CUBESIZE, 1.f );
	pVertex[22] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z - CUBESIZE, 1.f );
	pVertex[23] = float4( In[0].vPos.x + CUBESIZE, In[0].vPos.y, In[0].vPos.z + CUBESIZE, 1.f );

	float2 pUV[24];
	for( int i = 0; i < 6; ++i ) {
		int iIndex = i * 4;
		pUV[iIndex] = float2( 0.f, 1.f );
		pUV[iIndex + 1] = float2( 0.f, 0.f );
		pUV[iIndex + 2] = float2( 1.f, 1.f );
		pUV[iIndex + 3] = float2( 1.f, 0.f );
	}
	
	GS_OUT Out;
	for( int j = 0; j < 6; ++j ) {
		for( int i = 0; i < 4; ++i ) {
			int iIndex = i + j * 4;
			Out.vPos = mul( pVertex[iIndex], g_mtxView );
			Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );
			Out.vUV = pUV[iIndex];
			TriStream.Append( Out );
		}
		TriStream.RestartStrip();
	}
}

float4 PS( GS_OUT In ) : SV_Target
{
	float4 vColor = gTexture.Sample( gSamplerState, In.vUV );

	return vColor;
}