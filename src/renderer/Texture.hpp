#include "mygl.hpp"

struct Texture {
    GLuint id;
    GLenum type;

    Texture(GLenum _type) : type(_type) {
        glGenTextures(1, &id);
        glBindTexture(type, id);

        glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    ~Texture() {
        glDeleteTextures(1, &id);
    }

    void bind() const {
        glBindTexture(type, id);
    }

};