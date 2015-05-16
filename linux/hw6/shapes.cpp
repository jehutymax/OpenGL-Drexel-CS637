// CS637 HW6 - Shapes.cpp

#include "SMF.h"
#include <string>
#include <iomanip>
#include <iostream>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct Vertex {
    GLfloat position[4];
    GLfloat color[4];
    GLfloat normal[3];
    GLfloat shininess;
    GLubyte specular[4];
};

struct Mesh {
    GLuint vertex_buffer;
    GLuint element_buffer;
    GLsizei element_count;
};

struct Material {
    color4 material_ambient;
    color4 material_diffuse;
    color4 material_specular; 
    float material_shininess; 
};

static struct {
    struct Mesh object;
    std::vector<Vertex> object_vertex_array;

    struct {
        GLuint vertex_shader, fragment_shader, program, model_view, projection;

        struct {
            mat4 projection_matrix, modelview_matrix;
        } uniforms;

        struct {
            GLint normal, position, DiffuseProduct, Shininess, SpecularProduct;
        } attributes;
    } graphics_program;

    vec3 eye;
    vec2 window_size[2];
} Properties;


GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

GLfloat light_radius = 1.0;
GLfloat light_theta = 0.0;
GLfloat light_phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

// Projection transformation parameters

GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.3, zFar = 3.0;

bool persp = false;

 // Lighting
vec4 viewer = vec4(0.0, 0.0, 1.0, 0.0);
color4 ambient_product;
color4 diffuse_product;
color4 specular_product;

  // Initialize shader lighting parameters
point4 light_position( 0.0, 0.0, 2.0, 1.0 );
point4 light_position2( 0.0, 0.0, -2.0, 1.0 );
color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

Material bronze;
Material silver;
Material jade;

Material currentMaterial;

void initializeMaterials()
{
    bronze.material_ambient = vec4(0.2125, 0.1275, 0.054, 1.0);
    bronze.material_diffuse = vec4(0.714, 0.4284, 0.18144, 1.0);
    bronze.material_specular = vec4(0.393548, 0.271906, 0.166721, 1.0); 
    bronze.material_shininess = 25.6; 

    silver.material_ambient = color4(0.23125f, 0.23125f, 0.23125f, 1.0f);
    silver.material_diffuse = color4(0.2775f, 0.2775f, 0.2775f, 1.0f);
    silver.material_specular = color4(0.773911f, 0.773911f, 0.773911f, 1.0f); 
    silver.material_shininess = 89.6f; 

    jade.material_ambient = color4(0.135f, 0.2225f, 0.1575f, 0.95f);
    jade.material_diffuse = color4(0.54f, 0.89f, 0.63f, 0.95f);
    jade.material_specular = color4(0.316228f, 0.316228f, 0.316228f, 0.95f); 
    jade.material_shininess = 12.8f; 
}

static char*
myReadShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);

    return buf;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint myInitShader(const char* vShaderFile, const char* fShaderFile, 
        GLuint& vShader, GLuint& fShader)
{
    struct Shader {
    const char*  filename;
    GLenum       type;
    GLchar*      source;
    }  shaders[2] = {
    { vShaderFile, GL_VERTEX_SHADER, NULL },
    { fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
        Shader& s = shaders[i];
        s.source = myReadShaderSource( s.filename );
        if ( shaders[i].source == NULL ) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit( EXIT_FAILURE );
        }

        GLuint shader = glCreateShader( s.type );

        if (s.type == GL_VERTEX_SHADER)
            vShader = shader;
        else fShader = shader;

        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );

        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;

            exit( EXIT_FAILURE );
        }

        delete [] s.source;

        glAttachShader( program, shader );
    }

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
    std::cerr << "Shader program failed to link" << std::endl;
    GLint  logSize;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
    char* logMsg = new char[logSize];
    glGetProgramInfoLog( program, logSize, NULL, logMsg );
    std::cerr << logMsg << std::endl;
    delete [] logMsg;

    exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    return program;
}

void delete_program()
{
    glDetachShader(
        Properties.graphics_program.program,
        Properties.graphics_program.vertex_shader
    );
    glDetachShader(
        Properties.graphics_program.program,
        Properties.graphics_program.fragment_shader
    );
    glDeleteProgram(Properties.graphics_program.program);
    glDeleteShader(Properties.graphics_program.vertex_shader);
    glDeleteShader(Properties.graphics_program.fragment_shader);
}

