#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>						// C++ cin, cout, etc.
#include <glm/vec3.hpp>					// glm::vec3
#include <glm/vec4.hpp>					// glm::vec4
#include <glm/mat4x4.hpp>				// glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>						// "String stream"
#include <istream>
#include <string>
#include <vector>						// smart array, "array" in most languages
#include <random>
#include <chrono>

#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"
#include "Physics.h"
#include "cLightManager.h"
#include "cDebugRenderer.h"
#include "cCameraObject.h"

#include "cCommandScheduler.h"
#include "cLuaBrain.h"
#include "GLFW_keyboardCallback.h"

int g_GameObjNumber = 0;				// game object vector position number 
int g_LightObjNumber = 0;				// light object vector position

int g_targetShip = 0;
int g_selectedShip = 0;

bool g_lookAtON = false;
bool g_moveRiders = false;
bool g_movingViper = false;

int g_NUMBER_OF_LIGHTS = 9;

bool bIsWireframe = false;

// Remember to #include <vector>...
std::vector< cGameObject* > g_vecGameObjects;

cCommandScheduler g_theScheduler;

cCameraObject* g_pCamera = NULL;


cVAOMeshManager*	g_pVAOManager = 0;		// or NULL or nullptr
cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;

cDebugRenderer*		g_pDebugRenderer = 0;

cLuaBrain* p_LuaScripts = nullptr;

// Other uniforms:
GLint uniLoc_materialDiffuse = -1;
GLint uniLoc_materialAmbient = -1;
GLint uniLoc_ambientToDiffuseRatio = -1; 	// Maybe	// 0.2 or 0.3
GLint uniLoc_materialSpecular = -1;  // rgb = colour of HIGHLIGHT only
									 // w = shininess of the 
GLint uniLoc_eyePosition = -1;	// Camera position
GLint uniLoc_mModel = -1;
GLint uniLoc_mView = -1;
GLint uniLoc_mProjection = -1;
GLint uniLoc_mWorldInvTrans = -1;

struct sWindowConfig
{
public:
	int height = 480;
	int width = 640;
	std::string title = "TO BE REPLACED...";
};

struct sGOparameters		// for the Game Objects' input file
{
	std::string meshname;
	int nObjects;
	float x, y, z, scale;
	std::string random;
	float rangeX, rangeY, rangeZ, rangeScale;
};

struct sMeshparameters		// for the Meshes' input file
{
	std::string meshname;
	std::string meshFilename;
};

// Forward declare the Functions
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
void loadConfigFile( std::string fileName, sWindowConfig& wConfig );
sGOparameters parseObjLine( std::ifstream &source );
void loadObjectsFile( std::string fileName );
sMeshparameters parseMeshLine( std::ifstream &source );
void loadMeshesFile( std::string fileName, GLint ShaderID );
void loadLightObjects();
void PhysicsStep( double deltaTime );
void updateAllObjects( double deltaTime );
void DrawObject( cGameObject* pTheGO );
std::stringstream loadScriptFile( std::string fileName );

static void error_callback( int error, const char* description )
{
	fprintf( stderr, "Error: %s\n", description );
}

