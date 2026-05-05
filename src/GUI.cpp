#include "GUI.h"

//* Button

Button::Button(std::string l_name, std::function<void()> l_callback, WidgetText l_text, WidgetMesh l_mesh, Borders l_borders, WidgetHitbox l_hitbox, ButtonStyle l_style)
    : Widget(l_name, l_borders, l_style), m_style(l_style), m_callback{ l_callback }
{
    l_text.style = l_style.textStyle;
    l_mesh.style = l_style.meshStyle;
    m_texts.push_back(l_text);
    m_meshes.push_back(l_mesh);
    m_hitboxes.push_back(l_hitbox);
}

Button::~Button() {}

void Button::OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex < 0)
    {
        return;
    }
    {
        m_meshes[l_hitboxIndex].style.colour = m_style.glowUpColour;
    }
}

void Button::OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex < 0)
    {
        return;
    }
    m_meshes[l_hitboxIndex].style.colour = m_style.meshStyle.colour;
}

void Button::OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex < 0)
    {
        return;
    }
    m_meshes[l_hitboxIndex].style.colour = m_style.glowDownColour;
    m_isActive                           = true;
}

void Button::OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Button::OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex < 0)
    {
        m_isActive = false;
        return;
    }
    m_meshes[l_hitboxIndex].style.colour = m_style.meshStyle.colour;
    if (m_isActive)
    {
        m_callback();
    }
    m_isActive = false;
}

void Button::OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) {}

void Button::OnTextEntered(char32_t l_unicode) {}

void Button::Update() {}

//* Label

Label::Label(std::string l_name, std::function<std::string()> l_getStringFunction, WidgetText l_text, Borders l_borders, WidgetStyle l_style)
    : Widget{ l_name, l_borders, l_style }, m_getStringFunction{ l_getStringFunction }
{
    l_text.style = m_style.textStyle;
    m_texts.push_back(l_text);
}

Label::~Label() {}

void Label::OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Label::OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Label::OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Label::OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Label::OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Label::OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) {}

void Label::OnTextEntered(char32_t l_unicode) {}

void Label::Update()
{
    m_texts[0].text = m_getStringFunction();
}

//* Dropdown

Dropdown::Dropdown(std::string l_name, std::vector<std::string> l_options, GLint* l_valPtr,
                   std::function<void()> l_callback, WidgetText l_headerText, WidgetMesh l_headerMesh,
                   Borders l_headerBorders, WidgetHitbox l_headerHitbox,
                   std::vector<WidgetText> l_optTexts, std::vector<WidgetMesh> l_optMeshes,
                   std::vector<WidgetHitbox> l_optHitboxes, DropdownStyle l_style)
    : Widget(l_name, l_headerBorders, l_style), m_valuePtr(l_valPtr), m_options(l_options),
      m_callback(l_callback), m_style(l_style), m_isOpen(false)
{
    l_headerText.style        = m_style.textStyle;
    l_headerMesh.style        = m_style.meshStyle;
    l_headerMesh.style.colour = m_style.headerColour;

    m_texts.push_back(l_headerText);
    m_meshes.push_back(l_headerMesh);
    m_hitboxes.push_back(l_headerHitbox);

    for (size_t i = 0; i < l_optMeshes.size(); ++i)
    {
        l_optMeshes[i].style        = m_style.meshStyle;
        l_optMeshes[i].style.colour = m_style.optionColour;
        l_optTexts[i].style         = m_style.textStyle;

        m_meshes.push_back(l_optMeshes[i]);
        m_texts.push_back(l_optTexts[i]);
        m_hitboxes.push_back(l_optHitboxes[i]);
    }

    Close();
    Update();
}

Dropdown::~Dropdown() {}

void Dropdown::OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex >= 0 && l_hitboxIndex < static_cast<GLint>(m_meshes.size()))
    {
        m_meshes[l_hitboxIndex].style.colour = m_style.hoverColour;
    }
}

void Dropdown::OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex == 0)
    {
        m_meshes[l_hitboxIndex].style.colour = m_style.headerColour;
    }
    else if (l_hitboxIndex > 0 && l_hitboxIndex < static_cast<GLint>(m_meshes.size()))
    {
        m_meshes[l_hitboxIndex].style.colour = m_style.optionColour;
    }
}

