#include "Graphics.h"

Graphics::Graphics()
{
    ConfigureOpenGL();
    MakeShaders();
    ConfigureFreeType();
    MakeVAOs();
    MakeTextures();
}

Graphics::~Graphics()
{
    FreeObjects();
}

void Graphics::ConfigureOpenGL()
{
    gladLoadGL();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_CULL_FACE);  //* enable culling faces
    glCullFace(GL_BACK);     //* cull back face
    glFrontFace(GL_CCW);     //* counter clock-wise

    glEnable(GL_DEPTH_TEST); //* enable depth-testing
    glDepthFunc(GL_LESS);    //* write to z buffer then w value is less than present in buffer

    glEnable(GL_BLEND);      //* enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void Graphics::MakeShaders()
{
    MakeShader("shaders/Main.vert", "shaders/Main.frag", m_shader);
    m_shader->SetInt("material.diffuse", 1);
    m_shader->SetInt("material.specular", 2);
    MakeShader("shaders/Text.vert", "shaders/Text.frag", m_textShader);
    MakeShader("shaders/LightSource.vert", "shaders/LightSource.frag", m_lightSourceShader);
    MakeShader("shaders/GUI.vert", "shaders/GUI.frag", m_GUIShader);
}

void Graphics::MakeVAOs()
{
    MakeTriangleVAO();
    MakeCubeVAO();
    MakeLightSourceCubeVAO();
    MakeRectangleVAO();
    MakeSphereVAO();
    MakeLightSourceSphereVAO();
    MakeFullScreenRectangleVAO();
}

void Graphics::MakeTextures()
{
    m_textures.push_back(std::make_pair(MakeTexture("resources/textures/diamond_ore.png"),
                                        MakeTexture("resources/textures/diamond_ore_spec.png")));
}

