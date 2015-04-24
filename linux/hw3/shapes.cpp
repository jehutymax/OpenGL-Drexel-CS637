include "Angel.h"
#include <vector>

vec3 colors[747];
vec2 vertices[747];
float circleRadius = 0.5;
bool circleUp = true;
bool animate = false;
int window = 0;

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
void init( void )
{
 // triangle
    colors[0] = vec3(0, 1, 0);
    colors[1] = vec3(1, 0, 0);
    colors[2] = vec3(0, 0, 1);
    
    vertices[0] = vec2( -0.9, -0.8 );
    vertices[1] = vec2( -0.5, 0.8 );
    vertices[2] = vec2( -0.1, -0.8 );

    // Circle
    float radius = 0.5;
    float angleInc = 1.0;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    float currentAngle = 0.0;
    for (int i = 3; i < 363; i++)
    {
        vec2 vertice = vec2( (150.0/400.0) * radius * cos(currentAngle) + 0.4,
             radius * sin(currentAngle));
        
        vertices[i] = vertice;

        vec3 color = vec3(currentAngle/6 * 1.0, 0, 0);
        colors[i] = color;

        currentAngle += angleInc;
    }

   // Ellipse
    radius = 0.92;
    angleInc = 360.0 / 360.0;
    angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
    currentAngle = 0.0;
    for (int i = 363; i < 723; i++)
    {
        vec2 vertice = vec2(radius * cos(currentAngle),
             0.6*(radius * sin(currentAngle)));
        vertices[i] = vertice;

        vec3 color = vec3(currentAngle/6 * 1.0, currentAngle/6 * 0.5, 0);
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

void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window

    for (int i = 0; i < 6; ++i)
    {
        glDrawArrays( GL_TRIANGLE_STRIP, 723 + (4 * i), 4); // SquareN
    }
    
    glutSwapBuffers();
}

void displayWindow2(void)
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLES, 0, 3 );    // draw the 1st triangle
    glDrawArrays( GL_TRIANGLE_FAN, 3, 360); // Circle!

    glutSwapBuffers();
}

void displaySubWindow( void )
{   
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    
    glDrawArrays( GL_TRIANGLE_FAN, 363, 360); // Ellipse!

    glutSwapBuffers();
}

void setSquareFillColor(float r, float g, float b)
{
    for (int i = 0; i < 6; ++i)
    {
        float c = !(i % 2);
        vec2 squareCenter = vec2(0.0, -0.3);
        vec3 squareColor = vec3(c * r, c * g, c * b);

        createSquare(vertices, colors, squareCenter, squareColor, 
            1.2 - (0.2 * i), 723 + (4 * i));
    }

    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
}

void window_menu(int id)
{
    switch(id)
    {
        case 1:
        animate = true;
        break;
        case 2:
        animate = false;
        break;
        case 3:
        glClearColor( 1.0, 0.5, 0, 1.0 );
        break;
        case 4:
        glClearColor(0.2, 0.1, 1.0, 1.0 );
        break;
        case 5:
        glClearColor( 0, 1.0, 0, 1.0 );
        break;
    }

    glutPostRedisplay();
}

void submenu(int id)
{
    switch(id)
    {
        case 1:
            setSquareFillColor( 1, 1, 1 ); // white
            break;
        case 2:
            setSquareFillColor( 1.0, 0, 0 ); // red
            break;
        case 3:
            setSquareFillColor( 0, 1, 0); // green
            break;
    }

    glutPostRedisplay();
}

