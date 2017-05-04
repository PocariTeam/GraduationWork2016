#pragma once

#ifndef Shader_h__
#define Shader_h__

#include "Resources.h"
#include "Struct.h"

class CGameObject;
class CShader
	: public CResources
{
public:
	enum INPUT_TYPE { INPUT_POS_ONLY, INPUT_PT, INPUT_PNT, INPUT_PNTT, INPUT_PARTICLE, INPUT_NO, INPUT_ANIMATE, INPUT_END };
protected:
	explicit CShader();
	explicit CShader( const CShader& Instance );
	virtual ~CShader();
protected:
	HRESULT	CreateVS( ID3D11Device* pDevice, const char* pFilePath, D3D11_INPUT_ELEMENT_DESC* pInputElement, UINT iComponentCnt );
	HRESULT CreatePS( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT CreateGS( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT CreateHS( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT CreateDS( ID3D11Device* pDevice, const char* pFilePath );
	virtual HRESULT CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize = sizeof( CB_WORLD ) );
public:
	virtual CShader*	Clone( void )PURE;
	virtual void Update( const float& fTimeDelta )PURE;
	virtual void Render( ID3D11DeviceContext* pContext )PURE;
	virtual void Add_RenderObject( CGameObject* pGameObject )PURE;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption = XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) );
public:
	virtual DWORD Release();
protected:
	ID3D11Device*			m_pDevice;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11VertexShader*		m_pVS;
	ID3D11PixelShader*		m_pPS;
	ID3D11GeometryShader*	m_pGS;
	ID3D11HullShader*		m_pHS;
	ID3D11DomainShader*		m_pDS;
	ID3D11Buffer*			m_pConstantBuffer;
};

#endif // Shader_h__