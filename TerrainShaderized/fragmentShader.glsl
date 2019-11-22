#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;
in flat int Exportype;
in vec4 yheight;

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
		if (yheight.y <= 3.0f)
		{
			if (yheight.y >= 2.0f)
			{
				texcol = mix(fieldTexColor, mudTexColor, yheight.y / 10);
			} 
			else
			{
				texcol = mudTexColor;
			}
		} 
		else if (yheight.y >= 3.0f && yheight.y <=8.0f)
		{
			if (yheight.y <= 7.0f)
			{
				texcol = fieldTexColor;	
			} 
			else
			{
				texcol = mix(snowTexColor, fieldTexColor, yheight.y / 10);
			}
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

	normal = normalize(normalExport);
	lightDirection = normalize(vec3(light0.coords));
	fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFandB.difRefl); 
	if (Exportype == 1){
		colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0f) * texcol * fAndBDif;  
		colorsExport.a = 0.6f;
	}else if (Exportype == 3){
		colorsExport =  vec4(0.55f, 0.27f, 0.075f, 1.0f);
	}
	else {
		colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * texcol * fAndBDif;  
		colorsExport.a = 1.0f;
	}
}