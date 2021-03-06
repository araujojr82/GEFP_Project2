#include "cCameraObject.h"
#include <glm\gtc\matrix_transform.hpp>
#include "cGameObject.h"

cCameraObject::cCameraObject()
{
	// The camera position should always be 1 unit behind the lookAt position
	this->camPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->lookAtPosition = glm::vec3( 0.0f, 0.0f, -1.0f );
	this->camUpVector = glm::vec3( 0.0f, 1.0f, 0.0f );
	this->camOrientation = glm::mat4x4( 1.0f );
	this->camVelocity = 0.0f;
	this->cameraMode = MANUAL;
	this->controlledGameObject = NULL;
	this->myGO = NULL;
}

void cCameraObject::setMyGO( cGameObject* theCameraGO )
{
	this->myGO = theCameraGO;
	return;
}

cGameObject* cCameraObject::getMyGO()
{

	return this->myGO;
}

void cCameraObject::setCameraPosition( glm::vec3 newPosition )
{
	// Use the delta postion as an input to the new lookAt position
	//glm::vec3 derivedLookAt = this->lookAtPosition - this->camPosition;
	//this->camPosition = newPosition;
	//this->lookAtPosition = newPosition + derivedLookAt;

	glm::vec3 derivedLookAt = this->lookAtPosition - this->myGO->position;
	this->myGO->position = newPosition;
	this->lookAtPosition = newPosition + derivedLookAt;
}

void cCameraObject::setCameraTarget( glm::vec3 target )
{
	// Reset the Up Vector
	this->camUpVector = glm::vec3( 0.0f, 1.0f, 0.0f );

	// LookAt direction at the origin
	glm::vec3 lookAtOrigin = target - this->myGO->position;

	// Normalized lookAt at the origin
	lookAtOrigin = glm::normalize( lookAtOrigin );

	// New LooAt
	this->lookAtPosition = this->myGO->position + lookAtOrigin;

	// Care about the orientation too
	this->myGO->orientation = glm::inverse( glm::lookAt( glm::vec3( 0.0f, 0.0f, 0.0f ), lookAtOrigin, this->camUpVector ) );
}

void cCameraObject::lockOnGameObject( cGameObject* GO )
{
	this->controlledGameObject = GO;
	this->cameraMode = FOLLOW_CAMERA;
}

cGameObject * cCameraObject::getGameObject()
{
	return this->controlledGameObject;
}

void cCameraObject::releaseGameObject()
{
	this->cameraMode = MANUAL;
}

void cCameraObject::update()
{
	//if(this->cameraMode == FOLLOW_CAMERA)
	//{
	//    // Move the camera to the target
	//    this->camPosition = this->controlledGameObject->position;

	//    // Reorient the camera according to the target
	//    this->camUpVector = this->controlledGameObject->orientation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	//    glm::vec3 lookAtOrigin = (this->controlledGameObject->orientation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	//    this->camOrientation = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookAtOrigin, this->camUpVector));
	//    this->lookAtPosition = this->camPosition + lookAtOrigin;

	//    // Reposition the camera to a better 'Follow' style
	//    this->moveCameraBackNForth(6.0f);
	//    this->moveCameraUpNDown(2.0f);
	//    this->setCameraOrientationX(-10.0f);
	//}

	// Move the camera to the target
	this->camPosition = this->myGO->position;

	// Reorient the camera according to the target
	this->camUpVector = this->myGO->orientation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec3 lookAtOrigin = (this->myGO->orientation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	this->camOrientation = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookAtOrigin, this->camUpVector));
	this->lookAtPosition = this->camPosition - lookAtOrigin;

	//// Reposition the camera to a better 'Follow' style
	//this->moveCameraBackNForth(6.0f);
	//this->moveCameraUpNDown(2.0f);
	//this->setCameraOrientationX(-10.0f);
}

void cCameraObject::moveCameraBackNForth( float speed )
{
	// Set a vector at the origin with the change in position along the Z axis
	glm::vec4 originZ = glm::vec4( 0.0f, 0.0f, speed, 0.0f );

	// Transfor the vector according to the rotation of the camera
	glm::vec3 newOriginZ = this->myGO->orientation * originZ;

	setCameraPosition( this->myGO->position + newOriginZ );
}

void cCameraObject::moveCameraLeftNRight( float speed )
{
	// Set a vector at the origin with the change in position along the X axis
	glm::vec4 originX = glm::vec4( speed, 0.0f, 0.0f, 0.0f );

	// Transfor the vector according to the rotation of the camera
	glm::vec3 newOriginX = this->myGO->orientation * originX;

	setCameraPosition( this->myGO->position + newOriginX );
}

void cCameraObject::moveCameraUpNDown( float speed )
{
	// Set a vector at the origin with the change in position along the Y axis
	glm::vec4 originY = glm::vec4( 0.0f, speed, 0.0f, 0.0f );

	// Transfor the vector according to the rotation of the camera
	glm::vec3 newOriginY = this->myGO->orientation * originY;

	setCameraPosition( this->myGO->position + newOriginY );
}

void cCameraObject::changeAlongX( float change )
{
	glm::vec3 vecChange = glm::vec3( change, 0.0f, 0.0f );
	setCameraPosition( this->myGO->position + vecChange );
}

void cCameraObject::changeAlongY( float change )
{
	glm::vec3 vecChange = glm::vec3( 0.0f, change, 0.0f );
	setCameraPosition( this->myGO->position + vecChange );
}

