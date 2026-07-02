# Performance Analysis

## Optimizations Included

- Preallocated containers where practical
- Move-aware data flow for packets and outcomes
- Optional lock-free SPSC queue implementation
- Price-level aggregation for faster best bid and ask queries
- Background logger to reduce front-path blocking

## Future Improvements

- NUMA-aware partitioning of symbol buckets
- Cache-line padding in hot counters and queues
- Batched matching and batched risk checks
- Persistent event journaling
