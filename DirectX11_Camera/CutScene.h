#pragma once

#include "Base.h"

BEGIN(Engine)


class ENGINE_DLL CCutScene final : public CBase
{
private:
	CCutScene();
	virtual ~CCutScene() = default;

public:
	HRESULT		Initialize(_float _fTimeSpeed, _float _fAcceleration, _bool _Loop);
	void		Update_CutScene(const _float& fTimeDelta);
	void		LateUpdate_CutScene(const _float& fTimeDelta);


	// 컷신 초기화 ==========================================================================================================
	void			Ready_CutScene()
	{
		m_fAccTime = 0.f;
		m_bIsEnd = false;
	}

public:
	_tchar*			Get_CutSceneTag() { return m_pCutSceneTag; }
	void			Set_CutSceneTag(_tchar* pTag) { m_pCutSceneTag = pTag; }

// 시간 관련 ==============================================================================================================
	_float			Get_AccTime() { return m_fAccTime; }
	void			Set_AccTime(_float _AccTime) { m_fAccTime = _AccTime; }

	_float			Get_TimeSpeed() { return m_fTimeSpeed; }
	void			Set_TimeSpeed(_float _fTimeSpeed) { m_fTimeSpeed = _fTimeSpeed; }

	_float			Get_Acceleration() { return m_fAcceleration; }
	void			Set_Acceleration(_float _fAcceleration) { m_fAcceleration = _fAcceleration; }

// 벡터에 넣은 프레임 관련 =================================================================================================
	_int						Get_MaxFrameIdx();

	vector<CAMERA_KEYFRAME>&	Get_vecKeyFrame();

	CAMERA_KEYFRAME				Get_KeyFrame(_int _iIdx) { return m_KeyFrames[_iIdx]; }
	void						Set_KeyFrame(_int _iIdx, CAMERA_KEYFRAME _tKeyFrame) { m_KeyFrames[_iIdx] = _tKeyFrame; }

	_float						Get_KeyFrame_KeyTime(_int _iIdx) { return m_KeyFrames[_iIdx].fKeyTime; }
	void						Set_KeyFrame_KeyTime(_int _iIdx, _float _fKeyTime) { m_KeyFrames[_iIdx].fKeyTime = _fKeyTime; }

	_float4						Get_KeyFrame_EyePos(_int _iIdx) { return m_KeyFrames[_iIdx].vEye; }
	void						Set_KeyFrame_EyePos(_int _iIdx, _float4 _EyePos);

	_float4						Get_KeyFrame_LookAt(_int _iIdx) { return m_KeyFrames[_iIdx].vAt; }
	void						Set_KeyFrame_LookAt(_int _iIdx, _float4 _LookAt);

	_float3						Get_KeyFrame_Rot(_int _iIdx) { return m_KeyFrames[_iIdx].vRot; }
	void						Set_KeyFrame_Rot(_int _iIdx, _float3 _Rot);

// 반복 재생, 끝에 도달 여부 ===============================================================================================
	_bool			Get_Loop() { return m_bIsLoop; }
	void			Set_Loop(_bool bIsLoop) { m_bIsLoop = bIsLoop; }

	_bool			Get_End() { return m_bIsEnd; }
	void			Set_End(_bool bIsEnd) { m_bIsEnd = bIsEnd; }


// 현재 키프레임의 카메라의 위치, 보는 방향 =================================================================================
	_float4			Get_CurEye() { return m_vCurEye; }
	void			Set_CurEye(_float4 _CurEye) { m_vCurEye = _CurEye; }

	_float4			Get_CurAt() { return m_vCurAt; }
	void			Set_CurAt(_float4 _CurAt) { m_vCurAt = _CurAt; }

	_float3			Get_CurRot() { return m_vCurRot; }
	void			Set_CurRot(_float3 _CurRot) { m_vCurRot = _CurRot; }

// 키 프레임 추가, 삭제 ===================================================================================================
	void			Add_KeyFrame(CAMERA_KEYFRAME& tNewKeyFrame);
	void			Delete_KeyFrame(_uint iIdx);


private:
	CAMERA_KEYFRAME		GetKeyframeAtTime(_float _fKeyTime); // 현재 시간에 해당하는 보간된 키프레임 반환

// 보간 =================================================================================================================
	_vector			Lerp(const _fvector& vStart, const _fvector& vEnd, _float fTime);	// 선형보간


protected:
	_tchar*					m_pCutSceneTag;			// 맵에서 찾을 키
	vector<CAMERA_KEYFRAME>	m_KeyFrames;			// 모든 키 프레임 정보를 저장할 벡터 컨테이너. 프레임별로 각각 가지고 있을 정보

	_float					m_fAccTime		= { 0.f };		// 시간 누적
	_float					m_fTimeSpeed	= { 10.f };		// 재생 속도 (높을수록 느려짐. 10.f 추천)
	_float					m_fAcceleration = { 2.f };		// 가속도 조절 값 (높을수록 빨라짐. 2.f 추천)

	_bool					m_bIsLoop		= { FALSE };	// 반복 재생 여부
	_bool					m_bIsEnd		= { FALSE };	// 재생 끝에 도달 여부

	_float4					m_vCurEye		= {};			// 현재 Eye
	_float4					m_vCurAt		= {};			// 현재 At
	_float3					m_vCurRot		= {};

	_float					m_fTimeDelta	= { 0.f };

public:
	// 재생 속도(높을수록 느려짐), 가속도 조절 값, 반복재생 여부
	static CCutScene* Create(_float _fTimeSpeed, _float _fAcceleration, _bool _Loop);
	virtual void Free() override; 
};

END