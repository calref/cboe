uniform sampler2D texture;
uniform sampler2D mask;
uniform vec2 offset;
uniform vec2 src;

void main() {
	// Lookup the pixels in the textures
	vec4 maskpix = texture2D(mask, gl_TexCoord[0].xy);
	vec4 texpix = texture2D(texture, gl_TexCoord[0].xy);
	
	// If the mask is black, carry the soruce texture through
	if(maskpix.rgb == vec3(0.0,0.0,0.0))
		gl_FragColor = texpix;
	else gl_FragColor = vec4(0.0,0.0,0.0,0.0);
}
