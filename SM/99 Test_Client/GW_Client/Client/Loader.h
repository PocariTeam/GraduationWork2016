#ifndef _LOADER_H_
#define _LOADER_H_

class CLoader
{
private:
	explicit CLoader( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	~CLoader( void ) = default;
public:
	HRESULT Initialize( void );
	void	Load_Resource( void );
public:
	static CLoader* Create( ID3D11Device* pDevice, ID3D11DeviceContext*	pContext );
	static bool		m_bLoading;
private:
	static DWORD WINAPI Loading_Func( void* pArg );

public:
	bool Get_Complate() { return m_bComplete; }
	CRITICAL_SECTION* Get_CS() { return &m_CS; }

private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	CRITICAL_SECTION		m_CS;
	HANDLE					m_hThread;

private:
	bool					m_bComplete;
public:
	void Release( void );
};

#endif // Loading_h__