# C++ Order Book from Scratch

A fully custom implementation of an order book in C++, supporting multiple order types and designed for speed and extensibility.  
Built entirely from scratch — no external trading libraries — with a focus on realistic exchange-style behavior.

## Features

- **Limit Orders** – Rest in the book until matched or canceled.
- **Market Orders** – Execute immediately against the best available price levels.
- **Fill-or-Kill (FOK)** – Execute in full immediately or cancel entirely.
- **Immediate-or-Cancel (IOC)** – Execute immediately for the available quantity, cancel any remainder.
- **Custom Matching Engine** – Price-time priority with separate bid and ask books.
- **Memory-Safe Design** – Uses smart pointers (`std::unique_ptr`) for order storage.
- **High level of Abstraction** – Distinct classes for order management, price levels, and matching logic.

## Project Structure

```
├── main.cpp            # Entry point
├── emuns.h             # Type abstraction
├── includes.h          # All includes
├── order.h             # Order class (data structure and types)
├── orderManager.h      # Tracks all live orders
├── priceLevel.h        # Stores and manages orders at a specific price
├── orderBook.h         # End orderbook
├── gateway.h           # FIFO thread-safe queue 
├── exchange.h          # Core matching engine
├── trades.h            # Trades history
├── agents.h            # Multi-threaded agents

```

## Photos
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

Currently working on a **large-scale market simulation**:


* **HFT threads** with significantly lower latency will operate alongside, exploiting their speed advantage.
* Goal: **demonstrate how latency impacts order execution quality** and market dynamics.

This will involve:

* Multi-threaded architecture using `std::thread` and synchronization primitives.
* Event-driven simulation loop with per-agent latencies.
* Performance metrics: execution time, fill rates, and P\&L comparisons.

## License

MIT License.


