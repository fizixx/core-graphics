
#include <iostream>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "GL/glew.h"
#include "SFML/OpenGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const char *kVertexShader =
    "#version 400\n"
    "in vec3 in_Position;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 modelMatrix;\n"
    "void main() {\n"
    "  gl_Position = projectionMatrix * viewMatrix * modelMatrix * "
    "vec4(in_Position, 1.0);\n"
    "}\n";

const char *kFragmentShader =
    "#version 400\n"
    "out vec4 frag_colour;\n"
    "void main () {\n"
    "  frag_colour = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";

int main() {
  sf::ContextSettings settings(0, 0, 0, 3, 0);
  sf::Window window(sf::VideoMode(800, 600), "Core Graphics",
                    sf::Style::Default, settings);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    return 1;
  }

  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  GLuint vertexShaderId = 0;
  GLuint fragmentShaderId = 0;
  GLuint programId = 0;

  // Load the vertex shader.
  vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderId, 1, &kVertexShader, NULL);
  glCompileShader(vertexShaderId);

  // Load the fragment shader.
  fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &kFragmentShader, NULL);
  glCompileShader(fragmentShaderId);

  // Create the program.
  programId = glCreateProgram();
  glAttachShader(programId, fragmentShaderId);
  glAttachShader(programId, vertexShaderId);
  glLinkProgram(programId);

  glUseProgram(programId);

  // Set up the matrices.

  glm::mat4 projectionMatrix;  // Store the projection matrix
  glm::mat4 viewMatrix;        // Store the view matrix
  glm::mat4 modelMatrix;       // Store the model matrix

  // Create our perspective projection matrix.
  projectionMatrix =
      glm::perspective(60.0f, (float)800 / (float)600, 0.1f, 100.f);

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

  float points[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};

  GLuint vboId = 0;
  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

  GLuint vaoId = 0;
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vaoId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  while (window.isOpen()) {
    sf::Event evt;
    if (window.pollEvent(evt)) {
      if (evt.type == sf::Event::Closed)
        window.close();
    }

    // Draw points 0-2 from the currently bound VAO with current in-use shader.
    glDrawArrays(GL_TRIANGLES, 0, 3);

    window.display();
  }

  return 0;
}
