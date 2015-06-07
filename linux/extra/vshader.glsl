attribute  vec4 vPosition;


uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec3 Color;

varying vec4 color;

void main() 
{
    color = vec4(Color, 1.0);
    gl_Position = Projection * ModelView * vPosition;
} 
