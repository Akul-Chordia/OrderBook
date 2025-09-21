//
//  ui.h
//  OrderBook
//
void GUI::DrawUI() {
    const Snapshot& snapshot = dataBuffer.Read();
    
    
    ImGui::Begin("Performance");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
        if (!snapshot.bids.empty()) {
            ImGui::Text("Best Bid: %.2f", snapshot.bids.front().first / 100.0f);
        }
        if (!snapshot.asks.empty()) {
            ImGui::Text("Best Ask: %.2f", snapshot.asks.front().first / 100.0f);
        }
    ImGui::End();
}
