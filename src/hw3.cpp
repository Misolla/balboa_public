#include "hw3.h"
// #include <GL/glew.h>

#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"


using namespace hw3;

glm::vec3 cameraTranslation(0,0,0);


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    glm::vec3 cameraUp    = glm::vec3(0.0f, -1.0f,  0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 0.005f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraTranslation += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraTranslation -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraTranslation -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraTranslation += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } 
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        // remember to clear depth buffer for more than 2 triangles
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } 
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 view;"
        "void main()\n"
        "{\n"
        "   gl_Position = view * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.0f, 0.5f, 1.0f);\n"
        "}\0";
    
    unsigned int VBO; //Vertex Buffer Object
    glGenBuffers(1, &VBO);  //generate a buffer with a buffer ID
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    int  success;
    char infoLog[512];
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  //buffer type of  a vertex buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check compiler status
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
   
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check compiler status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    while(!glfwWindowShouldClose(window)) {
        // remember to clear depth buffer for more than 2 triangles
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float angle = glfwGetTime();
        float rotationMatrix [16] = {
            cos(angle), -sin(angle), 0.0f, 0.0f,
            sin(angle), cos(angle), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        int viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, rotationMatrix);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    int  success;
    char infoLog[512];

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Vector3f background = scene.background;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glClearColor(background.x, background.y, background.z, 1.0f);
    
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "out vec3 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   color = aColor;\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec3 color;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(color, 1.0f);\n"
        "}\0";

    float s = scene.camera.s; // scaling factor of the view frustrum
    float a = width / (float)height;
    std::cout << "A is " << a << std::endl;
    float z_near = scene.camera.z_near; // distance of the near clipping plane
    float z_far = scene.camera.z_far;

    // auto point_vector = [&](Vector3 &p) -> Vector4 {
    //     return {p.x, p.y, p.z, 1.0};
    // };

    // auto to_point = [&](Vector4 &p) -> Vector3 {
    //     return {p.x, p.y, p.z};
    // };

    // auto project = [&](Vector3 &p) -> Vector2 {
    //     return {-p.x / p.z, -p.y / p.z};
    // };

    // auto to_screen = [&](Vector2 &p) -> Vector2 {
    //     Real x_screen = width * (p.x + s * a) / (2.0 * s * a);
    //     Real y_screen = height * (s - p.y) / (2.0  * s);
    //     return {x_screen, y_screen};
    // };

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check compiler status
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
   
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check compiler status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> EBOs;


    for (const auto &mesh : scene.meshes) {
        int vertexCount = mesh.vertices.size();
        int numVertexData = 6;
        float vertices[numVertexData * vertexCount];
        int indices[mesh.faces.size()*3];
        for (int i = 0; i < vertexCount; i++) {
            // Copy vertex position xyz
            vertices[i * numVertexData + 0] = mesh.vertices[i].x;
            vertices[i * numVertexData + 1] = mesh.vertices[i].y;
            vertices[i * numVertexData + 2] = mesh.vertices[i].z;
            // std::cout << "Vert data: " << vertices[i * numVertexData + 0] << ", " << vertices[i * numVertexData + 1] << ", " << vertices[i * numVertexData + 2] << std::endl;

            // Copy vertex color rgb
            vertices[i * numVertexData + 3] = mesh.vertex_colors[i].x;
            vertices[i * numVertexData + 4] = mesh.vertex_colors[i].y;
            vertices[i * numVertexData + 5] = mesh.vertex_colors[i].z;

            // Copy faces
        }
        for (int i = 0; i< mesh.faces.size(); i++){
            indices[i * 3] = mesh.faces[i][0];
            indices[i * 3 + 1] = mesh.faces[i][1];
            indices[i * 3 + 2] = mesh.faces[i][2];
            // std::cout << "Index data: " << indices[i * 3 + 0] << ", " << indices[i * 3 + 1] << ", " << indices[i * 3 + 2] << std::endl;
        }
        std::cout << "Sizeof(verticies) = " << sizeof(vertices) << std::endl;
        unsigned int VBO, VAO, EBO; //Vertex Buffer Object
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);  //generate a buffer with a buffer ID
        glGenBuffers(1, &EBO);

        VAOs.push_back(VAO);
        VBOs.push_back(VBO);
        EBOs.push_back(EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);  //buffer type of  a vertex buffer object
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    }
    Matrix4x4f projectionMatrix = Matrix4x4f::identity();
    projectionMatrix.data[0][0] = 1/(a*s);
    projectionMatrix.data[1][1] = 1/s;
    projectionMatrix.data[2][2] = -(z_far) / (z_far - z_near);
    projectionMatrix.data[3][3] = 0;
    projectionMatrix.data[2][3] = -1;
    projectionMatrix.data[3][2] = -(z_far * z_near) / (z_far - z_near);
    Matrix4x4f viewMatrix = scene.camera.cam_to_world;
    cameraTranslation[0] = scene.camera.cam_to_world.data[3][0];
    cameraTranslation[1] = scene.camera.cam_to_world.data[3][1];
    cameraTranslation[2] = scene.camera.cam_to_world.data[3][2];
    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;

    std::cout << "Projection matrix " << projectionMatrix << std::endl;
    std::cout << viewMatrix << std::endl;
    // std::cout << "Camera translation is " << cameraTranslation[0] << ", " << cameraTranslation[1] << ", " << cameraTranslation[2] << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    while(!glfwWindowShouldClose(window)) {
        // remember to clear depth buffer for more than 2 triangles
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix.ptr());
        int viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
        glm::mat4x4 view = glm::mat4x4();
        for(int r = 0; r<4; r++){
            for(int c = 0; c<4; c++){
                view[r][c] = viewMatrix.data[r][c];
            }
        }
        view[3][0] = cameraTranslation[0];
        view[3][1] = cameraTranslation[1];
        view[3][2] = cameraTranslation[2];
        view = glm::inverse(view);
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view[0][0]);
        int modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");
        // std::cout << "Camera translation is " << cameraTranslation[0] << ", " << cameraTranslation[1] << ", " << cameraTranslation[2] << std::endl;

        for (size_t j = 0; j < scene.meshes.size(); j++) {
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, scene.meshes[j].model_matrix.ptr());
            glBindVertexArray(VAOs[j]);
            glDrawElements(GL_TRIANGLES, scene.meshes[j].faces.size() * 3, GL_UNSIGNED_INT, 0);
            // glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
        // Clean up resources
    for (GLuint vao : VAOs) glDeleteVertexArrays(1, &vao);
    for (GLuint vbo : VBOs) glDeleteBuffers(1, &vbo);
    for (GLuint ebo : EBOs) glDeleteBuffers(1, &ebo);

    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

}

