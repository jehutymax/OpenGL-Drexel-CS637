attribute  vec4 vPosition;
attribute vec4 vColor;

uniform mat4 Projection;
uniform mat4 Model_View;

attribute  vec2 vTexCoord;
varying vec2 texCoord;

varying vec4 color;

void main() 
{
    gl_Position = Projection * Model_View * vPosition;
    color = vColor;
    texCoord = vTexCoord;
} 
