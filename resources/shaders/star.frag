
#version 150

in  vec2  pass_TexCoord;
out vec4  out_Color;
uniform sampler2D ColorTex;

void main () {
	// texture
	vec3 color = texture(ColorTex, pass_TexCoord).rgb;
	out_Color = vec4(color, 1.0);
}
