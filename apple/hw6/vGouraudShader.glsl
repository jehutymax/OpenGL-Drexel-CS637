attribute  vec4 vPosition;
attribute  vec3 vNormal;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;
uniform vec4 LightPosition, LightPosition2;
uniform float Shininess;

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (modelview_matrix * vPosition).xyz;
	
    vec3 L = normalize( (modelview_matrix*LightPosition).xyz - pos );
    vec3 L2 = normalize( (modelview_matrix*LightPosition2).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );
    vec3 H2 = normalize( L2 + E );


    // Transform vertex normal into eye coordinates
    vec3 N = normalize( modelview_matrix*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    float Kd2 = max( dot(L2, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;
    vec4  diffuse2 = Kd2*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    float Ks2 = pow( max(dot(N, H2), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    vec4  specular2 = Ks2 * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    if( dot(L2, N) < 0.0 ) {
	specular2 = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = projection_matrix * modelview_matrix * vPosition;

    color = ambient + diffuse + specular + diffuse2 + specular2;
    color.a = 1.0;
}