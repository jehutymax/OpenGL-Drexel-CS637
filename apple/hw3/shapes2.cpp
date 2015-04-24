// CS637 HW2 - Shapes2.cpp

#include "Angel.h"

void createSquare(vec2* verticeList, vec3* colorList, vec2 center, vec3 color, float size, int index)
{
    vec2 firstPoint = vec2(center[0] - size/2.0, center[1] - size/2.0);
    verticeList[index] = firstPoint;
    colorList[index] = color;

    verticeList[++index] = vec2(firstPoint[0], firstPoint[1] + size);  colorList[index] = color;
   
    verticeList[++index] = vec2(firstPoint[0] + size, firstPoint[1]);  colorList[index] = color;
    verticeList[++index] = vec2(firstPoint[0] + size, firstPoint[1] + size);  colorList[index] = color; 
}

void createCircle(vec2* verticeList, vec3* colorList, vec2 center, vec3 color, float radius, int index)
{
    float angleInc = 1.0;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    float currentAngle = 0.0;
    for (int i = index; i < 360 + index; i++)
    {
        vec2 vertice = vec2(radius * cos(currentAngle) 
            + center[0],
             radius * sin(currentAngle) + center[1]);
        verticeList[i] = vertice;

        colorList[i] = color;

        currentAngle += angleInc;
    }
}

//--------------------------------------------------------------------------

void
init( void )
{
        vec3 colors[2180] = {
        vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0),
        vec3(1, 1, 0), vec3(1, 1, 0), vec3(1, 1, 0),
        vec3(1, 0, 1), vec3(1, 0, 1), vec3(1, 0, 1),
        vec3(0, 1, 1), vec3(0, 1, 1), vec3(0, 1, 1)
    };
    // Specifiy the vertices for a triangle
    vec2 vertices[2180] = {
        vec2( -1, 0.40 ), vec2( -0.5, 1 ), vec2( 0, 0.4 ), // triangle vertices
        vec2( -0.67, 0.70 ), vec2( -0.5, 1 ), vec2( -0.33, 0.7 ), // triangle vertices
        vec2( -1, 0.40 ), vec2( -0.67, 0.7 ), vec2( -0.5, 0.40 ), // triangle vertices
        vec2( -0.5, 0.40 ), vec2( -0.33, 0.7 ), vec2( 0, 0.4 ) // triangle vertices
    };



    float radius = 0.32;
    float angleInc = 360.0 / 8;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    float currentAngle = 0.0;
    for (int i = 12; i < 20; i++) // index for the pentagon vertices
    {
        vec2 vertice = vec2(radius * cos(currentAngle) 
            + 0.6, // translating slightly to the right!
             radius * sin(currentAngle) + 0.6);
        vertices[i] = vertice;
        colors[i] = vec3(currentAngle/6 * 1.0, currentAngle/6 * 1.0, currentAngle/6 * 1.0);
        currentAngle += angleInc;
    }

    for (int i = 0; i < 6; ++i)
    {
        float c = !(i % 2);
        vec2 circleCenter = vec2(0.0, -0.3);
        vec3 circleColor = vec3(c * 1.0, 0, 0);

        createCircle(vertices, colors, circleCenter, circleColor, 
            0.6 - (0.1 * i), 20 + (360 * i));
    }
       

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    

     // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

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
                           BUFFER_OFFSET(sizeof(vertices)) );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLES, 0, 3 );    // draw the 1st triangle
    glDrawArrays( GL_TRIANGLES, 3, 3 ); 
    glDrawArrays( GL_TRIANGLES, 6, 3 ); 
    glDrawArrays( GL_TRIANGLES, 9, 3 ); 
    glDrawArrays( GL_TRIANGLE_FAN, 12, 8); // Octogon!

    for (int i = 0; i < 6; ++i)
    {
        glDrawArrays( GL_TRIANGLE_FAN, 20 + (360 * i), 360); // Circles
    }
    
    glFlush();
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
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    std::cout << "CS637 HW2: Assignment 2 - Part 2" << std::endl;
    std::cout << "    About: application displays 2d objects." << std::endl;
    std::cout << "    Usage: Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    >.." << std::endl;
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 500, 500 );

    glutCreateWindow( "ICG: Assignment 2 - Part 2" );
    glewExperimental=GL_TRUE; 
    glewInit();    
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}