void recreate_program(bool useGouraud)
{

    std::string gouraud_v = "vGouraudShader.glsl";
    std::string gouraud_f = "fGouraudShader.glsl";
    std::string phong_v = "vPhongShader.glsl";
    std::string phong_f = "fPhongShader.glsl";
    std::string vshader; std::string fshader;

    if (useGouraud)
    {
        vshader = gouraud_v;
        fshader = gouraud_f;
    }
    else
    {
        vshader = phong_v;
        fshader = phong_f;
    }

    Properties.graphics_program.program = myInitShader( vshader.c_str(), fshader.c_str(),
    Properties.graphics_program.vertex_shader, Properties.graphics_program.fragment_shader);
    glUseProgram(Properties.graphics_program.program);

    Properties.graphics_program.projection
        = glGetUniformLocation(Properties.graphics_program.program, "projection_matrix");
    Properties.graphics_program.model_view
        = glGetUniformLocation(Properties.graphics_program.program, "modelview_matrix");

    // Initialize the vertex position attribute from the vertex shader
    Properties.graphics_program.attributes.position = 
        glGetAttribLocation( Properties.graphics_program.program, "vPosition" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.position);

    Properties.graphics_program.attributes.normal = 
        glGetAttribLocation( Properties.graphics_program.program, "vNormal" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.normal);
}

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

        v.shininess = 1.0;
        v.specular[0] = 1.0; v.specular[1] = 0.8;
        v.specular[2] = 0.0;
        v.specular[3] = 1.0;
        vertex_data.push_back(v);
    }

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

        for (int z = 0; z < 3; z++)
        {
            vertex_data[element_data[i]].normal[z] = normal[z]; 
            vertex_data[element_data[i + 1]].normal[z] = normal[z];
            vertex_data[element_data[i + 2]].normal[z] = normal[z];  
        }

    }

    Properties.object_vertex_array = vertex_data;

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

 /*   glVertexAttribPointer(
        Properties.graphics_program.attributes.color,
        4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, color))
        );
*/
    glVertexAttribPointer(
        Properties.graphics_program.attributes.normal,
        3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, normal))
        );

    initializeMaterials();
    currentMaterial = bronze;

}



void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    ambient_product = light_ambient * currentMaterial.material_ambient;
    diffuse_product = light_diffuse * currentMaterial.material_diffuse;
    specular_product = light_specular * currentMaterial.material_specular;

    light_position2 = vec4(light_radius*sin(light_theta),
         light_phi,
         light_radius*cos(light_theta),
         1.0);

     glUniform4fv( glGetUniformLocation(Properties.graphics_program.program, "AmbientProduct"),
          1, ambient_product );
    glUniform4fv( glGetUniformLocation(Properties.graphics_program.program, "DiffuseProduct"),
          1, diffuse_product );
    glUniform4fv( glGetUniformLocation(Properties.graphics_program.program, "SpecularProduct"),
          1, specular_product );
    
    glUniform4fv( glGetUniformLocation(Properties.graphics_program.program, "LightPosition"),
          1, light_position );

    glUniform4fv( glGetUniformLocation(Properties.graphics_program.program, "LightPosition2"),
          1, light_position2 );

    glUniform1f( glGetUniformLocation(Properties.graphics_program.program, "Shininess"),
         currentMaterial.material_shininess );

    // view cylinder
    point4  eye( radius*sin(theta),
         phi,
         radius*cos(theta),
         1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );
    glUniformMatrix4fv(Properties.graphics_program.model_view, 1, GL_TRUE, mv );

    mat4 p;
    if (persp)
        p = Frustum( left, right, bottom, top, zNear, zFar );
    else
        p = Ortho( left, right, bottom, top, zNear, zFar );

    glUniformMatrix4fv(Properties.graphics_program.projection, 1, GL_TRUE, p);

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
    Properties.graphics_program.program = myInitShader( "vPhongShader.glsl", "fPhongShader.glsl",
    Properties.graphics_program.vertex_shader, Properties.graphics_program.fragment_shader);
    glUseProgram(Properties.graphics_program.program);

    Properties.graphics_program.projection
        = glGetUniformLocation(Properties.graphics_program.program, "projection_matrix");
    Properties.graphics_program.model_view
        = glGetUniformLocation(Properties.graphics_program.program, "modelview_matrix");

    // Initialize the vertex position attribute from the vertex shader
    Properties.graphics_program.attributes.position = 
        glGetAttribLocation( Properties.graphics_program.program, "vPosition" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.position);

