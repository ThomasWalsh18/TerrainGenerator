#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;
in flat int Exportype;
in flat float yheight;

out vec4 colorsExport;

uniform sampler2D Tex;
uniform sampler2D mudTex;
uniform sampler2D snowTex;

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
	vec4 snowTexColor = texture(snowTex, texCoordsExport);
	
	if (Exportype == 0){
		if (yheight < 4.0f)
		{
			if (yheight > 3.5f)
			{
				texcol = mix(fieldTexColor, mudTexColor, 0.5f);
			} 
			else
			{
				texcol = mudTexColor;
			}
		} 
		else if (yheight > 4.0f)
		{
			if (yheight < 5.5f){
				texcol = mix(snowTexColor, fieldTexColor, 0.5f);
			} 
			else
			{
				texcol = snowTexColor;
			}
		}
		else
		{
			texcol = fieldTexColor;
		}

	}
	else 
	{
		texcol = fieldTexColor;
	}

	normal = normalize(normalExport);
	lightDirection = normalize(vec3(light0.coords));
	fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFandB.difRefl); 
	colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0)* texcol * fAndBDif;  
}