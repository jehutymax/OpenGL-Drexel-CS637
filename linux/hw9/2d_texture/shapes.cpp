#include <cstring>
#include <algorithm>
#include "SMF.h"


typedef Angel::vec4 point4;
typedef Angel::vec4 color4;
typedef GLfloat point3[3];

const int NumPatchVertices = 16;
const int NumTimesToSubdivide = 4;
const int PatchesPerSubdivision = 4;
const int NumQuadsPerPatch = 64;
const int NumTriangles = 1*64*2;
const int NumVertices = 1550;
const int TextureSize  = 64;
const int NumPatchPatches = 1;

GLuint textures[1];
GLubyte image[TextureSize][TextureSize][3];

int indices[NumPatchPatches][4][4] = {
    {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {8, 9, 10, 11},
    {12, 13, 14, 15}
    }
};

int lookupControlPoint(int index)
{
    int r = index % 4;
    int c = index / 4;

    return r * 4 + c;
}

int Index = 0, controlIndex = 0, texIndex = 0;
const int NumControlPoints = 16;
point4 points[NumVertices + NumControlPoints + 6];
color4 color[NumVertices + NumControlPoints + 6];
GLuint  Projection, Model_View;
bool persp = true;

GLfloat radius = 5.0;
GLfloat theta = 3.40339;
GLfloat phi = 0.0872665;
const GLfloat  dr = 5.0 * DegreesToRadians;

GLfloat  left = -0.3, right =0.3;// 1.33;
GLfloat  bottom = -0.3, top = 0.3;
GLfloat  zNear = 0.25, zFar = 10.0;//7.83282;

enum { X = 0, Y = 1, Z = 2 };

std::vector<vec3>vertices;
vec2 tex_coords[NumVertices];
int currentControlPoint = 0;

//----------------------------------------------------------------------------

void divide_curve( point4 c[4], point4 r[4], point4 l[4] )
{
    point4  t, mid = ( c[1] + c[2] ) / 2;

    l[0] = c[0];
    l[1] = ( c[0] + c[1] ) / 2;
    l[2] = ( l[1] + mid ) / 2;

    r[3] = c[3];
    r[2] = ( c[2] + c[3] ) / 2;
    r[1] = ( mid + r[2] ) / 2;

    l[3] = r[0] = ( l[2] + r[1] ) / 2;
    
    for ( int i = 0; i < 4; ++i ) {
    	l[i].w = 1.0;
    	r[i].w = 1.0;
    }
}

//----------------------------------------------------------------------------

void draw_patch( point4 p[4][4] )
{
    points[Index++] = p[0][0]; tex_coords[texIndex++] = vec2(0.0f, 0.0f);
    points[Index++] = p[3][0]; tex_coords[texIndex++] = vec2(0.0f, 0.0f);
    points[Index++] = p[3][3]; tex_coords[texIndex++] = vec2(1.0f, 0.0f);
    points[Index++] = p[0][0]; tex_coords[texIndex++] = vec2(1.0f, 0.0f);
    points[Index++] = p[3][3]; tex_coords[texIndex++] = vec2(0.0f, 1.0f);
    points[Index++] = p[0][3]; tex_coords[texIndex++] = vec2(0.0f, 1.0f);
}

//----------------------------------------------------------------------------

inline void transpose( point4 a[4][4] )
{
    for ( int i = 0; i < 4; i++ ) {
        for ( int j = i; j < 4; j++ ) {
	    point4 t = a[i][j];
	    a[i][j] = a[j][i];
	    a[j][i] = t;
	}
    }
}

void divide_patch( point4 p[4][4], int count )
{
    if ( count > 0 ) {
	point4 q[4][4], r[4][4], s[4][4], t[4][4];
	point4 a[4][4], b[4][4];

	// subdivide curves in u direction, transpose results, divide
	// in u direction again (equivalent to subdivision in v)
        for ( int k = 0; k < 4; ++k ) {
            divide_curve( p[k], a[k], b[k] );
	}

        transpose(a);
        transpose(b);

        for ( int k = 0; k < 4; ++k ) {
            divide_curve( a[k], q[k], r[k] );
            divide_curve( b[k], s[k], t[k] );
        }

	// recursive division of 4 resulting patches
        divide_patch( q, count - 1 );
        divide_patch( r, count - 1 );
        divide_patch( s, count - 1 );
        divide_patch( t, count - 1 );
    }
    else {
        draw_patch(p);
    }
}


