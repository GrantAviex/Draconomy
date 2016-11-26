#pragma once
#include "GridSystem.h"
#include "GridSystem.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "SphereTrigger.h"
#include "MeshCollider.h"
#include "Mesh.h"
#include <math.h>
struct TSphere
{
	XMFLOAT3 vCenter;
	float fRadius;
};
struct TCapsule
{
	XMFLOAT3 botPos, topPos;
	float fRadius;
};
struct TRayCastHit
{
	XMFLOAT3 vHitPt;
	XMFLOAT3 vDir;
	XMFLOAT3 vNorm;
	float fDistance;

};
class CAABBCollider;

class CCollisionManager
{
	CGridSystem* m_GridSystem;
	CGridSystem* m_PathGridSystem;

public:
	vector<TTriangle*> GetMeshTri(string _str);

	vector<ICollider*> gameObjects;
	vector<CSphereCollider*> sphereColliders;
	vector<CAABBCollider*> aabbCColliders;
	vector<ICollider*> bullets;
	vector<CSphereTrigger*> triggerObjects;
	vector<pair<ICollider*, ICollider*>> colObjects;
	friend class SphereCollider;
	void HandleCollisions();
	void cross_Product(XMFLOAT3 &_a, XMFLOAT3 &_b, XMFLOAT3 &_c);
	float dot_Product(XMFLOAT3 _a, XMFLOAT3 _b);
	/* LineSegmentToTriangle
	***Checks line segment intersection with list of triangles. Returns true if collision and returns the index of the triangle it collided with and the point of collision.
	** vOut = point of collision, TriIndex = index of the triangle that found a collision, pTris = list of tri's, vStart = beginning of segment, vEnd = end of segment.
	*/
	static bool LineSegmentToTriangle(XMFLOAT3 &vOut, int &uiTriIndex, vector<TTriangle*> pTris, unsigned int uiTriCount, const XMFLOAT3 &vStart, const XMFLOAT3 &vEnd);

	static bool SphereToTriangleRes(const TSphere& sphere, const TTriangle& tri, XMFLOAT3& displacement);
	static bool SphereToTriangle(const TSphere& sphere, const TTriangle& tri);
	static bool AABBtoAABB(const CAABBCollider& lhs, const CAABBCollider& rhs);
	static bool SphereToAABB(const TSphere& lhs, const CAABBCollider& rhs);
	static bool SphereToAABB(const TSphere& lhs, const CAABBCollider& rhs, float& _distance, XMVECTOR& _dir);
	static bool SphereToSphere(const TSphere& lhs, const TSphere& rhs);
	static void SphereToSphereRes(TSphere& lhs, TSphere& rhs, CGameObject* lVel, CGameObject* rVel);
	static bool SphereToSphereSingleRes(TSphere& lhs, TSphere& rhs);
	static bool CapsuleToSphere(const TCapsule& capsule, TSphere& sphere);
	bool RayToSphere(XMVECTOR _rayPos, XMVECTOR _rayDir, XMVECTOR _spherePos, float _radius);
	bool RayToCylinder(XMVECTOR &_rayPos, XMVECTOR &_rayDir, XMVECTOR &_botSpherePos, XMVECTOR &_topSpherePos, float _radius);
	bool RayToCapsule(XMVECTOR &_rayPos, XMVECTOR &_rayDir, XMVECTOR &_botSpherePos, XMVECTOR &_topSpherePos, float _radius, float& _time);
	bool RayCastToTri(const XMVECTOR _start, XMVECTOR _dir, float _distance, TRayCastHit &_rayHit);
	bool RayToPath(XMFLOAT3 _position);
	std::vector<CGameObject*> RayCastStructs(const XMVECTOR _start, XMVECTOR _dir);
	std::vector<CGameObject*> RayCastEnemies(const XMVECTOR _start, XMVECTOR _dir);
	std::vector<CGameObject*> RayCastActs(const XMVECTOR _start, XMVECTOR _dir);


	bool WallTriCollision(ICollider* _wall, ICollider* _col,int wallType);
	bool WallSphereCollision(ICollider* _wall, ICollider* _col);
	bool BulletCollision(ICollider*_bullet, ICollider* _col);
	bool BulletToWallCollision(ICollider* _bullet, ICollider* _col);
	bool CandyCollision(ICollider*_candy, ICollider* _col);
	bool BarrelCollision(ICollider*_candy, ICollider* _col);
	bool ManaCollision(ICollider*_candy, ICollider* _col);
	bool UpgradeCollision(ICollider*_candy, ICollider* _col);

	void Initialize();
	CGridSystem* GetGrid(){ return m_GridSystem; }
	vector<CGameObject*> OverlapSphere(TSphere _Sphere, SphereType _type);
	vector<CGameObject*> OverlapSphere(XMFLOAT3 _center, float _radius, TriggerType _type);

	vector<CGameObject*> OverlapCheckForAABB(TSphere _Sphere);
	vector<CGameObject*> OverlapCheckForAABB(XMFLOAT3 _center, float _radius);

	vector<CGameObject*> OverlapSphere(XMFLOAT3 _center, float _radius, SphereType _type);
	vector<CGameObject*> OverlapSphere(XMFLOAT3 _center, float _radius, CapsuleType _type);
	//vector<CGameObject*> OverlapEnemySphere(XMFLOAT3 _center, float _radius, int _type);

	CCollisionManager();
	~CCollisionManager();
};

