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

## How It Works: An Architectural Deep-Dive

This project is more than a simple order book; it's a complete, multi-threaded market simulation designed for high performance and realism. The architecture is built around classic concurrency patterns to ensure thread safety and non-blocking operation between the market engine and its participants.

The entire system is orchestrated by `main.cpp`, which initializes and launches three primary components that run in parallel:
1.  **The Agent Threads** (The *Producers*)
2.  **The Simulation Thread** (The *Consumer*)
3.  **The GUI Thread** (The *Reader*)

Here is a step-by-step breakdown of the data flow and concurrency management.

### 1. The Producers: Multithreaded Agents

The simulation begins with the `AgentManager`, which is responsible for creating and launching a unique `std::thread` for every single market participant (`agent->start()`).

* **Diverse Actors:** As seen in `agentmanager.h` and `agents.h`, the system spawns hundreds of agents of different types (Retail, HFT, MarketMaker, TWAP, Momentum).
* **Concurrent Action:** Each agent runs in its own loop, independently making decisions. They analyze market data (from their view of the `OrderBook` and `Trades`) and decide to send new orders, cancel existing ones, or modify them.
* **The Concurrency Problem:** This creates a classic "many-producers" problem. You have hundreds of threads all trying to submit work (order commands) to the central exchange simultaneously. This is where the `Gateway` comes in.

### 2. The Gateway: A Thread-Safe Producer-Consumer Queue

The `Gateway` (`gateway.h`) is the central nervous system for all order-related commands. It elegantly solves the "many-producers" problem using a **thread-safe producer-consumer queue**.

* **Producer Side (Agents):** When an agent wants to place an order, it creates a `CommandPtr` (a `std::unique_ptr<Command>`) and calls `gateway.Push()`. This function:
    1.  Locks a `std::mutex` to gain exclusive access to the internal queue.
    2.  Pushes the command onto the `std::queue`.
    3.  Notifies one waiting thread via a `std::condition_variable` (`cv.notify_one()`).
    4.  Unlocks the mutex.
    This entire operation is fast and ensures that agents can safely submit commands from any thread.

* **Consumer Side (Simulation):** The `RunSimulation` function, running in its own dedicated thread, acts as the *single consumer*. It continuously calls `gateway.WaitAndPop()`. This function:
    1.  Locks the same `std::mutex`.
    2.  Waits on the `std::condition_variable` (`cv.wait_for`) until the queue is no longer empty.
    3.  Pops the command from the queue, moves it to the output variable, and returns.

This design is highly efficient. The agent threads (producers) and the simulation thread (consumer) are decoupled, only interacting through this small, synchronized queue.

### 3. The Simulation Thread: A Serialized Matching Engine

The `RunSimulation` function (in `main.cpp`) is the heart of the matching engine. By acting as the *single consumer* of the `Gateway`, it creates a powerful architectural advantage: **all matching logic is serialized.**

* **Single-Threaded Processing:** This thread loops, pulls one `Command` from the `Gateway`, and processes it to completion before pulling the next.
* **No Locks Needed:** Because only this *one thread* ever calls `exchange.AddOrder()`, `exchange.CancelOrder()`, or `exchange.ModifyOrder()`, the `Exchange`, `OrderBook`, `OrderManager`, and `PriceLevel` classes **do not need any internal mutexes.**
* **High Performance:** This "single-writer" design is extremely fast. There is zero lock contention within the matching engine itself, which is a common bottleneck in real-world exchanges. The `OrderBook` (using `std::map`) can be modified without ever blocking.
* **Core Logic:** The `Exchange` (`exchange.h`) class receives the command and executes the matching logic (e.g., `LimitMatch`, `MarketMatch`). It follows strict **price-time priority**, matching incoming orders against the resting book, filling or partially-filling orders, and generating trade reports in the `Trades` object.

### 4. The GUI & The Reader-Writer Problem

A new concurrency problem arises:
* The **Simulation Thread** (the *Writer*) is constantly modifying the `OrderBook` and `Trades` data.
* The **GUI Thread** (the *Reader*, running on `main`) needs to read this data on every frame to render the order book visualization and price chart.

If the GUI thread tried to read the `OrderBook` maps directly, it would have to lock a mutex. This would force the high-speed Simulation Thread to pause every time the GUI renders (60+ times a second), completely destroying the matching engine's performance.

### 5. The Solution: Lock-Free Double-Buffering via Snapshots

This project solves the reader-writer problem with a sophisticated, **lock-free** data transfer mechanism: the `DataBuffer` and `Snapshot` (`snapshot.h`).

* **The `DataBuffer`:** This class holds two `Snapshot` objects (`dataBuffers[2]`) and an `std::atomic<int> bufferIndex`. This is a classic **double-buffering** pattern.
* **The `Snapshot`:** This struct is just simple `std::vector`s. It's designed to hold a *copy* of the market data.

This is the flow:

1.  **The Writer (Simulation Thread):**
    * Periodically (every 16ms in `RunSimulation`), it calls `dataBuffer.Write()`.
    * This function writes all `OrderBook` and `Trades` data into the *inactive* buffer (the one *not* being read by the GUI).
    * This is a copy operation. The live `OrderBook` is *never* locked; the simulation thread just quickly copies its state.
    * Once the copy is complete, it atomically "flips" the `bufferIndex` (`bufferIndex.store(writeIndex, std::memory_order_release)`). This instantly makes the newly written buffer the "active" one.

2.  **The Reader (GUI Thread):**
    * On every frame (in `GUI::MainLoop`), it calls `dataBuffer.Read()`.
    * This function atomically reads the *current* `bufferIndex` (`bufferIndex.load(std::memory_order_acquire)`).
    * It then returns a *copy* of the "active" `Snapshot` buffer.

