#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComRotate :	public iCommand
{
public:
	cComRotate();
	~cComRotate();

	virtual void init( glm::vec3 degreesToRotate, float time, glm::vec3 notUsed );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

private:
	cGameObject* theGO;

	double initialTime;
	
	float duration;
	float velocityX, velocityY, velocityZ;
	glm::vec3 finalOrientation;

	


};

