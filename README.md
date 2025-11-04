# C++ Order Book from Scratch

A full C++ simulation of the market with a custom implementation of an orderbook simulating real market dynamics with various multi-threaded agents
Built entirely from scratch — no external trading libraries — with a focus on realistic exchange-style behavior.

## Features

- **Different Order Types** – Limit, Market, Fill-or-Kill, Immediate-or-Cancel order types.
- **Custom Matching Engine** – Price-time priority with separate bid and ask books.
- **Different Multi-Threaded Agent Types** – Retail Agents, HFT (market makers) Agents, TWAP (buy/sell pressure) Agents.
- **GUI visualization** – ImGUI interface for orderbook, historical chart, order panel and agent control.
- **Memory-Safe Design** – Uses smart pointers (`std::unique_ptr`) for order storage.
- **High level of Abstraction** – Distinct classes for order management, price levels, and matching logic.

## Project Structure

```
├── main.cpp            # Entry point
├── enums.h             # Type abstraction
├── includes.h          # All includes
├── order.h             # Order class (data structure and types)
├── orderManager.h      # Tracks all live orders
├── priceLevel.h        # Stores and manages orders at a specific price
├── orderBook.h         # End orderbook
├── gateway.h           # FIFO thread-safe queue 
├── exchange.h          # Core matching engine
├── trades.h            # Trades history
├── agents.h            # Multi-threaded agents
├── agentmanager.h      # Manages agents
├── snapshot.h          # Double Buffer for lock-free GUI
├── gui.h               # GUI class
├── windows.h           # GUI windows
├── lib   
        ├── glad
        ├── imgui
        ├── implot

```

## Photos


### GUI

<table align="center">
   <tr>
    <td align="center"> <img width="1569" height="1069" alt="image" src="https://github.com/user-attachments/assets/6b7d686d-6b7e-4480-8b11-e6cb8b13fa0c" alt="graphical interface"/></td>
  </tr>
  <tr>
    <td align="center" width="400" style="border:none;">graphical interface</td>
  </tr>
</table>


### CLI

<table align="center">
   <tr>
    <td align="center"> <img src="https://github.com/user-attachments/assets/73fcfc0b-e4f6-4a6c-be2c-316da6e91039" alt="debugging mode" width="400"/></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/1969db5b-66dd-42b1-92bb-706234f38bdf" alt="orderbook" width="560"/></td>
  </tr>
  <tr>
    <td align="center" width="400" style="border:none;">debugging mode</td>
    <td align="center" width="400" style="border:none;">orderbook</td>
  </tr>
</table>



## In Progress

Currently working on **simulating various major market events**:

* Goal: **demonstrate how latency impacts order execution quality** and market dynamics.
* Goal: adding more agents, improving simulation to match real markets.


This will involve:

* Performance metrics: execution time, fill rates, and P\&L comparisons.

## License

MIT License.


