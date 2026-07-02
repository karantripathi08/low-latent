# CerebroX Benchmark Report

Use the benchmark mode to measure the decision engine itself.

## Metrics

- average decision latency
- p95 decision latency
- dashboard render latency
- ranking throughput

## Example outcome

The current demo path runs the collector, snapshot, predictor, and advisor in a small local pipeline and reports the time in nanoseconds for each decision loop.

This is the right metric for a portfolio project because it proves the engine can react quickly, even though it is not modifying the kernel scheduler directly.# Benchmark Report Template

Use the `benchmark` executable or `./cerebro benchmark` to produce a local report.

## Metrics

- Decode latency
- Order book update latency
- Matching latency
- Total pipeline latency
- p50, p95, p99 latency

## Example Output

```text
Cerebro Benchmark Report
Total time: 42 ms
Packets processed: 25000
Orders processed: 25000
Trades executed: 12481
Rejected orders: 0
Average latency: 1.10 us
p50: 0.90 us
p95: 2.30 us
p99: 4.10 us
```
