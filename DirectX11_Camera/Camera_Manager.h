#pragma once

#include "Base.h"

#include "CutScene.h"

/* 생성한 원형 객체를 모아놓는다. (프로토타입) */
/* 원형객체를 복제하여 생성한 사본객체를 레이어로 구분하여 모아놓는다. */
/* 보관하고 있는 사본객체들의 Tick관련 함수를 반복적으로 호출해준다.  */

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT			Initialize(_uint iNumLevels);
	HRESULT			Add_Prototype_Camera(_uint iLevelIndex, const wstring& strPrototypeTag, class CCamera* pPrototype);
	HRESULT			Add_Clone_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, const wstring& strCameraTag, void* pArg);
	
	void			Priority_Tick(_float fTimeDelta);
	void			Tick(_float fTimeDelta);
	void			Late_Tick(_float fTimeDelta);

	void			Clear(_uint iLevelIndex);
	void			Clear_Cameras();

public:
	class CCamera*		Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	CCamera*			Get_CurCamera();
	HRESULT				Set_CurCamera(const wstring& strCameraTag);


private:
	/* 클론의 고유 번호를 세팅한 이름을 리턴한다. */
	const wstring		Get_CloneNameWithPin(const wstring& strName);

private:
	_uint				m_iNumLevels = { 0 };

private:
	CGameObject*								m_pCurCamera = { nullptr };
	map<const wstring, class CCamera*>*			m_pPrototypes = { nullptr };	// 원형 레벨별로 구분
	map<const wstring, class CCamera*>			m_pCameras;

	typedef map<const wstring, class CCamera*>	PROTOTYPES;



// 컷신 관련 ====================================================================================================================
public:
	void					Initialize_CutScene();
	void					Clear_CutScene();

	HRESULT					Add_CutScene(_tchar* pTag, CCutScene* _pCutScene);
	CCutScene*				Find_CutScene(const _tchar* pTag);

	void					Play_CutScene(const _tchar* pTag);				// 컷신 재생
	void					Play_CutScene_FromPlayer(const _tchar* pTag);	// 컷신 재생 : 시작-끝을 플레이어 카메라 위치로
	void					Set_Stop() { m_bIsPlay = FALSE; }
	void					Set_FromPlayer(const _tchar* pTag);

	_bool					Is_ExistCutScene();
	CCutScene*				Get_CurCutScene() { return m_pCurCutScene; }
	void					Set_CurCutScene(_tchar* pTag);

	bool					Is_CurCutScene_End() { return m_pCurCutScene->Get_End(); }
	_bool					Get_IsPlay() { return m_bIsPlay; }

	_int					Get_CurFrame();

private:
	map<const _tchar*, CCutScene*>	m_mapCutScene;			// 컷신들을 넣어둘 맵 컨테이너

	_tchar*							m_pCurCutSceneTag;		// 현재 재생중인 컷신의 이름
	CCutScene*						m_pCurCutScene;			// 현재 재생중인 컷신

	_bool							m_bIsPlay = { FALSE };	// 재생 중인지


protected:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CCamera_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};

END