This design is brilliant for its performance:
* The **Simulation Thread never waits** for the GUI.
* The **GUI Thread never waits** for the Simulation.
* They communicate **without any mutexes, locks, or condition variables**, using only a single atomic integer. The GUI always has a perfectly consistent (though a few milliseconds stale) view of the market, which is exactly how real-world visualization tools work.

### Summary

This architecture demonstrates a robust, high-performance, and concurrent design:

* **Many-Producers (Agents)** are safely handled by a **Producer-Consumer Queue (Gateway)**.
* **Single-Consumer (Simulation)** ensures the **Matching Engine (Exchange/OrderBook)** is serialized and requires **zero internal locks**, maximizing throughput.
* A **Reader-Writer Problem (GUI vs. Simulation)** is solved using a **lock-free double-buffering (DataBuffer/Snapshot)** mechanism, ensuring non-blocking visualization.
---

### Realistic Market Participants (Agents)

The simulation comes alive thanks to a diverse ecosystem of agents, each modeling a real-world trading strategy. Every agent runs in its own thread, reacting to the market in real-time.

#### `HFTAgent` (High-Frequency Trader)
This is the most sophisticated agent, modeling a true HFT market-making and statistical arbitrage strategy.
* **Dynamic Quoting:** It calculates market volatility and short-term trends by analyzing recent trade data.
* **Dynamic Spread:** The agent's bid-ask spread is not fixed. It widens its spread during high volatility (to reduce risk) and tightens it in calm markets.
* **Inventory Skew:** The agent manages its own P&L and inventory risk. If it becomes too "long" (holding too much inventory), it **skews** its quotes downwards, offering a slightly better "ask" price and a worse "bid" price to attract sellers and discourage buyers. The opposite is done when "short".
* **Trend Following:** It incorporates the short-term market trend into its "fair price" calculation, skewing its quotes to trade with the momentum, not against it.
* **Risk Management:** It features a hard **PnL stop-loss** and a **volatility circuit breaker**, causing it to pull all its quotes from the market if conditions become too dangerous.
* **High-Speed:** It cancels and replaces its quotes (`lastBidID`, `lastAskID`) extremely frequently (every 40-120ms) to adapt to new information.

#### `MomentumAgent`
This agent models a trend-following strategy, attempting to capitalize on sustained price movements.
* **Signal Generation:** It calculates two key indicators:
    1.  **Momentum:** A simple moving average crossover (current price vs. 100-period average) to identify the long-term trend (`UPTREND`, `DOWNTREND`, `NEUTRAL`).
    2.  **RSI (Relative Strength Index):** A classic oscillator to identify "overbought" or "oversold" conditions, preventing the agent from buying the top or selling the bottom.
* **Execution:** The agent enters a long position (buys) when an `UPTREND` is detected and RSI is not "overbought". It enters a short position (sells) on a `DOWNTREND` when RSI is not "oversold". It flattens its position (exits) when the trend becomes `NEUTRAL`.
* **Risk Management:** Includes a PnL-based `stopLoss`.

#### `MarketMakerAgent`
This agent models a simpler, spread-capturing market maker.
* **Passive Quoting:** It only participates if the market's bid-ask spread is wide enough to be profitable (`minSpreadToJoin`).
* **Spread Capture:** It places `Limit` orders just inside the best bid and best ask, aiming to be filled on both sides and profit from the difference (the spread).
* **Inventory Management:** It manages risk by adjusting its order *quantity* based on its current position. If it's holding too much inventory, it will quote a smaller size.

#### `TWAPAgent` (Time-Weighted Average Price)
This agent simulates a large institutional fund or broker executing a massive order for a client without causing significant market impact.
* **Order Slicing:** It takes a single large order (e.g., sell 50,000 contracts) and a long time duration (e.g., 1 minute).
* **Paced Execution:** It slices this "parent" order into many small "child" orders (e.g., 60 orders of ~833 contracts).
* **Market Orders:** It submits these small orders as `Market` orders at regular time intervals (e.g., one every second) to execute gradually and achieve a price close to the average market price over that minute. This is a classic "buy/sell pressure" agent.

#### `RetailAgent`
This agent simulates the "dumb money" or noise traders in the market.
* **Random Strategy:** It places `Limit` orders at random times with a random quantity.
* **Price Insensitive:** The price is based on the last spot price plus a large random offset, simulating a less-informed trader who is not as price-sensitive.
* **Slow:** It acts infrequently (every 500-1500ms), simulating human-level reaction time.

---

### Summary

This architecture demonstrates a robust, high-performance, and concurrent design:

* **Many-Producers (Agents)** are safely handled by a **Producer-Consumer Queue (Gateway)**.
* **Single-Consumer (Simulation)** ensures the **Matching Engine (Exchange/OrderBook)** is serialized and requires **zero internal locks**, maximizing throughput.
* A **Reader-Writer Problem (GUI vs.Simulation)** is solved using a **lock-free double-buffering (DataBuffer/Snapshot)** mechanism, ensuring non-blocking visualization.
* **Realistic Market Dynamics** are achieved through a diverse ecosystem of multi-threaded agents, each implementing a distinct and sophisticated real-world trading strategy with its own risk management.


## In Progress

Currently working on **simulating various major market events**:

* Goal: **demonstrate how latency impacts order execution quality** and market dynamics.
* Goal: adding more agents, improving simulation to match real markets.


This will involve:

* Performance metrics: execution time, fill rates, and P\&L comparisons.

## License

MIT License.



