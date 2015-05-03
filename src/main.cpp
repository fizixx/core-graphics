
#include <iostream>

// These includes have to be in this order.
#include <GL/glew.h>
#include <windows.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "SFML/Graphics/Texture.hpp"

enum {
  DISPLAY_WIDTH = 800,
  DISPLAY_HEIGHT = 600,
};

const char *kVertexShader =
    "#version 400\n"
    "\n"
    "layout(location = 0) in vec2 in_Position;\n"
    //"layout(location = 1) in vec2 in_TexCoord;\n"
    "\n"
    //"uniform mat4 projectionMatrix;\n"
    //"uniform mat4 modelMatrix;\n"
    "\n"
    //"out vec4 VertexColor;\n"
    //"out vec2 TexCoord;\n"
    "\n"
    "void main() {\n"
    //"  TexCoord = in_TexCoord;\n"
    //"  gl_Position = \n"
    //"      projectionMatrix * modelMatrix * \n"
    //"      vec4(in_Position, 0.0, 1.0);\n"
    "  gl_Position = vec4(in_Position, 0.0, 1.0);\n"
    "}\n";

const char *kFragmentShader =
    "#version 400\n"
    "\n"
    //"in vec2 TexCoord;\n"
    "\n"
    //"uniform sampler2D Sampler;\n"
    "\n"
    "out vec4 FragmentColor;\n"
    "\n"
    "void main () {\n"
    //"  FragmentColor = texture(Sampler, TexCoord);\n"
    "  FragmentColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

void CheckError() {
  int error = glGetError();
  if (error != GL_NO_ERROR)
    __debugbreak();
}

void Log(std::string msg) {
  OutputDebugStringA(msg.c_str());
  OutputDebugStringA("\n");
}

GLuint CreateShader(GLenum type, std::string source) {
  GLuint id = glCreateShader(type);
  CheckError();

  const GLchar *s = static_cast<const GLchar *>(source.c_str());
  GLint length = static_cast<GLint>(source.length());
  glShaderSource(id, 1, &s, &length);
  CheckError();

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
  CheckError();

  glAttachShader(programId, vertexShaderId);
  CheckError();

  glAttachShader(programId, fragmentShaderId);
  CheckError();

  glLinkProgram(programId);
  CheckError();

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
  sf::ContextSettings settings(0, 0, 0, 4, 2, sf::ContextSettings::Default);
  sf::Window window(sf::VideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT),
                    "Core Graphics", sf::Style::Titlebar | sf::Style::Close,
                    settings);
  window.setVerticalSyncEnabled(true);
  CheckError();

  Log(std::move(std::string("OpenGL: ").append(
      reinterpret_cast<const char *>(glGetString(GL_VERSION)))));

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    return 1;
  }
  glGetError();

  Log(std::move(std::string("GLEW: ").append(
      reinterpret_cast<const char *>(glewGetString(GLEW_VERSION)))));

  CheckError();
  // Create the program we're going to use for rendering.
  GLuint programId = CreateProgram(kVertexShader, kFragmentShader);
  if (!programId) {
    Log("Could not create program.");
    return 1;
  }

  glUseProgram(programId);
  CheckError();

  // Load the texture.
  sf::Texture texture;
  std::string texturePath(__FILE__);
  texturePath = texturePath.substr(0, texturePath.find_last_of('\\'));
  texturePath = texturePath.substr(0, texturePath.find_last_of('\\'));
  texturePath.append("\\resources\\block_test.png");
  Log(texturePath);

  if (!texture.loadFromFile(texturePath)) {
    Log("Error loading texture. (" + texturePath + ")");
    return 1;
  }

  sf::Texture::bind(&texture);
  CheckError();

#if 0
  // Set up the matrices.

  // Create our perspective projection matrix.
  glm::mat4 projectionMatrix =
      glm::ortho(0.f, static_cast<float>(DISPLAY_WIDTH),
                 static_cast<float>(DISPLAY_HEIGHT), 0.f, -1.f, 1.f);

  GLint projectionMatrixLocation =
      glGetUniformLocation(programId, "projectionMatrix");
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE,
                     &projectionMatrix[0][0]);

  int x = 15;
  int y = 20;
  int width = 256;
  int height = 192;
  GLfloat modelMatrix[16] = {
      static_cast<float>(width), 0.0f,                       0.0f, 0.0f,
      0.0f,                      static_cast<float>(height), 0.0f, 0.0f,
      0.0f,                      0.0f,                       1.0f, 0.0f,
      static_cast<float>(x),     static_cast<float>(y),      0.0f, 1.0f,
  };
  GLint modelMatrixId = glGetUniformLocation(programId, "modelMatrix");
  glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, modelMatrix);
#endif  // 0

  // Set up the geometry.

  GLfloat points[] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f,
  };
  GLfloat texCoords[] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f,
  };

  // Create the vertex array.
  GLuint vaoId = 0;
  glGenVertexArrays(1, &vaoId);
  CheckError();

  glBindVertexArray(vaoId);
  CheckError();

  GLuint vboId[2];
  glGenBuffers(2, vboId);
  CheckError();

  // Create array buffer and bind it to attribute 0 on the vertex array.
  glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
  CheckError();
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  CheckError();
  glEnableVertexAttribArray(0);
  CheckError();
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  CheckError();

  // Create the texture coordinates array.
  glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
  CheckError();
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
  CheckError();
  glEnableVertexAttribArray(1);
  CheckError();
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  CheckError();

  glUseProgram(programId);
  CheckError();
  glBindVertexArray(vaoId);
  CheckError();

  while (window.isOpen()) {
    sf::Event evt;
    if (window.pollEvent(evt)) {
      if (evt.type == sf::Event::Closed)
        window.close();
    }

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    CheckError();

    glBindVertexArray(vaoId);
    CheckError();

    glDrawArrays(GL_POINTS, 0, 4);
    CheckError();

    window.display();
  }

  // Delete the vertex array object.
  glDeleteVertexArrays(1, &vaoId);

  // Delete the program.
  glDeleteProgram(programId);

  return 0;
}
