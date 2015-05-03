
#include <GL/glew.h>  // Always first

#include <nucleus/logging.h>
#include <nucleus/macros.h>
#include <SFML/Graphics/Image.hpp>
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
  std::string root{"C:\\Workspace\\core-graphics\\resources\\"};

  Shader vertexShader{Shader::Vertex};
  if (!vertexShader.loadFromFile(root + "shaders\\default.vert")) {
    LOG(Error) << "Could not load vertex shader.";
    return EXIT_FAILURE;
  }

  Shader fragmentShader{Shader::Fragment};
  if (!fragmentShader.loadFromFile(root + "shaders\\default.frag")) {
    LOG(Error) << "Could not load fragment shader.";
    return EXIT_FAILURE;
  }

  GLuint shaderProgram = glCreateProgram();
  GL_CHECK(glAttachShader(shaderProgram, vertexShader.getNativeHandle()));
  GL_CHECK(glAttachShader(shaderProgram, fragmentShader.getNativeHandle()));
  GL_CHECK(glLinkProgram(shaderProgram));

  // --------------------------------------------------------------------------

  sf::Image image;
  if (!image.loadFromFile(root + "block_test.png")) {
    LOG(Error) << "Could not load texture. (" << root << "block_test.png)";
    return EXIT_FAILURE;
  }

  GLuint texture = 0;
  GL_CHECK(glGenTextures(1, &texture));
  GL_CHECK(glActiveTexture(GL_TEXTURE0));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getSize().x,
                        image.getSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE,
                        image.getPixelsPtr()));

  // Nice trilinear filtering.
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           GL_LINEAR_MIPMAP_LINEAR));

  // --------------------------------------------------------------------------

  // Create the vertex buffer.
  // clang-format off
  GLubyte indices[] = {
    0, 1, 2
  };
  struct Vertex {
    GLfloat x, y;
    GLfloat r, g, b;
    GLfloat u, v;
  } vertices[] = {
    { 0.0f,  0.8f, 1.0f, 0.0f, 0.0f, 0.5f, 0.0f},
    {-0.8f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    { 0.8f, -0.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
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
                                   sizeof(GLfloat) * 7, NULL));
    GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                   sizeof(GLfloat) * 7,
                                   (void*)(sizeof(GLfloat) * 2)));
    GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                                   sizeof(GLfloat) * 7,
                                   (void*)(sizeof(GLfloat) * 5)));
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

    // Set the texture in the program.
    GLuint location = glGetUniformLocation(shaderProgram, "unif_texture");
    GL_CHECK(glUniform1i(location, texture));

    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glBindVertexArray(vertexAttributeObject));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
    GL_CHECK(glDisableVertexAttribArray(0));
    GL_CHECK(glDisableVertexAttribArray(1));
    GL_CHECK(glDisableVertexAttribArray(2));

    window.display();
  }

  GL_CHECK(glDeleteVertexArrays(1, &vertexAttributeObject));
  GL_CHECK(glDeleteProgram(shaderProgram));

  return EXIT_SUCCESS;
}
