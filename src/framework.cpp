
#include "framework.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Platform-specific includes for backtrace
#if defined(_WIN32)
#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp")
#elif defined(__linux__)
#include <execinfo.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

namespace framework{  // Debug Callback (for OpenGL debug output)
    static void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                                       GLenum severity, GLsizei /*length*/,

                                       const GLchar* message, const void* userParam)
    {
        std::ostream* os = reinterpret_cast<std::ostream*>(const_cast<void*>(userParam));
        if (os == nullptr)
            os = &std::cerr;

        *os << "---- OGL DEBUG ----" << std::endl;
        *os << "message <" << id << ">: '" << gldebug::sourceToString(source)
            << "' reported '" << gldebug::typeToString(type)
            << "' with '" << gldebug::severityToString(severity)
            << "' severity: " << std::endl << message << std::endl;
        *os << "---- BACKTRACE ----" << std::endl;
        gldebug::printBacktrace(os);
        *os << "---- ---- ---- ----" << std::endl;
    }

    // Initialization Functions
    bool initialize(int glVerMajor, int glVerMinor, DebugLevel debugLevel)
    {
        // Initialize GLEW - must set experimental for forward compatible contexts
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();

        if (err != GLEW_OK)
        {
            std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
            return false;
        }

        // Silently ignore error caused by glewInit() - happens in forward compatible contexts
        glGetError();

        // Check OpenGL version support
        std::stringstream vers;
        vers << "GL_VERSION_" << glVerMajor << "_" << glVerMinor;
        if (!glewIsSupported(vers.str().c_str()))
        {
            std::cerr << "OpenGL " << glVerMajor << "." << glVerMinor
                << " or higher not available" << std::endl;
            return false;
        }

        // Enable debug output if requested and available
        bool debugEnabled = false;
        if (debugLevel > DEBUG_OFF && glDebugMessageCallback != nullptr)
        {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debugCallback, &std::cerr);
            debugEnabled = true;

            if (glDebugMessageControl)
            {
                // Enable various severity levels according to debugLevel
                const int nSeverities = 3;
                GLenum severities[nSeverities] = {
                    GL_DEBUG_SEVERITY_HIGH,
                    GL_DEBUG_SEVERITY_MEDIUM,
                    GL_DEBUG_SEVERITY_LOW
                };

                for (int i = 0; i < nSeverities; ++i)
                {
                    GLboolean enable = (debugLevel > i) ? GL_TRUE : GL_FALSE;
                    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, severities[i], 0, NULL, enable);
                    glDebugMessageControl(GL_DEBUG_SOURCE_WINDOW_SYSTEM, GL_DONT_CARE, severities[i], 0, NULL, enable);
                    glDebugMessageControl(
                        GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DONT_CARE, severities[i], 0, NULL, enable);
                    glDebugMessageControl(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DONT_CARE, severities[i], 0, NULL, enable);
                    glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DONT_CARE, severities[i], 0, NULL, enable);
                    glDebugMessageControl(GL_DEBUG_SOURCE_OTHER, GL_DONT_CARE, severities[i], 0, NULL, enable);
                }
            }
        }

        // Note: stb_image doesn't need initialization like DevIL's ilInit()

        std::cout << "framework: initialized, using OpenGL " << glVerMajor << "." << glVerMinor
            << " debug " << (debugEnabled ? "enabled" : "disabled") << std::endl;

        return true;
    }

    std::string frameworkRoot()
    {
        char* root = getenv("FRAMEWORK_ROOT");
        return root ? root : "";
    }

    void dieWithError(const std::string& errMsg)
    {
#if defined(_WIN32) || defined(WIN32)
    MessageBoxA(NULL, errMsg.c_str(), "Fatal error", MB_ICONERROR | MB_OK);
#else
        std::cerr << "Fatal error: " << errMsg << std::endl;
#endif
        exit(1);
    }

    void checkGLError(const char* where, int line)
    {
        GLenum err = glGetError();
        if (err == GL_NO_ERROR)
            return;

        std::string errString = "<unknown>";
        switch (err)
        {
        case GL_INVALID_ENUM:
            errString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errString = "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            errString = "GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            errString = "GL_STACK_OVERFLOW";
            break;
        default:
            break;
        }

        if (where == nullptr || *where == '\0')
            std::cerr << "GL error occurred: " << errString << std::endl;
        else
            std::cerr << "GL error occurred in " << where << ":" << line << ": " << errString << std::endl;
    }

    // Shader Functions
    GLuint createShaderFromSource(GLenum eShaderType, const std::string& strShaderText)
    {
        GLuint shader = glCreateShader(eShaderType);
        const char* strFileData = strShaderText.c_str();
        glShaderSource(shader, 1, &strFileData, NULL);

        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar* strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

            const char* strShaderType = nullptr;
            switch (eShaderType)
            {
            case GL_VERTEX_SHADER: strShaderType = "vertex";
                break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment";
                break;
            case GL_GEOMETRY_SHADER: strShaderType = "geometry";
                break;
            case GL_TESS_CONTROL_SHADER: strShaderType = "tessellation control";
                break;
            case GL_TESS_EVALUATION_SHADER: strShaderType = "tessellation evaluation";
                break;
            case GL_COMPUTE_SHADER: strShaderType = "compute";
                break;
            default: strShaderType = "unknown";
                break;
            }

            std::cerr << "Compile failure in " << strShaderType << " shader:" << std::endl;
            std::cerr << strInfoLog << std::endl;

            delete[] strInfoLog;
            glDeleteShader(shader);
            return 0;
        }

        CHECK_GL_ERROR();
        return shader;
    }

    GLuint createShaderFromFile(GLenum eShaderType, const std::string& filename)
    {
        FILE* f = fopen(filename.c_str(), "rb");
        if (!f)
        {
            std::cerr << "Unable to open file " << filename << " for reading" << std::endl;
            return 0;
        }

        std::cout << "loading shader: " << filename << std::endl;

        fseek(f, 0, SEEK_END);
        long length = ftell(f);
        fseek(f, 0, SEEK_SET);

        char* buffer = new char[length + 1];
        size_t bytesRead = fread(buffer, 1, length, f);
        fclose(f);
        buffer[bytesRead] = '\0';

        GLuint sh = createShaderFromSource(eShaderType, buffer);
        delete[] buffer;
        return sh;
    }

    static bool linkProgram(GLuint program)
    {
        glLinkProgram(program);

        GLint status = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        CHECK_GL_ERROR();

        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar* strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
            std::cerr << "Linker failure: " << strInfoLog << std::endl;
            delete[] strInfoLog;
            return false;
        }

        return true;
    }

    GLuint createProgram(const GLuint* shaders)
    {
        GLuint program = glCreateProgram();

        while (shaders && *shaders)
            glAttachShader(program, *shaders++);
        CHECK_GL_ERROR();

        if (!linkProgram(program))
        {
            deleteProgramAndShaders(program);
            return 0;
        }

        return program;
    }

    GLuint createProgram(const std::vector<GLuint>& shaderList)
    {
        GLuint program = glCreateProgram();

        for (size_t i = 0; i < shaderList.size(); i++)
            glAttachShader(program, shaderList[i]);
        CHECK_GL_ERROR();

        if (!linkProgram(program))
        {
            deleteProgramAndShaders(program);
            return 0;
        }

        return program;
    }

    void deleteProgramAndShaders(GLuint program)
    {
        const GLsizei MAX_COUNT = 10;
        GLsizei count = 0;
        GLuint shaders[MAX_COUNT];

        if (program != 0)
        {
            glGetAttachedShaders(program, MAX_COUNT, &count, shaders);
            for (int i = 0; i < count; i++)
                glDeleteShader(shaders[i]);

            glDeleteProgram(program);
        }
    }

    // Texture Functions
    bool loadTexImage2D(const std::string& fileName, GLenum target)
    {
        // Set stb_image to flip images vertically (OpenGL expects origin at bottom-left)
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

        if (data == nullptr)
        {
            std::cerr << __FUNCTION__ << " cannot load image " << fileName
                << ": " << stbi_failure_reason() << std::endl;
            return false;
        }

        // Determine format based on number of channels
        GLenum format;
        GLenum internalFormat;
        switch (channels)
        {
        case 1:
            format = GL_RED;
            internalFormat = GL_RED;
            break;
        case 2:
            format = GL_RG;
            internalFormat = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA;
            break;
        default:
            std::cerr << __FUNCTION__ << " unsupported number of channels: " << channels << std::endl;
            stbi_image_free(data);
            return false;
        }

        glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
        return true;
    }

    GLuint createTexture(const std::string& fileName, bool mipmap)
    {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Set filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set wrapping mode (optional, but commonly needed)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Upload image data
        if (!loadTexImage2D(fileName, GL_TEXTURE_2D))
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            glDeleteTextures(1, &tex);
            return 0;
        }

        // Generate mipmaps if requested
        if (mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR();
        return tex;
    }

    // Debug Namespace Implementation
    namespace gldebug
    {
        const char* sourceToString(GLenum src)
        {
            switch (src)
            {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION: return "Application";
            case GL_DEBUG_SOURCE_OTHER: return "Other";
            default: return "<unknown>";
            }
        }

        const char* typeToString(GLenum type)
        {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
            case GL_DEBUG_TYPE_OTHER: return "Other";
            default: return "<unknown>";
            }
        }

        const char* severityToString(GLenum severity)
        {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH: return "High";
            case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
            case GL_DEBUG_SEVERITY_LOW: return "Low";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
            default: return "<unknown>";
            }
        }

        void printBacktrace(std::ostream* os)
        {
            if (os == nullptr)
                os = &std::cout;

            const int SIZE = 100;

#if defined(_WIN32)
    const int NAME_SIZE = 256;
    HANDLE process = GetCurrentProcess();
    SymSetOptions(SYMOPT_LOAD_LINES);
    SymInitialize(process, NULL, TRUE);

    void* stack[SIZE];
    unsigned short frames = CaptureStackBackTrace(0, SIZE, stack, NULL);
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + NAME_SIZE * sizeof(char), 1);
    symbol->MaxNameLen = NAME_SIZE;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (unsigned short i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD dwDisplacement;
        if (!strstr(symbol->Name, __FUNCTION__) &&
            !strstr(symbol->Name, "framework::debugCallback") &&
            SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dwDisplacement, &line)) {
            *os << symbol->Name << "():" << line.LineNumber << std::endl;
        }
        if (strcmp(symbol->Name, "main") == 0)
            break;
    }

    free(symbol);
#elif defined(__linux__)
            void* buffer[SIZE];
            char** strings;

            int nptrs = backtrace(buffer, SIZE);
            strings = backtrace_symbols(buffer, nptrs);
            if (strings == NULL)
            {
                *os << "error in backtrace_symbols" << std::endl;
                return;
            }
            for (int j = 0; j < nptrs; j++)
                *os << strings[j] << std::endl;
            free(strings);
#else
    *os << "backtrace not supported on this platform" << std::endl;
#endif
        }
    } // namespace gldebug
}