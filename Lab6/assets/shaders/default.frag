#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;

// Gets the color of the light from the main function
uniform vec3 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Sets values for light distance
uniform vec2 fade;
// Sets values for light direction
uniform vec3 lightPointed;
// Gets object color
uniform vec3 objectColor;
// Gets the position of the camera from the main function
uniform vec3 camPos;

// type of lighting
uniform int lightType;

vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = fade.x;
	float b = fade.y;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	vec3 res = (ambient + diffuse * inten + specular * inten) * objectColor * lightColor;
	return  vec4(res, 1.0);
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-lightPointed);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	vec3 res = (ambient + diffuse + specular) * objectColor * lightColor;
	return vec4(res, 1.0);
}

vec4 spotLight()
{
    vec3 lightDir = normalize(lightPos - crntPos);
    float theta = dot(lightDir, -lightPointed);
    float cutOff = cos(radians(45.0));

	// ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result;
    if (theta > cutOff) 
    {
        // diff lighting
        vec3 norm = normalize(Normal);
            
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
            
        // specular lighting
        float specularStrength = 1.0;
        vec3 viewDir = normalize(camPos - crntPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;

        // result
        result = (ambient + diffuse + specular) * objectColor;
    } 
    else 
    {
        result = ambient * objectColor;
    }
	return vec4(result, 1.0);
}

void main()
{
	FragColor = vec4(0.0);
	if (lightType == 1) {
        FragColor = pointLight();
    } else if (lightType == 2) {
        FragColor = direcLight();
    } else {
		FragColor = spotLight();
    }
}