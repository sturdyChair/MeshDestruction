#pragma once
#include "Engine_Defines.h"
namespace hwiVoro
{
	class CHV_Cell_Base;
}

BEGIN(Engine)

class CVoronoi_Manager
{
private:
	CVoronoi_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CVoronoi_Manager();

	HRESULT Initialize();

	void CellToMeshData(hwiVoro::CHV_Cell_Base& cell, shared_ptr<MESH_DATA>& pMeshData, _float3 vOffset, const string& szName, _uint pid, const _float3& vScale, MESH_VTX_INFO& Info);
	void TestFunction();

	void Debug_Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;

	shared_ptr<class CMesh> m_pTestMesh;
	vector<shared_ptr<class CMesh>> m_vecMesh;

public:
	static shared_ptr<CVoronoi_Manager> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END