void subwindow_menu(int id)
{
    switch(id)
    {
        case 1:
        glClearColor( 0, 0, 0, 1.0 );
        break;
        case 2:
        glClearColor( 1.0, 1.0, 1.0, 1.0 );
        break;
        case 3:
        glClearColor( 1.0, 0.5, 0, 1.0 );
        break;
        case 4:
        glClearColor(0.2, 0.1, 1.0, 1.0 );
        break;
        case 5:
        glClearColor( 0, 1.0, 0, 1.0 );
        break;
    }

    glutPostRedisplay();
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

void setWindow2FillColor(float r, float g, float b)
{
    for (int i = 0; i <  363; i++)
    {
        colors[i] = vec3(r, g, b);
    }

    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

}

void keyboard_window2(unsigned char key, int x, int y)
{

    switch ( key ) {
    case 033: // esc
    case 0x0D: // enter
        exit( EXIT_SUCCESS );
        break;
    case 0167: // w
        setWindow2FillColor(1, 1, 1);
        break;
    case 0162: // r
        setWindow2FillColor(1, 0, 0);
        break;
    case 0147: // g
        setWindow2FillColor(0, 1, 0);
        break;
    case 0142: // b
        setWindow2FillColor(0, 0, 1);
        break;  
    case 0171: // y
        setWindow2FillColor(1, 1, 0);
        break;
    case 0157: // o
        setWindow2FillColor(1, 0.5, 0);
        break;
    case 0160: // p
        setWindow2FillColor(0.5, 0, 0.5);
        break;                      
    }

    glutPostRedisplay();
}

void idle()
{
    if (animate)
    {
        int current = (++window % 2) + 1;
        glutSetWindow(current);

         mat4 ctm = Translate(0, -0.3, 0) * RotateZ(2.0) * Translate(0, 0.3, 0);

        for (int i = 723; i < 747; ++i)
        {
            vec4 newPoint = vec4(vertices[i][0], vertices[i][1], 1, 1);
            newPoint = ctm * newPoint;
            vertices[i] = vec2(newPoint[0], newPoint[1]);
        }

        ctm = Translate(-0.5, -0.1, 0) * RotateZ(-3.0) * Translate(0.5, 0.1, 0);

        for (int i = 0; i < 3; ++i)
        {
            vec4 newPoint = vec4(vertices[i][0], vertices[i][1], 1, 1);
            newPoint = ctm * newPoint;
            vertices[i] = vec2(newPoint[0], newPoint[1]);
        }

        if (circleRadius >= 0.8)
            circleUp = false;

        if (circleRadius <= 0.1)
            circleUp = true;
     
        if (circleUp)
            circleRadius += 0.01;
        else
            circleRadius -= 0.01;
        
        float radius = circleRadius;
        float angleInc = 1.0;
        angleInc *= M_PI / 180.0; // M_PI is defined in Angel.h
        float currentAngle = 0.0;
        for (int i = 3; i < 363; i++)
        {
            vec2 vertice = vec2( (150.0/400.0) * radius * cos(currentAngle) + 0.4,
                 radius * sin(currentAngle));
            
            vertices[i] = vertice;

            vec3 color = vec3(currentAngle/6 * 1.0, 0, 0);
            colors[i] = color;

            currentAngle += angleInc;
        }

        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glutPostRedisplay();
    }
}

int
main( int argc, char **argv )
{
    std::cout << "CS637 HW3:" << std::endl;
    std::cout << "    About: application displays 2d objects." << std::endl;
    std::cout << "    Usage: Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    >.." << std::endl;
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( 500, 500 );

    glewExperimental=GL_TRUE; 
    glewInit();    
    

 
    int win = glutCreateWindow("ICG: Assignment 3");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(myMouse);
    init();

    int sub_menu = glutCreateMenu(submenu);
    glutAddMenuEntry("white", 1);
    glutAddMenuEntry("red", 2);
    glutAddMenuEntry("green", 3);
    glutCreateMenu(window_menu);
    glutAddMenuEntry("Start Animation", 1);
    glutAddMenuEntry("Stop Animation", 2);
    glutAddSubMenu("Square Colors", sub_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutInitWindowPosition(550, 0);
    glutInitWindowSize(400, 150);
    int win2 = glutCreateWindow("window 2");
    glutDisplayFunc(displayWindow2);
    glutKeyboardFunc(keyboard_window2);
    init();
   
    glutCreateSubWindow(win, 0, 0, 200, 200);
    glutDisplayFunc(displaySubWindow);
    init();
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glutCreateMenu(subwindow_menu);
    glutAddMenuEntry("black", 1);
    glutAddMenuEntry("white", 2);
    glutAddMenuEntry("orange", 3);
    glutAddMenuEntry("blue", 4);
    glutAddMenuEntry("green", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

   glutIdleFunc(idle);


    glutMainLoop();
    return 0;
}
