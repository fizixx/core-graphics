// Copyright (c) 2015, Tiaan Louw
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "utils.h"

#include <fstream>

#include <nucleus/logging.h>
#include <SFML/OpenGL.hpp>

namespace detail {

void glCheck() {
  GLint error = glGetError();

  if (error != GL_NO_ERROR) {
    switch (error) {
      case GL_INVALID_ENUM:
        LOG(Error) << "GL_INVALID_ENUM: An unacceptable value is specified for "
                      "an enumerated argument. The offending command is "
                      "ignored and has no other side effect than to set the "
                      "error flag.";
        break;

      case GL_INVALID_VALUE:
        LOG(Error) << "GL_INVALID_VALUE";
        break;

      case GL_INVALID_OPERATION:
        LOG(Error) << "GL_INVALID_OPERATION";
        break;

      default:
        LOG(Error) << "Unknown OpenGL error.";
        break;
    }

    // Break into the debugger.
    __debugbreak();
  }
}

}  // namespace detail

bool getFileContents(const std::string& filename, std::vector<char>* buffer) {
  std::ifstream file(filename, std::ios::binary);
  if (file.is_open()) {
    file.seekg(0, std::ios_base::end);
    std::streamsize size = file.tellg();
    if (size > 0) {
      file.seekg(0, std::ios_base::beg);
      buffer->resize(static_cast<std::size_t>(size));
      file.read(&(*buffer)[0], size);
    }
    buffer->push_back('\0');
    return true;
  } else {
    return false;
  }
}
