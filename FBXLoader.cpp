
#include "FBXLoader.h"

FBXLoader::FBXLoader()
{
}

FBXLoader::~FBXLoader()
{
	//cleanup
	if (root)
		root->Destroy();
	if (scene)
		scene->Destroy();
	if (importer)
		importer->Destroy();
	if (ioSettings)
		ioSettings->Destroy();
	if (manager)
		manager->Destroy();
}

XMFLOAT4X4 RightHandToLeftHand(XMFLOAT4X4 input)
{
	XMFLOAT4X4 output = input;
	output._13 *= -1;
	output._23 *= -1;
	output._33 *= -1;
	output._43 *= -1;
	output._31 *= -1;
	output._32 *= -1;
	output._33 *= -1;
	output._34 *= -1;
	return output;
}

bool FBXLoader::Load(const std::string& fileName, std::vector<CMesh>& meshes, vector<CAnimation*>& animation, float uvMultiplier)
{
	//manager
	manager = FbxManager::Create();
	if (manager == nullptr)
		return false;

	//io settings
	ioSettings = FbxIOSettings::Create(manager, IOSROOT);
	if (ioSettings == nullptr)
		return false;

	manager->SetIOSettings(ioSettings);

	//importer
	importer = FbxImporter::Create(manager, "");
	if (importer == nullptr)
		return false;

	//init importer
	if (importer->Initialize(fileName.c_str(), -1, ioSettings) == false)
	{
		//		MessageBox(NULL, L"Importer Initialize Failed!", L"Error", MB_OK);
		return false;
	}

	//scene
	scene = FbxScene::Create(manager, "gameScene");
	if (scene == nullptr)
		return false;

	//load scene
	if (importer->Import(scene) == false)
		return false;


	// Our list of unique joints
	JointList unique_joints;

	root = scene->GetRootNode();
	if (ProcessNode(root, meshes, unique_joints, uvMultiplier) == false)
		return false;

	// Get the number of animation stacks
	int num_anim_stacks = scene->GetSrcObjectCount< FbxAnimStack >();

	FbxAnimStack* anim_stack;
	for (int i = 0; i < num_anim_stacks; ++i)
	{
		// Get the current animation stack
		anim_stack = scene->GetSrcObject< FbxAnimStack >(i);
		CAnimation* anim = new CAnimation();
		FbxString animation_name = anim_stack->GetName();
		anim->m_sName = animation_name;

		// Get the number of animation layers in the current animation stack
		int num_anim_layers = anim_stack->GetMemberCount< FbxAnimLayer >();

		FbxAnimLayer* anim_layer;
		for (int j = 0; j < num_anim_layers; ++j)
		{
			anim_layer = anim_stack->GetMember< FbxAnimLayer >(j);

			LoadAnimation(anim_layer, *anim, unique_joints);
		}
		animation.push_back(anim);
	}

	return true;
}

bool FBXLoader::ProcessNode(FbxNode* node, std::vector<CMesh>& meshes, JointList& unique_joints, float uvMultiplier)
{
	if (node->GetMesh())
	{
		CMesh daMesh;
		fbxsdk_2015_1::FbxMesh* fbMesh = node->GetMesh();
		FBXLoader::LoadMesh(fbMesh, daMesh,1);
		FBXLoader::LoadSkin(fbMesh, daMesh, unique_joints);
		meshes.push_back(daMesh);
	}
	for (int childIndex = 0; childIndex < node->GetChildCount(false); childIndex++)
	{
		ProcessNode(node->GetChild(childIndex), meshes, unique_joints,1);
	}
	return true;
}

