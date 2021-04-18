#include "main.hpp"

using namespace std;
using namespace glm;

const char* TITLE = "Teste";
int WIDTH = 800;
int HEIGHT = 800;
GLFWwindow* window = NULL;
const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLuint program;
GLuint vertex_array_object;
GLuint vertex_buffer_object;
GLuint vertex_buffer_object_index;
GLuint vertex_buffer_object_colors;

GLuint objIndex;

//mat4 valueptr;
GLuint viewLoc;
GLuint modelViewLoc;
GLuint projectionLoc;

mat4 viewMatrix;
mat4 modelMatrix;
mat4 viewCamera;

mat4 projectionMatrix = perspective(1.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 500000.0f);

int initalization() {
	if (!glfwInit()) {
		cerr << "GLFW inicialization failed" << endl;
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);

	if (window == NULL) {
		cerr << "Error" << endl;
		return 1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		cerr << "Error" << endl;
		glfwTerminate();
		return 1;
	}

	return 0;
}

void verifyError(GLuint who, int type, const char* msg) {
	GLint success;
	GLchar info_log[512];

	glGetShaderiv(who, type, &success);

	if (!success) {
		glGetShaderInfoLog(who, 512, NULL, info_log);
		cout << msg << endl;
	}
}

void compileShaders() {
	GLuint vertex_shader;
	GLuint fragment_shader;

	static const char* vertex_shader_source[] = {
		"#version 450 core																	\n"
		"layout (location = 0) in vec3 pos;													\n"
		"layout (location = 1) in vec3 color; 												\n"
		"																					\n"
		"uniform mat4 model; 																\n"
		"uniform mat4 view; 																\n"
		"uniform mat4 projection; 															\n"
		"																					\n"
		"uniform mat4 translate;															\n"
		"uniform mat4 rotation;																\n"
		"uniform mat4 scaling;																\n"
		"																					\n"
		"out vec3 vs_color;  																\n"
		"																					\n"
		"void main(void) {					 												\n"
		"	//gl_Position = projection * translate * rotation * scaling * vec4(pos, 1.0f);	\n"
		"	gl_Position = projection * view * model * vec4(pos, 1.0f);						\n"
		"	vs_color = color;																\n"
		"}																					\n"
	};

	static const char* fragment_shader_source[] = {
		"#version 450 core					\n"
		"									\n"
		"in vec3 vs_color;					\n"
		"out vec4 color;					\n"
		"									\n"
		"void main(void) {					\n"
		"	color = vec4(vs_color, 1.0);	\n"
		"}									\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	verifyError(vertex_shader, GL_COMPILE_STATUS, "vertexShader");

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	verifyError(fragment_shader, GL_COMPILE_STATUS, "fragmentShader");

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	verifyError(program, GL_LINK_STATUS, "program");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void handlePerspective() {
	glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projectionMatrix));
}

void draw(vector<GLfloat> vertices, GLsizeiptr size_vertices,
	vector<GLfloat> colors, GLsizeiptr size_colors,
	vector<GLushort> index, GLsizeiptr size_index,
	GLenum type, GLsizei count) {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_colors);
	glBufferData(GL_ARRAY_BUFFER, size_colors, colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_index, index.data(), GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawElements(type, count, GL_UNSIGNED_SHORT, 0);
	//glDrawArrays(type, 0, size_vertices / 4/3);
}

void handleCameraMovement(GLuint index) {
}

void handleCameraRotation() {
	viewCamera = lookAt(states.eye, states.center, states.up);

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(viewCamera));
}

void render() {
	viewMatrix = translate(mat4(1.0f), vec3(-states.cameraX, -states.cameraY, -states.cameraZ));

	handleRealTimeTransformations();
	handleCameraRotation();

	//glPointSize(5.0f);

	for (GLuint i = 0; i < objectsToRender.size(); i++) {
		objIndex = objectsToRender[i].index;

		modelMatrix = translate(mat4(1.0f), vec3(-states.cameraX, -states.cameraY, -states.cameraZ));
		modelMatrix *= translate(mat4(1.0f), objectsToRender[i].translation);
		modelMatrix *= scale(mat4(1.0f), objectsToRender[i].scaling);
		if (i == INDEX_SATURN_RING) {
			modelMatrix *= rotate(mat4(1.0f), 90*3.14f/180, vec3(1.0f, 1.0f, 0.0f));
		}
		modelMatrix *= rotate(mat4(1.0f), objectsToRender[i].rotation.radians, objectsToRender[i].rotation.vec);

		glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, value_ptr(modelMatrix));

		draw(objects[objIndex].vertices, objects[objIndex].vertices.size() * 4,
			objectsToRender[i].colors, objectsToRender[i].colors.size() * 4,
			objects[objIndex].indexs, objects[objIndex].indexs.size() * 2,
			GL_TRIANGLES, objects[objIndex].indexs.size());
	}
}

void startup() {
	compileShaders();
	glUseProgram(program);

	glfwSetKeyCallback(window, processInput);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glGenBuffers(1, &vertex_buffer_object);
	glGenBuffers(1, &vertex_buffer_object_colors);
	glGenBuffers(1, &vertex_buffer_object_index);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	viewLoc = glGetUniformLocation(program, "view");
	modelViewLoc = glGetUniformLocation(program, "model");
	projectionLoc = glGetUniformLocation(program, "projection");


	handlePerspective();
}

void shutdown() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertex_array_object);
	glfwTerminate();
}

int main() {
	if (initalization()) {
		return -1;
	}

	startup();

	processObjects();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearBufferfv(GL_COLOR, 0, color);
		glClear(GL_DEPTH_BUFFER_BIT);

		render();

		glfwSwapBuffers(window);
	}

	shutdown();

	return 0;
}