
#version 150

in vec3 normal;
in vec3 light;
in vec3 view;
in vec3 color;
in float shading;
out vec4 out_Color;

vec3 breakColor (vec3 c, float factor) {
	c.r = round(c.r * factor) / factor;
	c.g = round(c.g * factor) / factor;
	c.b = round(c.b * factor) / factor;
	return c;
}

void main () {
	// color settings
	vec3 lightColor  = vec3(1, 1, 1);
	// ambient
	float ambientStrength  = .1;
	vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 diffuse = max(dot(normal, light), .0) * lightColor;
	// blinn phong specular
	vec3 specular = vec3(0,0,0);
	if (true) {
		vec3 halfWay  = normalize(light + view);
		specular = pow(max(dot(normal, halfWay), .0), 16.0) * lightColor;
	// phong specular
	} else {
		float specularStrength = .5;
		vec3  refl = reflect(-light, normal);
		float spec = pow(max(dot(view, refl), .0), 32);
		specular = specularStrength * spec * lightColor;
	}
	// result
	vec3 result = (diffuse + ambient + specular) * color;
	// sun shading
	if (shading == 2.0f) {
		result = color;
	// cell
	} else if (shading == 1.0f) {
		// break the color
		result = breakColor(result, 4.f) + color * ambientStrength;
		// contour
		if (dot(view, normal) < .3f) result = ambient;
	}
	out_Color = vec4(result, 1.0);
}