/*
    // Initialize color
    Properties.graphics_program.attributes.color = 
        glGetAttribLocation( Properties.graphics_program.program, "vColor" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.color);
    */

    Properties.graphics_program.attributes.normal = 
        glGetAttribLocation( Properties.graphics_program.program, "vNormal" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.normal);

/*
     Properties.graphics_program.attributes.shininess = 
        glGetAttribLocation( Properties.graphics_program.program, "shininess" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.shininess);

     Properties.graphics_program.attributes.specular = 
        glGetAttribLocation( Properties.graphics_program.program, "specular" );
    glEnableVertexAttribArray(Properties.graphics_program.attributes.specular);

    */

    //transformationMatrix = glGetUniformLocation(Properties.graphics_program.program, "ctm");

    glEnable(GL_DEPTH_TEST);

    glClearColor( 135.0/256.0, 206.0/256.0, 235.0/256.0, 1.0 ); // (135, 206, 235)

}



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
    case 'q': light_radius *= 2.0; break;
    case 'Q': light_radius *= 0.5; break;
    case 'w': light_theta += dr; break;
    case 'W': light_theta -= dr; break;
    case 'e': light_phi += dr; break;
    case 'E': light_phi -= dr; break;
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

void idle()
{
    glutPostRedisplay();
}

void printHelp()
{
    std::cout << "CS637 HW6" << std::endl;
    std::cout << "GLSL version in use: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "    About: SMF model viewer." << std::endl;
    std::cout << "    Usage: " << std::endl;
    std::cout << "\t -f [file name]. If no file is specified, the bunny model is loaded (bound-bunny_5k.smf)." << std::endl << std::endl;
    std::cout << "\t r / R \t Increase / Decrease camera cylinder radius." << std::endl;
    std::cout << "\t o / O \t Increase / Decrease camera height." << std::endl;
    std::cout << "\t p / P \t Increase / Decrease camera azimuth." << std::endl << std::endl; 
    std::cout << "\t q / Q \t Increase / Decrease light2 cylinder radius." << std::endl;
    std::cout << "\t w / W \t Increase / Decrease light2 height." << std::endl;
    std::cout << "\t e / E \t Increase / Decrease light2 azimuth." << std::endl << std::endl;    
    std::cout << "\t x / X; y / Y; z / Z \t Increase / Decrease projection's (left+right), (top+bottom), (near+far), resp." << std::endl;
    std::cout << "\t <space> \t Reset all values." << std::endl;
    std::cout << "Press 'v' to alternate between Parallel and Perspective projection." << std::endl;
    std::cout << "Press 'Esc' or 'Enter' to exit. " << std::endl;
    std::cout << "    " << std::endl;
}

//----------------------------------------------------------------------------

void graphicsInitialization(int argc, char **argv)
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( 500, 500 );

    int win = glutCreateWindow("ICG: Assignment 6");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glewExperimental=GL_TRUE; 
    glewInit(); 
    init();

    glutIdleFunc(idle);
}

void window_menu(int id)
{}
void window_menu2(int id)
{}

void submenu(int id)
{
    switch(id)
    {
        case 1:
            delete_program();
            recreate_program(/*useGouraud*/ true);
            break;
        case 2:
            delete_program();
            recreate_program(/*useGouraud*/ false);
            break;
    }

    glutPostRedisplay();
}

void submenu2(int id)
{
    switch(id)
    {
        case 1: //bronze
            currentMaterial = bronze;
            break;
        case 2: //silver
            currentMaterial = silver;
            break;
        case 3: //jade
            currentMaterial = jade;
            break;
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{    
    SMF model; std::string filename = "bound-bunny_5k.smf"; 

    if (argc > 1)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            filename = argv[2];
        }
    }

    model.loadFile(filename);

    graphicsInitialization(argc, argv);

    int shading_menu = glutCreateMenu(submenu);
    glutAddMenuEntry("Gouraud", 1);
    glutAddMenuEntry("Phong", 2);

    int material_menu = glutCreateMenu(submenu2);
    glutAddMenuEntry("Bronze", 1);
    glutAddMenuEntry("Silver", 2);
    glutAddMenuEntry("Jade", 3);

    glutCreateMenu(window_menu);

    glutAddSubMenu("Shading", shading_menu);
    glutAddSubMenu("Material", material_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    loadObjectModel(model);

    printHelp();

    glutMainLoop();

    std::cout << std::endl;

    return 0;
}