void Dropdown::OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex >= 0 && l_hitboxIndex < static_cast<GLint>(m_meshes.size()))
    {
        m_isActive                           = true;
        m_meshes[l_hitboxIndex].style.colour = m_style.clickColour;
    }
}

void Dropdown::OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void Dropdown::OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (!m_isActive)
    {
        return;
    }
    m_isActive = false;

    if (l_hitboxIndex == 0)
    {
        Toggle();
    }
    else if (l_hitboxIndex > 0)
    {
        *m_valuePtr = l_hitboxIndex - 1;
        if (m_callback)
        {
            m_callback();
        }
        Close();
    }
}

void Dropdown::OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) {}

void Dropdown::OnTextEntered(char32_t l_unicode) {}

void Dropdown::Toggle()
{
    if (m_isOpen)
    {
        Close();
    }
    else
    {
        Open();
    }
}

void Dropdown::Open()
{
    m_isOpen                 = true;
    m_meshes[0].style.colour = m_style.clickColour;

    for (size_t i = 1; i < m_meshes.size(); ++i)
    {
        m_meshes[i].isVisible       = true;
        m_texts[i].isVisible        = true;
        m_hitboxes[i].isIntractable = true;
    }
}

void Dropdown::Close()
{
    m_isOpen                 = false;
    m_meshes[0].style.colour = m_style.headerColour;

    for (size_t i = 1; i < m_meshes.size(); ++i)
    {
        m_meshes[i].isVisible       = false;
        m_texts[i].isVisible        = false;
        m_hitboxes[i].isIntractable = false;
    }
}

void Dropdown::Update()
{
    if (*m_valuePtr >= 0 && *m_valuePtr < static_cast<GLint>(m_options.size()))
    {
        m_texts[0].text = std::format("{}: {}", m_name, m_options[*m_valuePtr]);
    }
    else
    {
        m_texts[0].text = std::format("{}: ...", m_name);
    }
}

//* TextInput

TextInput::TextInput(std::string l_name, std::string* l_valPtr, size_t l_maxLength,
                     WidgetText l_text, WidgetMesh l_mesh, Borders l_borders,
                     WidgetHitbox l_hitbox, TextInputStyle l_style)
    : Widget(l_name, l_borders, l_style), m_valuePtr(l_valPtr), m_maxLength(l_maxLength), m_style(l_style)
{
    l_text.style        = m_style.textStyle;
    l_mesh.style        = m_style.meshStyle;
    l_mesh.style.colour = m_style.inactiveColour;

    m_texts.push_back(l_text);
    m_meshes.push_back(l_mesh);
    m_hitboxes.push_back(l_hitbox);
}

TextInput::~TextInput() {}

void TextInput::OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}
void TextInput::OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}

void TextInput::OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)
{
    if (l_hitboxIndex < 0)
    {
        m_isActive               = false;
        m_meshes[0].style.colour = m_style.inactiveColour;
        return;
    }

    m_isActive               = true;
    m_meshes[0].style.colour = m_style.activeColour;
}

void TextInput::OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}
void TextInput::OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) {}
void TextInput::OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) {}

void TextInput::OnTextEntered(char32_t l_unicode)
{
    if (!m_isActive)
    {
        return;
    }

    if (l_unicode == 8)
    {
        if (!m_valuePtr->empty())
        {
            m_valuePtr->pop_back();
        }
    }
    else if (l_unicode >= 32 && l_unicode < 128)
    {
        if (m_valuePtr->size() < m_maxLength)
        {
            *m_valuePtr += static_cast<char>(l_unicode);
        }
    }
}

void TextInput::Update()
{
    std::string cursor = m_isActive ? "_" : "";

    m_texts[0].text = std::format("{}: {}{}", m_name, *m_valuePtr, cursor);
}

//* WidgetBox

WidgetBox::WidgetBox(std::string l_name, Borders l_borders, WidgetBoxStyle l_style)
    : m_name{ l_name }, m_borders{ l_borders }, m_style{ l_style }
{
    m_background.style    = m_style.meshStyle;
    m_background.position = sf::Vector2f((m_borders.left + m_borders.right) / 2.0f, (m_borders.top + m_borders.bottom) / 2.0f);
    m_background.scale    = sf::Vector3f((m_borders.right - m_borders.left), (m_borders.top - m_borders.bottom), 1.0f);
}

