#include "cShaderManager.h"

cShaderManager::cShader::cShader()
{
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShaderManager::cShader::~cShader() 
{
	return;
}

std::string cShaderManager::cShader::getShaderTypeString(void)
{
	switch ( this->shaderType )
	{
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}


void cShaderManager::cShader::parseStringIntoMultiLine( std::string singleLineSouce )
{
#define _CRT_SECURE_NO_WARNINGS
	this->vecSource.clear();

	unsigned int stringsizeX4 = ( unsigned int )singleLineSouce.size() * 4;
	char* charSourceString = new char[stringsizeX4];
	strcpy( charSourceString, singleLineSouce.c_str() );

	char * pCharLine;
	pCharLine = strtok( charSourceString, "\n" );
	//char *next_token = NULL;  
	//pCharLine = strtok_s(charSourceString,"\n",&next_token);

	while( pCharLine != NULL )
	{	// Push back this line
		this->vecSource.push_back( std::string( pCharLine ) );
		pCharLine = strtok( NULL, "\n" );
		//pCharLine = strtok_s(NULL,"\n",&next_token);
	}
	delete[] charSourceString;
	return;
#undef _CRT_SECURE_NO_WARNINGS
}
