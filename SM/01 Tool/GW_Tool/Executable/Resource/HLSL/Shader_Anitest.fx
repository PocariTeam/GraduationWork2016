float4 Lighting( float3 vPos, float3 vNormal );
/* VS */
cbuffer CbMtxWorld : register( b0 )
{
	matrix g_mtxWorld;
};

cbuffer CbMtxViewProj : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

cbuffer CbLight : register( b2 )
{
	float4 g_vLightDir;
};

cbuffer CbMtxBone : register( b4 )
{
	matrix g_mtxBone[30];
};

/* PS */
cbuffer CbLight : register( b2 )
{
	float4	 g_vLightDirection;
	float4	 g_vDiffuseLight;
	float4	 g_vAmbientLight;
	float4	 g_vSpecularLight;
};

cbuffer CbTextureInfo : register( b3 )
{
	float4   g_vTextureInfo;
}

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV	   : TEXCOORD;
	int4 vIndex  : BONEINDICES;
	float3 vWeight : WEIGHTS;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float3 vPosW : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV	   : TEXCOORD0;
};

/* PS */
Texture2D gDiffuseTexture : register( t0 );
Texture2D gNormalTexture : register( t1 );
SamplerState gSamplerState : register( s0 );

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	float3 vPos = (float3)0;
	float3 vNormal = (float3)0;
	float3 vTangent = (float3)0;

	int iIndex[4] = { 0, 0, 0, 0 };
	float fWeight[4] = { 0.f, 0.f, 0.f, 0.f };

	iIndex[0] = In.vIndex.x; iIndex[1] = In.vIndex.y; iIndex[2] = In.vIndex.z; iIndex[3] = In.vIndex.w;
	fWeight[0] = In.vWeight.x; fWeight[1] = In.vWeight.y; fWeight[2] = In.vWeight.z; fWeight[3] = 1.f - ( fWeight[0] + fWeight[1] + fWeight[2] );

	for (int i = 0; i < 4; ++i)
	{
		vPos += fWeight[i] * mul(float4(In.vPos, 1.f), g_mtxBone[iIndex[i]]).xyz;
		vNormal += fWeight[i] * mul((float3x3)g_mtxBone[iIndex[i]], In.vNormal).xyz;
		vTangent += fWeight[i] * mul((float3x3)g_mtxBone[iIndex[i]], In.vTangent).xyz;
	}

	Out.vPos = mul( float4( vPos, 1.f ), g_mtxWorld );
	Out.vPosW = Out.vPos.xyz;
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxView );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );
	Out.vNormal = mul( vNormal, ( float3x3 )g_mtxWorld );
	Out.vTangent = mul( vTangent, (float3x3)g_mtxWorld );
	Out.vUV = In.vUV;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float3 N = normalize( In.vNormal );
	
	if( g_vTextureInfo.x >= 2.f )
	{
		float3 T = normalize( In.vTangent );
		float3 B = cross( N, T );

		float3x3 TBN = float3x3( T, B, N );
		float3 vNormal = gNormalTexture.Sample( gSamplerState, In.vUV ).rgb;
		vNormal = 2.f * vNormal - 1.f;
		float3 vNormalW = mul( vNormal, TBN );

		return gDiffuseTexture.Sample( gSamplerState, In.vUV ) * Lighting( In.vPosW, vNormalW );
	}
		

	else
		return gDiffuseTexture.Sample( gSamplerState, In.vUV ) * Lighting( In.vPosW, N );
}

float4 Lighting( float3 vPos, float3 vNormal )
{
	float3 vCameraPos = g_vCameraPos.xyz;
	float3 vToCamera = normalize( vCameraPos - vPos );
	float4 cColor = ( float4 )0;
	float3 vSpecular = ( float3 )0;
	// For.Specular
	float fDiffuseFactor = dot( -g_vLightDirection.xyz, vNormal );
	float3 vDiffuse = ( float3 )0;
	float3 vAmbient = ( float3 )0;

	if( fDiffuseFactor > 0.f ) {
		float3 vReflect = reflect( g_vLightDirection.xyz, vNormal );
		float fSpecularFactor = pow( max( dot( vReflect, vToCamera ), 0.0f ), 0.5f );

		vSpecular = float3( 1.f, 1.f, 1.f ) * ( g_vSpecularLight.xyz * fSpecularFactor );
		vDiffuse = float3( 1.f, 1.f, 1.f ) * ( g_vDiffuseLight.xyz * fDiffuseFactor );
	}
	vAmbient = float3( 0.5f, 0.5f, 0.5f ) * g_vAmbientLight.xyz;

	cColor += float4( vAmbient * 2.f + vDiffuse + vSpecular, 0.f );
	cColor.a = 1.f;

	return( cColor );
}