#pragma once

#include "mygl.hpp"

#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

struct ShaderProgram {
    GLuint programID = 0;

    ShaderProgram(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath) {
        auto vs = createShader(GL_VERTEX_SHADER, vertexShaderFilepath);
        auto fs = createShader(GL_FRAGMENT_SHADER, fragmentShaderFilepath);

        programID = glCreateProgram();

        glAttachShader(programID, vs);
        glAttachShader(programID, fs);
        glLinkProgram(programID);

        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
            const size_t LOG_SIZE = 1024;
            GLchar infoLog[LOG_SIZE];

            glGetProgramInfoLog(programID, LOG_SIZE, nullptr, infoLog);
            throw std::runtime_error(std::string("ShaderProgram link failed:\n") + infoLog);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    ~ShaderProgram() {
        glDeleteProgram(programID);
    }

    void use() const {
        glUseProgram(programID);
    }

private:
    static std::string readFile(const std::string &filepath) {
        std::ifstream t(filepath);
        if (!t)
            throw std::runtime_error("File can't be open: " + filepath);

        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }

    static GLuint createShader(GLenum type, const std::string &filepath) {
        auto shader = glCreateShader(type);

        std::string shaderSrc = readFile(filepath);

        const char *shaderSrc_cstr = shaderSrc.c_str();

        glShaderSource(shader, 1, &shaderSrc_cstr, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            const size_t LOG_SIZE = 1024;
            GLchar infoLog[LOG_SIZE];

            glGetShaderInfoLog(shader, LOG_SIZE, nullptr, infoLog);
            throw std::runtime_error(std::string("Shader compilation failed " + filepath + '\n') + infoLog);
        }

        return shader;
    }
};
