#pragma once

#include "Component.h"
#include "Animation.h"


namespace hwiVoro
{
	class container;
}

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
public:
	typedef struct tagModelMaterial
	{
		weak_ptr<CTexture> pTextures[AI_TEXTURE_TYPE_MAX];
	}MODEL_MATERIAL;
	enum FRACTURE_MODE
	{
		VORONOI,
		SPIDER_WEB,
		HONEYCOMB,
		VOXEL,
		FRACTURE_END
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);

public:
	virtual ~CModel();

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_NumAnimation() const {
		return (_uint)m_Animations.size();
	}

	_bool Get_Finished() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	}

	void Set_Finished(_bool isFinished) {
		m_Animations[m_iCurrentAnimIndex]->Set_Finished(isFinished);
	}
	void Chander_Material(_uint idx, shared_ptr<CTexture> pTexture, aiTextureType eType);
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	void Set_BoneMatrixPtr(const _char* pBoneName, const _float4x4& Transform);
	void Set_BoneMatrix(_uint iBoneIdx, const _float4x4& Transform);
	void Set_BoneMatrix(_uint iBoneIdx, _fmatrix Transform);
	const _float4x4* Get_BoneBasicMatrixPtr(const _char* pBoneName) const;
	
	const _vector Get_BonePosition(const _char* pBoneName, _vector Pos) const;

	string Get_BoneName(_uint idx);
	const vector<shared_ptr<CBone>>& Get_Bones() { return m_Bones; }

	void Set_AnimIndex(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndex(m_AnimMapping[animName], isLoop);
	}
	void Set_AnimIndexNonCancle(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndexNonCancle(m_AnimMapping[animName], isLoop);
	}
	void Set_AnimIndex_NoInterpole(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndex(m_AnimMapping[animName], isLoop,false);
	}
	void Set_AnimIndexNonCancle_NoInterpole(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndexNonCancle(m_AnimMapping[animName], isLoop, false);
	}
	void Set_AnimIndex(_uint iAnimIndex, _bool isLoop, _bool bInterpole = true)
	{
		if (m_isLoop && m_iCurrentAnimIndex == iAnimIndex)
			return;

		if (m_iNumAnimations <= iAnimIndex)
			iAnimIndex = 0;

		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fPreFramePos = m_Animations[m_iPreAnimIndex]->m_fCurrentTrackPosition;

		m_iCurrentAnimIndex = iAnimIndex;
		m_Animations[m_iCurrentAnimIndex]->Reset();

		if (bInterpole)
			m_fRemainTime = 1.f;
		else
			m_fRemainTime = 0.f;
		m_isLoop = isLoop;
	}
	void Set_AnimIndexNonCancle(_uint iAnimIndex, _bool isLoop, _bool bInterpole = true) {
		if (iAnimIndex == m_iCurrentAnimIndex || iAnimIndex == m_iPreAnimIndex)
			return;

		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fPreFramePos = m_Animations[m_iPreAnimIndex]->m_fCurrentTrackPosition;

		m_iCurrentAnimIndex = iAnimIndex;
		m_Animations[m_iCurrentAnimIndex]->Reset();

		if (bInterpole)
			m_fRemainTime = 1.f;
		else
			m_fRemainTime = 0.f;

		m_isLoop = isLoop;
	}

	void Set_AnimSpeed(_float fSpeed) {
		m_Animations[m_iCurrentAnimIndex]->Set_AnimationMultiplier(fSpeed);
	}

	shared_ptr<class CMesh> Get_Mesh(_uint iMeshIndex) {
		if (m_iNumMeshes <= iMeshIndex)
			return nullptr;

		return m_Meshes[iMeshIndex];
	}
	_float Get_Duration() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
	}
	_float Get_Current_Track_Position() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Current_Track_Posiiton();
	}
	_float Get_Current_Anim_Track_Position_Percentage() {
		return Get_Current_Track_Position() / Get_Duration();
	}
	void  Set_CurrentTrackPosition(_float fCurrentTrackPosition) {
		m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fCurrentTrackPosition);
	}
	_float Get_Animation_PlayTime() {
		return Get_Duration() / m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
	}
