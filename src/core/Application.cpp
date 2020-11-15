#include "core/Application.h"

#include "renderer/Renderer.h"

namespace bubo {

    Application* Application::s_appInstance = nullptr;

    Application::Application() {
        BUBO_ASSERT(!s_appInstance, "Application already created!")
        s_appInstance = this;

        m_window = std::make_unique<Window>(WindowProperties_t());
        m_window->setEventCallbackFunction(BIND_EVENT_FUNC(onEvent));

        m_shaderProgram = std::make_unique<Shader>("../../res/shaders/vertex.shader", "../../res/shaders/fragment.shader");

        float vertices[] = {
                 0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f
        };

        unsigned int indices[] {
                0, 1, 3,
                1, 2, 3
        };

        m_vertexArray = std::make_shared<VertexArrayObject>();

        m_vertexBuffer = std::make_shared<VertexBufferObject>(vertices, sizeof (vertices));
        m_vertexBuffer->setFormat({
            {ShaderDataType::Vec3, "aPos"},
            {ShaderDataType::Vec4, "aColor"}
        });

        m_vertexArray->addVertexBuffer(m_vertexBuffer);

        m_indexBuffer = std::make_shared<IndexBufferObject>(indices, sizeof (indices), 6);
        m_vertexArray->setVertexBuffer(m_indexBuffer);

        m_vertexArray->unbind();

    }

    Application::~Application() {}

    void Application::run() {

        while (m_running) {
            m_shaderProgram->bind();
            m_shaderProgram->setFloat4("u_Color",
                                       glm::vec4((sin(2*glfwGetTime()) + 1) / 4.0f,
                                                 (cos(2*glfwGetTime()) + 1) / 4.0f,
                                                 (sin(2*glfwGetTime()) + 1) / 4.0f,
                                                 0.0f));
            m_shaderProgram->unbind();

            Renderer::setColor(glm::vec4(.1f, .1f, .1f, 1.0f));
            Renderer::clear();

            Renderer::beginScene();

            Renderer::submit(m_shaderProgram, m_vertexArray);

            Renderer::endScene();

            m_window->update();
        }

    }

    void Application::onEvent(Event &e) {
        EventDispatcher dispatcher(e);

        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(onWindowResize));
    }

    bool Application::onWindowClose(WindowCloseEvent &e) {
        m_running = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent &e) {
        BUBO_DEBUG_TRACE(e.toString());
        Renderer::setViewport(0, 0, e.getWindowWidth(), e.getWindowHeight());
        return true;
    }
}