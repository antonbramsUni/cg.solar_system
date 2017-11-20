
#version 150

in vec3 normal;
in vec3 light;
in vec3 view;
in vec3 color;
out vec4 out_Color;

void main () {
	// color settings
	vec3 lightColor  = vec3(1, 1, 1);
	// ambient
	float ambientStrength  = .1;
	vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 diffuse = max(dot(normal, light), .0) * lightColor;
	// phong specular
	// float specularStrength = .5;
	// vec3  refl = reflect(-light, normal);
	// float specular = pow(max(dot(view, refl), .0), 32);
	// vec3  spec = specularStrength * specular * lightColor;
	// blinn phong specular
	vec3 halfWay  = normalize(light + view);
	vec3 specular = pow(max(dot(normal, halfWay), .0), 16.0) * lightColor;
	// result
	vec3 result = (diffuse + ambient + specular) * color;
	out_Color = vec4(result, 1.0);
	// test
	// out_Color = vec4(abs(normalize(normal)), 1.0);
}
