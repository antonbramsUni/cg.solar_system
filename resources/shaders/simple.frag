
#version 150

flat in int shading;
in  vec3  normal;
in  vec3  light;
in  vec3  view;
in  vec3  color;
in  vec2  pass_TexCoord;
out vec4  out_Color;

uniform sampler2D ColorTex;

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
	vec3 col = true? texture(ColorTex, pass_TexCoord).rgb: color;
	// ambient
	float ambientStrength  = .2;
	vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 diffuse = max(dot(normal, light), .0) * lightColor;
	// Blinn Phong specular
	vec3 specular = vec3(0,0,0);
	if (true) {
		// code was done following https://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
		vec3 halfWay  = normalize(light + view);
		specular = pow(max(dot(normal, halfWay), .0), 16.0) * lightColor;
	// Phong specular
	} else {
		// code was done following https://learnopengl.com/#!Lighting/Basic-Lighting
		float specularStrength = .5;
		vec3  refl = reflect(-light, normal);
		float spec = pow(max(dot(view, refl), .0), 32);
		specular = specularStrength * spec * lightColor;
	}
	// result
	vec3 result = (diffuse + ambient + specular) * col;
	// sun shading so it is always yellow
	if (shading == 2) {
		result = col;
	// cell shading
	} else if (shading == 1) {
		// break the color, the step on the change on color is 4
		result = breakColor(result, 4.f) + color * ambientStrength;
		// contour with the same color as ambient
		if (dot(view, normal) < .3f) result = ambient;
	}
	out_Color = vec4(result, 1.0);
}
