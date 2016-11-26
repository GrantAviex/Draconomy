#pragma once

#include "Component.h"

#include "SharedDefinitions.h"

#include <vector>

class CInterpolator;
class CDeformer;
class CAnimation;
class CMesh;

class CAnimationComponent : public CComponent
{
public:
	CAnimationComponent( CGameObject* _owner );
	~CAnimationComponent();

	virtual CAnimationComponent*	clone() const;

	void Update(float _dt) override;
	void							UpdateAnimation( float _deltaTime );
	
	// CAnimationComponent::InitializeAnimationData
	// Call this function after the information from the xml file's been loaded.
	void							InitializeAnimationData();
	void							CloneAnimationData();
	void							ShutdownAnimationData();

	string							GetModelTag( void );
	void							SetModelTag( string _modelTag );

	CDeformer*						GetDeformer( void );
	CInterpolator*					GetInterpolator( void );

	std::vector< sdef::AnimVert >*	GetVertices(void);
	std::vector< unsigned >*		GetIndices( void );

	unsigned						GetVertexCount( void );
	unsigned						GetIndexCount( void );

	void							SetKeyframeCount( unsigned _count );

	void								PushBackAnimationFileName( std::string _name );
	std::vector< std::string > const &	GetAnimationFileNames( void );
	sdef::RENDER_NODE_INFO			GetTextureInfo() { return textureInfo; }
	void OnEnable() override;
	// Temporary animation states for testing
	enum AnimationsState { S0, S1, S2, S3, S4 };

private:
	void				LoadTexture(string _textureName, sdef::RENDER_NODE_INFO* _rnInfo);

	string						m_modelTag;
	//ID or some identifer for WHich animation we are on currently.

	// CAnimation instances and Indices are allocated and deallocated by CAssetManager
	std::vector< CAnimation* >*		keyframes;
	std::vector< unsigned >*		m_vIndices;

	// CDeformer and CInterpolator instances are allocated and deallocated by this class
	std::vector< CDeformer* >		m_pDeformers;
	std::vector< CInterpolator* >	m_pInterpolators;

	std::vector< sdef::AnimVert >	m_animatedVerts;

	unsigned						currentAnimationID = 0;
	unsigned						vertCount = 0;
	unsigned						indexCount = 0;
	unsigned						keyframeCount;

	std::vector< string >			animationFileNames;
	sdef::RENDER_NODE_INFO			textureInfo;
};

