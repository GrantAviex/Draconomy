#include "GridSystem.h"
#include "AssetManager.h"
#include "Mesh.h"
#include <vector>
#include "Manager.h"
#include <algorithm>
using namespace std;

CGridSystem::CGridSystem()
{
}
CGridSystem::~CGridSystem()
{
	for (auto& tri : m_vEntireTerrain)
	{
		delete tri;
	}
}

bool CGridSystem::LoadGrid(std::string TerrainName)
{
	vector<CMesh> meshes;
	
	meshes = *(CManager::GetInstance()->GetAssestManager()->GetMeshes(TerrainName));
	std::vector<XMFLOAT3> vertpositions;
	std::vector<XMFLOAT3> normals;
	std::vector<int> indices;

	size_t size = meshes[0].GetVerts().size();
	vertpositions.resize(size);
	normals.resize(size);
	for (size_t i = 0; i < size; i++)	{
		vertpositions[i] = meshes[0].GetVerts()[i].vertInfo.pos;
		normals[i] = meshes[0].GetVerts()[i].vertInfo.nrm;
	}
	size = meshes[0].GetIndices().size();
	indices.resize(size);
	for (size_t i = 0; i < size; i++)
		indices[i] = meshes[0].GetIndices()[i];


	//Load triangles into grid from asset manager.
	for (size_t i = 0; i < size; i += 3) //if issue with winding, swap 2 and the 0
	{
		TTriangle * temptri = new TTriangle;
		temptri->verts[0] = vertpositions[indices[i]];
		temptri->verts[1] = vertpositions[indices[i + 1]];
		temptri->verts[2] = vertpositions[indices[i + 2]];
		temptri->normal.x = (normals[indices[i]].x + normals[indices[i + 1]].x + normals[indices[i + 2]].x) / 3.0f; //does this work?
		temptri->normal.y = (normals[indices[i]].y + normals[indices[i + 1]].y + normals[indices[i + 2]].y) / 3.0f;
		temptri->normal.z = (normals[indices[i]].z + normals[indices[i + 1]].z + normals[indices[i + 2]].z) / 3.0f;
		m_vEntireTerrain.push_back(temptri);
	}

	size_t size2 = m_vEntireTerrain.size();
	for (size_t i = 0; i < size2; i++)
	{
		// find min/max of triangle (x/z)
		float minx, minz, maxx, maxz;
		minx = min(m_vEntireTerrain[i]->verts[2].x,min(m_vEntireTerrain[i]->verts[0].x, m_vEntireTerrain[i]->verts[1].x));
		minz = min(m_vEntireTerrain[i]->verts[2].z, min(m_vEntireTerrain[i]->verts[0].z, m_vEntireTerrain[i]->verts[1].z));
		maxx = max(m_vEntireTerrain[i]->verts[2].x, max(m_vEntireTerrain[i]->verts[0].x, m_vEntireTerrain[i]->verts[1].x));
		maxz = max(m_vEntireTerrain[i]->verts[2].z, max(m_vEntireTerrain[i]->verts[0].z, m_vEntireTerrain[i]->verts[1].z));

		//Find Row/COl the min/Max rests in
		int MaxX, MinX, MaxZ, MinZ;
		MaxX = ComputeRow(maxx); // Will retrun 0-49
		MaxZ = ComputeColumn(maxz); // **
		MinZ = ComputeColumn(minz); // **
		MinX = ComputeRow(minx); // **
		// Place into all buckets its min/max stretch

		if (MinX == MaxX) //side triangle
		{
			while (MaxZ < MinZ)
			{
				m_vGridArray[MinX][MaxZ].push_back(m_vEntireTerrain[i]);
				++MaxZ;
			}
		}
		else if (MinZ == MaxZ) //other side triangle
		{
			while (MinX < MaxX)
			{
				m_vGridArray[MinX][MaxZ].push_back(m_vEntireTerrain[i]);
				++MinX;
			}
		}
		else // flat triangle
		{
			int StartX = MinX;
			while (MaxZ < MinZ)
			{ 
				while (StartX < MaxX)
				{
					m_vGridArray[StartX][MaxZ].push_back(m_vEntireTerrain[i]);
					++StartX;
				}
				++MaxZ;
				StartX = MinX;

			}
		}
	
		//win
	}




	return true;
}
std::vector<TTriangle*> * CGridSystem::GetTriangles(unsigned int *pTriCount, const XMFLOAT3 &position)
{
	int PosRow = ComputeRow(position.x);
	int PosCol = ComputeColumn(position.z);

	temptris.clear();
	temptri.clear();

	//Huuurrrrrrrrrrrrrrr
	if (PosRow <= 1 || PosRow >= 99 || PosCol <= 1 || PosCol >= 99)
	{
		return &temptri;
	}

	for (auto itr = m_vGridArray[PosRow][PosCol].begin(); itr != m_vGridArray[PosRow][PosCol].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow][PosCol+1].begin();itr != m_vGridArray[PosRow][PosCol+1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow+1][PosCol + 1].begin(); itr != m_vGridArray[PosRow+1][PosCol + 1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow-1][PosCol + 1].begin(); itr != m_vGridArray[PosRow-1][PosCol + 1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow-1][PosCol].begin(); itr != m_vGridArray[PosRow-1][PosCol].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow+1][PosCol].begin(); itr != m_vGridArray[PosRow+1][PosCol].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow][PosCol - 1].begin(); itr != m_vGridArray[PosRow][PosCol - 1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow - 1][PosCol - 1].begin(); itr != m_vGridArray[PosRow - 1][PosCol - 1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	for (auto itr = m_vGridArray[PosRow+1][PosCol - 1].begin(); itr != m_vGridArray[PosRow+1][PosCol - 1].end(); itr++)
	{
		temptris.insert((*itr));
	}
	return &m_vEntireTerrain;
//	return &m_vGridArray[PosRow][PosCol];
	for (auto itr = temptris.begin(); itr != temptris.end(); itr++)
	{
		temptri.push_back((*itr));
	}
	return &temptri;
}
int CGridSystem::ComputeRow(float x)
{
	int test = abs((int)(((m_iMapWidth / 2.0f) + x) / m_iCellWidth));
	if (test >= 99)
		test = 98;
	return test;
}
int CGridSystem::ComputeColumn(float z)
{
	int test = abs((int)(((m_iMapWidth / 2.0f) - z) / m_iCellHeight));
	if (test >= 99)
		test = 98;
	return test;
}