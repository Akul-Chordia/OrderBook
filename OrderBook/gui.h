//
//  gui.h
//  OrderBook
//
struct ScrollingBuffer{
    static const int MaxSize = 4000;
    int Offset;
    std::vector<ImVec2> Data;

    ScrollingBuffer() {
        Offset = 0;
        Data.reserve(MaxSize);
    }

    void AddPoint(const float x, const float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }

    void Erase() {
        if (Data.size() > 0) {
            Data.clear();
            Offset = 0;
        }
    }
};


class GUI{
private:
    DataBuffer& dataBuffer;
    Gateway& gateway;
    std::atomic<bool>& flag;
    GLFWwindow* window = nullptr;
    
    ScrollingBuffer AskHistory;
    ScrollingBuffer BidHistory;
    
    Price price;
    Quantity quantity;
    Side side;
    OrderType orderType;
    
    Price bestAsk = 100, bestBid = 100, midPrice;
    
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
    
    void OrderBookVisualization(const Snapshot& snapshot);
    void Debug(const Snapshot& snapshot);
    void SendOrder(Gateway& gateway);
    
    void DrawUI(){
        const Snapshot& snapshot = dataBuffer.Read();
        OrderBookVisualization(snapshot);
        Debug(snapshot);
        SendOrder(gateway);
    }
};

