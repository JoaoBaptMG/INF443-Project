#include <glad/glad.h>

#include <iostream>

void enableOpenGLErrorHandler()
{
    glEnable(GL_DEBUG_OUTPUT_KHR);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);

    glDebugMessageCallbackKHR([](GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam)
        {
            const char* strSource = "";
            switch (source)
            {
            case GL_DEBUG_SOURCE_API_KHR: strSource = "API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR: strSource = "window system"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR: strSource = "shader compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY_KHR: strSource = "third party"; break;
            case GL_DEBUG_SOURCE_APPLICATION_KHR: strSource = "application"; break;
            case GL_DEBUG_SOURCE_OTHER_KHR: strSource = "other"; break;
            }

            const char* strType = "";
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR_KHR: strType = "error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR: strType = "deprecated behavior"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR: strType = "undefined behavior"; break;
            case GL_DEBUG_TYPE_PORTABILITY_KHR: strType = "portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE_KHR: strType = "performance"; break;
            case GL_DEBUG_TYPE_MARKER_KHR: strType = "marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP_KHR: strType = "push group"; break;
            case GL_DEBUG_TYPE_POP_GROUP_KHR: strType = "pop group"; break;
            case GL_DEBUG_TYPE_OTHER_KHR: strType = "other"; break;
            }

            const char* strSeverity = "";
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH_KHR: strSeverity = "HIGH"; break;
            case GL_DEBUG_SEVERITY_MEDIUM_KHR: strSeverity = "MEDIUM"; break;
            case GL_DEBUG_SEVERITY_LOW_KHR: strSeverity = "LOW"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION_KHR: strSeverity = "notification"; break;
            }

            std::cout << '[' << strSeverity << "] " << strType << " (" << strSource << "): ";
            std::cout << std::string_view(message, length) << std::endl;
        }, nullptr);
}
