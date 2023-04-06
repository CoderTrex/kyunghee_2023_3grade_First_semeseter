#ifndef SHADER_H_
#define SHADER_H_

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";


const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 0.3f);\n"
    "}\n\0";

// const char* fragmentShaderSource = "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "in vec3 ourColor;\n"
//     "float near = 0.1;\n"
//     "float far  = 15.0;\n"
//     "float LinearizeDepth(float depth)\n"
//     "{\n"
//     "   float z = depth * 2.0 - 1.0;\n"
//     "   return (2.0 * near * far) / (far + near - z * (far - near));\n"
//     "}\n"
//     "void main()\n"
//     "{\n"
//     "   float depth = LinearizeDepth(gl_FragCoord.z) / far;\n"
//     "   FragColor = vec4(depth * 5 * ourColor, 1.0f);\n"
//     "}\0";

// shading with lighting: https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/2.2.basic_lighting_specular/basic_lighting_specular.cpp

#endif // SHADER_H_