#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComOrientTo :	public iCommand
{
public:
	cComOrientTo();
	~cComOrientTo();

	virtual void init( glm::vec3 destination, float time, glm::vec3 unused );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

	void setTargetGO( cGameObject* target );
	cGameObject* getTargetGO();
	virtual void setMyID( int myID );

private:
	int m_uniqueID;
	cGameObject* theGO;

	double initialTime;
	double elapsedTime;
	float duration;

	float velocityX, velocityY, velocityZ;

	glm::vec3 initialPosition;
	glm::vec3 theUpVector;
	glm::vec3 lookAtOrigin;
	glm::vec3 lookAtPosition;
	glm::mat4x4 initialOrientation;
	glm::mat4x4 finalOrientation;
	//glm::mat4x4 deltaOrientation;
	//glm::mat4x4 orientationSpeed;

	bool hasStarted;
	bool isCommandDone;

	cGameObject* targetGO;
};

