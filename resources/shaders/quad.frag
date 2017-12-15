
#version 150

in vec2 textCoord;
flat in int FxControl;
uniform sampler2D Texture;
out vec4 out_Color;

// fxControl ^= 1 << (
// 	key == 55? 0: // b/w
// 	key == 56? 1: // h flip
// 	key == 57? 2: // v flip
// 	key == 48? 3: // blur
// 	4);

bool isActive(int i) {
	return (FxControl & (1 << i)) != 0;
}

void main () {
	// make a temp variable
	vec2 cur = textCoord;
	// flip functionality
	if (isActive(1)) cur.x = 1 - cur.x;
	if (isActive(2)) cur.y = 1 - cur.y;
	// find a px rgb value
	vec3 t = texture(Texture, cur).rgb;
	// blur effect
	if (isActive(3)) {
		float r = 3;
		vec3 acc;
		float reso = 500;
		for (float x = -r; x <= r; x ++)
			for (float y = -r; y <= r; y ++)
				if (x != 0 || y != 0)
					acc += texture(Texture, 
						cur + vec2(x/reso, y/reso)).rgb;
		t = acc / pow(r * 2 + 1, 2);
	}
	// black and white
	if (isActive(0)) {
		float bw = 
			.2126 * t.r + 
			.7152 * t.g + 
			.0722 * t.b;
		t = vec3(bw,bw,bw);
	}
	// return the result
	out_Color = vec4(t.r, t.g, t.b, 1);
}
