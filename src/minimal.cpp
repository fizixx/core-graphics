
#include <GL/glew.h>  // Always first

#include <nucleus/logging.h>
#include <nucleus/macros.h>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>

#include "shader.h"
#include "utils.h"
#include "vertex_buffer_object.h"

int main(int argc, char* argv[]) {
  // Create the window.
  sf::ContextSettings settings;
  settings.majorVersion = 4;
  settings.minorVersion = 2;
  settings.attributeFlags = sf::ContextSettings::Core;
  sf::Window window(sf::VideoMode{1024, 768}, "Minimal", sf::Style::Default,
                    settings);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    LOG(Error) << "Could not initialize GLEW.";
    return EXIT_FAILURE;
  }

  // Clear GL errors after initializing GLEW.
  glGetError();

  // --------------------------------------------------------------------------

  // Create the shaders and program.
  std::string root{"C:\\Workspace\\core-graphics\\resources\\shaders\\"};

  Shader vertexShader{Shader::Vertex};
  if (!vertexShader.loadFromFile(root + "default.vert")) {
    LOG(Error) << "Could not load vertex shader.";
    return EXIT_FAILURE;
  }

  Shader fragmentShader{Shader::Fragment};
  if (!fragmentShader.loadFromFile(root + "default.frag")) {
    LOG(Error) << "Could not load fragment shader.";
    return EXIT_FAILURE;
  }

  GLuint shaderProgram = glCreateProgram();
  GL_CHECK(glAttachShader(shaderProgram, vertexShader.getNativeHandle()));
  GL_CHECK(glAttachShader(shaderProgram, fragmentShader.getNativeHandle()));
  GL_CHECK(glLinkProgram(shaderProgram));

  // --------------------------------------------------------------------------

  // Create the vertex buffer.
  // clang-format off
  GLubyte indices[] = {
    0, 1, 2
  };
  struct Vertex {
    GLfloat x, y;
    GLfloat r, g, b;
  } vertices[] = {
    { 0.0f,  0.8f, 1.0f, 0.0f, 0.0f},
    {-0.8f, -0.8f, 0.0f, 1.0f, 0.0f},
    { 0.8f, -0.8f, 0.0f, 0.0f, 1.0f},
  };
  // clang-format on

  // Create the Vertex Buffer Object (VBO).
  VertexBufferObject vbo;
  if (!vbo.setData(vertices, sizeof(vertices))) {
    return EXIT_FAILURE;
  }

  // Create the Vertex Attribute Object (VAO).
  GLuint vertexAttributeObject = 0;
  GL_CHECK(glGenVertexArrays(1, &vertexAttributeObject));
  GL_CHECK(glBindVertexArray(vertexAttributeObject));
  {
    VertexBufferObject::ScopedBind binder(vbo);
    GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                   sizeof(GLfloat) * 5, NULL));
    GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                   sizeof(GLfloat) * 5,
                                   (void*)(sizeof(GLfloat) * 2)));
  }

  while (window.isOpen()) {
    sf::Event evt;
    if (window.pollEvent(evt)) {
      if (evt.type == sf::Event::Closed) {
        window.close();
        break;
      } else if (evt.type == sf::Event::Resized) {
        GL_CHECK(glViewport(0, 0, evt.size.width, evt.size.height));
      }
    }

    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    GL_CHECK(glUseProgram(shaderProgram));

    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glBindVertexArray(vertexAttributeObject));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
    GL_CHECK(glDisableVertexAttribArray(0));
    GL_CHECK(glDisableVertexAttribArray(1));

    window.display();
  }

  return EXIT_SUCCESS;
}