void Graphics::MakeShader(const GLchar* vertexPath, const GLchar* fragmentPath, Shader*& l_shader)
{
    l_shader = new Shader(vertexPath, fragmentPath);
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

void Graphics::FreeObjects()
{
    delete m_shader;
    delete m_textShader;
    delete m_GUIShader;
    delete m_lightSourceShader;
}

void Graphics::MakeTriangleVAO()
{
    GLfloat points[3][3] = {
        {  0.0f,  0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f }
    };

    GLfloat colours[3][3] = {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    GLuint pointsVBO;
    glCreateBuffers(1, &pointsVBO);
    glNamedBufferData(pointsVBO, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    GLuint coloursVBO;
    glCreateBuffers(1, &coloursVBO);
    glNamedBufferData(coloursVBO, 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_triangleVAO);

    glVertexArrayVertexBuffer(m_triangleVAO, 0, pointsVBO, 0, 3 * sizeof(GL_FLOAT));
    glVertexArrayVertexBuffer(m_triangleVAO, 1, coloursVBO, 0, 3 * sizeof(GL_FLOAT));

    glVertexArrayAttribFormat(m_triangleVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_triangleVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(m_triangleVAO, 0, 0);
    glVertexArrayAttribBinding(m_triangleVAO, 1, 1);

    glEnableVertexArrayAttrib(m_triangleVAO, 0);
    glEnableVertexArrayAttrib(m_triangleVAO, 1);
}

void Graphics::MakeCubeVAO()
{
    GLfloat vertices[24][8] = {
        //*Coordinates        |Normals             |Texture coordinates
        //*Front face         |                    |
        {  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f }, //* top right front
        {  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f }, //* bottom right front
        { -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f }, //* bottom left front
        { -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f }, //* top left front
        //* Back face         |                    |
        {  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f }, //* top right back
        {  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f }, //* bottom right back
        { -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f }, //* bottom left back
        { -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f }, //* top left back
        //* Left face         |                    |
        { -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, //* top left front
        { -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, //* bottom left front
        { -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f }, //* bottom left back
        { -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, //* top left back
        //* Right face        |                    |
        {  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, //* top right front
        {  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f }, //* bottom right front
        {  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, //* bottom right back
        {  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, //* top right back
        //* Top face          |                    |
        {  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f }, //* top right front
        {  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f }, //* top right back
        { -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f }, //* top left back
        { -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f }, //* top left front
        //* Bottom face       |                    |
        {  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f }, //* bottom right front
        {  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f }, //* bottom right back
        { -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f }, //* bottom left back
        { -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f }  //* bottom left front
    };

    GLuint indices[12][3] = {
        //* Front face
        {  0,  3,  2 },
        {  2,  1,  0 },
        //* Back face
        {  7,  4,  5 },
        {  5,  6,  7 },
        //* Left face
        {  8, 11, 10 },
        { 10,  9,  8 },
        //* Right face
        { 15, 12, 14 },
        { 12, 13, 14 },
        //* Top face
        { 19, 16, 17 },
        { 17, 18, 19 },
        //* Bottom face
        { 20, 23, 22 },
        { 22, 21, 20 }
    };

    glCreateBuffers(1, &m_cubeVBO);
    glNamedBufferData(m_cubeVBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glCreateBuffers(1, &m_cubeIBO);
    glNamedBufferData(m_cubeIBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_cubeVAO);

    glVertexArrayVertexBuffer(m_cubeVAO, 0, m_cubeVBO, 0, 8 * sizeof(GL_FLOAT));
    glVertexArrayElementBuffer(m_cubeVAO, m_cubeIBO);

    glVertexArrayAttribFormat(m_cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));
    glVertexArrayAttribFormat(m_cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT));

    glVertexArrayAttribBinding(m_cubeVAO, 0, 0);
    glVertexArrayAttribBinding(m_cubeVAO, 1, 0);
    glVertexArrayAttribBinding(m_cubeVAO, 2, 0);

    glEnableVertexArrayAttrib(m_cubeVAO, 0);
    glEnableVertexArrayAttrib(m_cubeVAO, 1);
    glEnableVertexArrayAttrib(m_cubeVAO, 2);
}

void Graphics::MakeLightSourceCubeVAO()
{
    glCreateVertexArrays(1, &m_lightSourceCubeVAO);

    glVertexArrayVertexBuffer(m_lightSourceCubeVAO, 0, m_cubeVBO, 0, 8 * sizeof(GL_FLOAT));
    glVertexArrayElementBuffer(m_lightSourceCubeVAO, m_cubeIBO);

    glVertexArrayAttribFormat(m_lightSourceCubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(m_lightSourceCubeVAO, 0, 0);

    glEnableVertexArrayAttrib(m_lightSourceCubeVAO, 0);
}

void Graphics::MakeFullScreenRectangleVAO()
{
    const GLfloat quadVertices[6][3] = {
        //* Positions         TexCoords
        { -1.0f,  1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f }
    };
    GLuint VBO;
    glGenVertexArrays(1, &m_fullScreenRectangleVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_fullScreenRectangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

void Graphics::MakeRectangleVAO()
{
    GLfloat vertices[4][6] = {
        //*coordinates        colors
        {  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f }, //* top right
        {  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f }, //* bottom right
        { -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f }, //* bottom left
        { -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f }  //* top left
    };
    GLuint indices[2][3] = {
        //* top part
        { 0, 3, 1 },
        //* bottom part
        { 3, 2, 1 }
    };

    GLuint pointsVBO;
    glCreateBuffers(1, &pointsVBO);
    glNamedBufferData(pointsVBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint pointsIBO;
    glCreateBuffers(1, &pointsIBO);
    glNamedBufferData(pointsIBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_rectangleVAO);

    glVertexArrayVertexBuffer(m_rectangleVAO, 0, pointsVBO, 0, 6 * sizeof(GL_FLOAT));
    glVertexArrayElementBuffer(m_rectangleVAO, pointsIBO);

    glVertexArrayAttribFormat(m_rectangleVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_rectangleVAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));

    glVertexArrayAttribBinding(m_rectangleVAO, 0, 0);
    glVertexArrayAttribBinding(m_rectangleVAO, 1, 0);

    glEnableVertexArrayAttrib(m_rectangleVAO, 0);
    glEnableVertexArrayAttrib(m_rectangleVAO, 1);
}

void Graphics::MakeSphereVAO()
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    const GLfloat PI = acos(-1.0f);

    GLfloat radius    = 1.0f;
    GLint sectorCount = 36;
    GLint stackCount  = 18;

    GLfloat x, y, z, xy;                           // vertex position
    GLfloat nx, ny, nz, lengthInv = 1.0f / radius; // normal

    GLfloat sectorStep = 2 * PI / sectorCount;
    GLfloat stackStep  = PI / stackCount;
    GLfloat sectorAngle, stackAngle;

    for (GLint i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;    // starting from pi/2 to -pi/2
        xy         = radius * cosf(stackAngle); // r * cos(u)
        z          = radius * sinf(stackAngle); // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (GLint j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    GLuint k1, k2;
    for (GLint i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (GLint j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                // k1---k2---k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                // k1+1---k2---k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // generate interleaved vertex array as well
    std::vector<float> interleavedVertices;

    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);
    }
    // Generate and bind the Vertex Array Object
    glCreateVertexArrays(1, &m_sphereVAO);
    glBindVertexArray(m_sphereVAO);

    // Generate and bind the Vertex Buffer Object
    glCreateBuffers(1, &m_sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glNamedBufferData(m_sphereVBO, interleavedVertices.size() * sizeof(GLfloat), interleavedVertices.data(), GL_STATIC_DRAW);

    // Generate and bind the Element Buffer Object
    glCreateBuffers(1, &m_sphereIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);
    glNamedBufferData(m_sphereIBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Define the vertex attributes for the interleaved data
    const GLuint stride              = 6 * sizeof(GLfloat);
    const GLuint positionAttribIndex = 0;
    const GLuint normalAttribIndex   = 1;

    // Position attribute
    glEnableVertexAttribArray(positionAttribIndex);
    glVertexAttribPointer(positionAttribIndex, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(normalAttribIndex);
    glVertexAttribPointer(normalAttribIndex, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

    m_sphereIndexCount = indices.size();
}

void Graphics::MakeLightSourceSphereVAO()
{
    glCreateVertexArrays(1, &m_lightSourceSphereVAO);

    glVertexArrayVertexBuffer(m_lightSourceSphereVAO, 0, m_sphereVBO, 0, 6 * sizeof(GL_FLOAT));
    glVertexArrayElementBuffer(m_lightSourceSphereVAO, m_sphereIBO);

    glVertexArrayAttribFormat(m_lightSourceSphereVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_lightSourceSphereVAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));

    glVertexArrayAttribBinding(m_lightSourceSphereVAO, 0, 0);
    glVertexArrayAttribBinding(m_lightSourceSphereVAO, 1, 0);

    glEnableVertexArrayAttrib(m_lightSourceSphereVAO, 0);
    glEnableVertexArrayAttrib(m_lightSourceSphereVAO, 1);
}

void Graphics::DrawRectangle()
{
    glBindVertexArray(m_rectangleVAO);
    m_GUIShader->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Graphics::DrawSphere()
{
    glBindVertexArray(m_sphereVAO);
    m_shader->Use();
    glDrawElements(GL_TRIANGLES, m_sphereIndexCount, GL_UNSIGNED_INT, 0);
}

void Graphics::DrawLightSourceSphere()
{
    glBindVertexArray(m_lightSourceSphereVAO);
    m_lightSourceShader->Use();
    glDrawElements(GL_TRIANGLES, m_sphereIndexCount, GL_UNSIGNED_INT, 0);
}

void Graphics::DrawCube()
{
    m_shader->Use();
    glBindVertexArray(m_cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Graphics::DrawLightSourceCube()
{
    m_lightSourceShader->Use();
    glBindVertexArray(m_lightSourceCubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Graphics::RenderText(const std::string& text, GLfloat x, GLfloat y,
                          GLfloat sx, GLfloat sy, GLfloat scale,
                          GLfloat colorR, GLfloat colorG, GLfloat colorB)
{
    // activate corresponding render state
    m_textShader->Use();
    m_textShader->SetFloatVec3("textColor", colorR, colorG, colorB);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_textVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.bearingLeft * scale * sx;
        GLfloat ypos = y - (ch.sizeY - ch.bearingTop) * scale * sy;

        GLfloat w = ch.sizeX * scale * sx;
        GLfloat h = ch.sizeY * scale * sy;
        // update VBO for each character
        GLfloat vertices[6][4] = {
            {     xpos, ypos + h, 0.0f, 0.0f },
            {     xpos,     ypos, 0.0f, 1.0f },
            { xpos + w,     ypos, 1.0f, 1.0f },

            {     xpos, ypos + h, 0.0f, 0.0f },
            { xpos + w,     ypos, 1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale * sx; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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