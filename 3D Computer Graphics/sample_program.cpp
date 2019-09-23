// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cstring>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include "shader.hpp"

// Function declarations
void Update();

GLint numLights = 2;
GLfloat theta = 0.0;
GLfloat light_position[] = {
        1.0, 10.0, 2.0,
        0.0, 0.0, 1.0,
};

GLfloat light_color[] = {
        0.81, 0.2, 0.5,
        0.25, 0.15, 0.39,
};

GLfloat vTransform[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
};

static const GLfloat g_vertex_buffer_data[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
};

static const GLfloat g_vertex_color_data[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
};

static const GLfloat g_vertex_normal_data[] = {
        -0.5f, -0.5f, 1.0f,
        0.5f, -0.5f, 1.0f,
        -0.5f, 0.5f, 1.0f,
        0.5f, -0.5f, 1.0f,
        0.5f, 0.5f, 1.0f,
        -0.5f, 0.5f, 1.0f,
};

static const GLushort g_vertex_indices[] = {
        0, 1, 2,
        1, 4, 2,
};




int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Sample 1.1 - Using GLSL Program Objects", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "../SimpleVertexShader.vertexshader", "../SimpleFragmentShader.fragmentshader" );

//    GLuint testHandle;
//    testHandle = glCreateShader( GL_VERTEX_SHADER );
//    glDeleteShader( testHandle );

    GLuint vertexbuffers[4];
	glGenBuffers(4, vertexbuffers);
	
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_color_data), g_vertex_color_data, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_normal_data), g_vertex_normal_data, GL_STATIC_DRAW);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_indices), g_vertex_indices, GL_STATIC_DRAW);
    
	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
        glUseProgram(programID);
        
		// 1st attribute buffer : vertex positions (model space)
		glEnableVertexAttribArray(0);
        
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
        glVertexAttribPointer(
            0,                  // attribute 0. MUST match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        // 2nd attribute buffer : vertex color
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
        glVertexAttribPointer(
            1,                  // attribute 1. MUST match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        // 3rd attribute buffer : vertex normals (model space)
        glEnableVertexAttribArray(2);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
        glVertexAttribPointer(
                              2,                  // attribute 0. MUST match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_TRUE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );


        // Uniform block (fragment shader)
        GLuint uboIndex;
        GLint  uboSize;
        GLubyte *buffer;
        
        GLuint indices[2];
        GLint offset[2];
        const GLchar *names[] = {
            "LightArray.LightPosition",
            "LightArray.LightColor"
        };
        
        // Step 1. Get the block index
        uboIndex = glGetUniformBlockIndex( programID, "LightArray" );
        if( uboIndex != GL_INVALID_INDEX )
        {
        
            // Step 2. Get the block size
            glGetActiveUniformBlockiv( programID,
                                  uboIndex,
                                  GL_UNIFORM_BLOCK_DATA_SIZE,
                                  &uboSize );
        
//            std::cout << "Uniform block ID: " << uboIndex << std::endl;
//            std::cout << "Uniform block size: " << uboSize << std::endl;
            
            // Step 3. Allocate the block
            buffer = (GLubyte *)malloc( uboSize );
            
            // Step 4. Get the block indices
            // Names must be qualified by block name (Outside GLSL)
            glGetUniformIndices(programID, 2, names, indices);
            
//            std::cout << "Indices : " << names[0] << ", " << indices[0] << std::endl;
            
//            std::cout << "Indices : " << names[1] << ", " << indices[1] << std::endl;
            
            
            glGetActiveUniformsiv(programID, 2, indices,
                                  GL_UNIFORM_OFFSET, offset);

//            std::cout << "Offset : " << names[0] << ", " << offset[0] << std::endl;
            
//            std::cout << "Offset : " << names[1] << ", " << offset[1] << std::endl;
            
            // Step 5. Copy the data over
            std::memcpy(buffer + offset[0], light_position, sizeof(light_position) );
            
            std::memcpy(buffer + offset[1], light_color, sizeof(light_color) );

            // Step 6. Create OpenGL Buffer object and bind data into it
            GLuint uboHandle;
            glGenBuffers( 1, &uboHandle );
            glBindBuffer( GL_UNIFORM_BUFFER, uboHandle );
            glBufferData( GL_UNIFORM_BUFFER, uboSize, buffer,
                         GL_DYNAMIC_DRAW );
            
            // Step 7. Bind the base of the buffer
            glBindBufferBase( GL_UNIFORM_BUFFER, uboIndex, uboHandle );
        
        }
        
        // Uniform transformation (vertex shader)
        GLint vTransformLoc = glGetUniformLocation( programID, "vertexTransform" );
        
		// Draw the triangle !
		// Triangle List mode
        vTransform[12] = 0.0f;
        vTransform[13] = 0.0f;
        glUniformMatrix4fv( vTransformLoc, 1, GL_FALSE, vTransform );

        
        glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
        
        // Now shift the elements by (-1, 0, 0 )
        // Use the uniform matrix to change the location
        vTransform[12] = -1.0f;
        vTransform[13] = 0.0f;
        glUniformMatrix4fv( vTransformLoc, 1, GL_FALSE, vTransform );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        
        // Now shift the elements by (-1, -1, 0 )
        // Use the uniform matrix to change the location
        vTransform[12] = -1.0f;
        vTransform[13] = -1.0f;
        glUniformMatrix4fv( vTransformLoc, 1, GL_FALSE, vTransform );
        glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, 0);
        
        
        // Now shift the elements by (0, -1, 0 )
        // Use the uniform matrix to change the location
        vTransform[12] = 0.0f;
        vTransform[13] = -1.0f;
        glUniformMatrix4fv( vTransformLoc, 1, GL_FALSE, vTransform );
        glDrawRangeElementsBaseVertex(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, 0, 3);
        
		glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        // Update the light Positions
        Update();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(4, vertexbuffers);
	glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void Update()
{
    // Light 0
    GLfloat radius = (float) rand() / 100.00 * 2.0;

    GLfloat theta_R = theta / M_1_PI;
    light_position[0] = radius * cosf(theta_R);
    light_position[2] = radius * sinf(theta_R);

    light_position[4] = radius * cosf(theta_R * 0.5);
    light_position[5] = radius * sinf(theta_R * 0.5);

    theta += 0.005;

    if( theta > 360.0 )
        theta = 0.0;

    return;
}
