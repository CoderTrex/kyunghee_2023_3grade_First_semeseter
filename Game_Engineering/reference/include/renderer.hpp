#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <vector>
#include <unistd.h>

// #include "../glad/glad.h" 
#include "../thirdparty/include/glad/glad.h" 
#include "glfw3.hpp"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <omp.h>

#include "common.hpp"
#include "timer.hpp"
#include "solver.hpp"
#include "OGL/shader.hpp"
#include "OGL/gl_object.hpp"


int default_src_width  = 3200;
int default_src_height = 2000;

float refresh_interval = (1/30);

// View parameters
float theta = 0.0;
float phi = 0.0;
float camradius = 6.0;
float cameraspeed = 0.5;

float camX = camradius;
float camY = 2.5;
float camZ = 1.5;

// Allow window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Keyboard input: JKIL for camera motion (also escape to close window)
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // J key will rotate left
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        theta -= cameraspeed;
        if (theta < 0.0) theta += 360.0;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // L key will rotate right
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        theta += cameraspeed;
        if (theta >= 360.0) theta -= 360.0;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // I key will rotate right
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (phi < 90.0-cameraspeed) phi += cameraspeed;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // K key will rotate right
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (phi > -90.0+cameraspeed) phi -= cameraspeed;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
}


class Renderer
{
private:
    GLFWwindow* window;

    GLuint shaderProgram;
    GLuint fragmentShader;
    GLuint vertexShader;

    GLuint modelLoc;
    GLuint viewLoc;
    GLuint projectionLoc;

    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;

    GLuint box_vao;
    GLuint box_vertex_buffer;

    GLuint particle_vao;
    GLuint particle_vertex_buffer[2];

    glm::mat4* modelMatrices;

    Timer timer;
    Solver sovler;

public:
    Renderer();
    ~Renderer();

    void initialize()
    {
        timer.reset(refresh_interval);
        modelMatrices =  new glm::mat4[k_num_particle];
      
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(default_src_width, default_src_height, k_project_name, NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
    
        // GLAD manages function pointers for OpenGL, so we want to initialize GLAD before we call any OpenGL function
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glViewport(0, 0, default_src_width, default_src_height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Enable depth buffering, backface culling
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glUseProgram(shaderProgram);

        // box
        glGenVertexArrays(1, &box_vao);
        glGenBuffers(1, &box_vertex_buffer);
        
        glBindVertexArray(box_vao);
            glBindBuffer(GL_ARRAY_BUFFER, box_vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLObj::box), GLObj::box, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);   
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        
        // particle
        glGenVertexArrays(1, &particle_vao);
        glGenBuffers(2, particle_vertex_buffer);

        glBindVertexArray(particle_vao);
            glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLObj::particle), GLObj::particle, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sovler.get_gl_particle_color().size() * sizeof(glm::vec3), glm::value_ptr(sovler.get_gl_particle_color()[0]), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(1);

        // Declare model/view/projection matrices
        model = glm::mat4(1.0f);
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        view = glm::mat4(1.0f);
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        projection = glm::mat4(1.0f);
        projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        // Set Projection matrix
        projection = glm::perspective(glm::radians(45.0f), 1.0f*default_src_width/default_src_height, 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void start_looping()
    {
        while(!glfwWindowShouldClose(window) && !timer.is_time_to_stop())
        {
            // processInput(window);
            glfwPollEvents();

            if (timer.is_time_to_draw()) 
            {
                timer.update_next_display_time();
                update_particle_position();
                draw();
            }
            sovler.compute_next_state();
            timer.update_simulation_time();
        }
        delete_GLBuffers();
        glfwTerminate();
    }

    void draw() 
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Set view matrix
        view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 2.0));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(box_vao);
            model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glBindVertexArray(particle_vao);
        #pragma parallel for 
        for(int i = 0; i < k_num_particle; i++)
        {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrices[i]));
            glDrawArrays(GL_TRIANGLES, 0, 24);
        }

        // glDisable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    void update_particle_position()
    {
        #pragma omp parallel for private(model)
        for (int i = 0; i < k_num_particle; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, sovler.get_gl_particle_position()[i]);
            modelMatrices[i] = model;
        }
    }

    void delete_GLBuffers()
    {
        glDeleteVertexArrays(1, &box_vao);
        glDeleteVertexArrays(1, &particle_vao);
        glDeleteBuffers(2, particle_vertex_buffer);
        glDeleteBuffers(1, &box_vertex_buffer);
        glDeleteProgram(shaderProgram);
    }

};

Renderer::Renderer() {};
Renderer::~Renderer() {};

#endif // RENDERER_H_