uniform vec3 CameraPos;

uniform vec3 Ambient;
uniform vec3 Diffuse;
uniform vec3 Specular;
uniform float Shininess;


uniform int HasDiffuseMap;
uniform sampler2D DiffuseMap;

in vec3 normalInterp;
in vec2 TexCoord2D;
in vec4 vertPos;
in vec4 shadowMapCoord;
out vec4 FragColor;

uniform sampler2DShadow ShadowMap;

void main()
{
	vec3 diffuseMap;
	
	if (HasDiffuseMap != 0)
		 diffuseMap = texture(DiffuseMap, TexCoord2D).rgb;
	else
		 diffuseMap = vec3(1.0);

    // TODO: Replace with Phong shading

	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(CameraPos - vec3(vertPos));

	float lambertian = max(dot(lightDir, normal), 0.0);
	float specularFloat = 0.0;

	if(lambertian > 0.0){
		vec3 viewDir = vec3(normalize(-vertPos));

		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0);
		specularFloat = pow(specAngle, Shininess);
	}

	vec3 colorLinear = lambertian * diffuseMap * Diffuse + specularFloat * Specular;

	float visibility = textureProj(ShadowMap, shadowMapCoord);
	//FragColor = shadowMapCoord.xxx / shadowMapCoord.yyy;
	//FragColor = shadowMapCoord.yyy / shadowMapCoord.www;
	FragColor = vec4(colorLinear * visibility, 1.0);
}
/*

void main()
{
	vec3 diffuseMap;
	vec3 n = normalize(normal);
	vec3 V = normalize(CameraPos - position.xyz);
	vec3 L = normalize(V);
	vec3 H = normalize(L + V);
	vec3 R = normalize(2*dot(L,n)*n - L);
	float visibility = 0.5*textureProj(ShadowMap, shadowMapCoord);

	if (HasDiffuseMap != 0)
		diffuseMap = texture(DiffuseMap, texCoord).rgb;
	else
		diffuseMap = vec3(1.0);
	
	// TODO: Replace with Phong shading
    float lambertian = max(dot(L,n), 0.0);
	float specAngle = max(dot(n,H),0.0);
	float specular = pow(specAngle,Shininess);
	vec3 color = visibility*0.5*Ambient + Diffuse*lambertian + Specular*specular;
	float gamma = 2.2;
	vec3 colorGammaCorrected = pow(color, vec3(1.0/gamma));
	FragColor = vec4(diffuseMap,1.0) *  vec4(color,1.0)

*/