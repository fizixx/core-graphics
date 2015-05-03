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

#ifndef VERTEX_BUFFER_OBJECT_H_
#define VERTEX_BUFFER_OBJECT_H_

#include <GL/glew.h>
#include <nucleus/logging.h>
#include <nucleus/macros.h>
#include <SFML/OpenGL.hpp>

#include "utils.h"

class VertexBufferObject {
public:
  // Scoper object to bind this object.
  struct ScopedBind {
    VertexBufferObject& vbo;
    ScopedBind(VertexBufferObject& vbo) : vbo(vbo) { vbo.bind(); }
    ~ScopedBind() { vbo.unbind(); }
  };

  VertexBufferObject() = default;
  ~VertexBufferObject();

  // Upload the given data to the GPU.
  bool setData(const void* data, size_t size);

private:
  // Create the name of the object.
  bool createInternal();

  // Bind the object.
  void bind() { GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_name)); }

  // Unbind the object.
  void unbind() { GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0)); }

  // The name of the vertex buffer object.
  GLuint m_name = 0;

  DISALLOW_COPY_AND_ASSIGN(VertexBufferObject);
};

#endif  // VERTEX_BUFFER_OBJECT_H_
