
#include <iostream>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "GL/glew.h"
#include "SFML/OpenGL.hpp"

const char *kVertexShader = "#version 400\n"
                            "in vec3 vp;\n"
                            "void main() {\n"
                            "  gl_Position = vec4(vp, 1.0);\n"
                            "}\n";

const char *kFragmentShader = "#version 400\n"
                              "out vec4 frag_colour;\n"
                              "void main () {\n"
                              "  frag_colour = vec4(1.0, 1.0, 1.0, 1.0);\n"
                              "}\n";

int main() {
  sf::Window window(sf::VideoMode(800, 600), "Core Graphics");

  glewInit();

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