void setPatchColor()
{
    for (int i = 0; i < NumVertices; ++i)
    {
        color[i] = color4(0.0, 0.0, 0.0, 1.0);
    }


    for (int i = NumVertices; i < NumVertices + NumControlPoints; i++)
    {
        if (i - NumVertices == currentControlPoint)
            color[i] = color4(1.0, 0.0, 0.0, 1.0);
        else
            color[i] = color4(0.0, 0.0, 1.0, 1.0);
    }
    
}

//----------------------------------------------------------------------------

void generatePatch()
{
    for ( int n = 0; n < NumPatchPatches; n++ ) {
    
        point4  patch[4][4];

        // Initialize each patch's control point data
        for ( int i = 0; i < 4; ++i ) {
            for ( int j = 0; j < 4; ++j ) {
            vec3& v = vertices[indices[n][j][i]];
            patch[i][j] = point4(v[X], v[Y], v[Z], 1.0);
            points[NumVertices + controlIndex++] = point4(v[X], v[Y], v[Z], 1.0);
            }
        }

        // Subdivide the patch
            divide_patch(patch, NumTimesToSubdivide);
    }

    setPatchColor();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(color), color);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(color), sizeof(tex_coords), tex_coords);
    Index = 0; controlIndex = 0; texIndex = 0;
}

