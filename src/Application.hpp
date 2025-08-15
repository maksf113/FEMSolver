#pragma once
#include "geometry/Domain.hpp"
#include"solver/Solver.hpp"
#include "graphics/Renderer.hpp"
#include "window/Window.hpp"
#include "window/InputManager.hpp"
#include "window/GUI.hpp"

class Application
{
public:
    Application(uint32_t width, uint32_t height);
    ~Application() = default;
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    void run();
private:
    Domain m_domain;
    std::unique_ptr<Solver> m_solver;
    std::shared_ptr<Window> m_window;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<GUI> m_gui;
    std::shared_ptr<Renderer> m_renderer;
};

Application::Application(uint32_t width, uint32_t height)
{
    m_solver = std::make_unique<Solver>(m_domain.getTriangulation());
    m_window = std::make_shared<Window>(width, height, "FEMSolver");
    m_renderer = std::make_shared<Renderer>(width, height);
    m_inputManager = std::make_unique<InputManager>();
    m_inputManager->setCallbacks(m_window->get());
    m_inputManager->addReciever(m_renderer);
    m_inputManager->addReciever(m_window);
    m_gui = std::make_unique<GUI>(m_window->get());
    m_renderer->setVertices(*m_solver);
}

inline void Application::run()
{
    while (!m_window->shouldClose())
    {
        m_window->pollEvents();
        m_renderer->processInput(*m_inputManager);
        m_renderer->draw();  
        m_gui->createFrame(*m_renderer);
        m_gui->draw();
        m_window->swapBuffers();
        m_inputManager->endFrame();
    }
}