bool FBXLoader::LoadMesh(FbxMesh* fbxMesh, CMesh& mesh, float uvMultiplier)
{
	int polyTotal = fbxMesh->GetPolygonCount();
	FbxVector4* controlPoints = fbxMesh->GetControlPoints();
	int vertId = 0;
	//polys
	for (int polyCounter = 0; polyCounter < polyTotal; ++polyCounter)
	{
		int polyVertTotal = fbxMesh->GetPolygonSize(polyCounter);
		//verts
		for (int polyVertCounter = 0; polyVertCounter < polyVertTotal; ++polyVertCounter)
		{
			UniqueVertex uniqueVert;

			int cpIndex = fbxMesh->GetPolygonVertex(polyCounter, polyVertCounter);

			uniqueVert.controlPointIndex = cpIndex;
			uniqueVert.vertInfo.pos.x = (float)controlPoints[cpIndex].mData[0];
			uniqueVert.vertInfo.pos.y = (float)controlPoints[cpIndex].mData[1];
			uniqueVert.vertInfo.pos.z = (float)controlPoints[cpIndex].mData[2] * -1.0f;

			//tex coords
			int uvTotal = fbxMesh->GetElementUVCount();
			for (int uvCounter = 0; uvCounter < uvTotal; ++uvCounter)
			{
				FbxGeometryElementUV* geoUV = fbxMesh->GetElementUV(uvCounter);

				FbxLayerElement::EMappingMode mappingMode = geoUV->GetMappingMode();
				FbxLayerElement::EReferenceMode refMode = geoUV->GetReferenceMode();

				int directIndex = -1;

				if (mappingMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
						directIndex = cpIndex;
					else if (refMode == FbxGeometryElement::eIndexToDirect)
						directIndex = geoUV->GetIndexArray().GetAt(cpIndex);
				}
				else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect || refMode == FbxGeometryElement::eIndexToDirect)
						directIndex = fbxMesh->GetTextureUVIndex(polyCounter, polyVertCounter);
				}

				if (directIndex != -1)
				{
					FbxVector2 uv = geoUV->GetDirectArray().GetAt(directIndex);

					uniqueVert.vertInfo.uv.x = (float)uv.mData[0];
					uniqueVert.vertInfo.uv.y = (1.0f - (float)uv.mData[1]);
				}
			}

			//norms
			int normTotal = fbxMesh->GetElementNormalCount();
			for (int normCounter = 0; normCounter < normTotal; ++normCounter)
			{
				FbxGeometryElementNormal* geoNorm = fbxMesh->GetElementNormal(normCounter);

				FbxLayerElement::EMappingMode mappingMode = geoNorm->GetMappingMode();
				FbxLayerElement::EReferenceMode refMode = geoNorm->GetReferenceMode();

				int directIndex = -1;

				if (mappingMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(cpIndex);

						uniqueVert.vertInfo.nrm.x = (float)norm[0];
						uniqueVert.vertInfo.nrm.y = (float)norm[1];
						uniqueVert.vertInfo.nrm.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray().GetAt(cpIndex);
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(id);

						uniqueVert.vertInfo.nrm.x = (float)norm[0];
						uniqueVert.vertInfo.nrm.y = (float)norm[1];
						uniqueVert.vertInfo.nrm.z = (float)norm[2] * -1.0f;

					}
				}
				else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(vertId);

						uniqueVert.vertInfo.nrm.x = (float)norm[0];
						uniqueVert.vertInfo.nrm.y = (float)norm[1];
						uniqueVert.vertInfo.nrm.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray().GetAt(vertId);
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(id);

						uniqueVert.vertInfo.nrm.x = (float)norm[0];
						uniqueVert.vertInfo.nrm.y = (float)norm[1];
						uniqueVert.vertInfo.nrm.z = (float)norm[2] * -1.0f;
					}
				}
			}

			std::vector<UniqueVertex>& uniqueVerts = mesh.GetVerts();

			size_t size = uniqueVerts.size();
			size_t i;
			for (i = 0; i < size; ++i)
			{
				if (uniqueVert == uniqueVerts[i])
					break;
			}

			if (i == size)
				uniqueVerts.push_back(uniqueVert);

			mesh.GetIndices().push_back(i);

			++vertId;
		}
	}

	return true;
}
bool FBXLoader::LoadSkin(FbxMesh* fbx_mesh, CMesh& mesh, JointList& unique_joints)
{
	mesh.GetInfluences().resize(mesh.GetVerts().size());
	for (int i = 0; i < fbx_mesh->GetDeformerCount(FbxDeformer::eSkin); i++)
	{

		FbxSkin* daSkin = reinterpret_cast<FbxSkin *>(fbx_mesh->GetDeformer(i, FbxDeformer::eSkin));

		for ( int clusterIndex = 0; clusterIndex < daSkin->GetClusterCount(); clusterIndex++)
		{
			unsigned int returnedIndex;
			fbxsdk_2015_1::FbxCluster* daCluster = daSkin->GetCluster(clusterIndex);
			std::vector<CMesh::JointInfluence> vecInfluences;


			FBXLoader::LoadSkeletonNode(daCluster->GetLink(), mesh, unique_joints, returnedIndex);
			XMFLOAT4X4 ogCluster, linkedCluster;
			fbxsdk_2015_1::FbxAMatrix tempMatrix, tempLinkMatrix;
			daCluster->GetTransformMatrix(tempMatrix);
			daCluster->GetTransformLinkMatrix(tempLinkMatrix);

			for (unsigned int row = 0; row < 4; row++)
			{
				for (unsigned int col = 0; col < 4; col++)
				{
					ogCluster.m[row][col] = (float)(tempMatrix.Get(row, col));
					linkedCluster.m[row][col] = (float)(tempLinkMatrix.Get(row, col));
				}
			}
			XMMATRIX mat = XMMatrixMultiply(XMLoadFloat4x4(&ogCluster), XMLoadFloat4x4(&linkedCluster));
			XMStoreFloat4x4(&ogCluster, mat);
			mesh.GetJoints()[returnedIndex].world_bind_pose_transform = RightHandToLeftHand(ogCluster);
			for (int ctrlPtIndex = 0; ctrlPtIndex < daCluster->GetControlPointIndicesCount(); ctrlPtIndex++)
			{
				//	daCluster->GetControlPointIndices()
				CMesh::JointInfluence jInfluence;
				jInfluence.joint_index = returnedIndex;
				jInfluence.weight = (float)(daCluster->GetControlPointWeights()[ctrlPtIndex]);
				for (unsigned int vertIndex = 0; vertIndex < mesh.GetVerts().size(); vertIndex++)
				{
					if (mesh.GetVerts()[vertIndex].controlPointIndex == daCluster->GetControlPointIndices()[ctrlPtIndex])
						mesh.GetInfluences()[vertIndex].push_back(jInfluence);

				}
			}


		}
	}
	return true;
}
bool FBXLoader::LoadSkeletonNode(FbxNode* node, CMesh& mesh, JointList& unique_joints, unsigned int& index)
{
	if (!node->GetSkeleton())
	{
		return false;
	}
	for (unsigned int i = 0; i < unique_joints.size(); i++)
	{
		unsigned long long mine = node->GetUniqueID();
		unsigned long long theirs = unique_joints[i].first->GetUniqueID();
		if (mine == theirs)
		{
			index = i;
			return true;
		}

	}
	//must be unique
	CMesh::Joint daJoint;
	mesh.GetJoints().push_back(daJoint);
	index = mesh.GetJoints().size() - 1;
	std::pair<fbxsdk_2015_1::FbxNode*, fbxsdk_2015_1::FbxNodeAttribute*> daPair;
	daPair.first = node;
	daPair.second = node->GetSkeleton();
	unique_joints.push_back(daPair);

	//recurse child
	for (int childIndex = 0; childIndex < node->GetChildCount(false); childIndex++)
	{
		LoadSkeletonNode(node->GetChild(childIndex), mesh, unique_joints, index);
		daJoint.children.push_back(index);
	}
	if (node->GetParent())
	{
		LoadSkeletonNode(node->GetParent(), mesh, unique_joints, index);
		daJoint.parent = index;
	}
	return true;
}
bool FBXLoader::LoadAnimation(FbxAnimLayer* anim_layer, CAnimation& animation, JointList& unique_joints)
{
	fbxsdk_2015_1::FbxAnimCurve* transCurve;
	fbxsdk_2015_1::FbxAnimCurve* rotCurve;
	fbxsdk_2015_1::FbxAnimCurve* scaleCurve;
	animation.m_vChannels.resize(unique_joints.size());
	for (size_t i = 0; i < unique_joints.size(); i++)
	{
		auto joint = unique_joints[i].first;
		transCurve = unique_joints[i].first->LclTranslation.GetCurve(anim_layer);
		rotCurve = unique_joints[i].first->LclRotation.GetCurve(anim_layer);
		scaleCurve = unique_joints[i].first->LclScaling.GetCurve(anim_layer);
		std::vector< float > key_times;

		LoadCurve(transCurve, key_times);
		LoadCurve(rotCurve, key_times);
		LoadCurve(scaleCurve, key_times);

		CAnimation::TChannel c;
		for (size_t j = 1; j < key_times.size(); j++)
		{
			FbxTime time;
			time.SetSecondDouble(key_times[j]);
			FbxAMatrix fbx_transform = joint->EvaluateGlobalTransform(time);
			XMFLOAT4X4 transform = RightHandToLeftHand(convertMatrix(fbx_transform));

			CAnimation::TKey key;
			key.m_mNode = transform;
			key.m_fTime = float(time.GetSecondDouble());
			c.m_vKeys.push_back(key);
		}
		animation.m_vChannels[i] = c;

	}

	return true;
}
void FBXLoader::LoadCurve(FbxAnimCurve* curve, std::vector< float >& key_times)
{
	if (curve)
	{
		for (int i = 0; i < curve->KeyGetCount(); i++)
		{
			bool unique = true;
			for (size_t j = 0; j < key_times.size(); j++)
			{
				if (curve->KeyGet(i).GetTime().GetSecondDouble() == key_times[j])
				{
					unique = false;
					break;
				}
			}
			if (unique)
				key_times.push_back((float)(curve->KeyGet(i).GetTime().GetSecondDouble()));
		}
	}
}
XMFLOAT4X4 FBXLoader::convertMatrix(FbxAMatrix& matrix)
{
	XMFLOAT4X4 transform;
	for (unsigned int y = 0; y < 4; y++)
	{

		for (unsigned int x = 0; x < 4; x++)
			transform.m[x][y] = (float)matrix.Get(x, y);
	}

	return transform;
}