public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Init_Model(const char* sModelKey, const string& szTexturePath = "", _uint iTimeScaleLayer = 0);
	void Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer);
	void Reset_Model();
	void Reset_Bones();
	void Reset_Bones_PreTransform(_fmatrix pret);

	void Play_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix);
	void Play_Non_InterPole_Animation(_float fTimeDelta, _float4x4& outMatrix);
	void Set_AnimFrame(_float fFramePos);

	HRESULT Bind_Buffers(_uint iMeshIndex);
	HRESULT Bind_Material(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_Mat_MatIdx(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex);

	_uint Get_CurrentAnimIndex() {
		return m_iCurrentAnimIndex;
	}

	_bool Push_Back_Mesh(shared_ptr<MESH_DATA> pMeshData);
	_bool Push_Back_Mesh(shared_ptr<CMesh> pMesh);

	_bool Replace_Mesh(shared_ptr<MESH_DATA> pMeshData, _uint iIdx, _bool bPreTrans = true);
	_bool Replace_Mesh(shared_ptr<CMesh> pMesh, _uint iIdx);

public:
	vector<shared_ptr<class CAnimation>>& Get_Animation_List() { return m_Animations; }
	HRESULT ExportAnimInfo(const string& strFolderPath);

	_uint Get_StringAnimIndex(const string& animName) {
		return m_AnimMapping[animName];
	}

public:
	HRESULT SetAnimCallback(const string& strAnimName, _uint iCallbackPos, function<void()> callback);
	HRESULT SetLastAnimCallback(const string& strAnimName, function<void()> callback);
	shared_ptr<MODEL_DATA> Get_ModelData() {
		return m_pModelData;
	}
	void Set_ModelType(MODEL_TYPE eModelType) { m_eModelType = eModelType; }

	void UpdateVertexInfo();
private:
	/* 디자이너가 제공해준 파일의 정보를 읽어서 aiScene에 저장해주는 역활을 수행한다. */
	Assimp::Importer			m_Importer = {};

	/* 모델에 대한 모든 정보를 다 들고 있는 객체다. */
	const aiScene* m_pAIScene = { nullptr };

protected:
	shared_ptr<MODEL_DATA>			m_pModelData;
	string							m_szModelKey;

private:
	MODEL_TYPE						m_eModelType = { MODEL_TYPE::TYPE_END };
	_float4x4						m_TransformMatrix = {};

private:
	_uint							m_iNumMeshes = {};
	vector<shared_ptr<class CMesh>>	m_Meshes;

private:
	_uint							m_iNumMaterials = {};
	vector<MATERIAL_MESH>			m_Materials;

private:
	/* 이 모델 전체의 뼈들을 모아놓은 벡터 컨테이너. */
	vector<shared_ptr<class CBone>>			m_Bones;

public:
	_bool IsRootAnim() const {
		return m_Animations[m_iCurrentAnimIndex]->m_isRootAnim;
	}
	_bool IsForcedNonRoot() const {
		return m_Animations[m_iCurrentAnimIndex]->m_isForcedNonRootAnim;
	}

private:
	_bool										m_isLoop = { false };
	_uint										m_iCurrentAnimIndex = {};

	_int										m_iPreAnimIndex = -1;
	_float										m_fPreFramePos = 0.f;
	_float										m_fRemainTime = 0.f;

	_uint										m_iNumAnimations = { 0 };
	vector<shared_ptr<class CAnimation>>		m_Animations;
	map<string, _uint>							m_AnimMapping;

#pragma region BLENDANIM
public:
	void Play_Animation_Blend(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix);
	//void Set_AnimIndex_Blend(_uint iAnimIndex, _bool isLoop, const string& strTag, _bool bInterpole = true);
	void Set_AnimIndex_Blend(vector<_uint>& AnimIndices, vector<_float>& AnimWeights, _bool bLoop = false);
	void Set_AnimIndex_Blend(vector<string>& AnimStrings, vector<_float>& AnimWeights, _bool bLoop = false);
private:
	vector<_uint>			m_vecAnimIndex;
	vector<_float>			m_vecAnimWeight;

	vector<_uint>			m_vecPreAnimIndex;
	vector<_float>			m_vecPreFramePos;
	vector<_float>			m_vecRemainTime;

	vector<_bool>			m_vecFinished;


#pragma endregion //BLENDANIM

#pragma region //Mesh Cut
public:
	pair<vector<_bool>, vector<_bool>> Divide(_uint iMeshIdx, const vector<VTXANIMMESH>& nexVertices, const vector<_uint3>& Up, const vector<_uint3>& Down, const _float4& plane);
	void Divide(_uint iMeshIdx, const vector<VTXMESH>& nexVertices, const vector<_uint3>& Up, const vector<_uint3>& Down, const _float4& plane);

	void Reconstruct_Buffer(_uint iMeshIdx, const vector<VTXANIMMESH>& nexVertices, const vector<_uint3>& Indicies, vector<VTXANIM>& outVert, vector<_uint>& outInd, const _float4& normal);
	void Reconstruct_Buffer(_uint iMeshIdx, const vector<VTXMESH>& nexVertices, const vector<_uint3>& Indicies, vector<VTXMESH>& outVert, vector<_uint>& outInd, const _float4& normal);

	shared_ptr<MESH_DATA> Reconstruct_MeshData(_uint iOriMeshIdx, const vector<VTXANIM>& vert, const vector<_uint>& indicies);
	shared_ptr<MESH_DATA> Reconstruct_MeshData(_uint iOriMeshIdx, const vector<VTXMESH>& vert, const vector<_uint>& indicies);
	void Make_NewFace(const map<_uint, vector<_uint>>& newFaceEdge, vector<_uint>& currPath, map<_uint,_uint>& pathIdx, vector<vector<_uint>>& cycles, _uint currVertex, set<_uint>& visited);

	vector<_bool> Valid_Bones(const vector<VTXANIM>& vertices);

private:
	_uint m_iSplitCount = 0;
#pragma endregion 
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();

	void Mesh_Array(shared_ptr<MODEL_DATA> pModelData);
	void Material_Array(const _char* pModelFilePath, int i);

	//void Create_ORM_Material(MODEL_MATERIAL& Out_Material, const _uint In_iMaterialIndex, const _char* pModelFilePath);
	HRESULT Create_Materials(const _char* pModelFilePath);
	HRESULT Create_Bone(shared_ptr<NODE_DATA> pNode, _int iParentIndex);
	HRESULT Create_Animations();

	shared_mutex m_Mtx;
	shared_mutex m_MaterialMtx;
	vector<shared_ptr<MESH_DATA>> Make_VoronoiMeshData(_uint iNumCell, const _float3& vScale, vector<_uint>& MeshToPid, vector<vector<int>>& neighbor,
		const _float3& vCenter, _uint iNumAngle, _uint iNumLayer, FRACTURE_MODE eMode);
	void Do_Intersect(_uint iNumCell,const vector<_uint>& Pid , const vector<shared_ptr<MESH_DATA>>& meshData, vector<shared_ptr<MODEL_DATA>>& out);

public:
	static shared_ptr<CModel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	vector<shared_ptr<MODEL_DATA>> Do_Voronoi(_uint iNumCell, const _float3& vScale, vector<vector<int>>& neighbor,
		const _float3& vCenter = { 0.5f,0.5f,0.5f },_uint iNumAngle = 4 ,_uint iNumLayer = 4 ,FRACTURE_MODE eMode = VORONOI);
	
	vector<shared_ptr<MODEL_DATA>> Blast_Voronoi(_uint iNumCell, const _float3& vScale, vector<vector<int>>& neighbor,
		const _float3& vCenter = { 0.5f,0.5f,0.5f }, _uint iNumAngle = 4, _uint iNumLayer = 4, FRACTURE_MODE eMode = VORONOI);
	vector<shared_ptr<MODEL_DATA>> Blast_Voronoi(_uint iNumCell);


	static shared_ptr<CModel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, shared_ptr<MODEL_DATA> pModelData, _fmatrix TransformMatrix);
	
	void Intersect(unique_ptr<class BSP_Node>& ori, unique_ptr<BSP_Node>& cell, shared_ptr<MESH_DATA>& out, _uint iBlendID);
	void Free();
};

