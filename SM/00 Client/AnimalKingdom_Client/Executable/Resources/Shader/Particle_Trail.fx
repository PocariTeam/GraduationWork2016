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
	matrix mtxWorld : WORLD;
	float4 vOption : OPTION;
	float4 vVelocity : VELOCITY;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float  fAge : AGE;
};

VS_OUT VS( VS_IN In, uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;
	if( iVertexNum == 0 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 1 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 2 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 3 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y + In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 4 ) Out.vPos = float4( In.vVelocity.x + In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	else if( iVertexNum == 5 ) Out.vPos = float4( In.vVelocity.x - In.vOption.z * 0.5f, In.vVelocity.y - In.vOption.w * 0.5f, In.vVelocity.z, 1.f );
	
	/*matrix mtxBill = g_mtxView;
	mtxBill._41 = 0.f;
	mtxBill._42 = 0.f;
	mtxBill._43 = 0.f;

	Out.vPos = mul( mtxBill, Out.vPos );*/

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( In.mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	Out.vPos = mul( Out.vPos, mtxWVP );
	Out.fAge = In.vOption.y;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	return float4( 9.f, 0.2f, 0.2f, In.fAge );
}