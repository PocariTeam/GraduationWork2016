#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

class CGameObject;
class CCamera;
class CScene
{
public:
	enum SCENE_TYPE { SCENE_LOAD, SCENE_STAGE, SCENE_END };
	enum OBJ_TYPE { OBJ_ENVIRONMENT, OBJ_MOVING, OBJ_END };
private:
	virtual HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Release( void );
	virtual void Render( ID3D11DeviceContext* pContext ) {}
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BYTE byStageNum );
protected:
	CScene();
	virtual ~CScene() = default;
protected:
	HWND				 m_hWnd;

	using OBJLIST = list<CGameObject*>;
	OBJLIST				 m_plistObj[OBJ_END];
	CCamera*			 m_pCamera;
};

#endif