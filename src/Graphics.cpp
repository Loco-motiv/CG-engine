#include "Graphics.h"

Graphics::Graphics()
{
    ConfigureOpenGL();
    ConfigureFreeType();
    MakePBOs();
    MakeFBOs();
}

Graphics::~Graphics()
{
    FreeObjects();
}

void APIENTRY
glDebugOutput(GLenum source,
              GLenum type,
              unsigned int id,
              GLenum severity,
              GLsizei length,
              const char* message,
              const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void Graphics::ConfigureOpenGL()
{
    gladLoadGL();

    glEnable(GL_CULL_FACE);  //* enable culling faces
    glCullFace(GL_BACK);     //* cull back face
    glFrontFace(GL_CCW);     //* counter clock-wise

    glEnable(GL_DEPTH_TEST); //* enable depth-testing
    glDepthFunc(GL_LESS);    //* write to z buffer then w value is less than present in buffer

    glEnable(GL_BLEND);      //* enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

void Graphics::ConfigureFreeType()
{
    if (FT_Init_FreeType(&m_ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(m_ft, "resources/fonts/minecraft.ttf", 0, &m_face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(m_face, 0, 32);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            m_face->glyph->bitmap.width,
            m_face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows,
            m_face->glyph->bitmap_left, m_face->glyph->bitmap_top,
            m_face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_ft);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //?

    glGenVertexArrays(1, &m_textVAO);
    glGenBuffers(1, &m_textVBO);
    glBindVertexArray(m_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_minTextHeight = Characters['a'].bearingTop;
    m_maxTextHeight = Characters['T'].bearingTop + Characters['p'].sizeY - Characters['p'].bearingTop;
}

void Graphics::MakePBOs()
{
    MakePickingPBO();
}

void Graphics::MakeFBOs()
{
    MakePickingFBO(m_pickingWidth, m_pickingHeight);
}

GLuint Graphics::MakeTexture(char const* path)
{
    unsigned int textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format         = GL_RGB;
        GLenum internalFormat = GL_RGB8;
        if (nrComponents == 1)
        {
            format         = GL_RED;
            internalFormat = GL_R8;
        }
        else if (nrComponents == 3)
        {
            format         = GL_RGB;
            internalFormat = GL_RGB8;
        }
        else if (nrComponents == 4)
        {
            format         = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        glTextureStorage2D(textureID, 1, internalFormat, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

void Graphics::FreeTexture(const GLuint l_id)
{
    glDeleteTextures(1, &l_id);
}

std::tuple<GLuint, GLuint, GLuint, GLuint> Graphics::MakeQuadMesh()
{
    GLfloat vertices[4][4] = {
        {  0.5f,  0.5f, 1.0f, 0.0f },
        {  0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f, -0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f, 0.0f, 0.0f }
    };

    GLuint indices[2][3] = {
        { 0, 3, 1 },
        { 3, 2, 1 }
    };

    GLuint pointsVBO;
    glCreateBuffers(1, &pointsVBO);
    glNamedBufferData(pointsVBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint pointsIBO;
    glCreateBuffers(1, &pointsIBO);
    glNamedBufferData(pointsIBO, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint m_rectangleVAO;
    glCreateVertexArrays(1, &m_rectangleVAO);

    glVertexArrayVertexBuffer(m_rectangleVAO, 0, pointsVBO, 0, 4 * sizeof(GLfloat));
    glVertexArrayElementBuffer(m_rectangleVAO, pointsIBO);

    glVertexArrayAttribFormat(m_rectangleVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_rectangleVAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

    glVertexArrayAttribBinding(m_rectangleVAO, 0, 0);
    glVertexArrayAttribBinding(m_rectangleVAO, 1, 0);

    glEnableVertexArrayAttrib(m_rectangleVAO, 0);
    glEnableVertexArrayAttrib(m_rectangleVAO, 1);

    return { m_rectangleVAO, pointsVBO, pointsIBO, 6 };
}

std::tuple<GLuint, GLuint, GLuint, GLuint> Graphics::MakeMesh(const std::string path) // TODO i definitely made this way too complicated
{
    struct Vertex
    {
        GLfloat px, py, pz;
        GLfloat nx, ny, nz;
        GLfloat u, v;

        bool operator==(const Vertex& other) const
        {
            return memcmp(this, &other, sizeof(Vertex)) == 0;
        }
    };

    struct VertexHash
    {
        static inline uint32_t mix(uint32_t x)
        {
            x ^= x >> 16;
            x *= 0x21f0aaad;
            x ^= x >> 15;
            x *= 0x735a2d97;
            x ^= x >> 15;
            return x;
        }

        static inline void hash_combine(uint32_t& seed, uint32_t v)
        {
            seed = mix(seed + 0x9e3779b9u + v);
        }

        size_t operator()(const Vertex& v) const
        {
            uint32_t seed = 0;

            constexpr size_t count = sizeof(Vertex) / sizeof(uint32_t);
            uint32_t data[count];
            memcpy(data, &v, sizeof(Vertex));

            for (size_t i = 0; i < count; i++)
            {
                hash_combine(seed, data[i]);
            }

            return seed;
        }
    };

    tinyobj::ObjReaderConfig readerConfig;
    // TODO readerConfig.mtl_search_path = "./";
    readerConfig.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, readerConfig))
    {
        if (!reader.Error().empty())
        {
            std::cout << "ERROR::TINYOBJREADER: " << reader.Error();
        }
        return { 0, 0, 0, 0 };
    }

    if (!reader.Warning().empty())
    {
        std::cout << "ERROR::TINYOBJREADER: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    // auto& materials = reader.GetMaterials();

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                Vertex vert{};

                //* position
                vert.px = attrib.vertices[3 * idx.vertex_index + 0];
                vert.py = attrib.vertices[3 * idx.vertex_index + 1];
                vert.pz = attrib.vertices[3 * idx.vertex_index + 2];

                //* normal
                if (idx.normal_index >= 0)
                {
                    vert.nx = attrib.normals[3 * idx.normal_index + 0];
                    vert.ny = attrib.normals[3 * idx.normal_index + 1];
                    vert.nz = attrib.normals[3 * idx.normal_index + 2];
                }

                //* uv
                if (idx.texcoord_index >= 0)
                {
                    vert.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vert.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                }

                //* deduplication
                if (uniqueVertices.count(vert) == 0)
                {
                    uint32_t newIndex    = static_cast<uint32_t>(vertices.size());
                    uniqueVertices[vert] = newIndex;
                    vertices.push_back(vert);
                }

                indices.push_back(uniqueVertices[vert]);
            }
            index_offset += fv;

            // per-face material
            // shapes[s].mesh.material_ids[f];
        }
        // TODO implement one shape = one mesh
    }
    GLuint VAO, VBO, IBO;

    glCreateVertexArrays(1, &VAO);

    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(Vertex),
                      vertices.data(), GL_STATIC_DRAW);

    glCreateBuffers(1, &IBO);
    glNamedBufferData(IBO, indices.size() * sizeof(uint32_t),
                      indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(VAO, IBO);

    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, px));
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, nx));
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, u));

    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribBinding(VAO, 2, 0);

    glEnableVertexArrayAttrib(VAO, 0);
    glEnableVertexArrayAttrib(VAO, 1);
    glEnableVertexArrayAttrib(VAO, 2);

    return { VAO, VBO, IBO, indices.size() };
}

void Graphics::FreeMesh(GLuint l_VAO, GLuint l_VBO, GLuint l_IBO)
{
    glDeleteVertexArrays(1, &l_VAO);
    glDeleteBuffers(1, &l_VBO);
    glDeleteBuffers(1, &l_IBO);
}

void Graphics::FreeObjects()
{
}

void Graphics::MakePickingPBO()
{
    glCreateBuffers(1, &m_pickingPBO);
    glNamedBufferData(m_pickingPBO, sizeof(GLuint), nullptr, GL_STREAM_READ);
}

void Graphics::MakePickingFBO(GLuint l_width, GLuint l_height)
{
    glCreateFramebuffers(1, &m_pickingFBO);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_pickingColorTexture);
    glTextureStorage2D(m_pickingColorTexture, 1, GL_R32UI, l_width, l_height);

    glNamedFramebufferTexture(m_pickingFBO, GL_COLOR_ATTACHMENT0,
                              m_pickingColorTexture, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_pickingDepthTexture);
    glTextureStorage2D(m_pickingDepthTexture, 1, GL_DEPTH_COMPONENT32F, l_width, l_height);

    glNamedFramebufferTexture(m_pickingFBO, GL_DEPTH_ATTACHMENT,
                              m_pickingDepthTexture, 0);

    glNamedFramebufferReadBuffer(m_pickingFBO, GL_COLOR_ATTACHMENT0);

    glNamedFramebufferDrawBuffer(m_pickingFBO, GL_COLOR_ATTACHMENT0);

    const GLenum fboStatus = glCheckNamedFramebufferStatus(m_pickingFBO, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER: not complete" << '\n';
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Graphics::DrawMesh(GLuint l_meshID, GLuint elementsCount)
{
    glBindVertexArray(l_meshID);
    glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
}

Character Graphics::GetCharacter(char c) const
{
    auto it = Characters.find(c);
    if (it != Characters.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "ERROR::FREETYPE: Character '" << c << "' not found" << std::endl;
        return Character{};
    }
}

sf::Vector2f Graphics::GetTextDimensions(const std::string& text, GLfloat sx, GLfloat sy, GLfloat scale)
{
    GLfloat x = 0;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        x += (ch.advance >> 6) * scale * sx; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    return sf::Vector2f(x, m_minTextHeight * scale * sy);
}

GLfloat Graphics::GetMaxTextHeight(GLfloat sy) const
{
    return m_maxTextHeight * sy;
}

void Graphics::BeginPickingDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_pickingFBO);
    glViewport(0, 0, m_pickingWidth, m_pickingHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::EndPickingDraw(GLuint l_width, GLuint l_height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, l_width, l_height);
}

void Graphics::ReadPixel(GLuint x, GLuint y)
{
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pickingPBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pickingFBO);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    if (m_pickingFence)
    {
        glDeleteSync(m_pickingFence);
    }
    m_pickingFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

GLuint Graphics::GetPickingResult()
{
    if (!m_pickingFence)
    {
        return 0;
    }

    GLenum syncStatus = glClientWaitSync(m_pickingFence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);

    if (syncStatus == GL_ALREADY_SIGNALED || syncStatus == GL_CONDITION_SATISFIED)
    {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pickingPBO);

        const GLuint* pixelData = (const GLuint*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        GLuint objectID         = 0;
        if (pixelData)
        {
            objectID = pixelData[0];
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        glDeleteSync(m_pickingFence);
        m_pickingFence = 0;

        return objectID;
    }
    return 0;
}
