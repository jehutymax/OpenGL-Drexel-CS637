// per-fragment interpolated values from the vertex shader
varying  vec3 fN;
varying  vec3 fL, fL2;
varying  vec3 fE;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 modelview_matrix;
uniform vec4 LightPosition, LightPosition2;
uniform float Shininess;

void main() 
{ 
    // Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
    vec3 L2 = normalize(fL2);

    vec3 H = normalize( L + E );
    vec3 H2 = normalize( L2 + E );
    
    vec4 ambient = AmbientProduct;

    float Kd = max(dot(L, N), 0.0);
    float Kd2 = max(dot(L2, N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;
    vec4 diffuse2 = Kd2*DiffuseProduct;
    
    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);
    vec4 specular = Ks*SpecularProduct;
    vec4 specular2 = Ks2*SpecularProduct;

    // discard the specular highlight if the light's behind the vertex
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

     if( dot(L2, N) < 0.0 ) {
	specular2 = vec4(0.0, 0.0, 0.0, 1.0);
    }

    gl_FragColor = ambient + diffuse + specular + diffuse2 + specular2;
    gl_FragColor.a = 1.0;
} 
