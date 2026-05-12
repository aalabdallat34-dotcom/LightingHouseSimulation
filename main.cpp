//libraries and headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "include/Shader.h"
#include <iostream>
#include <vector>
//window dimensions
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
//time variable for animation wave movement
float timeValue = 0.0f;
//callback function to adjust viewport when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);
}
//main function
int main()
{ 
    //initialize GLFW
    glfwInit();
    //set OpenGL version to 3.3 and use core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //create windowwith specified dimensions and title
    GLFWwindow* window =
        glfwCreateWindow(
            SCR_WIDTH,
            SCR_HEIGHT,
            "Water Simulation",
            NULL,
            NULL
        );
    //check if window creation was successful
    glfwMakeContextCurrent(window);
    //load OpenGL function pointers using GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    //set callback function for window resize events
    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );
    //enable depth testing for correct rendering of 3D objects
    glEnable(GL_DEPTH_TEST);
    //create shader program from vertex and fragment shader files
    Shader shader(
        "shaders/water.vert",
        "shaders/water.frag"
    );
    //generate vertices and indices for a grid of quads 
    //to represent the water surface
    const int SIZE = 500;
    const float SCALE = 0.03f;
    //store vertex positions and indices in vectors
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    //generate vertices for a grid of quads centered at the origin
    for(int z=0; z<=SIZE; z++)
    {
        for(int x=0; x<=SIZE; x++)
        {
            float xpos =
                (x - SIZE/2) * SCALE;

            float zpos =
                (z - SIZE/2) * SCALE;

            vertices.push_back(xpos);
            vertices.push_back(0.0f);
            vertices.push_back(zpos);
        }
    }
    //generate indices for the grid of quads to create triangles for rendering
    for(int z=0; z<SIZE; z++)
    {
        for(int x=0; x<SIZE; x++)
        {
            int start =
                z * (SIZE + 1) + x;

            indices.push_back(start);
            indices.push_back(start + SIZE + 1);
            indices.push_back(start + 1);

            indices.push_back(start + 1);
            indices.push_back(start + SIZE + 1);
            indices.push_back(start + SIZE + 2);
        }
    }
    //create vertex array object (VAO), vertex buffer object (VBO), 
    //and element buffer object (EBO) for rendering the water surface
    unsigned int VAO,VBO,EBO;
    //generate and bind VAO, VBO, and EBO for the water surface
    glGenVertexArrays(1,&VAO);
    //generate VBO and EBO for storing vertex data and index data
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    //bind VAO to store vertex attribute configuration
    glBindVertexArray(VAO);
    //bind VBO to store vertex data for the water surface
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    //upload vertex data to the VBO for rendering the water surface
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );
    //bind EBO to store index data for the water surface
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    //upload index data to the EBO for rendering the water surface
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );
   //set vertex attribute pointer for position data
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3*sizeof(float),
        (void*)0
    );
    //enable vertex attribute array for position data
    glEnableVertexAttribArray(0);
    //load water texture using stb_image library and create OpenGL texture object
    unsigned int waterTexture;

    glGenTextures(1,&waterTexture);

    glBindTexture(GL_TEXTURE_2D,waterTexture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    int width,height,nrChannels;
    //load texture data from file and create OpenGL texture for the water surface
    unsigned char* data =
        stbi_load(
            "textures/seaa.jpeg",//path to water texture image file or change to "textures/sea.jpeg"
            &width,
            &height,
            &nrChannels,
            0
        );
        //check if texture data was loaded successfully and create OpenGL texture object
    if(data)
    {
        GLenum format = GL_RGB;

        if(nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    //free memory used for texture data after creating OpenGL texture object
    stbi_image_free(data);

    glm::mat4 model = glm::mat4(1.0f);
    //set up view and projection matrices for rendering the water surface
    glm::mat4 view =
        glm::lookAt(
            glm::vec3(0.0f,2.2f,6.5f),
            glm::vec3(0.0f,0.2f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f)
        );
        //set up perspective projection matrix for rendering the water surface
    glm::mat4 projection =
        glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT,
            0.1f,
            100.0f
        );
        //main render loop to draw the water surface and handle user input/events
    while(!glfwWindowShouldClose(window))
    {
        timeValue = glfwGetTime();

        glClearColor(
            0.45f,
            0.75f,
            0.95f,
            1.0f
        );
        //clear color and depth buffers to prepare for rendering the next frame
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        shader.use();
        //set uniform variables for time, model, view, and projection matrices in the shader program
        glUniform1f(
            glGetUniformLocation(shader.ID,"time"),
            timeValue
        );
        //set uniform variables for model, view, and projection matrices in the shader program
        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID,"model"),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );
        //set uniform variables for view and projection matrices in the shader program
        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID,"view"),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );
        //set uniform variable for projection matrix in the shader program
        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID,"projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );
        //set uniform variable for the water texture sampler in the shader program
        glActiveTexture(GL_TEXTURE0);
        //bind the water texture to the active texture unit for use in the shader program
        glBindTexture(GL_TEXTURE_2D,waterTexture);
        //set uniform variable for the water texture sampler in the shader program to use texture unit 0
        glUniform1i(
            glGetUniformLocation(shader.ID,"waterTex"),
            0
        );
        //bind the VAO for the water surface and draw the elements using the index data
        glBindVertexArray(VAO);
        // draw the water surface using the index data to create triangles for rendering
        glDrawElements(
            GL_TRIANGLES,
            indices.size(),
            GL_UNSIGNED_INT,
            0
        );

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}