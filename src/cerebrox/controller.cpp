#include "cerebrox/controller.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <thread>

namespace cerebrox {

namespace {

const char* color_for_recommendation(Recommendation recommendation) {
    switch (recommendation) {
        case Recommendation::ProtectForeground: return "\033[1;32m";
        case Recommendation::IncreasePriority: return "\033[1;36m";
        case Recommendation::DecreasePriority: return "\033[1;33m";
        case Recommendation::PinToCore: return "\033[1;35m";
        case Recommendation::MoveToBackground: return "\033[1;31m";
        case Recommendation::BatchIOTasks: return "\033[1;34m";
        case Recommendation::NoChange: return "\033[0m";
    }
    return "\033[0m";
}

const char* recommendation_name(Recommendation recommendation) {
    switch (recommendation) {
        case Recommendation::ProtectForeground: return "ProtectForeground";
        case Recommendation::IncreasePriority: return "IncreasePriority";
        case Recommendation::DecreasePriority: return "DecreasePriority";
        case Recommendation::PinToCore: return "PinToCore";
        case Recommendation::MoveToBackground: return "MoveToBackground";
        case Recommendation::BatchIOTasks: return "BatchIOTasks";
        case Recommendation::NoChange: return "NoChange";
    }
    return "NoChange";
}

const char* class_name(ProcessClass classification) {
    switch (classification) {
        case ProcessClass::Interactive: return "Interactive";
        case ProcessClass::Game: return "Game";
        case ProcessClass::Background: return "Background";
        case ProcessClass::Server: return "Server";
        case ProcessClass::Unknown: return "Unknown";
    }
    return "Unknown";
}

} // namespace

void Controller::print_dashboard_header(const std::string& title) const {
    std::cout << "\033[1;35m==================================================\033[0m\n";
    std::cout << "\033[1;35m" << title << "\033[0m\n";
    std::cout << "\033[1;35m==================================================\033[0m\n";
}

void Controller::print_decision(const Decision& decision) const {
    const auto* color = color_for_recommendation(decision.recommendation);
    std::cout << color
              << std::left << std::setw(14) << decision.process_name
              << std::setw(8) << decision.pid
              << std::setw(14) << class_name(decision.classification)
              << std::setw(9) << std::fixed << std::setprecision(1) << decision.predicted_cpu_percent
              << std::setw(8) << std::setprecision(2) << decision.burst_score
              << std::setw(8) << std::setprecision(2) << decision.confidence
              << std::setw(22) << recommendation_name(decision.recommendation)
              << decision.rationale
              << "\033[0m\n";
}

std::vector<Decision> Controller::analyze_trace(const std::vector<ProcessSample>& trace) {
    for (const auto& sample : trace) {
        snapshot_.ingest(sample);
    }

    return advisor_.rank(snapshot_.all());
}

void Controller::render_dashboard(const std::vector<Decision>& decisions) const {
    print_dashboard_header("CEREBROX WORKLOAD DASHBOARD");
    std::cout << "Process       PID     Class         CPU%     Burst   Conf    Recommendation        Rationale\n";
    std::cout << "---------------------------------------------------------------------------------------------------\n";
    for (const auto& decision : decisions) {
        print_decision(decision);
    }
}

void Controller::demo() {
    const auto trace = collector_.synthetic_trace();
    const auto start = std::chrono::high_resolution_clock::now();
    const auto decisions = analyze_trace(trace);
    const auto end = std::chrono::high_resolution_clock::now();
    const auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    render_dashboard(decisions);

    std::cout << "\nOutcome: CerebroX detects foreground, bursty, and server-like workloads before pressure peaks.\n";
    std::cout << "Decision latency: " << latency << " ns\n";
}

void Controller::live_watch(std::size_t rounds, std::chrono::milliseconds pause) {
    for (std::size_t round = 0; round < rounds; ++round) {
        const auto round_start = std::chrono::high_resolution_clock::now();
        const auto decisions = analyze_trace(collector_.sample_once());
        const auto round_end = std::chrono::high_resolution_clock::now();
        const auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(round_end - round_start).count();
        render_dashboard(decisions);
        std::cout << "Decision loop latency: " << latency << " ns\n\n";
        std::this_thread::sleep_for(pause);
    }
}

void Controller::benchmark(std::size_t iterations) {
    const auto trace = collector_.synthetic_trace();
    std::vector<long long> samples;
    samples.reserve(iterations);

    for (std::size_t index = 0; index < iterations; ++index) {
        const auto start = std::chrono::high_resolution_clock::now();
        const auto decisions = analyze_trace(trace);
        const auto end = std::chrono::high_resolution_clock::now();
        samples.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        if (index == 0) {
            render_dashboard(decisions);
        }
    }

    std::sort(samples.begin(), samples.end());
    const auto total = std::accumulate(samples.begin(), samples.end(), 0LL);
    const auto average = total / static_cast<long long>(samples.size());
    const auto p95 = samples[static_cast<std::size_t>(samples.size() * 0.95)];
    std::cout << "Average decision latency: " << average << " ns\n";
    std::cout << "p95 decision latency: " << p95 << " ns\n";
}

} // namespace cerebrox
