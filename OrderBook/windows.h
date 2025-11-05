//
//  windows.h
//  OrderBook
//
void GUI::OrderBookVisualization(const Snapshot& snapshot){
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    
    ImGui::Begin("OrderBookVisualization", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    const float screen_width = viewport->WorkSize.x;
    const float screen_height = viewport->WorkSize.y;
    
    static const float max_bar_width_percentage = 0.2f;
    const float max_bar_width_pixels = screen_width * max_bar_width_percentage;
    
    static int visible_max_price_cents = 110;
    static int visible_min_price_cents = 90;
    Quantity max_volume = 1;
    
    static float time = 0;
    time += ImGui::GetIO().DeltaTime;
    
    if (!snapshot.bids.empty()){
        bestBid = snapshot.bids.front().first;
        for (const auto& [price, qty] : snapshot.bids) if (qty > max_volume) max_volume = qty;
        BidHistory.AddPoint(time, snapshot.bids.front().first / 100.0f);
    }
    if (!snapshot.asks.empty()){
        bestAsk = snapshot.asks.front().first;
        for (const auto& [price, qty] : snapshot.asks) if (qty > max_volume) max_volume = qty;
        AskHistory.AddPoint(time, snapshot.asks.front().first / 100.0f);
    }
    
    midPrice = (bestAsk+bestBid)/2;
    
    if (!snapshot.bids.empty() && !snapshot.asks.empty()){
        
        const float center_y = screen_height / 2.0f;
        static const float price_level_height = 2.0f;
        static const float price_tick_size = 5.0f;
        
        static const ImU32 bid_color = IM_COL32(100, 255, 100, 255);
        static const ImU32 ask_color = IM_COL32(255, 100, 100, 255);
        
        // DRAW BIDS
        for (const auto& [price, qty] : snapshot.bids) {
            float price_offset_ticks = static_cast<float>(midPrice - price) / price_tick_size;
            float y_pos = center_y + (price_offset_ticks * price_level_height);
            if (y_pos > screen_height) break;
            float bar_width = (static_cast<float>(qty) / max_volume) * max_bar_width_pixels;
            draw_list->AddRectFilled(ImVec2(screen_width - max_bar_width_pixels, y_pos), ImVec2(screen_width + bar_width - max_bar_width_pixels, y_pos + price_level_height), bid_color);
        }
        
        // DRAW SPREAD & MID-LINE
        draw_list->AddRectFilled(ImVec2(0, center_y + static_cast<float>(midPrice - bestBid) * price_level_height / price_tick_size), ImVec2(screen_width, center_y + static_cast<float>(midPrice - bestAsk) * price_level_height / price_tick_size), IM_COL32(255, 255, 255, 20));
        draw_list->AddRectFilled(ImVec2(0, center_y + 0.5), ImVec2(screen_width, center_y - 0.5), IM_COL32(255, 255, 255, 100));

        // DRAW ASKS
        for (auto it = snapshot.asks.begin(); it != snapshot.asks.end(); ++it) {
            const auto& [price, qty] = *it;
            float price_offset_ticks = static_cast<float>(price - midPrice) / price_tick_size;
            float y_pos = center_y - price_level_height - (price_offset_ticks * price_level_height);
            if (y_pos < 0) break;
            float bar_width = (static_cast<float>(qty) / max_volume) * max_bar_width_pixels;
            draw_list->AddRectFilled(ImVec2(screen_width - max_bar_width_pixels, y_pos), ImVec2(screen_width + bar_width - max_bar_width_pixels, y_pos + price_level_height), ask_color);
        }
        
        float ticks_in_half_screen = (screen_height / 2.0f) / price_level_height;
        float price_range_cents = ticks_in_half_screen * price_tick_size;
        visible_max_price_cents = midPrice + price_range_cents;
        visible_min_price_cents = midPrice - price_range_cents;
    }{
        float plot_width = screen_width - (max_bar_width_pixels);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::BeginChild("PricePlot", ImVec2(plot_width, viewport->WorkSize.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


        if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, -1), ImPlotFlags_NoFrame | ImPlotFlags_NoLegend | ImPlotFlags_NoInputs)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxisLimits(ImAxis_X1, time - 60, time, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, visible_min_price_cents/ 100.0f, visible_max_price_cents/ 100.0f, ImGuiCond_Always); // Using your fixed limits
            if (!BidHistory.Data.empty()){
                ImPlot::SetNextLineStyle(ImVec4(0.39f, 1.0f, 0.39f, 1.0f));
                ImPlot::PlotLine("bids", &BidHistory.Data[0].x, &BidHistory.Data[0].y, BidHistory.Data.size(),0, BidHistory.Offset, 2 * sizeof(float));
            }
            if (!AskHistory.Data.empty()){
                ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.39f, 0.39f, 1.0f));
                ImPlot::PlotLine("ask", &AskHistory.Data[0].x, &AskHistory.Data[0].y, AskHistory.Data.size(), 0, AskHistory.Offset, 2 * sizeof(float));
            }
            ImPlot::EndPlot();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}


