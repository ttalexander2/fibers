# fibers


TODO:

- Fibers should be created at start time
- Jobs exist in job_handle queues (low/mid/high priority)
- Worker thread takes a fiber from the fiber pool, allocates new stack space using the free list,
then starts execution
- If fiber yields, fiber is stored on wait list, atomic counter is associated with its dependency
and the worker thread retrieves a new job_handle.
- 