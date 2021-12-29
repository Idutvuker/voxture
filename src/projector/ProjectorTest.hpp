#pragma once

#include <iostream>

struct ProjectorTest {
    GLFWContext context;
    Resources resources;


    GLuint programID;

    void loadProgram() {
        GLuint shader = ShaderProgram::createShader(GL_COMPUTE_SHADER, "resources/shaders/test.comp.glsl");
        programID = glCreateProgram();

        glAttachShader(programID, shader);

        glLinkProgram(programID);

        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
            const size_t LOG_SIZE = 1024;
            GLchar infoLog[LOG_SIZE];

            glGetProgramInfoLog(programID, LOG_SIZE, nullptr, infoLog);
            throw std::runtime_error(std::string("ShaderProgram link failed:\n") + infoLog);
        }

        glDeleteShader(shader);
    }

    template<typename T>
    void printVec(const std::vector<T> &vec) {
        for (const auto &x: vec) {
            std::cout << x << ' ';
        }
        std::cout << std::endl;
    }

    void run() {
        loadProgram();
        glUseProgram(programID);

        std::vector<int> input = {9, 1, 2, 3, 4, 5, 6, 7};
        std::vector<int> output(input.size(), 0);
        auto byteSize = GLsizeiptr(input.size() * sizeof(GLint));

        GLuint outputSSBO;

        glGenBuffers(1, &outputSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, input.data(), GL_DYNAMIC_DRAW);

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, byteSize, output.data());

        printVec(output);
    }
};