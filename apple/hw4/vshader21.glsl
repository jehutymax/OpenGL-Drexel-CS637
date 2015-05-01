
attribute vec4 vPosition;
attribute vec4 vColor;
varying vec4 color;
uniform vec3 theta;
uniform vec3 translation;
uniform vec3 scale;
uniform mat4 projection;

void main() {
    // Compute the sines and cosines of theta for each of
    //   the three axes in one computation.
    vec3 angles = radians( theta );
    vec3 c = cos( angles );
    vec3 s = sin( angles );
    // Remember: these matrices are column-major
    mat4 rx = mat4( 1.0, 0.0,  0.0, 0.0,
                    0.0, c.x, -s.x, 0.0,
                    0.0, s.x,  c.x, 0.0,
                    0.0, 0.0,  0.0, 1.0 );
    mat4 ry = mat4(  c.y, 0.0, s.y, 0.0,
                     0.0, 1.0, 0.0, 0.0,
                    -s.y, 0.0, c.y, 0.0,
                     0.0, 0.0, 0.0, 1.0 );
    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
                    s.z,  c.z, 0.0, 0.0,
                    0.0,  0.0, 1.0, 0.0,    
                    0.0,  0.0, 0.0, 1.0 );

    mat4 t = mat4(  1.0, 0.0, 0.0, translation.x,
                    0.0, 1.0, 0.0, translation.y,
                    0.0,  0.0, 1.0, translation.z,
                    0.0,  0.0, 0.0, 1.0 );

    mat4 sc = mat4( scale.x, 0.0, 0.0, 0.0,
                    0.0, scale.y, 0.0, 0.0,
                    0.0,  0.0, scale.z, 0.0,
                    0.0,  0.0, 0.0, 1.0 );

    color = vColor;
    mat4 rotation = rx * ry * rz;
    mat4 ctm = t * rotation * sc;
    gl_Position = projection * t * vPosition;
}