WidgetBox::~WidgetBox() {}

void WidgetBox::OnWindowResize(const sf::Vector2u& l_windowSize)
{
    m_background.dimensions.x = static_cast<GLint>(std::abs(m_background.scale.x) * 0.5f * l_windowSize.x);
    m_background.dimensions.y = static_cast<GLint>(std::abs(m_background.scale.y) * 0.5f * l_windowSize.y);
}

//* WidgetManager

WidgetManager::WidgetManager(SharedContext* l_sharedContext, GLfloat l_elementHeight, GLfloat l_elementGap, WidgetBoxStyle l_style)
    : m_sharedContext{ l_sharedContext }, m_elementHeight{ l_elementHeight }, m_elementGap{ l_elementGap }, m_style{ l_style }
{
}

WidgetManager::~WidgetManager() {}

void WidgetManager::Update()
{
    if (m_sharedContext->sceneManager->GetPickedObjectId())
    {
        GLint pickedId    = m_sharedContext->sceneManager->GetPickedObjectId();
        Object* pickedObj = m_sharedContext->sceneManager->GetObject(pickedId);

        if (pickedObj != nullptr && pickedObj != currentSelection.obj)
        {
            currentSelection.obj = pickedObj;

            currentSelection.position = pickedObj->GetPosition();
            currentSelection.rotation = pickedObj->GetRotation();
            currentSelection.scale    = pickedObj->GetScale();

            ClearWidgetBox("InspectorBox");
            m_widgetBoxes["InspectorBox"]->m_background.isVisible = true;

            currentSelection.materialNames        = m_sharedContext->materialManager->GetNames();
            currentSelection.meshNames            = m_sharedContext->meshManager->GetNames();
            std::string materialName              = pickedObj->GetMaterial() ? pickedObj->GetMaterial()->name : "None";
            std::string meshName                  = pickedObj->GetMesh() ? pickedObj->GetMesh()->name : "None";
            currentSelection.currentMaterialIndex = std::distance(currentSelection.materialNames.begin(),
                                                                  std::find(currentSelection.materialNames.begin(), currentSelection.materialNames.end(), materialName));
            currentSelection.currentMeshIndex     = std::distance(currentSelection.meshNames.begin(),
                                                                  std::find(currentSelection.meshNames.begin(), currentSelection.meshNames.end(), meshName));

            MakeLabel("Title", [pickedObj]()
                      { return std::format("Selected Object ID: {}", pickedObj->GetID()); }, "InspectorBox");

            MakeSlider("Pos X", &currentSelection.position.x, -200.0f, 200.0f, "InspectorBox");
            MakeSlider("Pos Y", &currentSelection.position.y, -50.0f, 50.0f, "InspectorBox");
            MakeSlider("Pos Z", &currentSelection.position.z, -200.0f, 200.0f, "InspectorBox");

            MakeSlider("Rot X", &currentSelection.rotation.x, -180.0f, 180.0f, "InspectorBox");
            MakeSlider("Rot Y", &currentSelection.rotation.y, -180.0f, 180.0f, "InspectorBox");
            MakeSlider("Rot Z", &currentSelection.rotation.z, -180.0f, 180.0f, "InspectorBox");

            MakeSlider("Scale X", &currentSelection.scale.x, 0.01f, 100.0f, "InspectorBox");
            MakeSlider("Scale Y", &currentSelection.scale.y, 0.01f, 100.0f, "InspectorBox");
            MakeSlider("Scale Z", &currentSelection.scale.z, 0.01f, 100.0f, "InspectorBox");

            MakeDropdown("Material", currentSelection.materialNames, &currentSelection.currentMaterialIndex, [this, pickedObj]()
                         { pickedObj->SetMaterial(m_sharedContext->materialManager->Get(currentSelection.materialNames[currentSelection.currentMaterialIndex])); }, "InspectorBox");
            MakeDropdown("Mesh", currentSelection.meshNames, &currentSelection.currentMeshIndex, [this, pickedObj]()
                         { pickedObj->SetMesh(m_sharedContext->meshManager->Get(currentSelection.meshNames[currentSelection.currentMeshIndex])); }, "InspectorBox");
            MakeButton("Delete Object", [this, pickedObj]()
                       { m_sharedContext->sceneManager->DeleteObject(pickedObj->GetID()); }, "InspectorBox");
        }
    }
    else
    {
        if (currentSelection.obj != nullptr)
        {
            currentSelection.Reset();
            ClearWidgetBox("InspectorBox");
            m_hoveredWidget = nullptr;
            m_activeWidget  = nullptr;
        }
    }

    if (currentSelection.obj != nullptr)
    {
        if (currentSelection.position != currentSelection.obj->GetPosition())
        {
            currentSelection.obj->SetPosition(currentSelection.position);
        }

        if (currentSelection.rotation != currentSelection.obj->GetRotation())
        {
            currentSelection.obj->SetRotation(currentSelection.rotation);
        }

        if (currentSelection.scale != currentSelection.obj->GetScale())
        {
            currentSelection.obj->SetScale(currentSelection.scale);
        }
    }
    for (const auto& widget : m_widgets)
    {
        widget->Update();
    }
}