void GUI::Debug(const Snapshot& snapshot){
    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Snapshot Bids Count: %zu", snapshot.bids.size());
    ImGui::Text("Snapshot Asks Count: %zu", snapshot.asks.size());
    ImGui::Separator();
    ImGui::Text("Best Bid: %.2f", bestBid / 100.0f);
    ImGui::Text("Best Ask: %.2f", bestAsk / 100.0f);
    ImGui::Text("Spot Price: %.2f", snapshot.priceHistory.back()/100.0f);
    ImGui::End();
}

void GUI::PriceChart(const Snapshot& snapshot){
    ImGui::Begin("Price Chart");
    if (snapshot.priceHistory.empty()) {
        ImGui::Text("No trade data yet...");
    } else {
        if (ImPlot::BeginPlot("Spot Price", ImVec2(-1, -1), ImPlotFlags_NoMouseText)) {
            ImPlot::SetupAxes("Trade Index", "Price", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, 500, ImGuiCond_Always);
            // Calculate min, max, and padding
            float minPrice = *std::min_element(snapshot.priceHistory.begin(), snapshot.priceHistory.end());
            float maxPrice = *std::max_element(snapshot.priceHistory.begin(), snapshot.priceHistory.end());
            float range = maxPrice - minPrice;
            float padding = range * 0.1f;
            if (range == 0) padding = minPrice * 0.1f;
            float yMin = minPrice - padding;
            float yMax = maxPrice + padding;
            ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImGuiCond_Always);
            ImPlot::PlotLine(
                "Spot Price",
                snapshot.priceHistory.data(),
                static_cast<int>(snapshot.priceHistory.size())
            );
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}


void GUI::SendOrder(Gateway& gateway){
    ImGui::Begin("Send Order");
    ImGui::InputScalar("Price", ImGuiDataType_S64, &price);
    ImGui::InputScalar("Quantity", ImGuiDataType_S64, &quantity);
    static OrderID orderIDcounter = 1'000'000'000;
    
    ImGui::RadioButton("Market", &orderType, OrderType::Market);
    ImGui::SameLine();
    ImGui::RadioButton("Limit", &orderType, OrderType::Limit);
    
    if (ImGui::Button("Buy")){
        if (orderType == OrderType::Market){
            auto order = std::make_unique<Order>(orderIDcounter++, quantity, OrderType::Market, Side::Buy);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));
        } else {
            auto order = std::make_unique<Order>(orderIDcounter++, price, quantity, OrderType::Limit, Side::Buy);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Sell")){
        if (orderType == OrderType::Market){
            auto order = std::make_unique<Order>(orderIDcounter++, quantity, OrderType::Market, Side::Sell);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));
        } else {
            auto order = std::make_unique<Order>(orderIDcounter++, price, quantity, OrderType::Limit, Side::Sell);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));
        }
    }
    ImGui::End();
}


