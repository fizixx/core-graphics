
#include <iostream>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "GL/glew.h"
#include "SFML/OpenGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum {
  DISPLAY_WIDTH = 800,
  DISPLAY_HEIGHT = 600,
};

const char *kVertexShader =
    "#version 400\n"
    "\n"
    "layout(location = 0) in vec3 in_Position;\n"
    "layout(location = 1) in vec3 in_Color;\n"
    "\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 modelMatrix;\n"
    "\n"
    "out vec4 VertexColor;\n"
    "\n"
    "void main() {\n"
    "  VertexColor = vec4(in_Color, 1.0);\n"
    "  gl_Position = \n"
    "      projectionMatrix * \n"
    "      viewMatrix * \n"
    "      modelMatrix * \n"
    "      vec4(in_Position, 1.0);\n"
    "}\n";

const char *kFragmentShader =
    "#version 400\n"
    "in vec4 VertexColor;\n"
    "out vec4 frag_colour;\n"
    "void main () {\n"
    "  frag_colour = VertexColor;\n"
    "}\n";

void Log(std::string msg) {
  OutputDebugStringA(msg.c_str());
  OutputDebugStringA("\n");
}

GLuint CreateShader(GLenum type, std::string source) {
  GLuint id = glCreateShader(type);

  const GLchar* s = static_cast<const GLchar*>(source.c_str());
  GLint length = static_cast<GLint>(source.length());
  glShaderSource(id, 1, &s, &length);

  return id;
}

GLuint CreateProgram(std::string vertexShaderSource,
                   std::string fragmentShaderSource) {
  GLuint vertexShaderId =
      CreateShader(GL_VERTEX_SHADER, std::move(vertexShaderSource));

  GLuint fragmentShaderId =
      CreateShader(GL_FRAGMENT_SHADER, std::move(fragmentShaderSource));

  // Create and compile the program.
  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);
  glLinkProgram(programId);

  // Print out some information about the program.
  std::string buffer(1024, 0);
  GLsizei outLength = 0;
  glGetProgramInfoLog(programId, static_cast<GLsizei>(buffer.size()),
                      &outLength, (GLchar *)buffer.c_str());
  buffer[outLength] = 0;

  if (!buffer.empty())
    Log(std::move(buffer));

  return programId;
}

int main(int argc, char *argv[]) {
  sf::ContextSettings settings(0, 0, 0, 4, 2);
  sf::Window window(sf::VideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT),
                    "Core Graphics", sf::Style::Default, settings);

  Log(std::move(std::string("OpenGL: ").append(
      reinterpret_cast<const char *>(glGetString(GL_VERSION)))));

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    return 1;
  }

  Log(std::move(std::string("GLEW: ").append(
      reinterpret_cast<const char *>(glewGetString(GLEW_VERSION)))));

  // Create the program we're going to use for rendering.
  GLuint programId = CreateProgram(kVertexShader, kFragmentShader);
  if (!programId) {
    Log("Could not create program.");
    return 1;
  }

  glUseProgram(programId);

  // Set up the matrices.

  glm::mat4 projectionMatrix;  // Store the projection matrix
  glm::mat4 viewMatrix;        // Store the view matrix
  glm::mat4 modelMatrix;       // Store the model matrix

  // Create our perspective projection matrix.
  projectionMatrix = glm::perspective(
      60.0f,
      static_cast<float>(DISPLAY_WIDTH) / static_cast<float>(DISPLAY_HEIGHT),
      0.1f, 100.f);

  // Create our view matrix which will translate us back 5 units.
  viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f));
  // Create our model matrix which will halve the size of our model.
  modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

  // Bind the values to the program.

  // Get the location of our projection matrix in the shader.
  int projectionMatrixLocation =
      glGetUniformLocation(programId, "projectionMatrix");
  // Get the location of our view matrix in the shader.
  int viewMatrixLocation = glGetUniformLocation(programId, "viewMatrix");
  // Get the location of our model matrix in the shader.
  int modelMatrixLocation = glGetUniformLocation(programId, "modelMatrix");

  // Send our projection matrix to the shader.
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE,
                     &projectionMatrix[0][0]);
  // Send our view matrix to the shader
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
  // Send our model matrix to the shader
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

  // Set up the geometry.

  GLfloat points[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};
  GLubyte colors[] = {
      255, 0, 0, 255, 0, 0, 255, 0, 0,
  };

  // Create the vertex array.
  GLuint vaoId = 0;
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vboId[2];
  glGenBuffers(2, vboId);

  // Create array buffer and bind it to attribute 0 on the vertex array.
  glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  // Create the color array.
  glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, NULL);

  // Delete the vertex buffers. (The vertex array still has reference to it)
  // glDeleteBuffers(2, vboId);

  while (window.isOpen()) {
    sf::Event evt;
    if (window.pollEvent(evt)) {
      if (evt.type == sf::Event::Closed)
        window.close();
    }

    // Draw points 0-2 from the currently bound VAO with current in-use shader.
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    window.display();
  }

  // Delete the vertex array object.
  glDeleteVertexArrays(1, &vaoId);

  // Delete the program.
  glDeleteProgram(programId);

  return 0;
}
