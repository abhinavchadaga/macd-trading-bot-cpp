## async_rest_client – Architectural Design Plan

### 1. Purpose

A highly-reusable, generic, asynchronous REST client built on **Boost.Beast**/**Boost.Asio** that supports multiple request and response body types **without** imposing a template parameter on the class itself.  The design must be easy to extend, type-safe at compile time, and suitable for both the MACD trading bot and other future projects.

### 2. Guiding Principles

* **Separation of Concerns** – isolate HTTP I/O, request construction, and response parsing.
* **Type Erasure Internally** – keep the public surface non-templated while using type-safe templates behind the scenes.
* **Compile-Time Safety** – unsupported body types must fail at compile time.
* **Incremental Extensibility** – adding a new body type requires only a pair of small specialisations, no core changes.
* **Modern C++20** practices, consistent with existing project rules (proactor pattern, strands, RAII, etc.).

### 3. Public Interface

``get`` and ``post`` are the initial verbs, each deliberately minimal yet future-proof:

| Method | Template Parameters | Parameters | Notes |
|--------|--------------------|------------|-------|
| ``get<ResponseBody = http::string_body>`` | *Response body only* | ``url``, ``header_params``, ``CompletionHandler`` | Request body is always empty. |
| ``post<RequestBody = http::string_body, ResponseBody = http::string_body>`` | *Request + Response body* | ``url``, ``header_params``, ``body``, ``CompletionHandler`` | Initial request body support is ``http::string_body``.

Future verbs (``put``, ``patch``, ``delete``…) will follow the same pattern.

### 4. Core Architecture Overview

1. **Async Processing Core**
   * Manages the TCP/SSL stream, resolver, buffer, timeouts, and connection state.
   * Maintains a **type-erased task queue** so the client itself remains non-templated.
   * Provides generic scheduling: *connect → write → read → completion*.
2. **Type-Erased Task System**
   * ``base_task`` – abstract, non-templated; exposes virtual *write()* and *read()*.
   * ``typed_task<RequestBody, ResponseBody>`` – concrete template implementing ``base_task`` with the correct body types plus the user's completion handler.
3. **Request Builder Layer**
   * Primary template ``request_builder<BodyType>`` intentionally undefined; any unsupported type triggers a compile-time error via static_assert.
   * Specialisations exist for each supported request body (``http::empty_body``, ``http::string_body``, etc.).
   * Each specialisation returns a callable responsible for populating the Boost.Beast request object.
4. **Response Handler Layer**
   * Mirrors the request side: primary template ``response_handler<BodyType>`` undefined, with dedicated specialisations per supported response body.
   * Each specialisation provides a callable that parses the Boost.Beast response object and forwards results to the completion handler.
5. **Verb Dispatch Flow**
   1. Public ``get``/``post`` create a ``typed_task`` using the appropriate builder/handler specialisations.
   2. ``typed_task`` is cast to ``std::shared_ptr<base_task>`` and enqueued.
   3. Queue processing remains generic, unaware of body types.

### 5. Compile-Time Safety Mechanism

* ``always_false<T>`` helper (or concept-based constraints) forces a static_assert in the unspecialised primary templates.
* Users attempting an unsupported body combination receive a clear compiler diagnostic.

### 6. Extending to New Body Types

To add support for, say, ``http::file_body``:

1. Implement ``request_builder<http::file_body>`` if the body is used in requests.
2. Implement ``response_handler<http::file_body>`` if the body may appear in responses.
3. No modifications to the async core, task system, or existing specialisations.

### 7. Error & Timeout Handling

* Core layer checks ``beast::error_code`` after every async operation.
* Timeouts are governed by ``asio::steady_timer`` per operation, configurable via client config.
* All errors propagate to the user's completion handler through the response handler.

### 8. Thread Safety

* All I/O occurs on a single ``boost::asio::strand`` to honour the Proactor pattern while allowing multi-threaded callers to enqueue requests safely.

### 9. Implementation Phases

1. **Infrastructure** – establish async core, base/typed task classes, queue processing.
2. **Initial Specialisations** – ``request_builder<http::empty_body>``, ``request_builder<http::string_body>``, ``response_handler<http::string_body>``.
3. **Public API** – expose ``get`` and ``post`` for the initial body combinations.
4. **Validation** – integrate with the new *alpaca_order_client* and *alpaca_positions_client*.
5. **Future Work** – implement additional verbs and body-type specialisations as needed.

### 10. Expected Benefits

* **Reusability** – one client supports many APIs and body types.
* **Maintainability** – core logic untouched when introducing new body types.
* **Safety** – compile-time enforcement prevents accidental misuse.
* **Performance** – negligible runtime cost from type erasure; most work remains fully typed within the specialised layers.

---
This design aligns with project standards (Boost.Beast proactor pattern, strand-based concurrency, RAII) and paves the way for seamless expansion beyond the current trading-bot use case.

### 11. Flow Diagram – `get` Request to Completion Handler

Below is an ASCII sequence diagram showing how a call to `get<ResponseBody>()` moves through the system.  It emphasises how the **typed task** is converted into a type-erased **base_task** inside the queue.

```
Caller Thread                        async_rest_client Internals
┌────────────────────┐                                                 
│  user::get<T>()    │                                                 
└─────────┬──────────┘                                                 
          │ 1. template resolution picks                               
          │    ResponseBody = http::string_body (example)              
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ request_builder<http::empty_body>::build(…)                         │
│   → returns writer λ that fills request with empty body             │
└─────────────────────────────────────────────────────────────────────┘
          │                                                            
          │                                                            
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ response_handler<http::string_body>::create_reader()                │
│   → returns reader λ that extracts string body and invokes handler  │
└─────────────────────────────────────────────────────────────────────┘
          │                                                            
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ typed_task<http::empty_body, http::string_body>                     │
│   • stores writer λ                                                 │
│   • stores reader λ                                                 │
│   • stores user completion handler                                  │
└─────────────────────────────────────────────────────────────────────┘
          │ 2. cast to std::shared_ptr<base_task>                      
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│            enqueue(task) → _task_queue.push()                       │
└─────────────────────────────────────────────────────────────────────┘
          │                                                            
          │ 3. if not busy → process_request_queue()                   │
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ send_next_request()                                                 │
│   • pops front task                                                 │
│   • task->write(stream, ec_cb)   (invokes writer λ)                 │
└─────────────────────────────────────────────────────────────────────┘
          │ 4. async_write(stream, req, on_write)                      
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ on_write(ec)                                                        │
│   • on success → async_read(stream, res, on_read)                   │
└─────────────────────────────────────────────────────────────────────┘
          │ 5. async_read …                                            
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ on_read(ec)                                                         │
│   • task->read(response, ec, user_handler)  (invokes reader λ)      │
│   • reader λ parses body and calls user_handler(ec, json/string/…)  │
└─────────────────────────────────────────────────────────────────────┘
          │ 6. queue book-keeping                                      
          ▼                                                            
┌─────────────────────────────────────────────────────────────────────┐
│ response & buffer cleared                                           │
│ request_in_progress = false                                         │
│ process_request_queue()  (handles next task if any)                 │
└─────────────────────────────────────────────────────────────────────┘
```

**Key Points**

1. The **typed_task** encapsulates everything type-specific; once it is converted to `base_task`, the queue operates on a uniform interface.
2. Both writer and reader lambdas are fully typed, so body-specific logic is resolved at compile time, even though the queue stores them as type-erased callables.
3. The async core never needs to know which body types are involved; it simply defers to the polymorphic `write()` / `read()` on the current task.

### 12. Implementation Plan

| Phase | Goal | Key Tasks | Validation |
|-------|------|-----------|------------|
| 0 | **Project Setup** | • Create `include/async_rest_client/` and `src/async_rest_client/` sub-trees.<br>• Add top-level CMake target `async_rest_client` (static lib). | Configure + compile empty target. |
| 1 | **Class Skeleton & Connection Layer** | • Implement `async_rest_client` skeleton (`config`, `create`, `connect`, `disconnect`, `is_connected`).<br>• Integrate Boost.Beast resolver/stream, timers, strand.<br>• Unit-test TLS handshake against `example.com` with empty request. | Build specific test `TestAsyncRestClientConnection`. |
| 2 | **Task System Infrastructure** | • Add `base_task` abstract class with virtual `write()` / `read()`.<br>• Add `typed_task<RequestBody, ResponseBody>` template in `detail` namespace.<br>• Introduce internal queue (`std::queue<std::shared_ptr<base_task>>`) plus processing loop (`send_next_request`, `on_write`, `on_read`). | Mock socket in unit test; assert FIFO order. |
| 3 | **Request & Response Type Specialisations** | • Declare primary templates `request_builder<Body>` & `response_parser<Body>` with `static_assert(always_false<Body>).`<br>• Implement specialisations for `http::empty_body` (request) and `http::string_body` (request + response).<br>• Add helper `always_false` trait in `detail` namespace. | Compile-time tests for unsupported types; run unit tests for supported. |
| 4 | **Public API: `get`** | • Implement `get<ResponseBody>()` wrapper.<br>• Internally create `typed_task<http::empty_body, ResponseBody>` and enqueue.<br>• Support `http::string_body` response; add compile-time guard for others until implemented. | Functional test hitting `https://httpbin.org/get`. |
| 5 | **Public API: `post`** | • Implement `post<RequestBody, ResponseBody>()` wrapper.<br>• Provide first implementation for `<http::string_body, http::string_body>`.<br>• Ensure body size, content-type header, and payload preparation. | Functional test posting JSON to `https://httpbin.org/post`. |
| 6 | **Timeout & Error Propagation** | • Integrate per-operation timer (`asio::steady_timer`).<br>• Map common Beast/Asio errors to `boost::system::errc` enums.<br>• Surface all failures through user completion handler. | Unit test using intentionally unreachable host. |
| 7 | **Thread-Safety & Strand Verification** | • Ensure all I/O operations and queue manipulations occur on the strand.<br>• Multi-threaded stress test that enqueues 1 000 mixed requests. | `TestAsyncRestClientConcurrency` passes under `TSAN`. |
| 8 | **Integration With Alpaca Clients** | • Replace direct Beast usage in `alpaca_order_client` / `alpaca_positions_client` prototypes with new rest client.<br>• Compile and run existing order/position tests. | Existing Alpaca tests green. |
| 9 | **Additional Verbs & Body Types (Optional)** | • Add `put`, `delete`, etc.<br>• Implement `request_builder<http::file_body>` and `response_parser<http::dynamic_body>` as reference extensions. | New dedicated tests. |

**Incremental Build Strategy**

* Use `./build.sh Debug async_rest_client` for fast iteration per phase.
* Unit tests follow the pattern `TestAsyncRestClient*.cpp` and are built individually.

### 13. File-Organisation Guidelines

To prevent `async_rest_client.hpp` / `.cpp` from growing unwieldy, each substantive helper or internal component should reside in its own pair of files.

| Component | Header | Source | Notes |
|-----------|--------|--------|-------|
| **core connection & public API** | `include/async_rest_client/async_rest_client.hpp` | `src/async_rest_client/async_rest_client.cpp` | Contains only high-level class, public methods, thin delegates to internals. |
| `base_task` abstract interface | `include/async_rest_client/detail/base_task.hpp` | `src/async_rest_client/detail/base_task.cpp` | Pure-virtual write/read, minimal. |
| `typed_task` template | `include/async_rest_client/detail/typed_task.hpp` | (header-only) | Small wrapper that instantiates per body-type pair. |
| `request_builder<Body>` specialisations | `include/async_rest_client/request_builder_fwd.hpp` (primary) <br>`include/async_rest_client/request_builder_<body>.hpp` | Header-only | One header per body type; keeps compile units independent. |
| `response_parser<Body>` specialisations | `include/async_rest_client/response_parser_fwd.hpp` (primary) <br>`include/async_rest_client/response_parser_<body>.hpp` | Header-only | Mirrors request builders. |
| Utility traits / helpers | `include/async_rest_client/detail/type_traits.hpp` | Header-only | e.g., `always_false`. |

**Build System Notes**

* `async_rest_client` static library links all `src/async_rest_client/**/*.cpp` units.
* The `detail/` sub-namespace headers are excluded from public install/export sets to keep the user-facing interface minimal.

Following this separation ensures logical cohesion, faster incremental builds, and clear ownership of each abstraction layer.
