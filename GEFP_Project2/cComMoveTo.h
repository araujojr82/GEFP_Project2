#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComMoveTo :	public iCommand
{
public:
	cComMoveTo();
	~cComMoveTo();

	virtual void init( glm::vec3 destination, float time, glm::vec3 notUsed1 );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

private:
	cGameObject* theGO;

	glm::vec3 initPosition;
	glm::vec3 finalPosition;
	glm::vec3 direction;
	float distanceToTarget;
	float velocity;
	double initialTime;
	double elapsedTime;
	float duration;

	bool hasStarted;

};

