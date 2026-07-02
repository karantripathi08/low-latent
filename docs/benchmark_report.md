# Benchmark Report Template

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
