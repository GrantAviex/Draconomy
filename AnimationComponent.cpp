#include "AnimationComponent.h"

#ifndef IARESERVER
#include "Game.h"
#include "Renderer.h"
#include "Interpolator.h"
#include "Deformer.h"
#include "Animation.h"
#include "LocalUpdateManager.h"
#include "../DirectXTK/Inc/DDSTextureLoader.h"
#endif

#include "AssetManager.h"


CAnimationComponent::CAnimationComponent( CGameObject* _owner )
	: CComponent( _owner, "AnimationComponent" )
{
	//MESH NAME is set through the xml
	
}


CAnimationComponent::~CAnimationComponent()
{
#ifndef IARESERVER
	CRenderer* renderer = CGame::GetInstance()->GetRenderer();
	renderer->DeleteAnimator(m_Owner);
	CGame::GetInstance()->GetLocalUpdateManager()->RemoveAnimator(this);
#endif
}


void CAnimationComponent::ShutdownAnimationData()
{
#ifndef IARESERVER
	for( unsigned fIt = 0; fIt < keyframeCount; fIt++ )
	{
		delete m_pInterpolators[ fIt ];
		delete m_pDeformers[ fIt ];
	}
	
	m_pInterpolators.clear();
	m_pDeformers.clear();
#endif
}


std::string CAnimationComponent::GetModelTag( void )
{
	return m_modelTag;
}

void CAnimationComponent::SetModelTag( string _modelTag )
{
	m_modelTag = _modelTag;
}
void CAnimationComponent::InitializeAnimationData()
{
#ifndef IARESERVER
	CAssetManager*	assetManager = CGame::GetInstance()->GetAssestManager();
	CDeformer*		newDeformer = nullptr;
	CInterpolator*	newInterpolator	= nullptr;

	// Tell the asset manager to load the animations for this models if it hasn't already
	bool loadedAnimations = assetManager->HasModelAnimationFrames( m_modelTag );
	if( loadedAnimations == false )
	{
		for( unsigned it = 0; it < keyframeCount; it++ )
		{
			//assetManager->LoadAnimationFromBinary( m_modelTag, animationFileNames[ it ] );
			assetManager->LoadAnimationFromFBX( m_modelTag, animationFileNames[ it ], this );
		}
	}

	//Grab the animation data from the AssetManager based on the name of the mesh
	keyframes		= assetManager->GetModelAnimationFrames( m_modelTag );
	keyframeCount = keyframes->size();
	//Create Deformer per animation
	//Create Interpolator per animation
	//set deformer/interpolator data members;
	if (keyframeCount > 0)
	{

		for (unsigned fIt = 0; fIt < keyframeCount; fIt++)
		{
			newDeformer = new CDeformer();
			newInterpolator = new CInterpolator();
			m_pDeformers.push_back(newDeformer);
			m_pInterpolators.push_back(newInterpolator);

			m_pInterpolators[fIt]->SetAnimation((*keyframes)[fIt]);
			// m_pInterpolators[ fIt ]->SetTime( 0.0f );

			std::vector<CMesh>* m = assetManager->GetMeshes(m_modelTag);
			CMesh* mesh = &m->front();

			m_pDeformers[fIt]->SetMesh(mesh);
		}

		CMesh*		mesh = m_pDeformers[0]->GetMesh();
		vertCount = mesh->GetVerts().size();
		indexCount = mesh->GetIndices().size();
		std::vector<UniqueVertex>& modelVertices = mesh->GetVerts();

		m_vIndices = new std::vector < unsigned > ;
		std::vector<unsigned int>& ind = mesh->GetIndices();
		(*m_vIndices) = ind;

		// The normals and texture coordinates aren't changing so just set them once during initialization
		vector< std::vector< CMesh::JointInfluence >> vec = mesh->GetInfluences();
		for (unsigned vIt = 0; vIt < vertCount; vIt++)
		{
			AnimVert vert;
			vert.vertInfo = modelVertices[vIt].vertInfo;
			jointInfo joint;
			ZeroMemory(&joint, sizeof(jointInfo));
			for (size_t i = 0; i < vec[vIt].size(); i++)
			{
				joint.joint_Index[i] = (float)(vec[vIt][i].joint_index);
				joint.weight[i] = vec[vIt][i].weight;
			}
			vert.boneInfo = joint;
			m_animatedVerts.push_back(vert);
		}
	}
	LoadTexture(this->GetModelTag(), &textureInfo);
#endif
}