void cCameraObject::changeAlongZ( float change )
{
	glm::vec3 vecChange = glm::vec3( 0.0f, 0.0f, change );
	setCameraPosition( this->myGO->position + vecChange );
}

void cCameraObject::setCameraOrientationX( float degrees )
{
	// Transform the Camera Orientation
	float newOrientation = glm::radians( degrees );
	glm::mat4x4 matRotX = glm::mat4x4( 1.0f );
	matRotX = glm::rotate( matRotX, newOrientation, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	this->myGO->orientation = this->myGO->orientation * matRotX;

	// Transform the vector X to the new orientation
	glm::vec3 newVecX = this->myGO->orientation * glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );

	// Transform according to camera local axis
	glm::mat4x4 matRotX_Local = glm::mat4x4( 1.0f );
	matRotX_Local = glm::rotate( matRotX_Local, newOrientation, newVecX );

	// Camera Up Vector
	//-------------------------------------------------------------------------
	// Up Vector before transformation
	glm::vec4 v4_upVector = { this->camUpVector, 0.0f };

	// Transformed Up Vector
	glm::vec3 transUpVector = matRotX_Local * v4_upVector;

	// New Upvector
	this->camUpVector = transUpVector;
	//-------------------------------------------------------------------------

	// LookAt at origin
	glm::vec3 lookAtOrigin = this->lookAtPosition - this->myGO->position;
	glm::vec4 v4_lookAtOrigin = { lookAtOrigin, 0.0f };

	// Transformed LookAt at origin
	glm::vec3 transLookAtOrigin = matRotX_Local * v4_lookAtOrigin;

	// New LookAt position
	this->lookAtPosition = this->myGO->position + transLookAtOrigin;
}

void cCameraObject::setCameraOrientationY( float degrees )
{
	// Transform the Camera Orientation
	float newOrientation = glm::radians( degrees );
	glm::mat4x4 matRotY = glm::mat4x4( 1.0f );
	matRotY = glm::rotate( matRotY, newOrientation, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	this->myGO->orientation = this->myGO->orientation * matRotY;

	// Transform the vector Y to the new orientation
	glm::vec3 newVecY = this->myGO->orientation * glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f );

	// Transform according to camera local axis
	glm::mat4x4 matRotY_Local = glm::mat4x4( 1.0f );
	matRotY_Local = glm::rotate( matRotY_Local, newOrientation, newVecY );

	// Camera Up Vector
	//-------------------------------------------------------------------------
	// Up Vector before transformation
	glm::vec4 v4_upVector = { this->camUpVector, 0.0f };

	// Transformed Up Vector
	glm::vec3 transUpVector = matRotY_Local * v4_upVector;

	// New Upvector
	this->camUpVector = transUpVector;
	//-------------------------------------------------------------------------

	// LookAt at origin
	glm::vec3 lookAtOrigin = this->lookAtPosition - this->myGO->position;
	glm::vec4 v4_lookAtOrigin = { lookAtOrigin, 0.0f };

	// Transformed LookAt at origin
	glm::vec3 transLookAtOrigin = matRotY_Local * v4_lookAtOrigin;

	// New LookAt position
	this->lookAtPosition = this->myGO->position + transLookAtOrigin;
}

void cCameraObject::setCameraOrientationZ( float degrees )
{
	// We don't need to care about the Lookat translation for rotations around
	// the Z axis. The main deal of this rotation is Camera Up Vector

	// Transform the Camera Orientation
	float newOrientation = glm::radians( degrees );
	glm::mat4x4 matRotZ = glm::mat4x4( 1.0f );
	matRotZ = glm::rotate( matRotZ, newOrientation, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	this->myGO->orientation = this->myGO->orientation * matRotZ;

	// Transform the vector Z to the new orientation
	glm::vec3 newVecZ = this->myGO->orientation * glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f );

	// Transform according to camera local axis
	glm::mat4x4 matRotZ_Local = glm::mat4x4( 1.0f );
	matRotZ_Local = glm::rotate( matRotZ_Local, newOrientation, newVecZ );

	// Up Vector before transformation
	glm::vec4 v4_upVector = { this->camUpVector, 0.0f };

	// Transformed Up Vector
	glm::vec3 transUpVector = matRotZ_Local * v4_upVector;

	// New Upvector
	this->camUpVector = transUpVector;
}

void cCameraObject::setCameraMode( eCameraMode cameraMode )
{
	this->cameraMode = cameraMode;
}

void cCameraObject::getCameraInfo( glm::vec3 &camPosition,
	glm::vec3 &lookAtPosition )
{
	camPosition = this->myGO->position;
	lookAtPosition = this->lookAtPosition;
}

eCameraMode cCameraObject::getCameraMode()
{
	return this->cameraMode;
}

glm::vec3 cCameraObject::getCameraPosition()
{
	return this->myGO->position;
}

glm::vec3 cCameraObject::getLookAtPosition()
{
	return this->lookAtPosition;
}

glm::vec3 cCameraObject::getCameraUpVector()
{
	return this->camUpVector;
}

void cCameraObject::setVelocity( float newVelocity )
{
	this->camVelocity = newVelocity;
}

glm::vec3 cCameraObject::getVelocity()
{
	return this->lookAtPosition * this->camVelocity;
}