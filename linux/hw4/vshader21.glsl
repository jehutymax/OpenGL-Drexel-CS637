
attribute vec4 vPosition;
attribute vec4 vColor;
varying vec4 color;
uniform mat4 ctm;

void main() {
    // Compute the sines and cosines of theta for each of
    //   the three axes in one computation.

    color = vColor;

    gl_Position = ctm * vPosition;
}