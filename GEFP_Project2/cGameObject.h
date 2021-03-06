#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

enum eTypeOfObject
{	// "Ok people, here's the deal:"
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	CAPSULE = 2,    // - it's a capsule
	AABB = 3,		// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};

class cGameObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor

	int getID( void );			// ADDED

	glm::vec3 position;
	//glm::vec3 orientation;
	glm::mat4x4 orientation;
	glm::vec3 orientation2;		// HACK (will elimiate this with)
	glm::vec3 rotation;
	float scale;

	// **********************************
	// Add some physics things
	glm::vec3 vel;			// Velocity
	glm::vec3 accel;		// Acceleration
	bool bIsUpdatedInPhysics;		// 
	//	float mass;		INFINITY				// 
	//	float inverseMass;	// = 0.0f	// 
	// Refers to the enum above
	eTypeOfObject typeOfObject;		// (really an int)
	float radius;

	// **********************************
	// Lightn
	bool bIsLight;		// is this object a light representation?
	bool isMoving;
	int myLight;		// which light is this?

	glm::vec4 diffuseColour;	//  The actual color when lights hit it

	std::string meshName;		// mesh I'd  like to draw

	// right / front / top vector
	glm::vec3 frontUpVector;


private:
	int m_uniqueID;
	static int m_nextID;
};

#endif
