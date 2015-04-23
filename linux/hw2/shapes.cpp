// CS637 HW2 - Shapes.cpp

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


//--------------------------------------------------------------------------

void
init( void )
{
    vec3 colors[747] = {
        vec3(0, 1, 0), vec3(1, 0, 0), vec3(0, 0, 1)
    };
    // Specifiy the vertices for a triangle
    vec2 vertices[747] = {
        vec2( -0.3, 0.52 ), vec2( 0, 1 ), vec2( 0.3, 0.52 ) // triangle vertices
        // we don't initialize the vertices for the circle.
    };

    // Circle, radius = 0.24
    float radius = 0.24;
    float angleInc = 360.0 / 360.0;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    float currentAngle = 0.0;
    for (int i = 3; i < 363; i++) // index for the pentagon vertices
    {
        vec2 vertice = vec2(radius * cos(currentAngle) 
            + 0.65, // translating slightly to the right!
             radius * sin(currentAngle) + 0.75);
        vertices[i] = vertice;

        vec3 color = vec3(currentAngle/6 * 1.0, 0, 0);
        colors[i] = color;

        currentAngle += angleInc;
    }

   // Ellipse, radius = 0.24
    radius = 0.24;
    angleInc = 360.0 / 360.0;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    currentAngle = 0.0;
    for (int i = 363; i < 723; i++) // index for the pentagon vertices
    {
        vec2 vertice = vec2(radius * cos(currentAngle) 
            - 0.65, // translating slightly to the right!
             0.6*(radius * sin(currentAngle)) + 0.75);
        vertices[i] = vertice;

        vec3 color = vec3(1, 0, 0);
        colors[i] = color;
        currentAngle += angleInc;
    }

    for (int i = 0; i < 6; ++i)
    {
        float c = !(i % 2);
        vec2 squareCenter = vec2(0.0, -0.3);
        vec3 squareColor = vec3(c * 1.0, c * 1.0, c * 1.0);

        createSquare(vertices, colors, squareCenter, squareColor, 
            1.2 - (0.2 * i), 723 + (4 * i));
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

    glClearColor( 0, 0, 0, 1.0 ); // black background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLES, 0, 3 );    // draw the 1st triangle
    //glDrawArrays( GL_TRIANGLE_STRIP, 3, 4); // Quadrilateral: starts at index 3 with 4 vertices.
    glDrawArrays( GL_TRIANGLE_FAN, 3, 360); // Circle!
    glDrawArrays( GL_TRIANGLE_FAN, 363, 360); // Ellipse!

    for (int i = 0; i < 6; ++i)
    {
        glDrawArrays( GL_TRIANGLE_STRIP, 723 + (4 * i), 4); // SquareN
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
    std::cout << "CS637 HW2: Assignment 2 - Part 1" << std::endl;
    std::cout << "    About: application displays 2d objects." << std::endl;
    std::cout << "    Usage: Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    >.." << std::endl;
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 500, 500 );

    glutCreateWindow( "ICG: Assignment 2 - Part 1" );
    glewExperimental=GL_TRUE; 
    glewInit();    
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}
