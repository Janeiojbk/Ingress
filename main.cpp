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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

unsigned int loadCubemap(vector<std::string> faces);

// camera
Player player;

std::vector<Portal> portalVec;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ingress", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	/*
	vector<std::string> faces
	{
		"./image/right.jpg",
		"./image/left.jpg",
		"./image/top.jpg",
		"./image/bottom.jpg",
		"./image/front.jpg",
		"./image/back.jpg"
	};
	*/
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

	Shader ourShader("vertex_shader.vert", "fragment_shader.frag");
	Shader colorShader("OnlyColor.vert", "OnlyColor.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader simpleDepthShader("simpleDepthShader.vert", "simpleDepthShader.frag");
	glm::vec3 portalPos(0.0f, 0.0f, 0.0f);
	Portal portal(portalPos);
	portalVec.push_back(portal);
	Enemy enemy;
	Object scene("./wheelByYang/scene/scene.obj", glm::vec3(1.5f, 1.5f, 1.5f));
	Object skybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	cout << "import completed" << endl;
	
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

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

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// 1. Render depth of scene to texture (from ligth's perspective)
		// - Get light projection/view matrix.
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.0f, far_plane = 1000.0f;
		lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
		lightView = glm::lookAt(glm::vec3(-300.0f, 300.0f, -400.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// - now render scene from light's point of view
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
		enemy.draw(simpleDepthShader);
		scene.draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// 2. Render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		// render
		// ------
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDepthMask(GL_FALSE);
			skyboxShader.use();
			// ... 设置观察和投影矩阵
			glm::mat4 view = glm::mat4(glm::mat3(player.GetViewMatrix()));
			glm::mat4 projection = glm::mat4(1.0f);
			// basic setup
			projection = glm::perspective(glm::radians(player.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
			skyboxShader.setMat4("projection", projection);
			skyboxShader.setMat4("view", view);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			skybox.draw(skyboxShader);
			glDepthMask(GL_TRUE);

			// active shader
			ourShader.use();
			ourShader.setVec3("viewPos", player.Position);

			// light properties
			ourShader.setVec4("light.ambient", 0.4f, 0.4f, 0.4f, 1.0f);
			ourShader.setVec4("light.diffuse", 0.8f, 0.8f, 0.8f, 1.0f);
			ourShader.setVec4("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
			ourShader.setVec3("light.direction", 3.0f, -1.0f, 4.0f);
			ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			ourShader.setInt("shadowMap", 10);

			view = glm::mat4(1.0f);
			projection = glm::mat4(1.0f);
			// basic setup
			projection = glm::perspective(glm::radians(player.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 1000.0f);
			view = player.GetViewMatrix();
			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);

			colorShader.use();
			colorShader.setMat4("projection", projection);
			colorShader.setMat4("view", view);
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			ourShader.setVec4("ourColor", color);

			for (Portal portal : portalVec) {
				portal.draw(colorShader);
				ourShader.use();
				int i = 0;
				for (Resonator item : portal.resonator) {
					if (item.health > 0)
						item.draw(ourShader);
				}
			}
			enemy.draw(ourShader);
			scene.draw(ourShader);
		}
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
// ----------------------------------------------------------------------
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
}

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