int main( void )
{
	GLFWwindow* window;
	//GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location; //vpos_location, vcol_location;
	glfwSetErrorCallback( error_callback );

	if( !glfwInit() )
		exit( EXIT_FAILURE );

	sWindowConfig wConfig;

	loadConfigFile( "config.txt", wConfig );
	loadObjectsFile( "objects.txt" );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	window = glfwCreateWindow( wConfig.width, wConfig.height,
		wConfig.title.c_str(),
		NULL, // glfwGetPrimaryMonitor(), //
		NULL );
	if( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( window, key_callback );
	glfwMakeContextCurrent( window );
	gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress );
	glfwSwapInterval( 1 );

	std::cout << glGetString( GL_VENDOR ) << " "
		<< glGetString( GL_RENDERER ) << ", "
		<< glGetString( GL_VERSION ) << std::endl;
	std::cout << "Shader language version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if( !::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
	}
	std::cout << "The shaders compiled and linked OK" << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pDebugRenderer = new cDebugRenderer();
	if( !::g_pDebugRenderer->initialize( error ) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
	}

	//Load models
	::g_pVAOManager = new cVAOMeshManager();

	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	loadMeshesFile( "meshlist.txt", sexyShaderID );

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	// Get the uniform locations for this shader
	mvp_location = glGetUniformLocation( currentProgID, "MVP" );		// program, "MVP");
	uniLoc_materialDiffuse = glGetUniformLocation( currentProgID, "materialDiffuse" );
	uniLoc_materialAmbient = glGetUniformLocation( currentProgID, "materialAmbient" );
	uniLoc_ambientToDiffuseRatio = glGetUniformLocation( currentProgID, "ambientToDiffuseRatio" );
	uniLoc_materialSpecular = glGetUniformLocation( currentProgID, "materialSpecular" );
	uniLoc_eyePosition = glGetUniformLocation( currentProgID, "eyePosition" );
	uniLoc_mModel = glGetUniformLocation( currentProgID, "mModel" );
	uniLoc_mView = glGetUniformLocation( currentProgID, "mView" );
	uniLoc_mProjection = glGetUniformLocation( currentProgID, "mProjection" );
	uniLoc_mWorldInvTrans = glGetUniformLocation( currentProgID, "mWorldInvTranspose" );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights( g_NUMBER_OF_LIGHTS );	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations( currentProgID );

	// Change ZERO (the SUN) light position
	::g_pLightManager->vecLights[0].position = glm::vec3( -500.0f, 500.0f, 500.0f );
	::g_pLightManager->vecLights[0].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
	::g_pLightManager->vecLights[0].ambient = glm::vec3( 0.8f, 0.8f, 0.8f );
	::g_pLightManager->vecLights[0].attenuation.y = 0.003f;		// Change the linear attenuation

	// ADD 8 MORE LIGHTS========================================
	{
		// ENGINE LIGHTS
		::g_pLightManager->vecLights[1].position = glm::vec3( -6.5f, 4.5f, 78.0f );
		::g_pLightManager->vecLights[2].position = glm::vec3( 6.5f,  4.5f, 78.0f );
		::g_pLightManager->vecLights[3].position = glm::vec3( -5.5f, -2.8f, 75.0f );
		::g_pLightManager->vecLights[4].position = glm::vec3( 5.5f,  -2.8f, 75.0f );
		::g_pLightManager->vecLights[1].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[2].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[3].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[4].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[1].attenuation.y = 0.45;
		::g_pLightManager->vecLights[2].attenuation.y = 0.45;
		::g_pLightManager->vecLights[3].attenuation.y = 0.45;
		::g_pLightManager->vecLights[4].attenuation.y = 0.45;
		
		// LANDING BAY LEFT
		::g_pLightManager->vecLights[5].position = glm::vec3( -14.5f, 0.75f, 23.0f );
		::g_pLightManager->vecLights[6].position = glm::vec3( -14.5f, 0.75f, 47.0f );
		::g_pLightManager->vecLights[5].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
		::g_pLightManager->vecLights[6].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );

		// LANDING BAY RIGHT
		::g_pLightManager->vecLights[7].position = glm::vec3( 14.5f, 0.75f, 23.0f );
		::g_pLightManager->vecLights[8].position = glm::vec3( 14.5f, 0.75f, 47.0f );
		::g_pLightManager->vecLights[7].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
		::g_pLightManager->vecLights[8].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );

	}
	//=========================================================

	//loadLightObjects();

	// LUA 
	// Create the Lua script handler
	::p_LuaScripts = new cLuaBrain();
	// Point to all the game objects
	// (We are passing a pointer to the vector of game object pointers... yeah)
	//::p_LuaScripts->SetObjectVector( &( ::g_vecGameObjects ) );

	// Load the lua script from file
	std::stringstream ssTheScript;

	ssTheScript = loadScriptFile( "LuaScript1.txt" );

	// Print the script just in case
	std::cout << ssTheScript.str() << std::endl;

	::p_LuaScripts->LoadScript( "MoveTo", ssTheScript.str() );

	// Update the lua script manager
	::p_LuaScripts->Update();


	g_pCamera = new cCameraObject();
	g_pCamera->setCameraPosition( glm::vec3( -170.0f, 50.0f, 80.0f ) );
	g_pCamera->setCameraOrientationX( -20.0f );
	g_pCamera->setCameraOrientationY( -60.0f );	
	g_pCamera->setCameraOrientationZ( -20.0f );

	glEnable( GL_DEPTH );

	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	// Main game or application loop
	while( !glfwWindowShouldClose( window ) )
	{	
		float ratio;
		int width, height;
		//glm::mat4x4 m, mvp;			//  mat4x4 m, p, mvp;

		glfwGetFramebufferSize( window, &width, &height );
		ratio = width / ( float )height;
		glViewport( 0, 0, width, height );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		::g_pShaderManager->useShaderProgram( "mySexyShader" );
		GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader(); 

		// Camera block
		glm::mat4x4 matProjection;

		// Projection and view don't change per scene (maybe)
		matProjection = glm::perspective( 0.6f,			// FOV
			ratio,		// Aspect ratio
			1.0f,			// Near (as big as possible)
			200000.0f );	// Far (as small as possible)

		g_pCamera->update();

		// View or "camera" matrix
		glm::mat4 matView = glm::mat4( 1.0f );	// was "v"

		// Now the view matrix is taken right from the camera class
		matView = glm::lookAt( g_pCamera->getCameraPosition(),		// "eye" or "camera" position
							   g_pCamera->getLookAtPosition(),		// "At" or "target" 
							   g_pCamera->getCameraUpVector() );	// "up" vector

		glUniformMatrix4fv( uniLoc_mView, 1, GL_FALSE,
			( const GLfloat* )glm::value_ptr( matView ) );
		glUniformMatrix4fv( uniLoc_mProjection, 1, GL_FALSE,
			( const GLfloat* )glm::value_ptr( matProjection ) );

		unsigned int sizeOfVector = ::g_vecGameObjects.size();
		// "Draw scene" loop
		for( int index = 0; index != sizeOfVector; index++ )
		{
			cGameObject* pTheGO = ::g_vecGameObjects[index];

			DrawObject( pTheGO );
			
		}//for ( int index = 0...

		::g_pDebugRenderer->RenderDebugObjects( matView, matProjection ); // matProjection );
		
		std::stringstream ssTitle;
		glm::vec3 curCameraPosition = g_pCamera->getCameraPosition();
		glm::vec3 curCameraLookAt = g_pCamera->getLookAtPosition();
		ssTitle << "GEFP Project 2: Lua Integration"
			<< "Camera (xyz): "
			<< curCameraPosition.x << ", "
			<< curCameraPosition.y << ", "
			<< curCameraPosition.z << " | "
			<< "Camera LookAt(xyz): "
			<< curCameraLookAt.x << ", "
			<< curCameraLookAt.y << ", "
			<< curCameraLookAt.z;
		

		glfwSetWindowTitle( window, ssTitle.str().c_str() );

		glfwSwapBuffers( window );
		glfwPollEvents();

		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;
		
		updateAllObjects( deltaTime );

		lastTimeStep = curTime;

	}// while ( ! glfwWindowShouldClose(window) )

	glfwDestroyWindow( window );
	glfwTerminate();

	delete ::g_pShaderManager;
	delete ::g_pVAOManager;

	return 0;
}