void WidgetManager::Render()
{
    auto widgetBoxMeshes = m_widgetBoxes | std::views::values | std::views::transform(&WidgetBox::m_background);
    m_sharedContext->renderer->RenderWidgetMeshes(widgetBoxMeshes);
    m_sharedContext->renderer->RenderWidgets(m_widgets);
}

void WidgetManager::DispatchInput(Widget* l_widget, int l_index, const sf::Vector2f& l_mousePos)
{
    auto* input = m_sharedContext->inputManager;

    input->GUITookInput(true);

    if (input->IsMouseButtonPressed(sf::Mouse::Button::Left))
    {
        l_widget->OnMouseDown(l_index, l_mousePos);

        auto it = std::find_if(m_widgets.begin(), m_widgets.end(),
                               [&](const std::unique_ptr<Widget>& w)
                               { return w.get() == l_widget; });

        if (it != m_widgets.end())
        {
            std::rotate(it, it + 1, m_widgets.end());
        }
    }
    else if (input->IsMouseButtonHeld(sf::Mouse::Button::Left))
    {
        l_widget->OnMouseHold(l_index, l_mousePos);
    }
    else if (input->IsMouseButtonReleased(sf::Mouse::Button::Left))
    {
        l_widget->OnMouseUp(l_index, l_mousePos);
    }
    else if (auto delta = input->GetMouseWheelDelta(); std::abs(delta) > 1e-7f)
    {
        l_widget->OnMouseWheelScroll(l_index, delta, l_mousePos);
    }
    else if (auto enteredText = input->GetEnteredText(); enteredText != 0)
    {
        l_widget->OnTextEntered(enteredText);
    }
    else
    {
        l_widget->OnMouseEnter(l_index, l_mousePos);
    }
}

