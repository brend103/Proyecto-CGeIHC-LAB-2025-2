#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <map>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;



Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;



Texture pastoTexture;


Model BañoM_M;
Model BañoH_M;
//DARDOS
Model Carpa_M;
Model Dardo_M;
Model Escalera_M;
Model GloboA_M;
Model GloboV_M;
Model GloboR_M;
Model GloboM_M;
Model GloboN_M;
Model GloboC_M;
Model Peluche_M;
Model Tablero_M;
//PINOS
Model Bomba_M;
Model Linea_M;
Model Pantalla_M;
Model Pino_M;
// JAULA BATEO
Model Bola_M;
Model Caja_M;
Model Campo_M;
Model Cofre_M;
Model Espada_M;
Model Reflector_M;
Model Reja_M;

Skybox skybox;



//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};
	//Piso que mide 20 unidades
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		1.0f, 1.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj1);


}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);


	pastoTexture = Texture("Textures/pastoCamino.tga");
	pastoTexture.LoadTextureA();

	BañoH_M = Model();
	BañoH_M.LoadModel("Models/Baños/Hombre/bañoH.obj");
	BañoM_M = Model();
	BañoM_M.LoadModel("Models/Baños/Mujer/bañoM.obj");
	Carpa_M = Model();
	Carpa_M.LoadModel("Models/Dardos/Carpa/carpa.obj");
	Dardo_M = Model();
	Dardo_M.LoadModel("Models/Dardos/Dardo/dardo.obj");
	Escalera_M = Model();
	Escalera_M.LoadModel("Models/Dardos/Escalera/escalera.obj");
	GloboA_M = Model();
	GloboA_M.LoadModel("Models/Dardos/Globos/globo_amarillo.obj");
	GloboM_M = Model();
	GloboM_M.LoadModel("Models/Dardos/Globos/globo_magenta.obj");
	GloboN_M = Model();
	GloboN_M.LoadModel("Models/Dardos/Globos/globo_naranja.obj");
	GloboR_M = Model();
	GloboR_M.LoadModel("Models/Dardos/Globos/globo_rojo.obj");
	GloboV_M = Model();
	GloboV_M.LoadModel("Models/Dardos/Globos/globo_verde.obj");
	GloboC_M = Model();
	GloboC_M.LoadModel("Models/Dardos/Globos/globo_cian.obj");
	Peluche_M = Model();
	Peluche_M.LoadModel("Models/Dardos/Peluche/peluches.obj");
	Tablero_M = Model();
	Tablero_M.LoadModel("Models/Dardos/Tablero/tablero.obj");
	Bomba_M = Model();
	Bomba_M.LoadModel("Models/Boliche/Bomba/bomba.obj");
	Linea_M = Model();
	Linea_M.LoadModel("Models/Boliche/Linea/linea.obj");
	Pantalla_M = Model();
	Pantalla_M.LoadModel("Models/Boliche/Pantalla/pantalla.obj");
	Pino_M = Model();
	Pino_M.LoadModel("Models/Boliche/Pino/pino.obj");
	Bola_M = Model();
	Bola_M.LoadModel("Models/JaulaBateo/Bola/pinecone.obj");
	Caja_M = Model();
	Caja_M.LoadModel("Models/JaulaBateo/Caja/caja.obj");
	Campo_M = Model();
	Campo_M.LoadModel("Models/JaulaBateo/Campo/campo.obj");
	Cofre_M = Model();
	Cofre_M.LoadModel("Models/JaulaBateo/Cofre/cofre.obj");
	Espada_M = Model();
	Espada_M.LoadModel("Models/JaulaBateo/Espada/espada.obj");
	Reflector_M = Model();
	Reflector_M.LoadModel("Models/JaulaBateo/Reflector/reflector.obj");
	Reja_M = Model();
	Reja_M.LoadModel("Models/JaulaBateo/Reja/reja.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cube_right.png");
	skyboxFaces.push_back("Textures/Skybox/cube_left.png");
	skyboxFaces.push_back("Textures/Skybox/cube_down.png");
	skyboxFaces.push_back("Textures/Skybox/cube_up.png");
	skyboxFaces.push_back("Textures/Skybox/cube_back.png");
	skyboxFaces.push_back("Textures/Skybox/cube_front.png");

	skybox = Skybox(skyboxFaces);




	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales




	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);



	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//Este se hace dentro del while
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 20.0f));     //escalamos el piso por 30 (ahora mide 600 unidades)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pastoTexture.UseTexture();

		meshList[0]->RenderMesh();

		//Juego de daRdos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, -2.0f, 125.0f));
		model = glm::scale(model, glm::vec3(4.05f, 4.05f, 4.05f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carpa_M.RenderModel();
	
		//Tablero con globos
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 5.50f, -7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tablero_M.RenderModel();
		model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		//Segunda fila de globos
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		//Tercera lifa
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();

		//Cuarta 
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();;
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		//quinta 
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboC_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboV_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboM_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboN_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboA_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GloboR_M.RenderModel();
		model = modelaux;
		//escalera con peluches
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 1.540f, 5.30f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Escalera_M.RenderModel();
		model = glm::translate(model, glm::vec3(0.0f, 1.280f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Peluche_M.RenderModel();
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dardo_M.RenderModel();
		model = modelaux;

		//BOLOS 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(273.50f, 21.40f, -27.0f));
		model = glm::scale(model, glm::vec3(4.05f, 4.05f, 4.05f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pantalla_M.RenderModel();
		//Pista de bolos
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -5.50f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Linea_M.RenderModel();
		model = modelaux;
		//bolos 
		modelaux = model;
		model = glm::translate(model, glm::vec3(-5.60f, -4.9f, 1.60f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();

		model = glm::translate(model, glm::vec3(-2.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(-1.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(-0.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = modelaux;
		modelaux = model;
		model = glm::translate(model, glm::vec3(2.50f, -4.9f, 1.60f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();

		model = glm::translate(model, glm::vec3(-2.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();

		model = glm::translate(model, glm::vec3(-1.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		model = glm::translate(model, glm::vec3(-0.50f, -0.0f, 0.50f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino_M.RenderModel();
		
	
		//Baño 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 4.50f, 110.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();		
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();

		//baño 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-135.0f, 4.50f, 100.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();
		
		//baño 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(200.0f, 4.50f, 100.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();

		//baño 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(161.0f, 4.50f, -110.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();
		//baño 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 4.50f, -150.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();
		//baño6
				model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 4.50f, -10.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();
		//baño 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 4.50f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoM_M.RenderModel();

		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BañoH_M.RenderModel();
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}