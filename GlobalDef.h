
#ifndef GLOBALDEF_H
#define GLOBALDEF_H
namespace GlobalDef
{
	extern unsigned int windowWidth;
	extern unsigned int windowHeight;
	static const bool exportingToBinary = false;

	enum DamageID
	{
		eBullet = 0,
		ePoison, 
		eWall,
		eVortex
	};
}
#endif