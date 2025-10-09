Name: Eliseo Padilla
Course: CS 3502 – Operating Systems
Project: Multi-Threaded Programming

Instructions:
1. To compile a phase:
   gcc -Wall -pthread phase1.c -o phase1
   ./phase1

Phase Summaries:
Phase 1 – Demonstrates race condition with inconsistent results.
Phase 2 – Fixes race condition using a mutex.
Phase 3 – Introduces deadlock using two locks.
Phase 4 – Resolves deadlock using lock ordering.

Expected Outputs:
- Phase 1: Final balance changes every run.
- Phase 2: Consistent balance each run.
- Phase 3: Program freezes (deadlock).
- Phase 4: No freezing; consistent final balances.
