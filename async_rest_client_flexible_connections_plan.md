# Async REST Client Flexible Connection Management Implementation Plan

## Overview

This document outlines the implementation plan to enhance the `async_rest_client` with flexible connection management capabilities. The goal is to support two distinct workflows:

1. **Pre-connection workflow**: Users call `connect()` ahead of time, then make multiple requests to the same host without handshake overhead. Note that if the host for a given request task, we still change the host we are connected to - this is fully allowed.
2. **Auto-connection workflow**: Users call verb methods directly with different hosts, and the client automatically manages connections

## Key Requirements

- Reuse connections when making requests to the same host
- Automatically disconnect and reconnect when the host changes
- Support automatically connecting to the host in a request when calling verb methods without calling explicitly calling `connect()` first.
- Maintain serial request processing with single-threaded event loop architecture
- Use `boost::url` for all URL parsing and component extraction

## Implementation Phases

### Phase 1: Task System Refactoring

**Goal**: Make tasks URL-aware and connection-independent

#### 1.1 Update `base_task` interface

- Add `url()` pure virtual accessor returning `const boost::url&`
- Remove need for separate host/port methods since we can extract them from URL

#### 1.2 Update `typed_task` class

- Store `boost::url` object as member variable (parsed from URL string)
- Update constructor to create `boost::url` directly from URL string (throws on invalid URLs)
- Implement `url()` method
- Constructor will throw exception on invalid URLs - up to user to supply valid URLs

#### 1.3 Update request builders

- Modify to accept `boost::url` objects instead of URL strings
- Use `url.encoded_target()` for the HTTP target (path + query + fragment)
- Use `url.host()` for the Host header
- Ensure proper encoding is maintained

### Phase 2: Connection State Management

**Goal**: Track current connection and enable host switching

#### 2.1 Enhance connection state tracking

- Add `_current_host` and `_current_port` members (strings)
- Add `is_connected_to(const boost::url& url)` method using `url.host()` and port logic
- Add `needs_reconnection(const boost::url& url)` method

#### 2.2 Connection management methods

- Add `connect_to_url(const boost::url& url)` private method
- Extract host/port from URL using:
  - `url.host()` for hostname
  - `url.port()` if `url.has_port()` is true
  - Otherwise use scheme-based defaults: "443" for HTTPS, "80" for HTTP
- Update existing `connect()` method to set `_current_host` and `_current_port`

#### 2.3 Port resolution logic

```cpp
std::string get_effective_port(const boost::url& url) {
    if (url.has_port()) {
        return url.port();
    }
    
    if (url.scheme() == "https") {
        return "443";
    } else if (url.scheme() == "http") {
        return "80";
    }
    
    throw std::invalid_argument("Unsupported scheme");
}
```

### Phase 3: Request Processing Flow Updates

**Goal**: Integrate connection management into task execution

#### 3.1 Update `process_request_queue()`

- Remove the `!_connected` guard condition
- Check if next task requires different host connection
- Call `ensure_connected_for_task(task)` before proceeding
- Only proceed to `send_next_request()` when connected to correct host

#### 3.2 Add connection orchestration

- Add `ensure_connected_for_task(std::shared_ptr<detail::base_task> task)` method
- Logic flow:

  ```cpp
  void ensure_connected_for_task(std::shared_ptr<detail::base_task> task) {
      const auto& url = task->url();
      
      if (needs_reconnection(url)) {
          if (_connected) {
              disconnect(); // Close existing connection
          }
          connect_to_url(url); // Connect to new host
      } else if (!_connected) {
          connect_to_url(url); // First connection
      }
      // If already connected to correct host, proceed immediately
  }
  ```

#### 3.3 Handle connection completion

- Modify connection completion handlers to trigger `process_request_queue()`
- Ensure failed connections propagate errors to waiting tasks

### Phase 4: Public API Updates

**Goal**: Provide a clean, ergonomic interface that supports both connection workflows

#### 4.1 Connection method

- Keep `connect(host, port, handler)` for users who want to pre-connect to a host

#### 4.2 Verb method implementations

- Create `boost::url` object directly from URL string in verb methods (throws on invalid URLs)
- Create tasks with the URL object (constructor will throw on invalid URLs)
- Remove URL string passing to request builders
- Invalid URLs will result in exceptions thrown to caller

#### 4.3 URL validation

- Validate scheme (only HTTPS supported currently) and throw on unsupported schemes
- Handle malformed URLs via exceptions from `boost::url` constructor

### Phase 5: Error Handling & Edge Cases

**Goal**: Robust handling of connection scenarios

#### 5.1 URL parsing and validation

- Invalid URLs will throw exceptions from `boost::url` constructor
- Validate required URL components (scheme, host) after construction
- Validate scheme (only HTTPS supported currently) and throw on unsupported schemes

#### 5.2 Connection error handling

- Handle connection failures during automatic connection
- Propagate connection errors to pending tasks in queue
- Clear task queue on connection failures

#### 5.3 Edge case handling

- Multiple tasks with different hosts in queue
- Connection timeout during host switching
- `disconnect()` called while connection switch in progress
- Invalid URLs mixed with valid URLs in task queue

### Phase 6: Testing & Validation

**Goal**: Ensure robust functionality with comprehensive test coverage

#### 6.1 Unit tests

- Test URL parsing and validation
- Test connection state management
- Test task queue behavior with mixed hosts
- Test error propagation

#### 6.2 Integration tests

- Test pre-connection workflow
- Test auto-connection workflow  
- Test host switching scenarios
- Test error recovery

## Implementation Order

1. **Phase 1** (Task System) - Foundation changes to make tasks URL-aware
2. **Phase 2** (Connection State) - Enable host tracking and connection management
3. **Phase 3** (Request Processing) - Core logic changes for connection orchestration
4. **Phase 4** (Public API) - Update public interface while maintaining compatibility
5. **Phase 5** (Error Handling) - Comprehensive error handling and edge cases
6. **Phase 6** (Testing) - Validate functionality with comprehensive tests

## Key Design Decisions

1. **URL Storage**: Tasks store complete `boost::url` objects for efficient component access
2. **Connection Reuse**: Connections reused for same host:port combinations
3. **Automatic Connection**: First verb call without explicit `connect()` triggers automatic connection
4. **Serial Processing**: Maintain single-threaded, serial request processing architecture
5. **Error Propagation**: Connection and URL parsing errors propagated to task completion handlers
6. **Minimise Copies**: Prefer `const&` captures and forwarding references to avoid unnecessary copies of lambdas or large objects

## Benefits

- **Performance**: Eliminate handshake overhead for subsequent requests to same host
- **Flexibility**: Support both pre-connection and auto-connection workflows
- **Robustness**: Comprehensive error handling and validation
- **Maintainability**: Clean separation of concerns using `boost::url` for parsing
- **Compatibility**: Backward compatible with existing code
