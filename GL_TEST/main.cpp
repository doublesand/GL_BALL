
#include "Angel.h"
#include "TriMesh.h"

#pragma comment(lib, "glew32.lib")

using namespace std;

//���ֶ�������
GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexNormalID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint rotationMatrixID;
GLuint modelViewMatrixID;
GLuint projMatrixID;
GLuint lightPosID;
GLuint vNormalID;
GLuint shadowID;

int mainWindow;

// ����Ӽ������
float l = -2.0, r = 2.0;    // ���Ҳü�ƽ��
float b = -2.0, t = 2.0;    // ���²ü�ƽ��
float n = -2.0, f = 2.0;    // Զ���ü�ƽ��
float rotationAngle = -5.0; // ��ת�Ƕȣ�������ӰͶ����y=0ƽ���ϣ�������Ҫ��X����ת���ܿ���

std::vector<vec3> points;
std::vector<vec3i> faces;

vec4 black(0.0, 0.0, 0.0, 1.0);

float lightPos[3] = { -0.5, 2.0, 0.5 };   //��Դλ��

TriMesh* mesh = new TriMesh();

namespace Camera
{
	//����͸�Ӿ������Ӽ�����һ����׼�����壬�������Դ�����
	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		vec4 c1 = vec4(2 / (right - left), 0, 0, 0);
		vec4 c2 = vec4(0, 2 / (top - bottom), 0, 0);
		vec4 c3 = vec4(0, 0, -2 / (zFar - zNear), 0);
		vec4 c4 = vec4((right + left) / (left - right), (top + bottom) / (bottom - top), (zFar + zNear) / (zNear - zFar), 1);
		return mat4(c1, c2, c3, c4);
	}

	//͸��ͶӰ������ԶС���������Դ�����
	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		GLfloat top = zNear * tan(fovy);
		GLfloat right = top * aspect;
		vec4 c1 = vec4(zNear / right, 0, 0, 0);
		vec4 c2 = vec4(0, zNear / top, 0, 0);
		vec4 c3 = vec4(0, 0, (zFar + zNear) / (zNear - zFar), -1);
		vec4 c4 = vec4(0, 0, (2 * zFar * zNear) / (zNear - zFar), 0);
		return mat4(c1, c2, c3, c4);
	}

	// ����۲���������������Դ�����
	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		vec4 n = normalize(eye - at);
		vec4 u = vec4(normalize(cross(up, n)), 0);
		vec4 v = vec4(normalize(cross(n, u)), 0);
		vec4 t = vec4(0, 0, 0, 1);
		return mat4(u, v, n, t) * Translate(-eye);
	}

	mat4 modelMatrix(1.0);   //Ĭ��ģ�;�����ǵ�λ����
	mat4 viewMatrix(1.0);    //Ĭ�Ϲ۲������ǵ�λ������ô�е������������ϵԭ��
	mat4 projMatrix = ortho(-2, 2, -2, 2, -2, 2);  //ͶӰ������һ��4*4*4���������ڿɼ�
}

void init()
{
	// ���ñ���Ϊ��ɫ��Ϊ�˿���Ӱ��
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	programID = InitShader("vshader.glsl", "fshader.glsl");

	// �Ӷ�����ɫ���л�ȡ��Ӧ������λ��
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	rotationMatrixID = glGetUniformLocation(programID, "rotationMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");
	shadowID = glGetUniformLocation(programID, "isShadow");

	//��ȡOFF�ļ�
	mesh->read_off("sphere.off");

	vector<vec3f> vs = mesh->v();  //��������
	vector<vec3i> fs = mesh->f();  //Ƭ������
	vector<vec3f> ns;              //������������


	vec3f shift = vs[0];

	for (int i = 0; i < vs.size(); ++i)
		if (vs[i].y < shift.y) shift = vs[i];

	for (int i = 0; i < vs.size(); ++i)  //Ϊ���ܹ��õ�Ӱ�ӣ������������ƶ�
	{
		vs[i].y -= shift.y-0.1;
		ns.push_back(vs[i] - vec3(0.0, -shift.y, 0.0));
	}

	// ǰ�����ǵõ���������Ҫ�����ݺͱ����������濪ʼ��������
	//����VAO�������������
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	//���ɰ󶨶��������������
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

	//����Ƭ��Ķ�������
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

// ��ʼ��Ⱦ�����ǵ����ݴ���GPU�洢����Ҫ��Ⱦ������ʾ����Ļ��
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	glEnableVertexAttribArray(vPositionID);  //������������

	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	// ������ת������X����תʹ����Ӱ���ø�����
	mat4 rotationMatrix = RotateX(rotationAngle); 
	glUniformMatrix4fv(rotationMatrixID, 1, GL_TRUE, rotationMatrix);

	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	// ������Ӱ����
	mat4 shadowProjMatrix(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly);
	// ��Ӱ�����������ϵ�´���ģ������ǳ���ģ�Ӿ���
	shadowProjMatrix = shadowProjMatrix * modelViewMatrix; //��Ӱ��ģ�Ӿ���
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, Camera::projMatrix);
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, shadowProjMatrix);
	// ������Ӱ��־λΪ1
	glUniform1i(shadowID, 1);
	// �����Դ��λ����С���Ϸ��������Ӱ
	if (ly > 0) {
		glDrawElements(  //���ö����������ж�����ɫ���滭�����ҵ���������±����ʽ����
			GL_TRIANGLES,
			int(mesh->f().size() * 3),
			GL_UNSIGNED_INT,
			(void*)0
		);
	}

	//�滭ʵ���ʱ���Ҫ��ģ�Ӿ����޸�Ϊԭ���ģ�������Ϊ�ɹ���
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, modelViewMatrix);
	// ������Ӱ��־λΪ0
	glUniform1i(shadowID, 0);

	// ����С��
	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	// �����Դ��Ϣ
	glUniform3fv(lightPosID, 1, lightPos);

	//�󶨶��㻺��
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//�󶨲�����������������
	glEnableVertexAttribArray(vNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glVertexAttribPointer(
		vNormalID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//�󶨶�����������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

// �������ô���
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

// �����Ӧ����
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		lightPos[0] = (x - 250) / 10;
		lightPos[1] = (250 - y) / 10;
	}
	glutPostWindowRedisplay(mainWindow);
}

// ������Ӧ����
void specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		rotationAngle += 1.0;
	}
	if (key == GLUT_KEY_DOWN) {
		rotationAngle -= 1.0;
	}
	glutPostRedisplay();
}


//�ص��������������ʹ�ñ仯����̫��Ӳ
void idle(void)
{
	glutPostRedisplay();
}


//����ڴ�
void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	if (mesh)
	{
		delete mesh;
		mesh = NULL;
	}
}


//������
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);  //˫����
	glutInitWindowSize(500, 500);
	mainWindow = glutCreateWindow("���ķ� 2016150182");

	glewExperimental = GL_TRUE;
	glewInit();
	init();

	//���ֻص�����
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutSpecialFunc(specialKey);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();
	return 0;
}