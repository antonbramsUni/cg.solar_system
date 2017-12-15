
#version 150

in vec3 normal;
in vec3 light;
in vec3 view;
in vec2 textCoord;
in vec3 tangent;
flat in int shading;
flat in int id;
uniform sampler2D TextureColor;
uniform sampler2D TextureNormal;
out vec4 out_Color;

vec3 breakColor (vec3 c, float factor) {
	// color broken in RGB
	c.r = round(c.r * factor) / factor;
	c.g = round(c.g * factor) / factor;
	c.b = round(c.b * factor) / factor;
	return c;
}

void main () {
	// color settings
	vec3 lightColor = vec3(1, 1, 1);
	// texture
	vec3 textureColor = texture(TextureColor,  textCoord).rgb;
	vec3 norm = normal;
	if (id == 3) {
		vec3 tn  = texture(TextureNormal, textCoord).rgb;
		vec3 bi  = normalize(cross(normal, tangent));
		mat3 tbn = mat3(tangent, bi, normal);
		norm = normalize(tbn * tn);
	}
	// ambient
	float ambientStrength  = .15;
	vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 diffuse = max(dot(norm, light), .0) * lightColor;
	// Blinn Phong specular
	vec3 specular = vec3(0,0,0);
	if (true) {
		// code was done following https://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
		vec3 halfWay  = normalize(light + view);
		specular = pow(max(dot(norm, halfWay), .0), 16.0) * lightColor;
	// Phong specular
	} else {
		// code was done following https://learnopengl.com/#!Lighting/Basic-Lighting
		float specularStrength = .5;
		vec3  refl = reflect(-light, norm);
		float spec = pow(max(dot(view, refl), .0), 32);
		specular = specularStrength * spec * lightColor;
	}
	// result
	vec3 result = (diffuse + ambient + specular) * textureColor;
	// sun shading so it is always yellow
	if (id == 0) {
		result = textureColor;
	// cell shading
	} else if (shading == 1) {
		// break the color, the step on the change on color is 4
		result = breakColor(result, 4.f) + textureColor * ambientStrength;
		// contour with the same color as ambient
		if (dot(view, norm) < .3f) result = ambient;
	}
	out_Color = vec4(result, 1.0);
}
