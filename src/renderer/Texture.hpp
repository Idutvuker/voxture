#include "mygl.hpp"
#include "../common/Image.hpp"

struct Texture {
    GLuint id;

    Texture() {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    ~Texture() {
        glDeleteTextures(1, &id);
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, id);
    }

};