struct DebugDraw {
    GLuint VAO, VBO;

    DebugDraw() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);
    }

    void drawPoints(const Camera &camera, const Resources &res, const std::vector<glm::vec3> &points) {
        using namespace glm;

        glPointSize(10);

        glBindVertexArray(VAO);
        res.testSP.use();

        GLint MVPLoc = glGetUniformLocation(res.testSP.programID, "uModelViewProjMat");
        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, value_ptr(camera.getViewProj()));

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(points.size() * sizeof(glm::vec3)), points.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, GLsizei(points.size()));
    }
};