void WidgetManager::HandleInput()
{
    if (m_sharedContext->inputManager->IsWindowResized())
    {
        sf::Vector2u newSize = m_sharedContext->inputManager->GetWindowSize();
        for (const auto& widgetBox : m_widgetBoxes | std::views::values)
        {
            widgetBox->OnWindowResize(newSize);
        }
        for (const auto& widget : m_widgets)
        {
            widget->OnWindowResize(newSize);
        }
    }

    constexpr GLint InvalidIndex = -1;

    sf::Vector2f mouse = ConvertScreenCoordinates(
        m_sharedContext->inputManager->GetMousePosition());

    if (m_activeWidget)
    {
        size_t i = 0;
        for (const auto& hitbox : m_activeWidget->GetHitboxes())
        {
            if (hitbox.isIntractable && hitbox.Contains(mouse.x, mouse.y))
            {
                m_hoveredWidget    = m_activeWidget;
                m_hoveredMeshIndex = i;

                DispatchInput(m_activeWidget, i, mouse);

                if (!m_activeWidget->IsActive())
                {
                    m_activeWidget = nullptr;
                }

                return;
            }
            ++i;
        }

        DispatchInput(m_activeWidget, InvalidIndex, mouse);

        if (!m_activeWidget->IsActive())
        {
            m_activeWidget = nullptr;
        }

        return;
    }

    if (m_hoveredWidget)
    {
        const auto& hitbox = m_hoveredWidget->GetHitboxes()[m_hoveredMeshIndex];

        if (hitbox.Contains(mouse.x, mouse.y))
        {
            DispatchInput(m_hoveredWidget, m_hoveredMeshIndex, mouse);

            if (m_hoveredWidget->IsActive())
            {
                m_activeWidget = m_hoveredWidget;
            }

            return;
        }

        m_hoveredWidget->OnMouseExit(m_hoveredMeshIndex, mouse);
        m_hoveredWidget = nullptr;
    }

    for (const auto& widget : std::views::reverse(m_widgets))
    {
        size_t i = 0;
        for (const auto& hitbox : widget->GetHitboxes())
        {
            if (hitbox.isIntractable && hitbox.Contains(mouse.x, mouse.y))
            {
                m_hoveredWidget    = widget.get();
                m_hoveredMeshIndex = i;

                DispatchInput(widget.get(), i, mouse);

                if (widget->IsActive())
                {
                    m_activeWidget = widget.get();
                }

                return;
            }
            ++i;
        }
    }
}

void WidgetManager::MakeWidgetBox(std::string l_name, Borders l_borders, WidgetBoxStyle l_style)
{
    m_widgetBoxes[l_name]                 = std::make_unique<WidgetBox>(l_name, l_borders, l_style);
    WidgetBox* widgetBoxPtr               = m_widgetBoxes[l_name].get();
    widgetBoxPtr->m_background.shader     = m_sharedContext->shaderManager->Get("GUI.txt");
    widgetBoxPtr->m_background.mesh       = m_sharedContext->meshManager->Get("Quad");
    widgetBoxPtr->m_background.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(l_borders.right, l_borders.bottom)) -
                                            ConvertNDCToScreenCoordinates(sf::Vector2f(l_borders.left, l_borders.top));
}

void WidgetManager::MakeButton(std::string l_name, std::function<void()> l_callback, std::string l_widgetBoxName, ButtonStyle l_style)
{
    WidgetBox* widgetBoxPtr = m_widgetBoxes[l_widgetBoxName].get();

    GLfloat currentTop = widgetBoxPtr->m_borders.top - m_elementGap * 0.5f;
    if (!widgetBoxPtr->m_widgets.empty())
    {
        currentTop = widgetBoxPtr->m_widgets.back()->GetBorders().bottom - m_elementGap;
    }

    Borders borders;
    borders.top    = currentTop;
    borders.bottom = borders.top - m_elementHeight;
    borders.left   = widgetBoxPtr->m_borders.left + m_elementGap * 0.5f;
    borders.right  = widgetBoxPtr->m_borders.right - m_elementGap * 0.5f;

    WidgetMesh mesh;
    mesh.shader     = m_sharedContext->shaderManager->Get("GUI.txt");
    mesh.mesh       = m_sharedContext->meshManager->Get("Quad");
    mesh.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(borders.right, borders.bottom)) -
                      ConvertNDCToScreenCoordinates(sf::Vector2f(borders.left, borders.top));
    mesh.position   = sf::Vector2f((borders.left + borders.right) / 2.0f, (borders.top + borders.bottom) / 2.0f);
    mesh.scale      = sf::Vector3f((borders.right - borders.left), (borders.top - borders.bottom), 1.0f);

    WidgetText text;
    text.shader   = m_sharedContext->shaderManager->Get("GUI.txt");
    text.text     = l_name;
    text.scale    = m_elementHeight - m_elementGap;
    text.position = sf::Vector2f((borders.left + borders.right - m_sharedContext->renderer->GetTextDimensions(text).x) * 0.5f,
                                 borders.bottom + m_elementGap * 0.5);
    WidgetHitbox hitbox;
    hitbox.borders = borders;

    m_widgets.push_back(std::make_unique<Button>(l_name, l_callback, text, mesh, borders, hitbox, l_style));
    widgetBoxPtr->m_widgets.push_back(m_widgets.back().get());
}

