#version 120

uniform sampler2D texture;

uniform vec4 tintColor;

void main()
{
vec4 pixel_color = texture2D(texture, gl_TexCoord[0].xy);
float percent = tintColor.a;

vec4 colorDifference = vec4(0,0,0,1);

colorDifference.r = tintColor.r - pixel_color.r;
colorDifference.g = tintColor.g - pixel_color.g;
colorDifference.b = tintColor.b - pixel_color.b;
pixel_color.r = pixel_color.r + colorDifference.r * percent;
pixel_color.g = pixel_color.g +colorDifference.g * percent;
pixel_color.b =pixel_color.b + colorDifference.b * percent;


gl_FragColor = pixel_color;
}