struct posBlendIdx
{
	_float3 pos;
	XMUINT4 BlendIdx;
};

struct posEq
{
	static const _float eps;
	bool operator()(const posBlendIdx& _a, const posBlendIdx& _b) const {
		_float3 a{ floorf(_a.pos.x / eps)* eps,  floorf(_a.pos.y / eps) * eps, floorf(_a.pos.z / eps) * eps };
		_float3 b{ floorf(_b.pos.x / eps) * eps, floorf(_b.pos.y / eps) * eps, floorf(_b.pos.z / eps) * eps};

		return a.x == b.x&&
			a.y == b.y &&
			a.z == b.z;
	}
};
struct fltEq
{
	static const _float eps;
	bool operator()(const _float3& _a, const _float3& _b) const {
		_float3 a{ floorf(_a.x / eps) * eps,  floorf(_a.y / eps) * eps, floorf(_a.z / eps) * eps };
		_float3 b{ floorf(_b.x / eps) * eps, floorf(_b.y / eps) * eps, floorf(_b.z / eps) * eps };

		return a.x == b.x &&
			a.y == b.y &&
			a.z == b.z;
	}
};
const _float posEq::eps = 1e-2f;
const _float fltEq::eps = 1e-2f;

END

namespace std {
	template <>
	struct hash<Engine::posBlendIdx> {
		std::size_t operator()(const Engine::posBlendIdx& _v) const {
			_float3 v{ floorf(_v.pos.x / Engine::posEq::eps) * Engine::posEq::eps, floorf(_v.pos.y / Engine::posEq::eps) * 
				Engine::posEq::eps, floorf(_v.pos.z / Engine::posEq::eps) * Engine::posEq::eps };

			size_t h = 0;
			h ^= std::hash<float>()(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(_v.BlendIdx.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(_v.BlendIdx.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(_v.BlendIdx.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(_v.BlendIdx.w) + 0x9e3779b9 + (h << 6) + (h >> 2);
			return h;
		}
	};
	template <>
	struct hash<Engine::_float3> {
		std::size_t operator()(const Engine::_float3& _v) const {
			_float3 v{ floorf(_v.x / Engine::fltEq::eps) * Engine::fltEq::eps, floorf(_v.y / Engine::fltEq::eps) *
				Engine::fltEq::eps, floorf(_v.z / Engine::fltEq::eps) * Engine::fltEq::eps };

			size_t h = 0;
			h ^= std::hash<float>()(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
			return h;
		}
	};
}