void CAnimationComponent::CloneAnimationData()
{
#ifndef IARESERVER
	CAssetManager*	assetManager = CGame::GetInstance()->GetAssestManager();
	CDeformer*		newDeformer = nullptr;
	CInterpolator*	newInterpolator = nullptr;
	CAnimationComponent* anim = assetManager->GetAnimData(m_modelTag);
	keyframes = anim->keyframes;
	keyframeCount = anim->keyframeCount;
	animationFileNames = anim->animationFileNames;
	m_animatedVerts = anim->m_animatedVerts;
	textureInfo = anim->textureInfo;
	m_vIndices = anim->m_vIndices;
	indexCount = anim->indexCount;
	vertCount = anim->vertCount;
	for (size_t i = 0; i < keyframeCount; i++)
	{
		newDeformer = new CDeformer();
		newInterpolator = new CInterpolator();

		(*newDeformer) = (*anim->m_pDeformers[i]);
		(*newInterpolator) = (*anim->m_pInterpolators[i]);
		m_pDeformers.push_back(newDeformer);
		m_pInterpolators.push_back(newInterpolator);
	}
#endif
}

void CAnimationComponent::LoadTexture(string _textureName, RENDER_NODE_INFO* _rnInfo)
{
#ifndef IARESERVER
	CRenderer* renderer = CGame::GetInstance()->GetRenderer();
	// complete the path to the file
	string texPath("Assets/Textures/");
	texPath += _textureName;
	texPath += ".dds";

	// transform the string to a wstring
	wstring wTexPath = wstring(texPath.begin(), texPath.end());

	// load texture
	HRESULT hr = CreateDDSTextureFromFile(renderer->GetDevice(), wTexPath.c_str(), &_rnInfo->diffuseTex, &_rnInfo->diffuseTexSRV);

	// If the path didn't point to a file, load NotFound.dds to give a visual cue
	if (hr == BAD_PATH || hr == FILE_NOT_FOUND)
	{
		hr = 0;
		wTexPath = wstring(L"Assets/Textures/NotFound.dds");
		hr = CreateDDSTextureFromFile(renderer->GetDevice(), wTexPath.c_str(), &_rnInfo->diffuseTex, &_rnInfo->diffuseTexSRV);
	}

	assert(!FAILED(hr) && "CRenderDataManager::LoadTexture() - Failed to load texture.");
#endif

}

// This function is called when a new instance of a prefab with this component is created.
CAnimationComponent* CAnimationComponent::clone() const
{
	CAnimationComponent* m = new CAnimationComponent(*this);
	//m->InitializeAnimationData();
	m->currentAnimationID = 0;

#ifndef IARESERVER
	//m->InitializeAnimationData();
	m->CloneAnimationData();
	CGame::GetInstance()->GetLocalUpdateManager()->AddAnimator(m);
	CGame::GetInstance()->GetRenderer()->QueueAnimationComponent( m );

#endif
	return m;
}

void CAnimationComponent::Update(float _dt)
{
	m_Owner->IsAnimatedObject(true);
	UpdateAnimation(_dt);
}
void CAnimationComponent::UpdateAnimation( float _deltaTime ) // UPDATES CURRENT ANIMATION
{
#ifndef IARESERVER
	//Call AddTime on Interpolator pass in delta time
	//Call Process on Interpolator
	//Then Call process on the deformer passing in the interpolator's GetPose;
	m_pInterpolators[ currentAnimationID ]->AddTime( _deltaTime );
	m_pInterpolators[ currentAnimationID ]->Process();
	//m_pDeformers[ currentAnimationID ]->Process( m_pInterpolators[ currentAnimationID ]->GetPose() );
#endif
}

CDeformer* CAnimationComponent::GetDeformer( void ) //Parameter for which deformer
{
	return m_pDeformers[0];
}

CInterpolator* CAnimationComponent::GetInterpolator( void ) // same here
{
	return m_pInterpolators[0];
}
void CAnimationComponent::OnEnable()
{

#ifndef IARESERVER
	m_pInterpolators[currentAnimationID]->SetTime(0.0f);
#endif
}

std::vector< sdef::AnimVert >* CAnimationComponent::GetVertices( void ) 
{

#ifndef IARESERVER
	//Based on Current Animation Call deformers get skin verts. fill out obj_vert struct and return
	std::vector< XMFLOAT3 >& animatedVerts = m_pDeformers[ currentAnimationID ]->GetSkinVerts();

	for (unsigned vIt = 0; vIt < animatedVerts.size(); vIt++)
	{
		m_animatedVerts[ vIt ].vertInfo.pos = animatedVerts[ vIt ];
	}

	return &m_animatedVerts;

#endif

#ifdef IARESERVER
	return nullptr;
#endif

}

std::vector< unsigned >* CAnimationComponent::GetIndices( void )
{
	return m_vIndices;
}

unsigned CAnimationComponent::GetVertexCount( void )
{
	return vertCount;
}

unsigned CAnimationComponent::GetIndexCount( void )
{
	return m_vIndices->size();
}

void CAnimationComponent::PushBackAnimationFileName( std::string _name )
{
	animationFileNames.push_back( _name );
}

std::vector< std::string > const & CAnimationComponent::GetAnimationFileNames( void )
{
	return animationFileNames;
}

void CAnimationComponent::SetKeyframeCount( unsigned _count )
{
	keyframeCount = _count;
}