void init( void )
{
    generatePatch();

    points[NumVertices + NumControlPoints] = vec4(-0.6, -0.6, 0.0, 1.0);
    points[NumVertices + NumControlPoints + 1] = vec4(-0.3, -0.6, 0.0, 1.0);
    points[NumVertices + NumControlPoints + 2] = vec4(-0.6, -0.6, 0.0, 1.0);
    points[NumVertices + NumControlPoints + 3] = vec4(-0.6, -0.3, 0.0, 1.0);
    points[NumVertices + NumControlPoints + 4] = vec4(-0.6, -0.6, 0.0, 1.0);
    points[NumVertices + NumControlPoints + 5] = vec4(-0.6, -0.6, 0.3, 1.0);

    color[NumVertices + NumControlPoints]     = vec4(1, 0, 0.0, 1.0);
    color[NumVertices + NumControlPoints + 1] = vec4(1, 0, 0.0, 1.0);
    color[NumVertices + NumControlPoints + 2] = vec4(0, 1, 0.0, 1.0);
    color[NumVertices + NumControlPoints + 3] = vec4(0, 1, 0.0, 1.0);
    color[NumVertices + NumControlPoints + 4] = vec4(0, 0, 1.0, 1.0);
    color[NumVertices + NumControlPoints + 5] = vec4(0, 0, 1.0, 1.0);

    for ( int i = 0; i < 64; i++ ) {
        for ( int j = 0; j < 64; j++ ) {
            GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8)  == 0)) * 255;
            image[i][j][0]  = c;
            image[i][j][1]  = c;
            image[i][j][2]  = c;
        }
    }

    glGenTextures(1, textures);

    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
          GL_RGB, GL_UNSIGNED_BYTE, image );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textures[0] );

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(color) + sizeof(tex_coords), NULL, GL_DYNAMIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(color), color);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points)+ sizeof(color), sizeof(tex_coords), tex_coords);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader101.glsl", "fshader101.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(points)) );

    Projection = glGetUniformLocation( program, "Projection" );
    Model_View = glGetUniformLocation( program, "Model_View" );

    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(points) + sizeof(color)) );

    glUniform1i( glGetUniformLocation(program, "texture"), 0 );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glEnable( GL_DEPTH_TEST );

    glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glPointSize(5.0);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
            // view cylinder
    point4  eye( radius*sin(theta),
         phi,
         radius*cos(theta),
         1.0 );
    point4  at( 3.0, 3.0, 1.5, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );
    glUniformMatrix4fv(Model_View, 1, GL_TRUE, mv);

    mat4 p;
    if (persp)
        p = Frustum( left, right, bottom, top, zNear, zFar );
    else
        p = Ortho( left, right, bottom, top, zNear, zFar );

    glUniformMatrix4fv(Projection, 1, GL_TRUE, p);

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    glDrawArrays(GL_POINTS, NumVertices, NumControlPoints);
    glDrawArrays(GL_LINES, NumVertices + NumControlPoints, 6);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat  aspect = GLfloat(width)/height;

    if ( aspect > 0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }


}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033: // esc
    case 0x0D: // enter
        exit( EXIT_SUCCESS );
        break;
    case 'c': currentControlPoint = ++currentControlPoint % NumControlPoints; generatePatch(); break;
    case 'C': currentControlPoint = (--currentControlPoint < 0 ? 15 : currentControlPoint) % NumControlPoints; generatePatch(); break;
    case 'x': left *= 1.1; right *= 1.1; break;
    case 'X': left *= 0.9; right *= 0.9; break;
    case 'y': bottom *= 1.1; top *= 1.1; break;
    case 'Y': bottom *= 0.9; top *= 0.9; break;
    case 'z': zNear  *= 1.1; zFar *= 1.1; break;
    case 'Z': zNear *= 0.9; zFar *= 0.9; break;
    case 'r': radius *= 2.0; break;
    case 'R': radius *= 0.5; break;
    case 'o': theta += dr; break;
    case 'O': theta -= dr; break;
    case 'p': phi += dr; break;
    case 'P': phi -= dr; break;
    case 'v': persp = !persp; break;
    case 'q': vertices[lookupControlPoint(currentControlPoint)][X] += 0.1; generatePatch(); break;
    case 'Q': vertices[lookupControlPoint(currentControlPoint)][X] -= 0.1; generatePatch(); break; 
    case 'w': vertices[lookupControlPoint(currentControlPoint)][Y] += 0.1; generatePatch(); break;
    case 'W': vertices[lookupControlPoint(currentControlPoint)][Y] -= 0.1; generatePatch(); break; 
    case 'e': vertices[lookupControlPoint(currentControlPoint)][Z] += 0.1; generatePatch(); break;
    case 'E': vertices[lookupControlPoint(currentControlPoint)][Z] -= 0.1; generatePatch(); break; 
    }

    glutPostRedisplay();
}

void printHelp()
{
    std::cout << "CS637 HW9" << std::endl;
    std::cout << "GLSL version in use: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "    About: Bézier patch viewer." << std::endl;
    std::cout << "    Usage: " << std::endl;
    std::cout << "\t -f [file name]. If no file is specified, the file patchPoints.txt is loaded." << std::endl << std::endl;
    std::cout << "\t r / R \t Increase / Decrease camera cylinder radius." << std::endl;
    std::cout << "\t o / O \t Increase / Decrease camera height." << std::endl;
    std::cout << "\t p / P \t Increase / Decrease camera azimuth." << std::endl << std::endl; 
    std::cout << "\t q / Q \t Increase / Decrease control point X position." << std::endl;
    std::cout << "\t w / W \t Increase / Decrease control point Y position." << std::endl;
    std::cout << "\t e / E \t Increase / Decrease control point Z position." << std::endl << std::endl;    
    std::cout << "\t x / X; y / Y; z / Z \t Increase / Decrease projection's (left+right), (top+bottom), (near+far), resp." << std::endl;
    std::cout << "\t <space> \t Reset all values." << std::endl;
    std::cout << "Press 'c' or 'C' to alternate between control points." << std::endl;
    std::cout << "Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    " << std::endl;
}

//----------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
     SMF model; std::string filename = "patchPoints.txt"; 

    if (argc > 1)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            filename = argv[2];
        }
    }

    model.loadFile(filename);
    vertices = model.getVertices();

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( "patch" );

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glewExperimental=GL_TRUE; 
    glewInit(); 

    printHelp();    

    glutMainLoop();
    return 0;
}
