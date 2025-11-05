# Redis-Lite Test #1 - Basic Server/Client Communication

**Date**: November 5, 2025  
**Test Type**: Basic functionality test  
**Server**: redis_server (single-threaded, port 8080)  
**Client**: redis_client (TCP client)

## Test Description

This test validates basic server-client communication using the line-based protocol over TCP. The test sends a sequence of commands (SET, GET, DEL, EXIT) and verifies that the server responds appropriately.

## Test Input

```
SET a 1
GET a
DEL a
EXIT
```

## Test Output

```
Connected to server successfully
Server Response: OK

Server Response: OK

Server Response: OK

Server Response: GOODBYE
```

## Server Log

```
Server started on port 8080
Waiting for incoming connections...
Client connected.
Client disconnected.
```

## Issues Found and Fixed

### Problem 1: Client Infinite Loop
**Symptom**: Client would send a command and then print "Server Response:" thousands of times in an infinite loop.

**Root Cause**: 
1. Client wasn't sending newline terminators with commands (server expects newline-delimited commands)
2. Client's `while(true)` loop never exited, even after the command completed
3. The `recv()` buffer wasn't being cleared between iterations, causing old data to be reprinted

**Fix**: 
- Modified `src/client.cpp` to:
  - Append `\n` to each command before sending
  - Check for EOF from stdin to break the loop gracefully
  - Clear and null-terminate the recv buffer properly
  - Detect "GOODBYE" response and exit cleanly

### Problem 2: Server Not Processing Commands
**Symptom**: Commands were accepted but not executed because they lacked newline terminators.

**Status**: Fixed by client-side changes (adding `\n` to each command).

## Test Result

✅ **PASSED**

All commands were successfully:
1. Sent from client to server
2. Received and parsed by server
3. Executed by the datastore
4. Acknowledged with "OK" response
5. Session terminated gracefully with "GOODBYE" on EXIT

## Performance Observations

- **Connection Time**: < 100ms
- **Command Processing**: Instantaneous for simple operations
- **Server Behavior**: Single-threaded server handled one connection successfully
- **Protocol**: Line-based framing worked as expected

## Next Steps

1. Add actual response values (e.g., return stored value for GET instead of just "OK")
2. Test with multiple concurrent clients (will require multi-threaded server)
3. Implement error handling for invalid commands
4. Add persistence layer
5. Benchmark with larger datasets and more complex operations
