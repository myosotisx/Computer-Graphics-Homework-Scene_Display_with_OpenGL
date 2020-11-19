#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "camera.h"
#include "object.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCREEN_WIDTH/2.0f;
float lastY = SCREEN_HEIGHT/2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


GLFWwindow* init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scene", NULL, NULL);
    if(window == NULL) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return nullptr;
    }
    glEnable(GL_DEPTH_TEST);
    return window;
}

void verticesSetup(int verticesNum, unsigned int VAO[], unsigned int VBO[], int bufLength, int attribNum, int lengths[], int sizes[], float* verticesArray[]) {
    glGenVertexArrays(verticesNum, VAO);
    glGenBuffers(verticesNum, VBO);

    for(int i=0; i<verticesNum; i++) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizes[i], verticesArray[i], GL_STATIC_DRAW);
        int offset = 0;
        for(int j=0; j<attribNum; j++) {
            glVertexAttribPointer(j, lengths[j], GL_FLOAT, GL_FALSE, bufLength*sizeof(float), (void*)(offset*sizeof(float)));
            glEnableVertexAttribArray(j);
            offset += lengths[j];
        }
    }
}

bool loadTexture(unsigned int& texture, const char* path) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture!" << std::endl;
        return false;
    }
    stbi_image_free(data);
    return true;
}


int main() {
    GLFWwindow *window = init();
    if(!window) {
        glfwTerminate();
        return -1;
    }
	Shader shader("vshader.vs", "fshader.fs");

    // vertices
    float verticesPlane[] ={
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    };
    float verticesCube[] = {
        // front
        0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
        1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
        0.0f, 1.0f, 1.0f,  0.0f,  1.0f,
        0.0f, 0.0f, 1.0f,  0.0f,  0.0f,

        // back
        1.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        0.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        1.0f, 0.0f, 0.0f,  0.0f,  0.0f,

        // left
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 1.0f,  1.0f,  1.0f,
        0.0f, 1.0f, 1.0f,  1.0f,  1.0f,
        0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
         
        // right
        1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f, 1.0f, 1.0f,  0.0f,  1.0f,
        1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

        // top
        0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        1.0f, 1.0f, 1.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        0.0f, 1.0f, 1.0f,  0.0f,  0.0f,

        // down
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
        0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    };
    float verticesPyramid[] = {
        // front
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 1.0f, 0.5f,  0.5f, 1.0f,

        // back
        1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 1.0f, 0.5f,  0.5f, 1.0f,

        // left
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 1.0f, 0.5f,  0.5f, 1.0f,

        // right
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 1.0f, 0.5f,  0.5f, 1.0f,

        // down
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
        0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    };
    float verticesDiamond[] = {
        // front up
        0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 2.0f, 0.5f,  0.5f, 1.0f,

        // back up
        1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 2.0f, 0.5f,  0.5f, 1.0f,

        // left up
        0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 2.0f, 0.5f,  0.5f, 1.0f,

        // right up
        1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 2.0f, 0.5f,  0.5f, 1.0f,

        // front down
        1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,  0.5f, 1.0f,

        // back down
        0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,  0.5f, 1.0f,

        // left down
        0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,  0.5f, 1.0f,

        // right down
        1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,  0.5f, 1.0f,
    };
    float* verticesArray[] ={verticesCube, verticesPyramid, verticesDiamond, verticesPlane};
    int sizes[] ={sizeof(verticesCube), sizeof(verticesPyramid), sizeof(verticesDiamond), sizeof(verticesPlane)};
    int lengths[] ={3, 2};
    unsigned int VAO[4], VBO[4];
    verticesSetup(4, VAO, VBO, 5, 2, lengths, sizes, verticesArray);

    // object
    Cube cube(2.0f, (float)glfwGetTime(), 36, 5, verticesCube, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    Pyramid pyramid(18, 5, verticesPyramid, glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.5f, 1.0f));
    Diamond diamond(24, 5, verticesDiamond, glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    Plane front(FRONT_FACE, 6, 5, verticesPlane, glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 5.0f, 1.0f));
    Plane back(BACK_FACE, 6, 5, verticesPlane, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 5.0f, 1.0f));
    Plane left(LEFT_FACE, 6, 5, verticesPlane, glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 5.0f, 1.0f));
    Plane right(RIGHT_FACE, 6, 5, verticesPlane, glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 5.0f, 1.0f));
    Plane top(TOP_FACE, 6, 5, verticesPlane, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 1.0f));
    Plane down(DOWN_FACE, 6, 5, verticesPlane, glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 1.0f));

    // texture
    unsigned int background, texture1, texture2, texture3;
    if(!loadTexture(background, "texture/background.jpg")
        || !loadTexture(texture1, "texture/marble.jpg")
        || !loadTexture(texture2, "texture/interesting.jpg")
        || !loadTexture(texture3, "texture/rgb.jpg")) {
        glfwTerminate();
        return -1;
    }

    while(!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture3);

        shader.use();

        // draw background
        shader.setInt("texture1", 0);
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(VAO[3]);
        shader.setMat4("projection", glm::mat4(1.0f));
        shader.setMat4("view", glm::mat4(1.0f));
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);

        // draw objects
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // draw planes
        shader.setInt("texture1", 1);

        glBindVertexArray(VAO[3]);
        glm::mat4 model = front.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        model = back.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        model = left.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        model = right.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        model = top.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        model = down.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // draw pyramid
        shader.setInt("texture1", 3);

        glBindVertexArray(VAO[1]);
        model = pyramid.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // draw diamond
        shader.setInt("texture1", 3);

        glBindVertexArray(VAO[2]);
        model = diamond.model(0.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 24);

        // draw cube
        shader.setInt("texture1", 3);

        glBindVertexArray(VAO[0]);
        model = cube.model(2.0f*currentFrame);

        if(cube.collide(diamond) || cube.collide(pyramid)) {
            shader.setInt("texture1", 2);
        }

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    } 
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }    
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos-lastX;
    float yoffset = lastY-ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
