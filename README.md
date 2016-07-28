# Readers-writers
Readers-writers problem I, II and III written in C.

1. rw1 (1st RW problem) - variant favorable for readers. Readers don't have to wait in queue for entry to the library if other reader is inside. Writers can starve. Synchronization mechanism: mutex.
2. rw2 (2nd RW problem) - variant favorable for writers. Readers cannot enter the library if at least one writer waits in queue. Readers can starve. Synchronization mechanism: mutex with condition variable.
3. rw3 (3rd RW problem) - variant without starvation, fulfilled when semaphores comply with FIFO order when locking and unlocking threads. Synchronization mechanism: semaphore.