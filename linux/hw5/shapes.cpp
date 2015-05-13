// CS637 HW4 - Shapes.cpp

#include "SMF.h"
#include <string>
#include <iomanip>
#include <iostream>
#include <cstring>


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

vec4 product(vec4 a, vec4 b)
{
   return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}

struct Vertex {
    GLfloat position[4];
    GLfloat color[4];
    GLfloat normal[4];
    GLfloat shininess;
    GLubyte specular[4];
};

struct Mesh {
    GLuint vertex_buffer;
    GLuint element_buffer;
    GLsizei element_count;
};

static struct {
    struct Mesh object;
    std::vector<Vertex> object_vertex_array;

    struct {
        GLuint vertex_shader, fragment_shader, program;

        struct {
            GLint p_matrix, mv_matrix;
        } uniforms;

        struct {
            GLint position, color, normal, shininess, specular;
        } attributes;
    } graphics_program;

    GLfloat eye[3];
    GLsizei window_size[2];
} Properties;

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint  model_view;  // model-view matrix uniform shader variable location

// Projection transformation parameters

GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.4, zFar = 3.0;

bool persp = true;

GLuint  projection; // projection matrix uniform shader variable location

 // Lighting
vec4 viewer = vec4(0.0, 0.0, 1.0, 0.0);
point4 light_position = point4(0.0, -1.0, -1.0, 1.0);
point4 light2_position = point4(0.0, -1.0, 1.0, 1.0);
color4 light_diffuse = color4(1.0, 1.0, 1.0, 1.0);

color4 material_diffuse = color4(0.3, 0.2, 0.5, 1.0);

void loadObjectModel(SMF smfFile)
{
    std::vector<Vertex> vertex_data;
    std::vector<GLushort> element_data; 

    std::vector<vec3>::iterator it;
    std::vector<vec3> vertex_array = smfFile.getVertices();
    for (it = vertex_array.begin();
            it < vertex_array.end();
            it++)
    {
        struct Vertex v;

        for (int i = 0; i < 3; i++)
        {
            v.position[i] = (*it)[i];
        }

        v.position[3] = 1.0f;

        vertex_data.push_back(v);
    }

    Properties.object_vertex_array = vertex_data;

    std::vector<vec3>::iterator is;
    std::vector<vec3> faces = smfFile.getFaces();
    for (is = faces.begin();
            is < faces.end();
            is++)
    {
        element_data.push_back((*is)[0] - 1);
        element_data.push_back((*is)[1] - 1);
        element_data.push_back((*is)[2] - 1);
    }

    // Normals & Color
    for (int i = 0; i < element_data.size(); i += 3)
    {
        // Normal
        Vertex a = vertex_data[element_data[i]];
        Vertex b = vertex_data[element_data[i + 1]];
        Vertex c = vertex_data[element_data[i + 2]];

        GLfloat glVU[3] = {b.position[0] - a.position[0], b.position[1] - a.position[1], b.position[2] - a.position[2]};
        GLfloat glVV[3] = {c.position[0] - b.position[0], c.position[1] - b.position[1], c.position[2] - b.position[2]}; 
        vec3 vu = vec3(glVU[0], glVU[1], glVU[2]);
        vec3 vv = vec3(glVV[0], glVV[1], glVV[2]);
        vec3 normal = cross(vu, vv); 
        normal = normalize(normal); 
        
        vec4 n = vec4 (normal[0], normal[1], normal[2], 0.0f);
        n = n * (-1);

        // Color
        color4 diffuse_color;
        float dd = dot(light_position, n); 
        float dd2 = dot(light2_position, n);

        if(dd > 0.0) 
            diffuse_color = ((0.5) * (dd * product(light_diffuse, material_diffuse))) + 
                ((0.5) * (dd2 * product(light_diffuse, material_diffuse)));
        else 
            diffuse_color =  color4(0.0, 0.0, 0.0, 1.0);

        diffuse_color *= 0.72;

         /* ambient_color[i] + */ 

        Properties.object_vertex_array[element_data[i]].color[0] = diffuse_color[0];
        Properties.object_vertex_array[element_data[i]].color[1] = diffuse_color[1];
        Properties.object_vertex_array[element_data[i]].color[2] = diffuse_color[2];
        Properties.object_vertex_array[element_data[i]].color[3] = diffuse_color[3];

        Properties.object_vertex_array[element_data[i+1]].color[0] = diffuse_color[0];
        Properties.object_vertex_array[element_data[i+1]].color[1] = diffuse_color[1];
        Properties.object_vertex_array[element_data[i+1]].color[2] = diffuse_color[2];
        Properties.object_vertex_array[element_data[i+1]].color[3] = diffuse_color[3];

        Properties.object_vertex_array[element_data[i+2]].color[0] = diffuse_color[0];
        Properties.object_vertex_array[element_data[i+2]].color[1] = diffuse_color[1];
        Properties.object_vertex_array[element_data[i+2]].color[2] = diffuse_color[2];
        Properties.object_vertex_array[element_data[i+2]].color[3] = diffuse_color[3];    
    }


    glGenBuffers(1, &Properties.object.vertex_buffer);
    glGenBuffers(1, &Properties.object.element_buffer);

    Properties.object.element_count = element_data.size(); 

    glBindBuffer(GL_ARRAY_BUFFER, Properties.object.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(Vertex), &(Properties.object_vertex_array[0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Properties.object.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_data.size() * sizeof(GLushort), &(element_data[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(
        Properties.graphics_program.attributes.position,
        4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, position)) 
        );

    glVertexAttribPointer(
        Properties.graphics_program.attributes.color,
        4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, color))
        );
}

void idle()
{
	glutPostRedisplay();
}

void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    // view cylinder
    point4  eye( radius*sin(theta),
         phi,
         radius*cos(theta),
         1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

    mat4 p;
    if (persp)
        p = Frustum( left, right, bottom, top, zNear, zFar );
    else
        p = Ortho( left, right, bottom, top, zNear, zFar );

    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

    glDrawElements(GL_TRIANGLES, Properties.object.element_count, GL_UNSIGNED_SHORT, (void*)0);
   
    glutSwapBuffers();
}


void init( void )
{
    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    

    // Load shaders and use the resulting shader program
    Properties.graphics_program.program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram(Properties.graphics_program.program);

    // Initialize the vertex position attribute from the vertex shader
    Properties.graphics_program.attributes.position = 
        glGetAttribLocation( Properties.graphics_program.program, "vPosition" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.position);

    // Initialize color
    Properties.graphics_program.attributes.color = 
        glGetAttribLocation( Properties.graphics_program.program, "vColor" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.color);

    Properties.graphics_program.attributes.normal = 
        glGetAttribLocation( Properties.graphics_program.program, "vNormal" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.normal);

    model_view = glGetUniformLocation( Properties.graphics_program.program, "model_view" );
    projection = glGetUniformLocation( Properties.graphics_program.program, "projection" );

    //transformationMatrix = glGetUniformLocation(Properties.graphics_program.program, "ctm");

    glEnable(GL_DEPTH_TEST);

    glClearColor( 135.0/256.0, 206.0/256.0, 235.0/256.0, 1.0 ); // (135, 206, 235)
}

//----------------------------------------------------------------------------


void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033: // esc
    case 0x0D: // enter
        exit( EXIT_SUCCESS );
        break;

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

    case ' ':  // reset values to their defaults
        left = -1.0;
        right = 1.0;
        bottom = -1.0;
        top = 1.0;
        zNear = 0.4;
        zFar = 3.0;

        radius = 1.0;
        theta  = 0.0;
        phi    = 0.0;
        break;
    }
}