void WidgetManager::MakeLabel(std::string l_name, std::function<std::string()> l_getStringFunction, std::string l_widgetBoxName, WidgetStyle l_style)
{
    WidgetBox* widgetBoxPtr = m_widgetBoxes[l_widgetBoxName].get();

    GLfloat currentTop = widgetBoxPtr->m_borders.top - m_elementGap * 0.5f;
    if (!widgetBoxPtr->m_widgets.empty())
    {
        currentTop = widgetBoxPtr->m_widgets.back()->GetBorders().bottom - m_elementGap;
    }

    Borders borders;
    borders.top    = currentTop;
    borders.bottom = borders.top - m_elementHeight;
    borders.left   = widgetBoxPtr->m_borders.left + m_elementGap * 0.5f;
    borders.right  = widgetBoxPtr->m_borders.right - m_elementGap * 0.5f;

    WidgetText text;
    text.shader   = m_sharedContext->shaderManager->Get("GUI.txt");
    text.text     = l_getStringFunction();
    text.position = sf::Vector2f(borders.left, borders.bottom + m_elementGap * 0.5f);
    text.scale    = m_elementHeight - m_elementGap;

    m_widgets.push_back(std::make_unique<Label>(l_name, l_getStringFunction, text, borders, l_style));
    widgetBoxPtr->m_widgets.push_back(m_widgets.back().get());
}

void WidgetManager::MakeDropdown(std::string l_name, std::vector<std::string> l_options, GLint* l_valPtr,
                                 std::function<void()> l_callback, std::string l_widgetBoxName, DropdownStyle l_style)
{
    WidgetBox* box = m_widgetBoxes[l_widgetBoxName].get();

    GLfloat currentTop = box->m_widgets.empty() ? box->m_borders.top - m_elementGap * 0.5f : box->m_widgets.back()->GetBorders().bottom - m_elementGap;

    Borders headerBorders;
    headerBorders.top    = currentTop;
    headerBorders.bottom = headerBorders.top - m_elementHeight;
    headerBorders.left   = box->m_borders.left + m_elementGap * 0.5f;
    headerBorders.right  = box->m_borders.right - m_elementGap * 0.5f;

    WidgetMesh headerMesh;
    headerMesh.shader     = m_sharedContext->shaderManager->Get("GUI.txt");
    headerMesh.mesh       = m_sharedContext->meshManager->Get("Quad");
    headerMesh.position   = sf::Vector2f((headerBorders.left + headerBorders.right) / 2.0f, (headerBorders.top + headerBorders.bottom) / 2.0f);
    headerMesh.scale      = sf::Vector3f((headerBorders.right - headerBorders.left), (headerBorders.top - headerBorders.bottom), 1.0f);
    headerMesh.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(headerBorders.right, headerBorders.bottom)) -
                            ConvertNDCToScreenCoordinates(sf::Vector2f(headerBorders.left, headerBorders.top));

    WidgetText headerText;
    headerText.shader   = m_sharedContext->shaderManager->Get("GUI.txt");
    headerText.scale    = m_elementHeight - m_elementGap;
    headerText.position = sf::Vector2f(headerBorders.left + m_elementGap * 0.5f, headerBorders.bottom + m_elementGap * 0.5f);

    WidgetHitbox headerHitbox;
    headerHitbox.borders = headerBorders;

    std::vector<WidgetMesh> optMeshes;
    std::vector<WidgetText> optTexts;
    std::vector<WidgetHitbox> optHitboxes;

    GLfloat optionTop = headerBorders.bottom;

    for (size_t i = 0; i < l_options.size(); ++i)
    {
        Borders optBorders;
        optBorders.top    = optionTop;
        optBorders.bottom = optBorders.top - m_elementHeight;
        optBorders.left   = headerBorders.left;
        optBorders.right  = headerBorders.right;

        WidgetMesh optMesh = headerMesh;
        optMesh.position   = sf::Vector2f((optBorders.left + optBorders.right) / 2.0f, (optBorders.top + optBorders.bottom) / 2.0f);
        optMesh.scale      = sf::Vector3f((optBorders.right - optBorders.left), (optBorders.top - optBorders.bottom), 1.0f);
        optMesh.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(optBorders.right, optBorders.bottom)) -
                             ConvertNDCToScreenCoordinates(sf::Vector2f(optBorders.left, optBorders.top));

        WidgetText optText = headerText;
        optText.text       = l_options[i];
        optText.position   = sf::Vector2f(optBorders.left + m_elementGap * 0.5f, optBorders.bottom + m_elementGap * 0.5f);
        optText.scale      = m_elementHeight - m_elementGap;

        WidgetHitbox optHitbox;
        optHitbox.borders = optBorders;

        optMeshes.push_back(optMesh);
        optTexts.push_back(optText);
        optHitboxes.push_back(optHitbox);

        optionTop -= m_elementHeight;
    }

    m_widgets.push_back(std::make_unique<Dropdown>(l_name, l_options, l_valPtr, l_callback,
                                                   headerText, headerMesh, headerBorders, headerHitbox,
                                                   optTexts, optMeshes, optHitboxes, l_style));
    box->m_widgets.push_back(m_widgets.back().get());
}

