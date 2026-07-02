#include "cerebro/cli.hpp"

#include <iostream>

namespace cerebro {

Cli::Cli(Engine& engine) : engine_(engine) {}

int Cli::run(int argc, char** argv) {
    if (argc < 2) {
        interactive_menu();
        return 0;
    }

    const std::string mode = argv[1];
    if (mode == "simulate") {
        return run_simulate();
    }
    if (mode == "demo") {
        return run_demo();
    }
    if (mode == "benchmark") {
        return run_benchmark();
    }
    if (mode == "replay" && argc >= 3) {
        return run_replay(argv[2]);
    }
    if (mode == "stress") {
        std::uint64_t rate = 1000;
        for (int index = 2; index + 1 < argc; ++index) {
            if (std::string(argv[index]) == "--rate") {
                rate = std::stoull(argv[index + 1]);
            }
        }
        return run_stress(rate);
    }
    if (mode == "orderbook") {
        return run_orderbook();
    }

    interactive_menu();
    return 0;
}

int Cli::run_simulate() {
    interactive_menu();
    return 0;
}

int Cli::run_demo() {
    std::cout << "CEREBRO DEMO\n";
    std::cout << "Showing a simple buy/sell match and live order book snapshot.\n\n";

    std::cout << "Initial Order Book\n";
    std::cout << engine_.order_book_snapshot(5) << '\n';

    const auto buy_order = Order{1000, "AAPL", Side::Buy, to_ticks(150.20), 100, 1};
    const auto sell_order = Order{1001, "AAPL", Side::Sell, to_ticks(150.20), 100, 2};

    std::cout << "Submitting BUY AAPL 100 @ 150.20\n";
    const auto buy_result = engine_.submit_order(buy_order);
    std::cout << (buy_result.accepted ? "Order Accepted\n" : "Order Rejected\n");

    std::cout << "Submitting SELL AAPL 100 @ 150.20\n";
    const auto sell_result = engine_.submit_order(sell_order);
    if (sell_result.accepted) {
        std::cout << "Order Accepted\n";
    } else {
        std::cout << "Order Rejected: " << sell_result.rejection_reason << '\n';
    }

    std::cout << "\nFinal Order Book\n";
    std::cout << engine_.order_book_snapshot(5) << '\n';

    const auto stats = engine_.statistics();
    std::cout << "Statistics\n";
    std::cout << "Packets processed: " << stats.packets_processed << '\n';
    std::cout << "Orders processed: " << stats.orders_processed << '\n';
    std::cout << "Trades executed: " << stats.trades_executed << '\n';
    std::cout << "Rejected orders: " << stats.rejected_orders << '\n';
    std::cout << "Average latency: " << stats.average_latency_us << " us\n";
    return 0;
}

int Cli::run_benchmark() {
    engine_.benchmark(10000);
    const auto stats = engine_.statistics();
    std::cout << stats.average_latency_us << " us average latency\n";
    std::cout << stats.throughput_packets_per_second << " packets/sec\n";
    return 0;
}

int Cli::run_replay(const std::string& path) {
    engine_.replay_csv(path);
    const auto stats = engine_.statistics();
    std::cout << stats.packets_processed << " packets replayed\n";
    return 0;
}

int Cli::run_stress(std::uint64_t rate) {
    engine_.run_stress_test(rate, std::chrono::seconds{5});
    const auto stats = engine_.statistics();
    std::cout << "Packets/sec: " << stats.throughput_packets_per_second << '\n';
    std::cout << "Latency: " << stats.average_latency_us << " us\n";
    std::cout << "Queue Utilization: " << stats.queue_size << '\n';
    return 0;
}

int Cli::run_orderbook() {
    std::cout << engine_.order_book_snapshot(5);
    return 0;
}

void Cli::interactive_menu() {
    while (true) {
        std::cout << "# ==================================\n";
        std::cout << "CEREBRO EXCHANGE SIMULATOR\n";
        std::cout << "1 View Order Book\n";
        std::cout << "2 Inject Buy Order\n";
        std::cout << "3 Inject Sell Order\n";
        std::cout << "4 Show Statistics\n";
        std::cout << "5 Run Benchmark\n";
        std::cout << "6 Exit\n";
        std::cout << "Selection: ";

        int selection = 0;
        std::cin >> selection;
        if (!std::cin) {
            return;
        }

        switch (selection) {
            case 1:
                std::cout << engine_.order_book_snapshot(5);
                break;
            case 2:
                inject_order(Side::Buy);
                break;
            case 3:
                inject_order(Side::Sell);
                break;
            case 4: {
                const auto stats = engine_.statistics();
                std::cout << stats.packets_processed << " packets processed\n";
                std::cout << stats.trades_executed << " trades executed\n";
                std::cout << stats.rejected_orders << " rejections\n";
                std::cout << stats.average_latency_us << " us avg latency\n";
                break;
            }
            case 5:
                engine_.benchmark(5000);
                std::cout << engine_.statistics().average_latency_us << " us average latency\n";
                break;
            case 6:
                return;
            default:
                std::cout << "Invalid selection\n";
                break;
        }
    }
}

void Cli::inject_order(Side side) {
    std::string symbol;
    double price = 0.0;
    std::uint32_t quantity = 0;

    std::cout << "Enter Symbol: ";
    std::cin >> symbol;
    std::cout << "Enter Price: ";
    std::cin >> price;
    std::cout << "Enter Quantity: ";
    std::cin >> quantity;

    static std::uint64_t next_id = 1000;
    const auto order = Order{next_id++, std::move(symbol), side, to_ticks(price), quantity, 0};
    const auto outcome = engine_.submit_order(order);
    if (outcome.accepted) {
        std::cout << "Order Accepted\nOrder ID: " << order.order_id << '\n';
        if (!outcome.trades.empty()) {
            std::cout << "Matched " << outcome.trades.size() << " trade(s)\n";
        }
    } else {
        std::cout << "Order Rejected: " << outcome.rejection_reason << '\n';
    }
}

} // namespace cerebro