// Load Lua external Script
std::stringstream loadScriptFile( std::string fileName )
{
	// TODO change this config formating
	std::ifstream infoFile( fileName );
	if( !infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 

		std::string temp; // , theLine;
		std::stringstream tempLine;
		
		tempLine.str( "" );

		while( !infoFile.eof() && infoFile.is_open() )
		{
			infoFile >> temp;
			if( !infoFile.eof() )
			{
				tempLine << temp << " ";
			}
			else
			{	// the end of the line 
				tempLine << temp;
			}

		}
		infoFile.close();  // Close the file
		return tempLine;
	}
}

//Load Config.txt
void loadConfigFile( std::string fileName, sWindowConfig& wConfig )
{
	// TODO change this config formating
	std::ifstream infoFile( fileName );
	if( !infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string a;

		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"
		infoFile >> wConfig.width;	// 1080
		infoFile >> a;	// "height"
		infoFile >> wConfig.height;	// 768
		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading = false;
				wConfig.title = ssTitle.str();
			}
		} while( bKeepReading );
	}
}

// Generate real random numbers
float generateRandomNumber( float min, float max )
{

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::default_random_engine generator( seed );
	std::uniform_real_distribution<float> distribution( min, max );

	float randomNumber = 0.0;

	randomNumber = distribution( generator );
	return randomNumber;

}

bool CheckDistance( glm::vec3 position, glm::vec3 fromPoint, float minDistance, float maxDistance )
{
	float thisDistance = 0.0f;
	thisDistance = glm::distance( position, fromPoint );

	if( thisDistance >= minDistance || thisDistance <= maxDistance ) return true;

	return false;
}

