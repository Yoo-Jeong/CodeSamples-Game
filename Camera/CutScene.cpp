#include "..\Public\CutScene.h"

CCutScene::CCutScene()
{

}

HRESULT CCutScene::Initialize(_float _fTimeSpeed, _float _fAcceleration, _bool _Loop)
{

	m_fTimeSpeed = _fTimeSpeed;
	m_fAcceleration = _fAcceleration;
	m_bIsLoop = _Loop;

	return S_OK;
}

void CCutScene::Update_CutScene(const _float& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	if (!m_KeyFrames.empty())	// 벡터에 키프레임이 존재 할 때만 실행
	{
		// 재생이 끝났고, 루프가 아니면 재생 안함.
		if (m_bIsEnd && !m_bIsLoop)
			return;

		m_fAccTime += m_fTimeDelta;					// 시간이 누적
		_float fTime = m_fAccTime / m_fTimeSpeed;

		if (m_fAccTime >= m_fTimeSpeed)				// 한번 재생이 완료됐다면
		{
			if (m_bIsLoop)
			{
				m_bIsEnd = true;
			}
			else
			{
				// 반복재생이 아니라면 마지막 프레임에 고정
				m_vCurEye = m_KeyFrames.back().vEye;
				m_vCurAt = m_KeyFrames.back().vAt;
				m_vCurRot = m_KeyFrames.back().vRot;
				m_bIsEnd = true;
				return;
			}
		}
		else
		{
			if (m_bIsLoop)
				m_bIsEnd = false;
		}

		// 이동 속도(시간) 보정
		_float fAdjustedTime;

		if (fTime < 0.5f)
			fAdjustedTime = 0.5f * pow(fTime * 2.0f, 1.0f / m_fAcceleration);	// 시작 부분 가속
		else
			fAdjustedTime = 1.0f - 0.5f * pow((1.0f - fTime) * 2.0f, 1.0f / m_fAcceleration); // 끝 부분 감속

		fTime = fAdjustedTime;


		// 보간된 위치와 방향으로 이동
		// D3DXVec3Lerp 함수사용으로 바꿔도 될듯 
		CAMERA_KEYFRAME tInterpolatedFrame = GetKeyframeAtTime(fTime);
		m_vCurEye = tInterpolatedFrame.vEye;
		m_vCurAt = tInterpolatedFrame.vAt;
		m_vCurRot = tInterpolatedFrame.vRot;
	}
}

void CCutScene::LateUpdate_CutScene(const _float& fTimeDelta)
{
	
}

_int CCutScene::Get_MaxFrameIdx()
{
	if (!m_KeyFrames.empty())
		return m_KeyFrames.size() - 1;
	else
		return 0;
}

vector<CAMERA_KEYFRAME>& CCutScene::Get_vecKeyFrame()
{
	return m_KeyFrames;
}

void CCutScene::Set_KeyFrame_EyePos(_int _iIdx, _float4 _EyePos)
{
	m_KeyFrames[_iIdx].vEye = _EyePos;
}

void CCutScene::Set_KeyFrame_LookAt(_int _iIdx, _float4 _LookAt)
{
	m_KeyFrames[_iIdx].vAt = _LookAt;
}

void CCutScene::Set_KeyFrame_Rot(_int _iIdx, _float3 _Rot)
{
	m_KeyFrames[_iIdx].vRot = _Rot;
}

void CCutScene::Add_KeyFrame(CAMERA_KEYFRAME& tNewKeyFrame)
{
	m_KeyFrames.push_back(tNewKeyFrame);

	// 키프레임 위치순(시간순 오름차순)으로 정렬
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](const CAMERA_KEYFRAME& tLeft, const CAMERA_KEYFRAME& tRight)
		{ return tLeft.fKeyTime < tRight.fKeyTime; });

}

void CCutScene::Delete_KeyFrame(_uint iIdx)
{
	if (iIdx >= m_KeyFrames.size())
		return;

	m_KeyFrames.erase(m_KeyFrames.begin() + iIdx);

}

CAMERA_KEYFRAME CCutScene::GetKeyframeAtTime(_float _fKeyTime)
{
	// interpolation : 보간
	for (size_t i = 0; i < m_KeyFrames.size() - 1; ++i)
	{
		if (_fKeyTime >= m_KeyFrames[i].fKeyTime && _fKeyTime < m_KeyFrames[i + 1].fKeyTime)
		{
			const CAMERA_KEYFRAME& tKeyFrame1 = m_KeyFrames[i];
			const CAMERA_KEYFRAME& tKeyFrame2 = m_KeyFrames[i + 1];

			_float t = (_fKeyTime - tKeyFrame1.fKeyTime) / (tKeyFrame2.fKeyTime - tKeyFrame1.fKeyTime);
	
			_vector vNewEyePos = Lerp(XMLoadFloat4(&tKeyFrame1.vEye), XMLoadFloat4(&tKeyFrame2.vEye), t);
			_vector vNewLookAt = Lerp(XMLoadFloat4(&tKeyFrame1.vAt), XMLoadFloat4(&tKeyFrame2.vAt), t);

			CAMERA_KEYFRAME tInterpolatedFrame;
			tInterpolatedFrame.fKeyTime = _fKeyTime;

			XMStoreFloat4(&tInterpolatedFrame.vEye, vNewEyePos);
			XMStoreFloat4(&tInterpolatedFrame.vAt, vNewLookAt);

			tInterpolatedFrame.vRot = m_KeyFrames[i].vRot;

			return tInterpolatedFrame; // 보간된 키프레임 반환
		}
	}
	return m_KeyFrames.back(); // 현재 시간이 어떤 키프레임 범위에도 속하지 않는다면(벡터의 마지막 요소 이후), 벡터의 마지막 키프레임을 반환
}

_vector CCutScene::Lerp(const _fvector& vStart, const _fvector& vEnd, _float fTime)
{
	return vStart + (vEnd - vStart) * fTime;
}


CCutScene * CCutScene::Create(_float _fTimeSpeed, _float _fAcceleration, _bool _Loop)
{
	CCutScene*		pInstance = new CCutScene();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(_fTimeSpeed, _fAcceleration, _Loop)))
	{
		MSG_BOX("Failed to Created : CCutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCutScene::Free()
{
	__super::Free();

}
