//
// Trivial Vertex shader
//

#version 130

in vec4 position;
in vec2 texCoord;

out vec2 texture_coordinate;
out vec4 colors;

void main(void) { 
	gl_Position = gl_ModelViewProjectionMatrix * position;
    
    texture_coordinate = vec2(gl_MultiTexCoord0);
    colors = gl_Color;
}