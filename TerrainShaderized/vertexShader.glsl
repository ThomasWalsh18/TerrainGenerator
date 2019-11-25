#version 430 core

layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;
layout(location=2) in vec2 terrainTexCoords;


uniform vec4 globAmb;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform mat4 viewMat;

uniform vec3 campos;

uniform mat3 normalMat;
uniform int type;
uniform float time;

out vec3 normalExport;
out vec2 texCoordsExport;
out flat int Exportype;
out vec4 yheight;

out vec3 skyBoxTextures;


void main(void)
{
	Exportype = type;
	vec4 pos;
	pos = terrainCoords;
	if (type == 0){
		yheight = pos;
	} 
	else if (type == 1)
	{
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
	
	if (type == 4){ // if you are the sky box
		pos = projMat * viewMat * vec4(terrainCoords.x + campos.x, terrainCoords.y+ campos.y, terrainCoords.z + campos.z, 1.0f);// + vec4(campos.x, campos.y, campos.z, 1.0f);
		//pos = projMat * viewMat * vec4(terrainCoords.x, terrainCoords.y, terrainCoords.z , 1.0f);// + vec4(campos.x, campos.y, campos.z, 1.0f);
		gl_Position = pos.xyww;
		skyBoxTextures = vec3(terrainCoords.x, terrainCoords.y, terrainCoords.z);
	}
	else 
	{
		normalExport = terrainNormals;
		//normalExport = normalize(normalMat * normalExport);
		texCoordsExport = terrainTexCoords;
		if (type == 5) {
			vec4 repeat;
				repeat = vec4(pos.x + gl_InstanceID / 2, pos.y + sin(gl_InstanceID), pos.z , pos.w);
	
			gl_Position = projMat * viewMat * modelMat * repeat;
		} else {
			gl_Position = projMat * viewMat * modelMat * pos;
		}
	}
}