glm::vec3 GetRandomPosition()
{
	glm::vec3 center = glm::vec3( 0.0f );
	glm::vec3 newPosition = glm::vec3( 0.0f );
	bool validPosition = false;

	while( !validPosition )
	{
		newPosition = glm::vec3( generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ) );
		validPosition = CheckDistance( newPosition, center, 30.0f, 100.0f );
	}

	return newPosition;
}


//Load objects.txt
void loadObjectsFile( std::string fileName )
{
	//sGOparameters sGOpar;
	std::vector <sGOparameters> allObjects;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allObjects.push_back( parseObjLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allObjects.size(); index++ )
	{
		// Check, Number of Objects must be at least 1
		if( allObjects[index].nObjects == 0 ) allObjects[index].nObjects = 1;

		// Create the number of gameObjects specified in the file for each line 
		for ( int i = 0; i != allObjects[index].nObjects; i++ )
		{
			// Create a new GO
			cGameObject* pTempGO = new cGameObject();

			pTempGO->meshName = allObjects[index].meshname; // Set the name of the mesh

			pTempGO->diffuseColour = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f );
			pTempGO->rotation = glm::vec3( 0.0f );
			
			// SOME OBJECTS ARE RANDOMLY PLACED WHEN RANDOM=TRUE ON FILE
			if ( allObjects[index].random == "true" )
			{  
				if( allObjects[index].meshname == "viper" )
				{   // Vipers have their on random placement
					float random = generateRandomNumber( -5000.0f, 5000.0f );

					if ( random < 0 )
						pTempGO->position.x = generateRandomNumber( -30.0f, -20.0f );
					else 
						pTempGO->position.x = generateRandomNumber( 20.0f, 30.0f );
					
					pTempGO->position.y = generateRandomNumber( 0.0f, 10.0f );
					pTempGO->position.z = generateRandomNumber( -10.0f, 20.0f );

				}
				else if( allObjects[index].meshname == "asteroid1" ||
					allObjects[index].meshname == "asteroid2" ||
					allObjects[index].meshname == "asteroid3" ||
					allObjects[index].meshname == "asteroid4" )
				{   // Asteroids have their on random placement
					pTempGO->position = GetRandomPosition();
					pTempGO->rotation.x = generateRandomNumber( -0.05f, 0.05f );
					pTempGO->rotation.y = generateRandomNumber( -0.05f, 0.05f );
					pTempGO->rotation.z = generateRandomNumber( -0.05f, 0.05f );
				}
				else
				{
					pTempGO->position.x = generateRandomNumber( -allObjects[index].rangeX, allObjects[index].rangeX );
					pTempGO->position.y = generateRandomNumber( -allObjects[index].rangeY, allObjects[index].rangeY );
					pTempGO->position.z = generateRandomNumber( -allObjects[index].rangeZ, allObjects[index].rangeZ );
				}
				pTempGO->scale = allObjects[index].rangeScale;
			}
			else
			{   // position and scale are fixed
				pTempGO->position.x = allObjects[index].x;
				pTempGO->position.y = allObjects[index].y;
				pTempGO->position.z = allObjects[index].z;
				pTempGO->scale = allObjects[index].scale;
			}
			// NO VELOCITY
			pTempGO->vel = glm::vec3( 0.0f );

			::g_vecGameObjects.push_back( pTempGO );
		}
	}
}

// Parse the file line to fit into the structure
sGOparameters parseObjLine( std::ifstream &source ) {

	sGOparameters sGOpar;

	//Scanning a line from the file
	source >> sGOpar.meshname >> sGOpar.nObjects
		>> sGOpar.x >> sGOpar.y >> sGOpar.z >> sGOpar.scale
		>> sGOpar.random
		>> sGOpar.rangeX >> sGOpar.rangeY >> sGOpar.rangeZ
		>> sGOpar.rangeScale;


	return sGOpar;
}

//Load meshlist.txt
void loadMeshesFile( std::string fileName, GLint ShaderID )
{
	std::vector <sMeshparameters> allMeshes;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allMeshes.push_back( parseMeshLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allMeshes.size(); index++ )
	{
		cMesh testMesh;
		testMesh.name = allMeshes[index].meshname;
		if( !LoadPlyFileIntoMesh( allMeshes[index].meshFilename, testMesh ) )
		{
			std::cout << "Didn't load model" << std::endl;
			// do something??
		}
		if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID ) )
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}	
}

