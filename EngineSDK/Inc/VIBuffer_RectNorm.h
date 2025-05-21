#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectNorm : public CVIBuffer
{
private:
	CVIBuffer_RectNorm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_RectNorm(const CVIBuffer_RectNorm& rhs);
public:
	virtual ~CVIBuffer_RectNorm();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static shared_ptr<CVIBuffer_RectNorm> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END