void hw_3_4(const std::vector<std::string> &params) {
        // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    int  success;
    char infoLog[512];

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Vector3f background = scene.background;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glClearColor(background.x, background.y, background.z, 1.0f);
    
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "out vec3 fragPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   color = aColor;\n"
        "   normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "   fragPos = vec3(model * vec4(aPos, 1.0));\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "in vec3 fragPos;\n"
        "out vec4 FragColor;\n"
        "uniform vec3 viewPos;"
        "void main()\n"
        "{\n"
        "   vec3 norm = normalize(normal);"
        "   vec3 lightPos = normalize(vec3(1,1,1));"
        "   vec3 lightDir = normalize(lightPos - fragPos);"
        "   float diff = max(dot(norm, lightPos), 0.0);"
        "   vec3 diffuse = diff * vec3(0.5,0.5,0.5);"
        "   vec3 ambient = vec3(0.1, 0.1, 0.1);"
        "   float specularStrength = 0.5;"
        "   vec3 viewDir = normalize(viewPos - fragPos);"
        "   vec3 reflectDir = reflect(-lightPos, norm); "
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
        "   vec3 specular = specularStrength * spec * vec3(1,1,1);  "
        "   vec3 result = (ambient + diffuse + specular) * color;"
        "   FragColor = vec4(result, 1.0f);\n"
        "}\0";

    float s = scene.camera.s; // scaling factor of the view frustrum
    float a = width / (float)height;
    std::cout << "A is " << a << std::endl;
    float z_near = scene.camera.z_near; // distance of the near clipping plane
    float z_far = scene.camera.z_far;

    // auto point_vector = [&](Vector3 &p) -> Vector4 {
    //     return {p.x, p.y, p.z, 1.0};
    // };

    // auto to_point = [&](Vector4 &p) -> Vector3 {
    //     return {p.x, p.y, p.z};
    // };

    // auto project = [&](Vector3 &p) -> Vector2 {
    //     return {-p.x / p.z, -p.y / p.z};
    // };

    // auto to_screen = [&](Vector2 &p) -> Vector2 {
    //     Real x_screen = width * (p.x + s * a) / (2.0 * s * a);
    //     Real y_screen = height * (s - p.y) / (2.0  * s);
    //     return {x_screen, y_screen};
    // };

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check compiler status
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
   
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check compiler status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> EBOs;


    for (const auto &mesh : scene.meshes) {
        int vertexCount = mesh.vertices.size();
        int numVertexData = 9;
        float vertices[numVertexData * vertexCount];
        int indices[mesh.faces.size()*3];
        for (int i = 0; i < vertexCount; i++) {
            // Copy vertex position xyz
            vertices[i * numVertexData + 0] = mesh.vertices[i].x;
            vertices[i * numVertexData + 1] = mesh.vertices[i].y;
            vertices[i * numVertexData + 2] = mesh.vertices[i].z;
            // std::cout << "Vert data: " << vertices[i * numVertexData + 0] << ", " << vertices[i * numVertexData + 1] << ", " << vertices[i * numVertexData + 2] << std::endl;

            // Copy vertex color rgb
            vertices[i * numVertexData + 3] = mesh.vertex_colors[i].x;
            vertices[i * numVertexData + 4] = mesh.vertex_colors[i].y;
            vertices[i * numVertexData + 5] = mesh.vertex_colors[i].z;

            vertices[i * numVertexData + 6] = mesh.vertex_normals[i].x;
            vertices[i * numVertexData + 7] = mesh.vertex_normals[i].y;
            vertices[i * numVertexData + 8] = mesh.vertex_normals[i].z;
            

            // Copy faces
        }
        for (int i = 0; i< mesh.faces.size(); i++){
            indices[i * 3] = mesh.faces[i][0];
            indices[i * 3 + 1] = mesh.faces[i][1];
            indices[i * 3 + 2] = mesh.faces[i][2];
            // std::cout << "Index data: " << indices[i * 3 + 0] << ", " << indices[i * 3 + 1] << ", " << indices[i * 3 + 2] << std::endl;
        }
        std::cout << "Sizeof(verticies) = " << sizeof(vertices) << std::endl;
        unsigned int VBO, VAO, EBO; //Vertex Buffer Object
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);  //generate a buffer with a buffer ID
        glGenBuffers(1, &EBO);

        VAOs.push_back(VAO);
        VBOs.push_back(VBO);
        EBOs.push_back(EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);  //buffer type of  a vertex buffer object
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numVertexData * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, numVertexData * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);
        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, numVertexData * sizeof(float), (void*)(6* sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    }
    Matrix4x4f projectionMatrix = Matrix4x4f::identity();
    projectionMatrix.data[0][0] = 1/(a*s);
    projectionMatrix.data[1][1] = 1/s;
    projectionMatrix.data[2][2] = -(z_far) / (z_far - z_near);
    projectionMatrix.data[3][3] = 0;
    projectionMatrix.data[2][3] = -1;
    projectionMatrix.data[3][2] = -(z_far * z_near) / (z_far - z_near);
    Matrix4x4f viewMatrix = scene.camera.cam_to_world;
    cameraTranslation[0] = scene.camera.cam_to_world.data[3][0];
    cameraTranslation[1] = scene.camera.cam_to_world.data[3][1];
    cameraTranslation[2] = scene.camera.cam_to_world.data[3][2];
    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;

    std::cout << "Projection matrix " << projectionMatrix << std::endl;
    std::cout << viewMatrix << std::endl;
    // std::cout << "Camera translation is " << cameraTranslation[0] << ", " << cameraTranslation[1] << ", " << cameraTranslation[2] << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    while(!glfwWindowShouldClose(window)) {
        // remember to clear depth buffer for more than 2 triangles
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix.ptr());
        int viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
        glm::mat4x4 view = glm::mat4x4();
        for(int r = 0; r<4; r++){
            for(int c = 0; c<4; c++){
                view[r][c] = viewMatrix.data[r][c];
            }
        }
        view[3][0] = cameraTranslation[0];
        view[3][1] = cameraTranslation[1];
        view[3][2] = cameraTranslation[2];
        view = glm::inverse(view);
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view[0][0]);
        int modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");

        int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos");
        glUniform3fv(viewPosLocation, 1, &cameraTranslation[0]);
        // std::cout << "Camera translation is " << cameraTranslation[0] << ", " << cameraTranslation[1] << ", " << cameraTranslation[2] << std::endl;

        for (size_t j = 0; j < scene.meshes.size(); j++) {
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, scene.meshes[j].model_matrix.ptr());
            glBindVertexArray(VAOs[j]);
            glDrawElements(GL_TRIANGLES, scene.meshes[j].faces.size() * 3, GL_UNSIGNED_INT, 0);
            // glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
        // Clean up resources
    for (GLuint vao : VAOs) glDeleteVertexArrays(1, &vao);
    for (GLuint vbo : VBOs) glDeleteBuffers(1, &vbo);
    for (GLuint ebo : EBOs) glDeleteBuffers(1, &ebo);

    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

}
