Special thanks to Beej's Guide to Network Programming. The functions that establishes sockets and connecting with other terminals are heavily inspired by the listener.c and talker.c files from Beej's Guide to Network Programming on page 30 and 33. Since I am not experienced and are not sure about what I am doing with the sockets, I closely followed the structure of his code to how I was able to connect to the other terminal. Also thanks to the Dr Brian Fraser for his amazing workshops content that helped me tremendously with this assignment

s-talk memory leaks:

Valgrind still indicates 4 blocks of still reachable memory originating from pthread_cancel_init (I assume). Feel free to deduct marks from me if I hadn't handled that to the best of my ability.

s-talk errors:

Condition Variables: I haven't implemented this, so I take full mark deduction for this

Sleep Functions: This worked merely as a cancellation point for the pthread_cancel function, and has no relation to condition variables or mutexes


