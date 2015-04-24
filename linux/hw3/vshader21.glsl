attribute vec4 vPosition;
attribute vec3 vColor;
varying vec4 color;

void
main()
{
    gl_Position = vPosition;
    color = vec4( vColor, 1.0);
}
