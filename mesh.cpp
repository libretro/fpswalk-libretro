#include "mesh.hpp"

using namespace glm;

namespace GL
{
   Mesh::Mesh() : 
      vertex_type(GL_TRIANGLES),
      model(glm::mat4(1.0)),
      view(glm::mat4(1.0)),
      projection(glm::mat4(1.0))
   {
      SYM(glGenBuffers)(1, &vbo);
      mvp = projection * view * model;
   }

   Mesh::~Mesh()
   {
      if (dead_state)
         return;

      SYM(glDeleteBuffers)(1, &vbo);
   }

   void Mesh::set_vertices(std::vector<Vertex> vertex)
   {
      set_vertices(std::tr1::shared_ptr<std::vector<Vertex> >(new std::vector<Vertex>(vertex)));
   }

   void Mesh::set_vertex_type(GLenum type)
   {
      vertex_type = type;
   }

   void Mesh::set_vertices(const std::tr1::shared_ptr<std::vector<Vertex> >& vertex)
   {
      this->vertex = vertex;

      SYM(glBindBuffer)(GL_ARRAY_BUFFER, vbo);
      SYM(glBufferData)(GL_ARRAY_BUFFER, vertex->size() * sizeof(Vertex),
            vertex->data(), GL_STATIC_DRAW);
      SYM(glBindBuffer)(GL_ARRAY_BUFFER, 0);
   }

   void Mesh::set_texture(const std::tr1::shared_ptr<Texture>& texture)
   {
      this->texture = texture;
   }

   void Mesh::set_shader(const std::tr1::shared_ptr<Shader>& shader)
   {
      this->shader = shader;
   }

   void Mesh::set_model(const glm::mat4& model)
   {
      this->model = model;
      mvp = projection * view * model;
   }

   void Mesh::set_view(const glm::mat4& view)
   {
      this->view = view;
      mvp = projection * view * model;
   }

   void Mesh::set_projection(const glm::mat4& projection)
   {
      this->projection = projection;
      mvp = projection * view * model;
   }

   void Mesh::render()
   {
      if (!vertex || !shader)
         return;

      if (texture)
         texture->bind();

      shader->use();

      SYM(glUniform1i)(shader->uniform("sTexture"), 0);

      SYM(glUniformMatrix4fv)(shader->uniform("uModel"),
            1, GL_FALSE, value_ptr(model));
      SYM(glUniformMatrix4fv)(shader->uniform("uMVP"),
            1, GL_FALSE, value_ptr(mvp));

      GLint aVertex = shader->attrib("aVertex");
      GLint aNormal = shader->attrib("aNormal");
      GLint aTex    = shader->attrib("aTex");

      SYM(glBindBuffer)(GL_ARRAY_BUFFER, vbo);

      if (aVertex >= 0)
      {
         SYM(glEnableVertexAttribArray)(aVertex);
         SYM(glVertexAttribPointer)(aVertex, 3, GL_FLOAT,
               GL_FALSE, sizeof(Vertex),
               reinterpret_cast<const GLvoid*>(offsetof(Vertex, vert)));
      }

      if (aNormal >= 0)
      {
         SYM(glEnableVertexAttribArray)(aNormal);
         SYM(glVertexAttribPointer)(aNormal, 3, GL_FLOAT,
               GL_FALSE, sizeof(Vertex),
               reinterpret_cast<const GLvoid*>(offsetof(Vertex, normal)));
      }

      if (aTex >= 0)
      {
         SYM(glEnableVertexAttribArray)(aTex);
         SYM(glVertexAttribPointer)(aTex, 2, GL_FLOAT,
               GL_FALSE, sizeof(Vertex),
               reinterpret_cast<const GLvoid*>(offsetof(Vertex, tex)));
      }

      SYM(glDrawArrays)(vertex_type, 0, vertex->size());

      if (aVertex >= 0)
         SYM(glDisableVertexAttribArray)(aVertex);
      if (aNormal >= 0)
         SYM(glDisableVertexAttribArray)(aNormal);
      if (aTex >= 0)
         SYM(glDisableVertexAttribArray)(aTex);

      SYM(glBindBuffer)(GL_ARRAY_BUFFER, 0);

      Texture::unbind();
      Shader::unbind();
   }
}

