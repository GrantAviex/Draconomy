#include "Deformer.h"


CDeformer::CDeformer()
{
	m_pMesh = nullptr;
}


CDeformer::~CDeformer()
{
}
void CDeformer::Process(const std::vector< XMFLOAT4X4 >& pose)
{
	if (m_pMesh == 0 || m_pMesh->GetJoints().size() == 0)
	{
		return;
	}

	m_vSkin_verts.resize(m_pMesh->GetVerts().size());
	memset(&m_vSkin_verts[0], 0, m_vSkin_verts.size()*sizeof(XMFLOAT3));
	// TODO: Fill out "skin_verts"
	//   "mesh->GetJoints( )" contains the joints in their bind pose <-B
	//   "pose" contains the interpolated joints <-C
	//   "mesh->GetVertices( )" contains the skin space vertices of the model <- V
	//   "mesh->GetInfluences( )" contains a vector of influence information for each unique vertex <- W
	for (size_t i = 0; i < m_vSkin_verts.size(); i++)
	{
		for (size_t j = 0; j < m_pMesh->GetInfluences()[i].size(); j++)
		{
			float weight = m_pMesh->GetInfluences()[i][j].weight;
			unsigned int joint_index = m_pMesh->GetInfluences()[i][j].joint_index;
			XMFLOAT4X4 inversebindpose;
			OrthoNormalInverse(inversebindpose, m_pMesh->GetJoints()[joint_index].world_bind_pose_transform);
			XMFLOAT3 result;
			Multiply(result, m_pMesh->GetVerts()[i].vertInfo.pos, inversebindpose);
			XMFLOAT3 result2;
			Multiply(result2, result, pose[joint_index]);
			result2.x *= weight;
			result2.y *= weight;
			result2.z *= weight;
			m_vSkin_verts[i].x += result2.x;
			m_vSkin_verts[i].y += result2.y;
			m_vSkin_verts[i].z += result2.z;
		}
	}


}

void CDeformer::SetMesh(CMesh* mesh)
{
	this->m_pMesh = mesh;
}

CMesh* CDeformer::GetMesh() const
{
	return m_pMesh;
}

std::vector< XMFLOAT3 >& CDeformer::GetSkinVerts()
{
	return m_vSkin_verts;
}

void CDeformer::OrthoNormalInverse(XMFLOAT4X4& r, const XMFLOAT4X4& m)
{

	
	// TODO: Take the orthonormal inverse of m and store it in r
	r._11 = m._11; //r.xx = m.xx;
	r._21 = m._12; //r.yx = m.xy;
	r._31 = m._13; //r.zx = m.xz;
	r._41 = -((m._11*m._41) + (m._12*m._42) + (m._13*m._43)); //r.wx = -dot_product(m.axis_x, m.axis_w);
	
	r._12 = m._21; //r.xy = m.yx;
	r._22 = m._22; //r.yy = m.yy;
	r._32 = m._23; //r.zy = m.yz;
	r._42 = -((m._21*m._41) + (m._22*m._43) + (m._23*m._43)); //r.wy = -dot_product(m.axis_y, m.axis_w);
	
	r._13 = m._31; //r.xz = m.zx;
	r._23 = m._32; //r.yz = m.zy;
	r._33 = m._33; //r.zz = m.zz;
	r._43 = -((m._31*m._41) + (m._32*m._42) + (m._33*m._43)); //r.wz = -dot_product(m.axis_z, m.axis_w);
	
	r._14 = 0;
	r._24 = 0;
	r._34 = 0;
	r._44 = 1;
	
}

void CDeformer::Multiply(XMFLOAT3& r, const XMFLOAT3& v, const XMFLOAT4X4& m)
{
	// TODO: Multiply m and v, store the result in r
	r.x = m._11*v.x + m._21*v.y + m._31*v.z + m._41 * 1.0f;
	r.y = m._12*v.x + m._22*v.y + m._32*v.z + m._42 * 1.0f;
	r.z = m._13*v.x + m._23*v.y + m._33*v.z + m._43 * 1.0f;

//	r.x = m.axis_x.x*v.x + m.axis_y.x*v.y + m.axis_z.x*v.z + m.axis_w.x * 1;
//	r.y = m.axis_x.y*v.x + m.axis_y.y*v.y + m.axis_z.y*v.z + m.axis_w.y * 1;
//	r.z = m.axis_x.z*v.x + m.axis_y.z*v.y + m.axis_z.z*v.z + m.axis_w.z * 1;

}