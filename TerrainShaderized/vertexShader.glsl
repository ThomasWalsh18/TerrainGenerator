#version 430 core

layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;
layout(location=2) in vec2 terrainTexCoords;


uniform vec4 globAmb;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform mat4 viewMat;

uniform mat3 normalMat;
uniform int type;
uniform float time;

out vec3 normalExport;
out vec2 texCoordsExport;
out flat int Exportype;
out vec4 yheight;


void main(void)
{
	Exportype = type;
	vec4 pos;
	pos = terrainCoords;
	if (type == 0){
		yheight = pos;
	}
	if (type == 1){
		//float amplitude = 1.;
		//float frequency = 1.;
		//pos.y += sin(pos.x * frequency);
		//float t = 0.01*(-time * 129.0);
		//pos.y += sin(pos.x*frequency*2.0 + t)*3.0;
		//pos.y += sin(pos.x*frequency*3.0 + t*1.0)*4.0;
		//pos.y += sin(pos.x*frequency*1.0 + t*1.0)*5.0;
		//pos.y += sin(pos.x*frequency*4.0+ t*4.0)*2.0;
		//pos.y *= amplitude*0.06;
		//pos.y += 0.1f * cos(pos.z + time);
		pos.y += 0.2f * (sin(pos.x + time) + cos(pos.z + time));
	}
		
	normalExport = terrainNormals;
	//normalExport = normalize(normalMat * normalExport);
	texCoordsExport = terrainTexCoords;
	gl_Position = projMat * viewMat * modelMat * pos;
	
 
}