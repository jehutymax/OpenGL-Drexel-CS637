attribute   vec4 vPosition;
attribute   vec3 vNormal;

// output values that will be interpretated per-fragment
varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;
varying  vec3 fL2;

uniform mat4 modelview_matrix;
uniform vec4 LightPosition, LightPosition2;
uniform mat4 projection_matrix;

void main()
{
    fN = vNormal;
    fE = vPosition.xyz;
    fL = (modelview_matrix*LightPosition).xyz;
    fL2 = (modelview_matrix*LightPosition2).xyz;
    
    if( LightPosition.w != 0.0 ) {
	fL = LightPosition.xyz - vPosition.xyz;
    }

    if( LightPosition2.w != 0.0 ) {
	fL2 = LightPosition2.xyz - vPosition.xyz;
    }

    gl_Position = projection_matrix*modelview_matrix*vPosition;
}