// Parse the file line to fit into the structure
sMeshparameters parseMeshLine( std::ifstream &source ) {

	sMeshparameters sMeshpar;

	//Scanning a line from the file
	source >> sMeshpar.meshname >> sMeshpar.meshFilename;

	return sMeshpar;
}

void loadLightObjects()
{
	for ( int index = 0; index < g_NUMBER_OF_LIGHTS; index++ )
	{
		// Create a new GO
		cGameObject* pTempGO = new cGameObject();

		pTempGO->meshName = "sphere"; // Set the name of the mesh
		
		// position is based on light position
		pTempGO->position = ::g_pLightManager->vecLights[index].position;

		if ( index == 0 ) pTempGO->scale = 3.0f;
		else pTempGO->scale = 1.0f;

		// Each light is initially white
		pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );

		pTempGO->bIsLight = true;
		pTempGO->myLight = index;

		::g_vecGameObjects.push_back( pTempGO );
	}
}

void updateAllObjects( double deltaTime )
{
 	for( int i = 0; i != g_theScheduler.commandGroups.size(); i++ )
	{
		if( !g_theScheduler.commandGroups[i]->isDone() )
		{
			// Run the Command groups in sequence
			// Each command group runs its commands in paralel
			g_theScheduler.commandGroups[i]->update( deltaTime );
			break;	
		}
	}

	return;	
}

// Draw a single object
void DrawObject( cGameObject* pTheGO )
{
	// Is there a game object? 
	if( pTheGO == 0 )
	{	// Nothing to draw
		return;		// Skip all for loop code and go to next
	}

	// Was near the draw call, but we need the mesh name
	std::string meshToDraw = pTheGO->meshName;		//::g_GameObjects[index]->meshName;

	sVAOInfo VAODrawInfo;
	if( ::g_pVAOManager->lookupVAOFromName( meshToDraw, VAODrawInfo ) == false )
	{	// Didn't find mesh
		std::cout << "WARNING: Didn't find mesh " << meshToDraw << " in VAO Manager" << std::endl;
		return;
	}

	// Change Light Objects position based on light position
	// The game object sphere that "contains" the light follows the light
	if( pTheGO->bIsLight == true )
	{
		int lightIndex = pTheGO->myLight;
		pTheGO->position = ::g_pLightManager->vecLights[lightIndex].position;
		pTheGO->diffuseColour = glm::vec4( ::g_pLightManager->vecLights[lightIndex].diffuse, 1.0f );
	}

	// There IS something to draw
	glm::mat4x4 mModel = glm::mat4x4( 1.0f );
	
	glm::mat4 trans = glm::mat4x4( 1.0f );
	trans = glm::translate( trans, pTheGO->position );
	mModel = mModel * trans;

	mModel = mModel * pTheGO->orientation;

	float finalScale = pTheGO->scale;

	glm::mat4 matScale = glm::mat4x4( 1.0f );
	matScale = glm::scale( matScale, glm::vec3( finalScale, finalScale, finalScale ) );
	mModel = mModel * matScale;

	glUniformMatrix4fv( uniLoc_mModel, 1, GL_FALSE, ( const GLfloat* )glm::value_ptr( mModel ) );

	glm::mat4 mWorldInvTranpose = glm::inverse( glm::transpose( mModel ) );
	glUniformMatrix4fv( uniLoc_mWorldInvTrans, 1, GL_FALSE, ( const GLfloat* )glm::value_ptr( mWorldInvTranpose ) );

	
	glUniform4f( uniLoc_materialDiffuse,
		pTheGO->diffuseColour.r,
		pTheGO->diffuseColour.g,
		pTheGO->diffuseColour.b,
		pTheGO->diffuseColour.a );
	//... and all the other object material colours
	
	// Check if the bIsWireframe is true and set PolygonMonde to GL_LINE
	if( bIsWireframe )	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Default

	glEnable( GL_DEPTH_TEST );	// Test for Z and store in z buffer
	glCullFace( GL_BACK );		// Draw only the normals that are "front-facing"

	glBindVertexArray( VAODrawInfo.VAO_ID );

	glDrawElements( GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,
		GL_UNSIGNED_INT,	// 32 bit int
		0 ); // g_numberOfVertices

	glBindVertexArray( 0 );

	return;
}