// CS637 HW4 - Shapes.cpp

#include "Angel.h"
#include <string>
#include <iomanip>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)
                            //  (3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

mat4 N = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
mat4 ctm;
GLuint transformationMatrix;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
enum Transformation { SCALE = 0, ROTATION = 1, TRANSLATION = 2, 
    NumberOfTransformations = 3, NoTransformationSelected };
std::string ttext[NumberOfTransformations] = {
    "Scale", "Rotation", "Translation"
};
GLfloat deltas[NumberOfTransformations] = {0.0, 0.0, 0.0};
Transformation currentTransformation = NoTransformationSelected;
vec3 currentValuesXYZ[NumberOfTransformations] = 
    { vec3(1.0, 1.0, 1.0),
      vec3(0.0, 0.0, 0.0),  
      vec3(0.0, 0.0, 0.0)
    };
int Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat Translation[NumAxes] = {0.0, 0.0, 0.0 };
GLfloat Scaling[NumAxes] = {1.0, 1.0, 1.0};
GLuint theta;  // The location of the "theta" shader uniform variable
GLuint translation;
GLuint scale;
    
int Index = 0;
void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}


// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}


void init( void )
{
      // Setup the initial position for the cube
   mat4 initialCtm = RotateX(-45.0) * RotateY(45.0);

   for (int i = 0; i < 8; ++i)
    {
        point4 newPoint = initialCtm * vertices[i];
        vertices[i] = newPoint;
    }

   colorcube();

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    

     // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    // Initialize color
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points)) );

    glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glUniformMatrix4fv(transformationMatrix, 1, GL_TRUE, ctm);

    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
   
    glutSwapBuffers();
}

void printState()
{
    std::cout << "\r\033[K" << std::flush;

    if (currentTransformation == NoTransformationSelected)
    {
        std::cout << " No transformation has been selected. Right-click to choose." << std::flush;
    }
    else
    {
    std::cout << ttext[currentTransformation] << ": " <<
        "+/- " << deltas[currentTransformation] <<
        " | (X, Y, Z) = (" << currentValuesXYZ[currentTransformation][0] <<
        ", " << currentValuesXYZ[currentTransformation][1] <<
        ", " << currentValuesXYZ[currentTransformation][2] << ")" << std::flush;
    }
    
}

void updateDelta(bool increase, bool bigDiff)
{
    GLfloat value = (bigDiff ? 0.1 : 0.01);
    GLfloat diff = (increase ? value : value * (-1.0));
    deltas[currentTransformation] += diff;
}

void updateTransformation(bool increase, int axis)
{
    GLfloat diff = (increase ? deltas[currentTransformation] : 
        deltas[currentTransformation] * (-1.0));

    currentValuesXYZ[currentTransformation][axis]+= diff;
}

void updateAllValues()
{
    for (int i = 0; i < NumAxes; i++)
    {
        Scaling[i] = currentValuesXYZ[SCALE][i];
        Translation[i] = currentValuesXYZ[TRANSLATION][i];
        Theta[i] = currentValuesXYZ[ROTATION][i];
    }

    mat4 T = Translate(Translation[0], Translation[1], Translation[2]);
    mat4 Tinv = Translate(-1.0 * Translation[0], -1.0 * Translation[1], -1.0 * Translation[2]);
    mat4 R = RotateZ(Theta[2]) * RotateY(Theta[1]) * RotateX(Theta[0]);
    mat4 finalRot = T * R * Tinv;
    mat4 S = Scale(Scaling[0], Scaling[1], Scaling[2]);

    ctm =  S * finalRot * T;
}

void resetAllValues()
{
    currentValuesXYZ[0] = vec3(1.0, 1.0, 1.0);
    currentValuesXYZ[1] = vec3(0.0, 0.0, 0.0);
    currentValuesXYZ[2] = vec3(0.0, 0.0, 0.0);
      
    Axis = Xaxis;

    for (int i = 0; i < NumAxes; i++)
    {
        Theta[i] = 0.0;
        Translation[i] = 0.0;
        Scaling[i] = 1.0;
        deltas[i] = 0;
    }

    ctm = identity();
}

void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033: // esc
    case 0x0D: // enter
        exit( EXIT_SUCCESS );
        break;
    case 075: // =
        updateDelta(true, true);
        break;
    case 055: // -
        updateDelta(false, true);
        break;
    case 0141: // a
        updateTransformation(true, Xaxis);
        break;
    case 0163: // s
        updateTransformation(true, Yaxis);
        break;
    case 0144: // d
        updateTransformation(true, Zaxis);
        break;
    case 0172: // z
        updateTransformation(false, Xaxis);
        break;
    case 0170: // x
        updateTransformation(false, Yaxis);
        break;
    case 0143: // c
        updateTransformation(false, Zaxis);
        break;
    case 0162: // r
        resetAllValues();
        break;  
    }

    updateAllValues();
    printState();
}



void idle()
{
    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    // doesn't do anything in this case!              
}

void setCurrentTransformation(Transformation t)
{
    currentTransformation = t;
}

void menu(int id)
{
    setCurrentTransformation((Transformation)id);
    printState();

    glutPostRedisplay();
}

void printHelp()
{
    std::cout << "CS637 HW4" << std::endl;
    std::cout << "GLSL version in use: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "    About: displays a 3d cube." << std::endl;
    std::cout << "    Usage: " << std::endl;
    std::cout << "\t Right-click to select a transformation. " << std::endl;
    std::cout << "\t = / - \t Increase / Decrease the delta for the selected transformation." << std::endl;
    std::cout << "\t a / z \t Apply / Inverse Apply the current transformation on the X axis." << std::endl;
    std::cout << "\t s / x \t Apply / Inverse Apply the current transformation on the Y axis." << std::endl;
    std::cout << "\t d / c \t Apply / Inverse Apply the current transformation on the Z axis." << std::endl;
    std::cout << "\t r \t Reset all values." << std::endl;
    std::cout << "Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    " << std::endl;
}

int main( int argc, char **argv )
{

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( 500, 500 );

    int win = glutCreateWindow("ICG: Assignment 4");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glewExperimental=GL_TRUE; 
    glewInit();     
    init();

    glutCreateMenu(menu);
    glutAddMenuEntry("Scale", SCALE);
    glutAddMenuEntry("Rotation", ROTATION);
    glutAddMenuEntry("Translation", TRANSLATION);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

   glutIdleFunc(idle);

    printHelp();

    glutMainLoop();

    std::cout << std::endl;
    
    return 0;
}
