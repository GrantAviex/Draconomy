#pragma once

#include <vector>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;
#include <set>

struct TTriangle
{
	XMFLOAT3 verts[3];
	XMFLOAT3 normal;
	
};



class CGridSystem
{
public:
	CGridSystem();
	~CGridSystem();

	bool LoadGrid(std::string TerrainName);
	std::vector<TTriangle*> * GetTriangles(unsigned int *pTriCount, const XMFLOAT3 &position);
	std::vector<TTriangle*>* GetAllTriangles(){ return &m_vEntireTerrain; }
protected:
	int ComputeRow(float x);
	int ComputeColumn(float z);
	std::vector<TTriangle*> m_vEntireTerrain;
	std::vector<TTriangle*> m_vGridArray[100][100];
	std::set<TTriangle*> temptris;
	std::vector<TTriangle*> temptri;
	int m_iMapWidth = 16000;
	int m_iCellWidth = 240;
	int m_iCellHeight = 240;


};

