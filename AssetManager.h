
//AssetManager.h
//Created May 2015

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <unordered_map>
#include "GameObject.h"
#include "Mesh.h"
#include "SharedDefinitions.h"
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;

class CModel;
class Emitter;
class CAnimation;
class CDeformer;
class CInterpolator;
class CAnimationComponent;

struct TRenderInfo
{
	TRenderInfo()
	{

	}
	TRenderInfo(unsigned int n, unsigned int s)
	{
		renderNodeTag = n;
		renderSetTag = s;
	}
	unsigned int renderNodeTag;
	unsigned int renderSetTag;
};
class CAssetManager
{
public:
	CAssetManager();
	~CAssetManager();

	
	

	

	
	//----------------------------------------------------------------------------------------
	// Exporting
	//----------------------------------------------------------------------------------------
	void ExportMeshesToBinary();
	void ExportAnimationsToBinary();

	
	//----------------------------------------------------------------------------------------
	// Loading
	//----------------------------------------------------------------------------------------
	void			LoadPrefab(string _path);
	void			LoadModel(string _path, unsigned int setTag);
	CGameObject*	LoadGameObject(XMLElement* pRoot, CGameObject* nObj);
	
	bool			LoadMeshFromBinary(string MeshName);
	bool			LoadAnimationFromBinary(string AnimName);
	
	void			LoadServerModel(string _path);
	void			LoadEmitter(string _path);

	//----------------------------------------------------------------------------------------
	// Accessors and mutators
	//----------------------------------------------------------------------------------------
	CGameObject*		GetPrefab(string name)		{ return m_maPrefabs[name]; }
	vector<CMesh>*		GetMeshes(string name)		{ return m_maModels[name]; }
	TRenderInfo			GetRenderInfo(string name)	{ return m_modelRenders[name]; }
	vector<OBJ_VERT>*	GetObjVerts(string name)	{ return m_maModelVerts[name]; }
	CAnimationComponent* GetAnimData(string name)	{ return m_maAnimData[name]; }
	void				DeletePrefab(CGameObject* pref);

	//----------------------------------------------------------------------------------------
	std::vector< CAnimation* >*		GetModelAnimationFrames( std::string _modelTag );
	bool							HasModelAnimationFrames( std::string _modelTag );
	
	// Parameters:
	//	string fileName:	The name of the file you want to load
	//	string modelName:	The name of the model to which the animation belongs to
	void LoadAnimationFromBinary( string _modelName, string _fileName);
	void LoadAnimationFromFBX( string _modelName, string _fileName, CAnimationComponent* _ac );
	//----------------------------------------------------------------------------------------

private:
	int											m_NodeTag;

	unordered_map<string, CGameObject*>			m_maPrefabs;
	unordered_map<string, vector<CMesh>*>		m_maModels;
	unordered_map<string, TRenderInfo>			m_modelRenders;
	unordered_map<string, vector<OBJ_VERT>*>	m_maModelVerts;
	unordered_map<string, Emitter*>				m_maEmitters;

	float			FloatAttribute(XMLElement* element, string name);
	int				IntAttribute(XMLElement* element, string name);
	std::string		StringAttribute(XMLElement* element, string name);

	//----------------------------------------------------------------------------------------
	unordered_map< string, std::vector< CAnimation* > >		animationFrames;
	
	

	
	//----------------------------------------------------------------------------------------
	
	//----------------------------------------------------------------------------------------
	// Outdated Interface
	//----------------------------------------------------------------------------------------
	// These are no longer used for the game, but they are necessary for exporting files to binary
	unordered_map< string, vector<CAnimation*> >		m_maAnims;
	unordered_map< string, CDeformer* >			m_maDeforms;
	unordered_map< string, CInterpolator* >		m_maInterpols;

	unordered_map<string, CAnimationComponent*> m_maAnimData;
	vector<CAnimation*>		GetAnimations( string name )	{ return m_maAnims[ name ]; }
	CDeformer*		GetDeformers( string name )		{ return m_maDeforms[ name ]; }
	CInterpolator*	GetInterpols( string name )		{ return m_maInterpols[ name ]; }


	bool m_bLoadedDefaultModels;
	vector<string> m_vDefaultedModels;

};

#endif
