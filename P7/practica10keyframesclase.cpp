/* Semestre 2024-1
Animaci�n por keyframes
La textura del skybox fue conseguida desde la p�gina https://opengameart.org/content/elyvisions-skyboxes?page=1
y edit� en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

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

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
float donas_mov;
float canica_mov;
float resorte_mov;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture DiamanteTexture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Cuerpo_Pinball;
Model Fondo_Pinball;
Model Obstaculos_Pinball;
Model Pantalla_Pinball;
Model Fondo_Pantalla_Pinball;
Model Resorte;
Model Canica1;
Model Canica2;
Model Canica3;
Model Paleta1;
Model Paleta2;
Model Paleta3;
Model Kirby;
Model Kirby_pierna_d;
Model Kirby_pierna_i;
Model Kirby_brazo_d;
Model Kirby_brazo_i;
Model dona1;
Model dona2;
Model dona3;
Model Estrella;

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
SpotLight spotLights2[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//funci�n para teclado de keyframes 
void inputKeyframes(bool* keys);

//c�lculo del promedio de las normales para sombreado de Phong
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
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};


	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

}
void CrearDiamante()
{
	unsigned int cubo_indices[] = {
	0, 1, 2,
	3, 4, 5,
	6, 7, 8,
	9, 10, 11,
	12, 13, 14,
	15, 16, 17,
	18, 19, 20,
	21, 22, 23,
	24, 25, 26,
	27, 28, 29,
	30, 31, 32,
	33, 34, 35
	};

	GLfloat cubo_vertices[] = {
		// Cara 1
		0.0, 0.5f, 0.0f, 0.0f, 0.33f, 0.0f, 0.0f, -1.0f, // 0
		0.0f, 0.0f, 0.5f, 0.25f, 0.33f, 0.0f, 0.0f, -1.0f, // 1
		0.5f, 0.0f, 0.0f, 0.25f, 0.66f, 0.0f, 0.0f, -1.0f, // 2

		// Cara 2
		0.0f, 0.5f, 0.0f, 0.25f, 0.33f, -1.0f, 0.0f, 0.0f, // 3
		0.0f, 0.0f, -0.5f, 0.5f, 0.33f, -1.0f, 0.0f, 0.0f, // 4
		-0.5f, 0.0f, 0.0f, 0.5f, 0.66f, -1.0f, 0.0f, 0.0f, // 5

		// Cara 3
		0.0f, 0.5f, 0.0f, 0.5f, 0.33f, 0.0f, 0.0f, 1.0f, // 6
		0.5f, 0.0f, 0.0f, 0.75f, 0.33f, 0.0f, 0.0f, 1.0f, // 7
		0.0f, 0.0f, -0.5f, 0.75f, 0.66f, 0.0f, 0.0f, 1.0f, // 8

		// Cara 4
		0.0f, 0.5f, 0.0f, 0.75f, 0.33f, 1.0f, 0.0f, 0.0f, // 9
		-0.5f, 0.0f, 0.0f, 1.0f, 0.33f, 1.0f, 0.0f, 0.0f, // 10
		0.0f, 0.0f, 0.5f, 1.0f, 0.66f, 1.0f, 0.0f, 0.0f, // 11

		// Cara 5
		0.0f, -0.5f, 0.0f, 0.5f, 0.66f, 0.0f, 1.0f, 0.0f, // 12
		0.0f, 0.0f, 0.5f, 0.75f, 0.66f, 0.0f, 1.0f, 0.0f, // 13
		0.5f, 0.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f, // 14

		// Cara 6
		0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, // 15
		0.0f, 0.0f, -0.5f, 0.75f, 0.0f, 0.0f, -1.0f, 0.0f, // 16
		0.0f, 0.0f, -0.5f, 0.75f, 0.33f, 0.0f, -1.0f, 0.0f, // 17

		// Cara 7
		0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, // 18
		0.5f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, -1.0f, 0.0f, // 19
		0.0f, 0.0f, -0.5f, 0.75f, 0.33f, 0.0f, -1.0f, 0.0f, // 20

		// Cara 8
		0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, // 21
		-0.5f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, -1.0f, 0.0f, // 22
		0.0f, 0.0f, 0.5f, 0.75f, 0.33f, 0.0f, -1.0f, 0.0f, // 23

		// Cara 9
		0.0f, -0.5f, 0.0f, 0.25f, 0.0f, 0.0f,
			-1.0f, 0.0f, // 24
	0.0f, 0.0f, -0.5f, 0.25f, 0.33f, 0.0f, -1.0f, 0.0f, // 25
	0.0f, 0.0f, 0.5f, 0.5f, 0.33f, 0.0f, -1.0f, 0.0f, // 26

	// Cara 10
	0.0f, -0.5f, 0.0f, 0.25f, 0.0f, 0.0f, -1.0f, 0.0f, // 27
	0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, // 28
	-0.5f, 0.0f, 0.0f, 0.5f, 0.33f, 0.0f, -1.0f, 0.0f, // 29

	// Cara 11
	0.0f, -0.5f, 0.0f, 0.25f, 0.0f, 0.0f, -1.0f, 0.0f, // 30
	-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, // 31
	0.0f, 0.0f, -0.5f, 0.5f, 0.33f, 0.0f, -1.0f, 0.0f // 32
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 300, 60);
	meshList.push_back(dado);

}

void CrearBase() {
	unsigned int octagono_indices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5,
	0, 5, 6,
	0, 6, 7,
	0, 7, 8,
	0, 8, 1
	};

	GLfloat octagono_vertices[] = {
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Centro del oct�gono

		// V�rtices exteriores del oct�gono
		0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f, // 1
		0.3536f, 0.3536f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 2
		0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, // 3
		-0.3536f, 0.3536f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 4
		-0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, // 5
		-0.3536f, -0.3536f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 6
		0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // 7
		0.3536f, -0.3536f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f // 8
	};
	Mesh* base = new Mesh();
	base->CreateMesh(octagono_vertices, octagono_indices, 300, 60);
	meshList.push_back(base);
}
void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 7;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;
	float giroAvionInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 7;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion= giroAvion;//completar
	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animaci�n entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animaci�n con �ltimo frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolaci�n del pr�ximo cuadro
			{
				
				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animaci�n
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1024, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	CrearDiamante();


	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/Skybox/pasto.jpg");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
	DiamanteTexture = Texture("Textures/diamante.jpg");
	DiamanteTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	//Maquina Pinball
	Cuerpo_Pinball = Model();
	Cuerpo_Pinball.LoadModel("Models/cuerpo_pinball.obj");
	Fondo_Pinball = Model();
	Fondo_Pinball.LoadModel("Models/fondo_pinball.obj");
	Fondo_Pantalla_Pinball = Model();
	Fondo_Pantalla_Pinball.LoadModel("Models/fondo_pantalla_pinball.obj");
	Obstaculos_Pinball = Model();
	Obstaculos_Pinball.LoadModel("Models/obstaculos_pinball.obj");
	Pantalla_Pinball = Model();
	Pantalla_Pinball.LoadModel("Models/pantalla_pinball.obj");
	//Canicas
	Canica1 = Model();
	Canica1.LoadModel("Models/canica1.obj");
	Canica2 = Model();
	Canica2.LoadModel("Models/canica1.obj");
	Canica3 = Model();
	Canica3.LoadModel("Models/canica1.obj");

	//Paletas
	Paleta1 = Model();
	Paleta1.LoadModel("Models/paleta_izq.obj");
	Paleta2 = Model();
	Paleta2.LoadModel("Models/paleta_der.obj");
	Paleta3 = Model();
	Paleta3.LoadModel("Models/paleta_der.obj");

	//Resorte
	Resorte = Model();
	Resorte.LoadModel("Models/resorte.obj");

	//Avatar Kirby
	Kirby = Model();
	Kirby.LoadModel("Models/kirby_cuerpo.obj");
	Kirby_pierna_d = Model();
	Kirby_pierna_d.LoadModel("Models/pie_der_kirby.obj");
	Kirby_pierna_i = Model();
	Kirby_pierna_i.LoadModel("Models/pie_izq_kirby.obj");
	Kirby_brazo_d = Model();
	Kirby_brazo_d.LoadModel("Models/brazo_derecho_kirby.obj");
	Kirby_brazo_i = Model();
	Kirby_brazo_i.LoadModel("Models/brazo_izq_kirby.obj");

	//Donas
	dona1 = Model();
	dona1.LoadModel("Models/jerar_1.obj");
	dona2 = Model();
	dona2.LoadModel("Models/jerar_2.obj");
	dona3 = Model();
	dona3.LoadModel("Models/jerar_3.obj");
	//ESTRELLAS
	Estrella = Model();
	Estrella.LoadModel("Models/Star.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");
	skyboxFaces.push_back("Textures/Skybox/sky.jpg");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	unsigned int spotLightCount2 = 0;
	

	//luz fija alumbra el Pinball
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.06f, 0.06f,
		0.0f, 30.0f, 5.0f,
		0.0f, -5.0f, 0.0f,
		0.2f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;
	//Luz zona de las paletas
	spotLights2[0] = SpotLight(1.0f, 0.0f, 0.0f,
		0.1f, 0.1f,
		-1.2f, 30.0f, 10.0f,
		0.0f, -5.0f, 0.0f,
		0.2f, 0.0f, 0.0f,
		7.0f);
	spotLightCount2++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.0001f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
	
	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQU�

	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;


	KeyFrame[1].movAvion_x = -1.0f;
	KeyFrame[1].movAvion_y = 2.0f;
	KeyFrame[1].giroAvion = 0;


	KeyFrame[2].movAvion_x = -2.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].giroAvion = 0;


	KeyFrame[3].movAvion_x = -3.0f;
	KeyFrame[3].movAvion_y = -2.0f;
	KeyFrame[3].giroAvion = 0;


	KeyFrame[4].movAvion_x = -3.0f;
	KeyFrame[4].movAvion_y = -2.0f;
	KeyFrame[4].giroAvion = 180.0f;

	KeyFrame[5].movAvion_x = 0.0f;
	KeyFrame[5].movAvion_y = 0.0f;
	KeyFrame[5].giroAvion = 180.0;

	KeyFrame[6].movAvion_x = 0.0f;
	KeyFrame[6].movAvion_y = 0.0f;
	KeyFrame[6].giroAvion = 0.0f;

	//Se agregan nuevos frames 

		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");

	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

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

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);

		//prender y apagar luz paletas

		if (mainWindow.getarregloluces() == 1)
		{
			if (mainWindow.getlinterna() == true)
			{
				shaderList[0].SetSpotLights(spotLights, spotLightCount);
			}
			else
			{
				shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			}

		}
		else
		{
			if (mainWindow.getlinterna() == true)
			{
				shaderList[0].SetSpotLights(spotLights2, spotLightCount2);
			}
			else
			{
				shaderList[0].SetSpotLights(spotLights2, spotLightCount2 - 1);
			}
		}
		
		
		



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Obstaculo jerarquico
		if (donas_mov < 0.5f) {
			donas_mov += 0.001* deltaTime;
		}
		else  {
			donas_mov = 0;
		}	

		// OBSTACULO 1
		//dona 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.5f,  11.35f - donas_mov, 5.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona1.RenderModel();
		//dona 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona2.RenderModel();
		//dona 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona3.RenderModel();

		// OBSTACULO 2
		//dona 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.5f, 11.35f-donas_mov, 5.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona1.RenderModel();
		//dona 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona2.RenderModel();
		//dona 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona3.RenderModel();

		// OBSTACULO 3
		//dona 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 10.8f + donas_mov, 7.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona1.RenderModel();
		//dona 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona2.RenderModel();
		//dona 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dona3.RenderModel();

		//ESTRELLA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 11.35f, 3.5f));
		model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrella.RenderModel();
		//Estrella 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 11.35f, 2.0f));
		model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrella.RenderModel();

		//DIAMANTE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 11.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DiamanteTexture.UseTexture();
		meshList[6]->RenderMesh();

		//Instancias cuerpo del Pinball

		//Instancia del cuerpo del Pinball
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_Pinball.RenderModel();

		//Instancia Fondo del cuerpo Pinball
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Fondo_Pinball.RenderModel();
		
		//Instancia Pantalla Pinball
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -2.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pantalla_Pinball.RenderModel();

		//Instancia Fondo de Pantalla Pinball
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -2.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Fondo_Pantalla_Pinball.RenderModel();
		//Intancia obstaculos contorno
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -2.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Obstaculos_Pinball.RenderModel();

		//Intancias Canicas
		if (canica_mov < 6.5f) {
			canica_mov += 0.05 * deltaTime;

		}
		else {
			canica_mov = 0;
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.8f, 11.5f, -canica_mov+11.0f));

		model = glm::scale(model, glm::vec3(0.045f, 0.045f, 0.045f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Canica1.RenderModel();

		//Instancia Resorte
		if (resorte_mov< 0.05f) {
			resorte_mov += 0.0005 * deltaTime;

		}
		else {
			resorte_mov = 0;
		}
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(1.8f, 11.5f, 12.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f - resorte_mov));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Resorte.RenderModel();
		modelaux = model;

		//Instancia Paletas
		//Paleta1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.8f, 11.5f, 12.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f ));
		model = glm::translate(model, glm::vec3(-47.5f, -5.0f, -15.0f)); 
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta1.RenderModel();
		modelaux = model;

		//Paleta2
		model=modelaux;
		model = glm::translate(model, glm::vec3(10.0f,-12.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta2.RenderModel();
		modelaux = model;
		
		//Paleta3
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.8f, 0.0f, -8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta2.RenderModel();

		//kirby Cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, 11.4f, 10.0f)); 
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby.RenderModel();
		//piernas
		//Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.15f, -0.18f, -0.22f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby_pierna_d.RenderModel();
		// Izquier
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.45f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby_pierna_i.RenderModel();
		//Brazos
		//Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.15f, 0.2f, 0.1f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby_brazo_i.RenderModel();
		// Izquier
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.65f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby_brazo_d.RenderModel();

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animaci�n con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;
			
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1 && guardoFrame>0)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1 && ciclo>0)
		{
			ciclo = 0;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}

}
