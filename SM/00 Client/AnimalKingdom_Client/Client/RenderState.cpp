#include "stdafx.h"
#include "RenderState.h"
#include "Function.h"

ID3D11BlendState*			CRenderState::m_pBlendState[ BL_END ];
ID3D11DepthStencilState*	CRenderState::m_pDepthStencilState[ DS_END ];
ID3D11RasterizerState*		CRenderState::m_pRasterizerState[ RS_END ];

void CRenderState::Set_DepthStencilState( ID3D11DeviceContext* pContext, DS_STATE eDS_State )
{
	pContext->OMSetDepthStencilState( m_pDepthStencilState[ eDS_State ], 0 );
}

void CRenderState::Set_Rasterize( ID3D11DeviceContext* pContext, RS_STATE eRS_State )
{
	pContext->RSSetState( m_pRasterizerState[ eRS_State ] );
}

void CRenderState::Set_BlendState( ID3D11DeviceContext* pContext, BL_STATE eBL_State )
{
	pContext->OMSetBlendState( m_pBlendState[ eBL_State ], nullptr, 0xffffffff );
}

HRESULT CRenderState::Initialize( ID3D11Device* pDevice )
{
	m_pBlendState[ BL_NULL ] = nullptr;
	m_pDepthStencilState[ DS_NULL ] = nullptr;
	m_pRasterizerState[ RS_NULL ] = nullptr;

	/* BL_ALPHA */
	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory( &BlendStateDesc, sizeof( D3D11_BLEND_DESC ) );
	BlendStateDesc.IndependentBlendEnable = false;
	ZeroMemory( &BlendStateDesc.RenderTarget[ 0 ], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
	BlendStateDesc.RenderTarget[ 0 ].BlendEnable = true;
	BlendStateDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState( &BlendStateDesc, &m_pBlendState[ BL_ALPHA ] );

	/* DS_NO_WRITE */
	D3D11_DEPTH_STENCIL_DESC DepthDesc;
	ZeroMemory( &DepthDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	DepthDesc.DepthEnable = true;
	DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	DepthDesc.StencilEnable = false;
	DepthDesc.StencilReadMask = 0xff;
	DepthDesc.StencilWriteMask = 0xff;
	DepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	DepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	pDevice->CreateDepthStencilState( &DepthDesc, &m_pDepthStencilState[ DS_NO_WRITE ] );

	/* DS_NO_TEST */
	DepthDesc.DepthEnable = false;
	pDevice->CreateDepthStencilState( &DepthDesc, &m_pDepthStencilState[ DS_NO_TEST ] );

	/* RS_NULL */
	D3D11_RASTERIZER_DESC RSDefaultDesc;
	ZeroMemory( &RSDefaultDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	RSDefaultDesc.FillMode = D3D11_FILL_SOLID;
	RSDefaultDesc.CullMode = D3D11_CULL_BACK;
	RSDefaultDesc.FrontCounterClockwise = false;
	RSDefaultDesc.DepthClipEnable = true;
	RSDefaultDesc.DepthBias = 10000;
	RSDefaultDesc.DepthBiasClamp = 0.f;
	RSDefaultDesc.SlopeScaledDepthBias = 1.f;
	// RSNullDesc.MultisampleEnable = TRUE;
	// RSNullDesc.AntialiasedLineEnable = TRUE;
	pDevice->CreateRasterizerState( &RSDefaultDesc, &m_pRasterizerState[ RS_NULL ] );

	/* RS_CCW */
	D3D11_RASTERIZER_DESC RS_CCWDesc;
	ZeroMemory( &RS_CCWDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	RS_CCWDesc.FillMode = D3D11_FILL_SOLID;
	//RS_CCWDesc.MultisampleEnable = TRUE;
	RS_CCWDesc.CullMode = D3D11_CULL_FRONT;
	RS_CCWDesc.FrontCounterClockwise = false;
	RS_CCWDesc.DepthClipEnable = true;
	//RSWireDesc.AntialiasedLineEnable = TRUE;
	pDevice->CreateRasterizerState( &RS_CCWDesc, &m_pRasterizerState[ RS_CCW ] );

	/* RS_WIREFRAME */
	D3D11_RASTERIZER_DESC RSWireDesc;
	ZeroMemory( &RSWireDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	RSWireDesc.FillMode = D3D11_FILL_WIREFRAME;
	//RSWireDesc.MultisampleEnable = TRUE;
	RSWireDesc.CullMode = D3D11_CULL_BACK;
	RSWireDesc.FrontCounterClockwise = false;
	RSWireDesc.DepthClipEnable = true;
	//RSWireDesc.AntialiasedLineEnable = TRUE;
	pDevice->CreateRasterizerState( &RSWireDesc, &m_pRasterizerState[ RS_WIREFRAME ] );

	/* RS_NO_CULL */
	D3D11_RASTERIZER_DESC RSNoCull;
	ZeroMemory( &RSNoCull, sizeof( D3D11_RASTERIZER_DESC ) );
	RSNoCull.FillMode = D3D11_FILL_SOLID;
	RSNoCull.CullMode = D3D11_CULL_NONE;
	//RSNoCull.AntialiasedLineEnable = TRUE;
	//RSNoCull.MultisampleEnable = TRUE;
	RSNoCull.FrontCounterClockwise = false;
	RSNoCull.DepthClipEnable = true;
	pDevice->CreateRasterizerState( &RSNoCull, &m_pRasterizerState[ RS_NO_CULL ] );

	return S_OK;
}

void CRenderState::Release()
{
	for( int i = 1; i < ( int )BL_END; ++i )
		::Safe_Release( m_pBlendState[ i ] );

	for( int i = 1; i < ( int )DS_END; ++i )
		::Safe_Release( m_pDepthStencilState[ i ] );

	for( int i = 0; i < ( int )RS_END; ++i )
		::Safe_Release( m_pRasterizerState[ i ] );
}
