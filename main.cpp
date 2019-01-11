#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "Player.h"
#include "Portal.h"
#include "Enemy.h"
#include "Model.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>
#include <map>

#include <iostream>
#include <sstream>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

// callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// some function about render
unsigned int loadCubemap(vector<std::string> faces);
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

// Player based on camera
Player player;

// Vector storage every portal and enemy
std::vector<Portal> portalVec;
vector<Enemy> enemyVec;

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

// Something about camera mouse_move
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// things about Text
float showText = 0.0f;
float isShowText = false;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ingress", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// To hide mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load skybox textures
	vector<std::string> faces
	{
		"./image/right.jpg",
		"./image/left.jpg",
		"./image/top.jpg",
		"./image/bottom.jpg",
		"./image/front.jpg",
		"./image/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// Compile shaders 
	Shader shadowShader("./shader/vertex_shader.vert", "./shader/fragment_shader.frag");
	Shader colorShader("./shader/OnlyColor.vert", "./shader/OnlyColor.frag");
	Shader skyboxShader("./shader/skybox.vert", "./shader/skybox.frag");
	Shader simpleDepthShader("./shader/simpleDepthShader.vert", "./shader/simpleDepthShader.frag");
	Shader noShadow("./shader/noshadow.vert", "./shader/noshadow.frag");
	Shader reflectionShader("./shader/reflection.vert", "./shader/reflection.frag");
	Shader boomShader("./shader/boom.vert", "./shader/boom.frag", "./shader/boom.geom");
	Shader boomPointShader("./shader/boom.vert", "./shader/boom.frag", "./shader/boomtoPoint.geom");
	Shader particleShader("./shader/particle.vert", "./shader/particle.frag");
	Shader textShader("./shader/free_type.vert", "./shader/free_type.frag");
	Shader UIShader("./shader/UI.vert", "./shader/UI.frag");
	Shader fireringShader("./shader/fire_ring.vert", "./shader/fire_ring.frag");

	// Load and Create model, initialize them.
	glm::vec3 portalPos(3.0f, 1.5f, 1.5f);
	Portal portal1(portalPos);
	portalPos = glm::vec3(100.0f, 1.5f, 150.0f);
	Portal portal2(portalPos);
	portalVec.push_back(portal1);
	portalVec.push_back(portal2);
	Enemy enemy1(glm::vec3(10.0f, 0.0f, 0.0f), 1);
	Enemy enemy2(glm::vec3(0.0f, 0.0f, 10.0f), 2);
	Enemy enemy3(glm::vec3(0.0f, 0.0f, 0.0f), 3);
	Enemy enemy4(glm::vec3(10.0f, 0.0f, 10.0f), 4);
	enemyVec.push_back(enemy1);
	enemyVec.push_back(enemy2);
	enemyVec.push_back(enemy3);
	enemyVec.push_back(enemy4);
	Object scene("./model/scene/scene.obj", glm::vec3(1.5f, 1.5f, 1.5f), 0.0f);
	scene.isScene = true;
	Object b2("./model/2B/2B.obj", glm::vec3(0.01f, 0.01f, 0.01f), 0.0f);
	b2.position = glm::vec3(95.0f, -0.8f, 150.0f);
	b2.theta = glm::radians(-70.0f);
	Object skybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	Object gun("./model/gun/gun.obj", glm::vec3(0.03f, 0.03f, 0.03f), 0.0f);
	Object powerCube("./model/powerCube/powerCube.obj", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	Object resonator("./model/resonator/resonator.obj", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	Object xmp("./model/xmp/xmp.obj", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	Object kulou("./model/Enemy/enemy.obj", glm::vec3(0.05f, 0.05f, 0.05f), 0.0f);
	kulou.position = glm::vec3(95.0f, 0.0f, 145.0f);
	ParticleGenerator part = ParticleGenerator(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 100);

	// Load textures which use in plane
	Texture2D texture("./image/powder.png", GL_TRUE);
	Texture2D UItexture("./image/Ingress_UI.png", GL_TRUE);
	Texture2D fireringtexture("./image/fire_ring.png", GL_TRUE);
	Texture2D gunfireTexture("./image/gun_fire.png", GL_TRUE);

	cout << "import completed" << endl;

	// Create depthMap FBO to render shadow, refer to https://learnopengl-cn.github.io/05%20Advanced%20Lighting/03%20Shadows/01%20Shadow%20Mapping/
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const GLuint SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render text, refer to https://learnopengl-cn.github.io/06%20In%20Practice/02%20Text%20Rendering/
	textShader.use();
	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	textShader.setMat4("projection", textProjection);

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// drawUI
	GLuint UIVBO, UIVAO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &UIVAO);
	glGenBuffers(1, &UIVBO);
	glBindVertexArray(UIVAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, UIVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), &particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	bool start = false;
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// Render Shadow Map
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.0f, far_plane = 600.0f;

		lightProjection = glm::ortho(-300.0f, +300.0f, -300.0f, +300.0f, near_plane, far_plane);
		lightView = glm::lookAt(player.Position + glm::vec3(-150.0f, 150.0f, -200.0f), player.Position, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (Portal portal : portalVec) {
			portal.draw(simpleDepthShader);
			int i = 0;
			for (Resonator item : portal.resonator) {
				if (item.health > 0)
					item.draw(simpleDepthShader);
			}
		}
		for (Enemy enemy : enemyVec) {
			if (enemy.health > 0.0f)
				enemy.draw(simpleDepthShader);
		}
		b2.draw(simpleDepthShader);
		scene.draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Render 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Render UI
			UIShader.use();
			UItexture.Bind();
			glBindVertexArray(UIVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			// Render Information
			textShader.use();
			string s(" "), res(" "), xmpp(" "), cubee(" ");
			stringstream ss, s1, s2, s3;
			ss << player.xm;
			ss >> s;
			s1 << player.resonatorNum;
			s1 >> res;
			s2 << player.xmpBursterNum;
			s2 >> xmpp;
			s3 << player.powerCubeNum;
			s3 >> cubee;
			RenderText(textShader, "XM : " + s + " Resonator: " + res + " Xmp: " + xmpp + " powerCube: " + cubee
				, 80.0f, 730.0f, 0.5f, glm::vec3(12.0 / 255, 79 / 255.0, 15 / 255.0));

			// if player is Chatting with 2B, then render these text
			if (isShowText) {
				if (glfwGetTime() - showText > 8.0f) {
					isShowText = false;
					if (player.isWin)
						break;
				}
				if (player.isWin == false) {
					RenderText(textShader, "2B: Go find the mechanical skull and take over their Portal"
						, 150.0f, 400.0f, 0.5f, glm::vec3(12.0 / 255, 79 / 255.0, 150 / 255.0));
					RenderText(textShader, "otherwise this community will be bombed soon!"
						, 150.0f, 350.0f, 0.5f, glm::vec3(12.0 / 255, 79 / 255.0, 150 / 255.0));
					start = true;
				}
				else {
					RenderText(textShader, "2B: You saved the neighborhood. See you soon!"
						, 150.0f, 400.0f, 0.5f, glm::vec3(12.0 / 255, 79 / 255.0, 150 / 255.0));
				}
			}

			glm::mat4 view = glm::mat4(glm::mat3(player.GetViewMatrix()));
			glm::mat4 projection = glm::mat4(1.0f);


			shadowShader.use();
			shadowShader.setVec3("viewPos", player.Position + glm::vec3(1.0f, 0.0f, 1.0f));

			// light properties
			shadowShader.setVec4("light.ambient", 0.4f, 0.4f, 0.4f, 1.0f);
			shadowShader.setVec4("light.diffuse", 0.8f, 0.8f, 0.8f, 1.0f);
			shadowShader.setVec4("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
			shadowShader.setVec3("light.direction", 3.0f, -1.0f, 4.0f);
			shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			// set Shadow Map
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			shadowShader.setInt("shadowMap", 10);

			view = glm::mat4(1.0f);
			projection = glm::mat4(1.0f);
			// basic setup
			projection = glm::perspective(glm::radians(player.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 300.0f);
			view = player.GetViewMatrix();
			shadowShader.setMat4("projection", projection);
			shadowShader.setMat4("view", view);

			colorShader.use();
			colorShader.setMat4("projection", projection);
			colorShader.setMat4("view", view);

			noShadow.use();
			noShadow.setVec3("viewPos", player.Position + glm::vec3(1.0f, 0.0f, 1.0f));
			// light properties
			noShadow.setVec4("light.ambient", 0.4f, 0.4f, 0.4f, 1.0f);
			noShadow.setVec4("light.diffuse", 0.8f, 0.8f, 0.8f, 1.0f);
			noShadow.setVec4("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
			noShadow.setVec3("light.direction", 3.0f, -1.0f, 4.0f);
			noShadow.setMat4("projection", projection);
			noShadow.setMat4("view", view);

			fireringShader.use();
			fireringShader.setMat4("projection", projection);
			fireringShader.setMat4("view", view);

			//reflection shader
			reflectionShader.use();
			reflectionShader.setMat4("projection", projection);
			reflectionShader.setMat4("view", view);
			reflectionShader.setVec3("cameraPos", player.Position);

			particleShader.use();
			particleShader.setMat4("projection", projection);
			particleShader.setMat4("view", view);
			glm::mat4 model = glm::mat4();
			particleShader.setMat4("model", model);

			float time = glfwGetTime();
			int count = 0;

			// Render every Portal
			for (Portal &portal : portalVec) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
				portal.draw(reflectionShader);
				int i = 0;

				// Render every Portal's Resonator 
				for (Resonator &item : portal.resonator) {
					if (item.health > 0) {
						if (time - item.putTime <= 1.0f)
							item.scale = glm::vec3(time - item.putTime);
						item.draw(noShadow);
						// Render every Resonator's Link particle
						particleShader.use();
						item.Particle.Update(0.1f, 2);
						item.Particle.Draw(particleShader, texture);
						count++;
					}
				}
			}

			// Victory conditions : bulid 16 resonator
			if (count == 16)
				player.isWin = true;
			if (player.xm <= 0) {
				cout << "YOU LOSE" << endl;
				Sleep(1000);
				break;
			}

			// the skeleton guide
			glm::vec3 objPos = kulou.position;
			if (start)
				kulou.position -= kulou.position / 500.0f;
			kulou.draw(noShadow);

			// Render the player's action animation
			if (player.getR) {
				resonator.position = player.HackPosition + glm::vec3(0.0f, (time - player.HackTime) * 3 + 1.0, 0.0f);
				resonator.scale = glm::vec3(1.0 + (time - player.HackTime));
				if (time - player.HackTime > 1.0f)
					player.getR = false;
				resonator.draw(noShadow);
			}
			if (player.getX) {
				float time = glfwGetTime();
				xmp.position = player.HackPosition + glm::vec3(0.0f, (time - player.HackTime) * 3 + 1.0, 0.0f);
				xmp.scale = glm::vec3(1.0 + (time - player.HackTime));
				if (time - player.HackTime > 1.0f)
					player.getX = false;
				xmp.draw(noShadow);
			}
			if (player.getC) {
				float time = glfwGetTime();
				powerCube.position = player.HackPosition + glm::vec3(0.0f, (time - player.HackTime) * 3 + 1.0, 0.0f);
				powerCube.scale = glm::vec3(1.0 + (time - player.HackTime));
				if (time - player.HackTime > 1.0f)
					player.getC = false;
				powerCube.draw(noShadow);
			}
			if (player.isShoot) {
				if (time - player.ShootTime > 0.05f)
					player.isShoot = false;

				fireringShader.use();
				model = glm::mat4(1.0f);
				fireringShader.setMat4("projection", glm::mat4(1.0f));
				fireringShader.setMat4("view", glm::mat4(1.0f));

				model = glm::translate(model, glm::vec3(0.0f, -0.05f, -0.8f));
				model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.2f, 0.4f, 0.2f));
				model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
				fireringShader.setMat4("model", model);
				gunfireTexture.Bind();
				glBindVertexArray(UIVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -0.05f, -0.8f));
				model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.2f, 0.4f, 0.2f));
				model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
				fireringShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);

			}
			if (player.isUsingXmp) {
				if (time - player.XmpTime > 1.0f)
					player.isUsingXmp = false;

				fireringShader.use();
				model = glm::mat4(1.0f);

				model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
				model = glm::translate(model, player.Position);
				model = glm::scale(model, glm::vec3(8.0f * (time - player.XmpTime))); // translate it down so it's at the center of the scene
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.8f));
				model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));

				fireringShader.setMat4("model", model);
				fireringtexture.Bind();

				glBindVertexArray(UIVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
			}

			// Render skeleton and its motion animation
			for (Enemy &enemy : enemyVec) {
				if (enemy.health > 0.0f)
					enemy.draw(noShadow);
				else if (glfwGetTime() - enemy.deadtime < 0.11f) {
					boomShader.use();
					boomShader.setMat4("projection", projection);
					boomShader.setMat4("view", view);
					// add time component to geometry shader in the form of a uniform
					boomShader.setFloat("time", glfwGetTime() - enemy.deadtime);
					enemy.draw(boomShader);
				}
				else if (glfwGetTime() - enemy.deadtime < 0.7f) {
					boomPointShader.use();
					boomPointShader.setMat4("projection", projection);
					boomPointShader.setMat4("view", view);
					// add time component to geometry shader in the form of a uniform
					boomPointShader.setFloat("time", glfwGetTime() - enemy.deadtime);
					enemy.draw(boomPointShader);
				}
				enemy.doSomething(player.Position);
				if (enemy.health > 0.0f) {
					if (enemy.isShoot) {
						fireringShader.use();
						model = glm::mat4(1.0f);

						model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
						model = glm::translate(model, enemy.position);
						model = glm::scale(model, glm::vec3(8.0f * (time - enemy.ShootTime))); // translate it down so it's at the center of the scene
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
						model = glm::scale(model, glm::vec3(0.8f));
						model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));

						fireringShader.setMat4("model", model);
						fireringtexture.Bind();

						glBindVertexArray(UIVAO);
						glDrawArrays(GL_TRIANGLES, 0, 6);
						glBindVertexArray(0);



						if (time - enemy.ShootTime < 0.1f)
							player.xm -= 100;
						if (time - enemy.ShootTime > 1.0f)
							enemy.isShoot = false;
					}
				}
			}

			b2.draw(shadowShader);
			scene.draw(shadowShader);
			gun.position = glm::vec3(0.0f, -0.33f, -1.2f);
			gun.theta = glm::radians(90.0f);
			gun.draw(noShadow);

			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			view = glm::mat4(glm::mat3(player.GetViewMatrix()));
			skyboxShader.setMat4("projection", projection);
			skyboxShader.setMat4("view", view);
			// Render skybox where no objects are rendered
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			skybox.draw(skyboxShader);
			glDepthFunc(GL_LESS);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	player.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	player.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		player.hack(portalVec);
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		player.UseResonator(portalVec);
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		player.UseXmpBurster(portalVec);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		player.UsePowerCube();
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		player.fly = !player.fly;
	// Chat with 2B
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		glm::vec3 b2position(95.0f, -0.03f, 150.0f);
		glm::vec3 ObjVec = b2position - player.Position;
		float distan = glm::sqrt(glm::dot(ObjVec, ObjVec));
		float theta = glm::dot(glm::normalize(ObjVec), glm::normalize(player.Front));
		if (distan < 20.0f && abs(theta) > 0.9f) {
			showText = glfwGetTime();
			isShowText = true;
		}
	}
	// to be a God
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		player.xmpBursterNum = 999;
		player.resonatorNum = 999;
		player.powerCubeNum = 999;
		player.xm = 9999;
	}
}

// Refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			player.shoot(enemyVec);
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		default:
			return;
		}
	return;
}

// Refer to https://learnopengl-cn.github.io/06%20In%20Practice/02%20Text%20Rendering/
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.use();
	shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
