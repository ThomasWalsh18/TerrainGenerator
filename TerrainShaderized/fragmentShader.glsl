#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;
in flat int Exportype;
in vec4 yheight;

in vec3 skyBoxTextures;

out vec4 colorsExport;

uniform sampler2D Tex;
uniform sampler2D mudTex;
uniform sampler2D rockTex;

uniform samplerCube skybox;

struct Light
{
   vec4 ambCols;
   vec4 difCols;
   vec4 specCols;
   vec4 coords;
};

uniform Light light0;

uniform vec4 globAmb;
  
struct Material
{
   vec4 ambRefl;
   vec4 difRefl;
   vec4 specRefl;
   vec4 emitCols;
   float shininess;
};
uniform Material terrainFandB;

vec3 normal, lightDirection;
vec4 fAndBDif;

void main(void)
{
	vec4 texcol;
	vec4 fieldTexColor = texture(Tex, texCoordsExport);
	vec4 mudTexColor = texture(mudTex, texCoordsExport);
	vec4 rockTexColor = texture(rockTex, texCoordsExport);

	if (Exportype == 0){
		if (yheight.y <= 0){
			texcol = mudTexColor;
		}
		if (yheight.y < 1 && yheight.y > 0){
			texcol = mix(mudTexColor, fieldTexColor, yheight.y);
		}		
		if (yheight.y >=1 && yheight.y <=10){
			texcol = mix(fieldTexColor, rockTexColor, yheight.y /10);
		}
		if (yheight.y > 10){
			texcol = rockTexColor;
		}
	}
	else 
	{
		texcol = fieldTexColor;
	}


	normal = normalize(normalExport);
	lightDirection = normalize(vec3(light0.coords));
	fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFandB.difRefl); 
	if (Exportype == 1){
		colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0f) * texcol * fAndBDif;  
		colorsExport.a = 0.6f;
	}
	else if (Exportype == 3)
	{
		colorsExport =  vec4(0.45f, 0.27f, 0.075f, 1.0f);
	}
	else if (Exportype == 4) { // if you are sky box
		colorsExport =  texture(skybox, skyBoxTextures);
	}
	else 
	{
		colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * texcol * fAndBDif;  
		colorsExport.a = 1.0f;
	}
}