void printHelp()
{
    std::cout << "CS637 HW5" << std::endl;
    std::cout << "GLSL version in use: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "    About: displays a 3d cube." << std::endl;
    std::cout << "    Usage: " << std::endl;
    std::cout << "\t -f [file name]. If no file is specified, the bunny model is loaded (bound-bunny_5k.smf)." << std::endl;
    std::cout << "\t r / R \t Increase / Decrease camera cylinder radius." << std::endl;
    std::cout << "\t o / O \t Increase / Decrease camera height." << std::endl;
    std::cout << "\t p / P \t Increase / Decrease camera azimuth." << std::endl;
    std::cout << "\t x / X; y / Y; z / Z \t Increase / Decrease projection's (left+right), (top+bottom), (near+far), resp." << std::endl;
    std::cout << "\t <space> \t Reset all values." << std::endl;
    std::cout << "Press 'v' to alternate between Parallel and Perspective projection." << std::endl;
    std::cout << "Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    " << std::endl;
}

int main( int argc, char **argv )
{
    SMF model; std::string filename; 
    std::cout << argc << std::endl;
    std::cout << argv[1] << " " << argv[2] << " "  << std::endl;
    if (argc > 1 && strcmp(argv[1], "-f") == 0)
    {
        filename = argv[2];
        model.loadFile(filename);
    }
    else
        model.loadFile("bound-bunny_5k.smf");

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( 500, 500 );

    int win = glutCreateWindow("ICG: Assignment 4");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glewExperimental=GL_TRUE; 
    glewInit();     
    init();

   glutIdleFunc(idle);

   loadObjectModel(model);
    printHelp();

    glutMainLoop();

    std::cout << std::endl;
    
    return 0;
}
