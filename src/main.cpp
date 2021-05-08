
#if defined(WIN32)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

constexpr int DISPLAY_WIDTH = 1600;
constexpr int DISPLAY_HEIGHT = 900;

void GLError() {
#if !defined(NDEBUG)
  auto error = glGetError();

  if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL error: ";

    switch (error) {
      case GL_INVALID_OPERATION:
        std::cerr << "Invalid operation.";
        break;

      default:
        std::cerr << error << " (" << std::hex << error << ')';
    }
    std::cerr << '\n';
#if defined(WIN32)
    __debugbreak();
#endif
    std::exit(1);
  }
#endif
}

const char* kVertexShader = R"(
#version 430

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_tex_coord;

out vec4 vertex_color;
out vec2 vertex_tex_coord;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

void main() {
  gl_Position = u_projection_matrix * u_view_matrix * vec4(in_position, 1.0);
  vertex_color = in_color;
  vertex_tex_coord = in_tex_coord;
}
)";

const char* kFragmentShader = R"(
#version 430

in vec4 vertex_color;
in vec2 vertex_tex_coord;

out vec4 frag_color;

uniform sampler2D u_texture;

void main () {
  vec4 color = texture(u_texture, vertex_tex_coord);
  frag_color = color;
  // frag_color = vec4(vertex_color);
}
)";

struct Vertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 tex_coords;
};

struct Mesh {
  GLuint vertex_array_object;
  GLsizei vertex_count;

  ~Mesh() {
    glDeleteVertexArrays(1, &vertex_array_object);
  }
};

Mesh create_mesh(Vertex* vertices, size_t count) {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create the vertex buffer.
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, GLuint(count * sizeof(Vertex)), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 7));

  glBindVertexArray(0);

  return {
      vao,
      (GLsizei)count,
  };
}

struct Texture {
  GLuint texture_id;
  int width;
  int height;
};

Texture load_texture(std::string_view path) {
  Texture result{};

  int channels = 0;
  auto* image = stbi_load(path.data(), &result.width, &result.height, &channels, 4);

  std::cout << "Image: width(" << result.width << "), height(" << result.height << "), channels("
            << channels << ")\n";

  glGenTextures(1, &result.texture_id);
  GLError();

  glBindTexture(GL_TEXTURE_2D, result.texture_id);
  GLError();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               image);
  GLError();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GLError();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  GLError();

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);

  return result;
}

GLuint CreateShader(GLenum type, const char* source) {
  GLuint id = glCreateShader(type);
  GLError();

  const GLchar* s = source;
  auto length = static_cast<GLint>(std::strlen(source));
  glShaderSource(id, 1, &s, &length);
  GLError();

  return id;
}

GLuint CreateProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
  GLuint vertexShaderId = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fragmentShaderId = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  // Create and compile the program.
  GLuint programId = glCreateProgram();
  GLError();

  glAttachShader(programId, vertexShaderId);
  GLError();

  glAttachShader(programId, fragmentShaderId);
  GLError();

  glLinkProgram(programId);
  GLError();

  // Print out some information about the program.
  std::string buffer(1024, 0);
  GLsizei outLength = 0;
  glGetProgramInfoLog(programId, static_cast<GLsizei>(buffer.size()), &outLength,
                      (GLchar*)buffer.data());
  buffer.resize(outLength);
  if (!buffer.empty()) {
    std::cerr << "Error in shader:\n" << buffer;
  }

  return programId;
}

void render_mesh(GLuint program, GLuint mode, GLuint mesh, size_t count, GLuint texture) {
  glUseProgram(program);
  GLError();

  glBindVertexArray(mesh);
  GLError();

  if (texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    // glActiveTexture(GL_TEXTURE0 + 0);
    GLError();
  }

  auto location = glGetUniformLocation(program, "u_texture");
  glUniform1i(location, 0);
  GLError();

  glDrawArrays(mode, 0, (GLsizei)count);
  GLError();
}

void render_mesh(GLuint program, GLuint mode, const Mesh& mesh, Texture* texture) {
  render_mesh(program, mode, mesh.vertex_array_object, mesh.vertex_count,
              texture ? texture->texture_id : 0);
}

void window_size_changed(GLFWwindow* window, int width, int height) {
  std::cout << "size changed " << width << ", " << height << '\n';
  glViewport(0, 0, width, height);
  GLError();
}

#if defined(WIN32)
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
#else
int main(int argc, char* argv[]) {
#endif
  if (glfwInit() != GLFW_TRUE) {
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window =
      glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Core Graphics", nullptr, nullptr);
  if (!window) {
    return 1;
  }

  glfwSetWindowSizeCallback(window, window_size_changed);

  glfwMakeContextCurrent(window);

  // glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "Could not initialize GLEW.\n";
    return 1;
  }

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
  std::cout << "GLEW: " << glewGetString(GLEW_VERSION) << '\n';

  GLError();

  // Create the program we're going to use for rendering.
  GLuint programId = CreateProgram(kVertexShader, kFragmentShader);
  if (!programId) {
    std::cerr << "Could not create program.\n";
    return 1;
  }

  glUseProgram(programId);
  GLError();

  // Set up the geometry.
  constexpr float size = 0.5;

  Vertex vertices[] = {
      {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      {{size, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{size, size, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
      {{0.0f, size, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
  };

  auto mesh = create_mesh(vertices, sizeof(vertices) / sizeof(Vertex));

  // auto texture = load_texture("/home/tilo/code/core-graphics/resources/block_test.png");
  auto texture = load_texture(R"(C:\Code\core-graphics\resources\block_test.png)");
  glBindTexture(GL_TEXTURE_2D, texture.texture_id);
  GLError();

  GLint projection_matrix_location = glGetUniformLocation(programId, "u_projection_matrix");
  GLError();
  GLint view_matrix_location = glGetUniformLocation(programId, "u_view_matrix");
  GLError();
  GLint model_matrix_location = glGetUniformLocation(programId, "u_model_matrix");

  // glm::mat4 projection{1.0f};
  glm::mat4 projection = glm::perspective(60.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.01f, 100.0f);
  glm::mat4 view{1.0f};
  view = glm::lookAt(glm::vec3{0.0f, 0.0f, -0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
  glm::mat4 model{1.0f};

  glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection));
  GLError();
  glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, glm::value_ptr(view));
  GLError();
  glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));
  GLError();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    GLError();

    render_mesh(programId, GL_TRIANGLE_FAN, mesh, &texture);

    glfwSwapBuffers(window);
  }

  // Delete the program.
  glDeleteProgram(programId);

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}
