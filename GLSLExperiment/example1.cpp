//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints];

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_loc;
GLuint view_loc;
GLuint projection_loc;
/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(1.0, 0.0, 2.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);

float material_shininess = 200.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );
    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));


	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
mat4 instance;
vec4 colorCode;
GLfloat quayBasexyz[3] = { 0,0,0 };
mat4 quayBase;
GLfloat xeye =0, yeye = 0, zeye = 1, yat = 0;

void Block(mat4 mt ,vec4 colorCode)
{
	material_diffuse = colorCode;  // Mã màu
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	point4 eye(xeye, yeye, zeye, 1);
	point4 at(xeye , yeye - yat, zeye -1 , 1);
	vec4 up(0, 1, 0, 1);
	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);
	mat4 projection = Frustum(-10, 10, -10, 10,4, 10 );
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, mt);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


GLfloat wKN = 7, dKN = 12, hKN = 3.5 ; // chiều d r c của mô hình nhà
GLfloat doDatT = 0.02 ; // độ dày của tường tuongwf trái 3.9
GLfloat hMN = 0.5;  // độ dày của nền nhà


void KhungNha() {
	// mặt nền 
	instance = Translate(wKN * 0.5,  -hMN * 0.5, -dKN*0.5) * Scale(wKN, hMN, dKN);
	colorCode = vec4(28.0 / 255, 28.0 / 255, 28.0 / 255, 1.0);  //mã màu nền nhà
	Block(instance, colorCode);
	// đường viền tường trước bên trái
	instance = Translate(3.9 * 0.5, 1 * 0.5, -doDatT * 0.5) * Scale(3.9, 1, doDatT);
	colorCode = vec4(112.0 / 255, 128.0 / 255, 144.0 / 255, 1.0);  //mã màu nền nhà
	Block(instance, colorCode);
	// đường viền tường trước bên trái
}




void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );    
	const vec3 viewer_pos(0.0, 0.0, 0.0);  /*Trùng với eye của camera*/
	quayBase = RotateX(quayBasexyz[0]) * RotateY(quayBasexyz[1]) * RotateZ(quayBasexyz[2]);

	KhungNha();

	glutSwapBuffers();									   
}

void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

	switch (key) {
		case 'w':
			xeye -= 0.2;
			break;
		case 's':
			xeye += 0.2;
			break;
		case 'a':
			zeye -= 0.2;
			break;
		case 'd':
			zeye += 0.2;
			break;
		case 'q':
			yeye += 0.2;
			break;
		case 'e':
			yeye -= 0.2;
			break;
	}
	glutPostRedisplay();
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry();                          
    initGPUBuffers();							   
    shaderSetup();                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );       

	glutMainLoop();
    return 0;
}
