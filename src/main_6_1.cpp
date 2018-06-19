#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor; // Identyfikator shadera uzywanego do rysowania
GLuint programTexture; // Identyfikator shadera uzywanego do rysowania
GLuint mainTexture; // Identyfikator shadera uzywanego do rysowania

Core::Shader_Loader shaderLoader;

float appLoadingTime;

obj::Model sharkModel;
obj::Model fishModel;
obj::Model rockModel;
obj::Model backgModel;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

GLuint textureFish;
GLuint textureRock;
GLuint waterTexture;
GLuint sharkTexture;

static const int NUM_FISHES = 40;

glm::mat4 fish[NUM_FISHES];

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	// Skoro chcemy patrzec na obiekty lezace w poczatku ukladu wspolrzednych, wektor patrzenia "fwd" powinien byc caly czas przeciwny do wektora pozycji kamery.
	// Wektor "up" jest skierowany do gory.
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle)); //fwd
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

//Funkcja umożliwia rysowanie obiektów oteksturowanych
//drawObjectTextureMain shader bez świtła
void drawObjectTextureMain(obj::Model * model, glm::mat4 modelMatrix, GLuint textureID)
{
	GLuint program = mainTexture;

	// Aktywowanie shadera
	glUseProgram(program);
	//Obliczanie oświetlenia obiektu, natężenia światła za pomocą danego shadera
	//Odwołanie do zmiennej "sampler2dtype" w shader_tex1.frag
	//Ustawianie zmiennej sampler2D na wczytaną wcześniej teksturę przekazaną jako parametr
	Core::SetActiveTexture(textureID, "sampler2dtype", 1, 0);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	// "transformation" jest automatycznie inicjalizowane macierza jednostkowa 4 x 4
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	
	// Polecenie glUniformMatrix4fv wysyla zmienna "transformation" do GPU i przypisuje ja do zmiennej typu mat4 o nazwie "modelViewProjectionMatrix" w shaderze.
	// Shader uzyje tej macierzy do transformacji wierzcholkow podczas rysowania.
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	// Wylaczenie shadera
	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	Core::SetActiveTexture(textureId, "textureSampler", 1, 0);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	//Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void renderScene()
{
	//Tworzenie macierzy perspektywy za pomoca createPerspectiveMatrix(), uzyskujemy obraz 3D
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	
	// Zmienna "time" po wykonaniu tej linii kodu zawiera liczbe sekund od uruchomienia programu
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;

	//"Przyczepianie" modelu rekina do kamery, uzaleznienie jego macierzy modelu od pozycji i orientacji kamery tak, aby wygladalo jakby użytkownik poruszal sie drapieżnikiem.
	//Model w trakcie poruszania (dzieki zmianie zmiennych pozycji kamery) animuje ruch poruszania się.
	glm::mat4 sharkModelMatrix = glm::translate(cameraPos + cameraDir * 1.9f + glm::vec3(0,-0.75f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.01f));
	sharkModelMatrix *= glm::rotate(sin(cameraPos[0] + cameraPos[1] + cameraPos[2] + cameraAngle) / 4, glm::vec3(0, 1, 0));
	drawObjectTexture(&sharkModel, sharkModelMatrix, sharkTexture);
	//drawObjectColor(&sharkModel, sharkModelMatrix, glm::vec3(0.6f));

	glm::mat4 rotation;
	glm::mat4 fishb[NUM_FISHES];

	//Ustalenie macierzy rotacji
	rotation[0][0] = cos(time);
	rotation[2][0] = sin(time);
	rotation[0][2] = -sin(time);
	rotation[2][2] = cos(time);
	for (int i = 0; i < NUM_FISHES; ++i)
	{

		fishb[i][3][0] = fish[i][3][0] * sin(time);
		fishb[i][3][2] = fish[i][3][2] * cos(time);
		drawObjectTexture(&fishModel, glm::translate(glm::vec3(fish[i][0][0], fish[i][0][1], fish[i][0][2]))*fishb[i]*rotation*glm::rotate((sin(time * 10) / 4), glm::vec3(0, 1, 0))*glm::scale(glm::vec3(0.01f)), textureFish);

	}

	drawObjectTextureMain(&backgModel, glm::translate(glm::vec3(0, 0, 0))* glm::scale(glm::vec3(70.0f)), waterTexture);

	drawObjectTexture(&rockModel, glm::translate(glm::vec3(3, -2, 2)) * glm::scale(glm::vec3(0.005f)), textureRock);
	
	// Nalezy wywolac na koncu rysowania aktualnej klatki obrazu
	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	// Wektor normalny wierzcholka dostepny jest w vertex shaderze (plik z rozszerzeniem .vert). Kolor powierzchni (piksela) ustala sie jednak we fragment shaderze (.frag).
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	mainTexture = shaderLoader.CreateProgram("shaders/shader_tex1.vert", "shaders/shader_tex1.frag");

	//Wczytanie modeli z plików
	fishModel = obj::loadModelFromFile("models/fish.obj");
	rockModel = obj::loadModelFromFile("models/rocks.obj");
	sharkModel = obj::loadModelFromFile("models/shark.obj");
	backgModel = obj::loadModelFromFile("models/backg.obj");

	//Wczytywanie tekstur z plików
	textureFish = Core::LoadTexture("textures/fish.png");
	textureRock = Core::LoadTexture("textures/rock.png");
	waterTexture = Core::LoadTexture("textures/water.png");
	sharkTexture = Core::LoadTexture("textures/shark.png");

	//Generowanie losowych pozycji dla pływających rybek
	for (int i = 0; i < NUM_FISHES; ++i)
	{
		fish[i][0][0] = ((rand() % 3) + 1);
		fish[i][0][1] = ((rand() % 18) % 3);
		fish[i][0][2] = ((rand() % 3) + 1);
		fish[i][3][0] = ((rand() % 3) + 6);
		fish[i][3][2] = ((rand() % 3) + 6);
	}

	//Zapisanie czasu załadowania się programu
	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown()
{
	//Usuwanie załadowanych shaderów z pamięci
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(mainTexture);
}

void idle()
{
	// Ta funkcja informuje freegluta, ze nalezy odzwiezyc zawartosc okna przy uzyciu funkcji podanej w glutDisplayFunc
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	// Stworzenie okna przy użyciu biblioteki freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Podwodny raj");
	// Inicjalizacja rozszerzen OpenGL
	glewInit();

	// Inicjalizacja programu (ladowanie shaderow, tekstur itp)
	init();
	glutKeyboardFunc(keyboard);
	// Poinformowanie freegluta jaka funkcja bedzie sluzyc do odswiezania obrazu
	glutDisplayFunc(renderScene);
	// Poinformowanie freegluta jaka funkcja bedzie wywolywana w glownej petli programu
	glutIdleFunc(idle);

	// Uruchomienie glownej petli
	glutMainLoop();

	// Sprzatanie (usuwanie shaderow itp)
	shutdown();

	return 0;
}
