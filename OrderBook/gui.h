//
//  gui.h
//  OrderBook
//
class GUI{
private:
    DataBuffer& dataBuffer;
    Gateway& gateway;
    std::atomic<bool>& flag;
    GLFWwindow* window = nullptr;
public:
    GUI(DataBuffer& dataBuffer, Gateway& gateway, std::atomic<bool>& flag)
    : dataBuffer(dataBuffer),
    gateway{gateway},
    flag{flag}
    {}
    
    ~GUI(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    };
    
    void run(){
        Initialize();
        MainLoop();
    }
    
private:
    void Initialize(){
        glfwInit();
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
        
        window = glfwCreateWindow(1920, 1080, "Order Book Visualizer", NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 150");
    }
    
    void MainLoop(){
        while (!glfwWindowShouldClose(window) && flag) {

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DrawUI();

            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::Render();
            
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
        flag = false;
    }
    
    void DrawUI();
    
};

