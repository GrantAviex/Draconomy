#pragma once
#include <vector>
#include "Mesh.h"
#include <DirectXMath.h>
using namespace DirectX;

class CDeformer
{
public:
	CDeformer();
	~CDeformer();

	void Process(const std::vector< XMFLOAT4X4 >& pose);

	void SetMesh(CMesh* mesh);
	CMesh* GetMesh() const;

	std::vector< XMFLOAT3 >& GetSkinVerts();
	bool IsAnimated() { return m_vSkin_verts.size() > 0; }

private:

	void OrthoNormalInverse(XMFLOAT4X4& r, const XMFLOAT4X4& m);
	void Multiply(XMFLOAT3& r, const XMFLOAT3& v, const XMFLOAT4X4& m);


	CMesh* m_pMesh;
	std::vector< XMFLOAT3 > m_vSkin_verts;
};

