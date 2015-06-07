
#include "Angel.h"

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
GLint color;

int arm_index = 0;

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

// RGBA olors
color4 vertex_colors[3] = {
    color4( 0.4, 0.58, 0.93, 1.0 ),  // blue
    color4( 1.0, 0.55, 0.0, 1.0 ),  // dark orange
    color4( 0.42, 0.55, 0.14, 1.0 )  // olive
};


// Parameters controlling the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;

// Shader transformation matrices
mat4  model_view;
GLuint ModelView, Projection;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int      Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };

// Menu option values
const int  Quit = 4;

GLfloat radius = 24;  
GLfloat theta = 1.0472;
GLfloat phi = 0.959931;

GLfloat  left = -10.0, right = 10.0;
GLfloat  bottom = -5.0, top = 15.0;
GLfloat  zNear = -15.0, zFar = 10.0;

const GLfloat  dr = 5.0 * DegreesToRadians;


//----------------------------------------------------------------------------

int Index = 0;

void
quad( int a, int b, int c, int d )
{
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[b]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[d]; Index++;
}

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

void
base()
{
    mat4 instance = ( Translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ) *
		 Scale( BASE_WIDTH,
			BASE_HEIGHT,
			BASE_WIDTH ) );

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glUniform3fv(color, 1, vertex_colors[0]);

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
upper_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) *
		      Scale( UPPER_ARM_WIDTH,
			     UPPER_ARM_HEIGHT,
			     UPPER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glUniform3fv(color, 1, vertex_colors[1]);
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
lower_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) *
		      Scale( LOWER_ARM_WIDTH,
			     LOWER_ARM_HEIGHT,
			     LOWER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glUniform3fv(color, 1, vertex_colors[2]);
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
display( void )
{
    point4  eye(radius*sin(theta)*cos(phi),
         radius*sin(theta)*sin(phi),
         radius*cos(theta),
         1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, -1.0, 0.0, 0.0 );

    mat4 mv = LookAt(eye, at, up);

    mat4 projection = Frustum( left, right, bottom, top, zNear, zFar );
    projection = projection * mv;
    
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Accumulate ModelView Matrix as we traverse the tree
    model_view = RotateY(Theta[Base] );
    base();

    model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
		    RotateZ(Theta[LowerArm]) );
    lower_arm();

    model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
		    RotateZ(Theta[UpperArm]) );
    upper_arm();

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
init( void )
{
    colorcube();
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points),
		  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    color = glGetUniformLocation(program, "Color");

    glEnable( GL_DEPTH );

    glClearColor( 0.63, 0.63, 0.63, 1.0 ); 
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{

    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
	// Incrase the joint angle
	Theta[Axis] += 5.0;
	if ( Theta[Axis] > 360.0 ) { Theta[Axis] -= 360.0; }
    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
	// Decrase the joint angle
	Theta[Axis] -= 5.0;
	if ( Theta[Axis] < 0.0 ) { Theta[Axis] += 360.0; }
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
menu( int option )
{
    if ( option == Quit ) {
	exit( EXIT_SUCCESS );
    }
    else {
	Axis = option;
    }
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }

    model_view = mat4( 1.0 );  // An Identity matrix
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    case 'r': radius *= 2.0; break;
    case 'R': radius *= 0.5; break;
    case 'o': theta += dr; break;
    case 'O': theta -= dr; break;
    case 'p': phi += dr; break;
    case 'P': phi -= dr; break;
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "robot arm" );
    glewExperimental=GL_TRUE; 
    glewInit(); 
    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );

    glutCreateMenu( menu );
    // Set the menu values to the relevant rotation axis values (or Quit)
    glutAddMenuEntry( "Base", Base );
    glutAddMenuEntry( "Lower arm", LowerArm );
    glutAddMenuEntry( "Upper arm", UpperArm );
    glutAddMenuEntry( "Quit", Quit );
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    glutMainLoop();
    return 0;
}