void WidgetManager::MakeTextInput(std::string l_name, std::string* l_valPtr, size_t l_maxLength, std::string l_widgetBoxName, TextInputStyle l_style)
{
    WidgetBox* box = m_widgetBoxes[l_widgetBoxName].get();

    GLfloat currentTop = box->m_widgets.empty() ? box->m_borders.top - m_elementGap * 0.5f : box->m_widgets.back()->GetBorders().bottom - m_elementGap;

    Borders b;
    b.top    = currentTop;
    b.bottom = b.top - m_elementHeight;
    b.left   = box->m_borders.left + m_elementGap * 0.5f;
    b.right  = box->m_borders.right - m_elementGap * 0.5f;

    WidgetMesh mesh;
    mesh.shader     = m_sharedContext->shaderManager->Get("GUI.txt");
    mesh.mesh       = m_sharedContext->meshManager->Get("Quad");
    mesh.position   = sf::Vector2f((b.left + b.right) / 2.0f, (b.top + b.bottom) / 2.0f);
    mesh.scale      = sf::Vector3f((b.right - b.left), (b.top - b.bottom), 1.0f);
    mesh.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(b.right, b.bottom)) -
                      ConvertNDCToScreenCoordinates(sf::Vector2f(b.left, b.top));

    WidgetText text;
    text.shader   = m_sharedContext->shaderManager->Get("GUI.txt");
    text.position = sf::Vector2f(b.left + m_elementGap * 0.5f, b.bottom + m_elementGap * 0.5f);
    text.scale    = m_elementHeight - m_elementGap;

    WidgetHitbox hitbox;
    hitbox.borders = b;

    m_widgets.push_back(std::make_unique<TextInput>(l_name, l_valPtr, l_maxLength, text, mesh, b, hitbox, l_style));
    box->m_widgets.push_back(m_widgets.back().get());
}

void WidgetManager::ClearWidgetBox(std::string l_widgetBoxName)
{
    if (!m_widgetBoxes.contains(l_widgetBoxName))
    {
        return;
    }

    WidgetBox* box = m_widgetBoxes[l_widgetBoxName].get();

    std::erase_if(m_widgets, [box](const std::unique_ptr<Widget>& w)
                  { return std::ranges::find(box->m_widgets, w.get()) != box->m_widgets.end(); });

    box->m_widgets.clear();
    box->m_background.isVisible = false;
}

sf::Vector2i WidgetManager::ConvertNDCToScreenCoordinates(sf::Vector2f l_point)
{
    return sf::Vector2i(static_cast<int>((l_point.x + 1.f) * 0.5f * m_sharedContext->window->GetWindowSize().x),
                        static_cast<int>((1.f - l_point.y) * 0.5f * m_sharedContext->window->GetWindowSize().y));
}

sf::Vector2f WidgetManager::ConvertScreenCoordinates(sf::Vector2i l_point)
{
    return sf::Vector2f((l_point.x / static_cast<float>(m_sharedContext->window->GetWindowSize().x) * 2.0f - 1.0f),
                        (1.0f - l_point.y / static_cast<float>(m_sharedContext->window->GetWindowSize().y) * 2.0f));
}
