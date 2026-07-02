#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

namespace cerebro {

template <typename T, std::size_t Capacity>
class LockFreeQueue {
public:
    static_assert(Capacity > 1, "Capacity must be greater than 1");

    bool try_push(T value) {
        const auto head = head_.load(std::memory_order_relaxed);
        const auto next_head = increment(head);
        if (next_head == tail_.load(std::memory_order_acquire)) {
            return false;
        }
        buffer_[head].emplace(std::move(value));
        head_.store(next_head, std::memory_order_release);
        return true;
    }

    bool try_pop(T& value) {
        const auto tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return false;
        }
        value = std::move(*buffer_[tail]);
        buffer_[tail].reset();
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

    [[nodiscard]] std::size_t size_approx() const {
        const auto head = head_.load(std::memory_order_acquire);
        const auto tail = tail_.load(std::memory_order_acquire);
        if (head >= tail) {
            return head - tail;
        }
        return Capacity - (tail - head);
    }

private:
    static constexpr std::size_t increment(std::size_t index) noexcept {
        return (index + 1U) % Capacity;
    }

    std::array<std::optional<T>, Capacity> buffer_{};
    std::atomic<std::size_t> head_{0};
    std::atomic<std::size_t> tail_{0};
};

} // namespace cerebro
