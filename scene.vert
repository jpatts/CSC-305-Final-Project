layout(location = SCENE_POSITION_ATTRIB_LOCATION)
in vec4 Position;

layout(location = SCENE_TEXCOORD_ATTRIB_LOCATION)
in vec2 TexCoord;

layout(location = SCENE_NORMAL_ATTRIB_LOCATION)
in vec3 Normal;


uniform mat4 ModelWorld;
uniform mat4 ModelViewProjection;
uniform mat3 Normal_ModelWorld;
uniform mat4 lightMatrix;

in vec3 vertex_color;
out vec2 TexCoord2D;
out vec3 normalInterp;
out vec4 vertPos;
out vec4 shadowMapCoord;

void main()
{
    // TODO: Set to MVP * P

    gl_Position = ModelViewProjection * Position;
    
    // TODO: Pass vertex attributes to fragment shader
	TexCoord2D = TexCoord;
	normalInterp = Normal_ModelWorld * Normal;
	vertPos = ModelWorld * Position;
	shadowMapCoord = lightMatrix * Position;
}