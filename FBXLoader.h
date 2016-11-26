
#ifndef FBX_LOADER
#define FBX_LOADER
#include <vector>
using namespace std;
#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include "Mesh.h"
#include "Animation.h"
#include <DirectXMath.h>
using namespace DirectX;
class FBXLoader
{
	// Create a type to represent an array of fbx joints
	typedef std::vector< std::pair< FbxNode*, FbxNodeAttribute* >> JointList;
private:
	FbxManager*		manager;
	FbxIOSettings*	ioSettings;
	FbxImporter*	importer;
	FbxScene*		scene;
	FbxNode*		root;
public:
	FBXLoader();
	~FBXLoader();
	
	bool Load(const std::string& fileName, std::vector<CMesh>& meshes, vector<CAnimation*>& animation, float uvMultiplier);
	bool ProcessNode(FbxNode* node, std::vector<CMesh>& meshes, JointList& unique_joints, float uvMultiplier);
	bool LoadMesh(FbxMesh* fbxMesh, CMesh& mesh, float uvMultiplier);
	bool LoadSkin(FbxMesh* fbx_mesh, CMesh& mesh, JointList& unique_joints);
	bool LoadSkeletonNode(FbxNode* node, CMesh& mesh, JointList& unique_joints, unsigned int& index);
	bool LoadAnimation(FbxAnimLayer* anim_layer, CAnimation& animation, JointList& unique_joints);
	void LoadCurve(FbxAnimCurve* curve, std::vector< float >& key_times);
	XMFLOAT4X4 FBXLoader::convertMatrix(FbxAMatrix& matrix);

};

#endif
