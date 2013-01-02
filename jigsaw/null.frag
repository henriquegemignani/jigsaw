//
// Fragment shader
//

#version 130

in vec2 texture_coordinate;
in vec4 colors;
uniform sampler2D texture_id;
out vec4 FragmentColor;

void main(void) {
	FragmentColor = texture(texture_id, texture_coordinate) * colors;
}