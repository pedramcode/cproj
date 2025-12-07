# C distributed hash table (CDHT)

*A High-Performance Distributed Key–Value Node with Custom Networking and Hashing*

---

## Table of Contents

1. [Overview](#overview)
2. [Core Features](#core-features)
3. [Architecture](#architecture)

   * [Node Structure](#node-structure)
   * [Storage Engine](#storage-engine)
   * [Networking Layer](#networking-layer)
   * [Packet Protocol](#packet-protocol)
   * [Concurrency Model](#concurrency-model)
   * [Node Identity & Hashing](#node-identity--hashing)
4. [Build and Run](#build-and-run)

   * [Prerequisites](#prerequisites)
   * [Compiling](#compiling)
   * [Running](#running)
   * [Debugging](#debugging)
5. [Configuration](#configuration)
6. [Developer Guide](#developer-guide)

   * [Project Layout](#project-layout)
   * [Coding Standards](#coding-standards)
   * [Extending the Protocol](#extending-the-protocol)
   * [Adding New Node Commands](#adding-new-node-commands)
   * [Modifying the Storage System](#modifying-the-storage-system)
7. [Editing this README](#editing-this-readme)
8. [License](#license)

---

## Overview

This project implements a lightweight, high-performance **distributed key–value node** written in C. The system focuses on minimal dependencies, clear extensibility, and a predictable execution model. A custom UDP server handles incoming requests, while a worker-based concurrency model ensures fast parallel processing. Nodes generate their own unique identities, communicate via compact binary packets, and maintain an internal hash-bucket storage structure.

This codebase is engineered to serve as a foundation for building a more advanced distributed system—such as a small Redis-like database, a distributed hash table (DHT), or a lightweight NoSQL store.

---

## Core Features

* Lightweight UDP-based request handling.
* Custom binary packet protocol supporting extensible command types.
* Multi-worker server architecture for concurrency.
* Configurable hash-bucket in-memory storage engine.
* Node identity generation using MAC address + port + secure hashing.
* Graceful shutdown and signal management.
* Optional NASM assembly modules for low-level functionality.
* Modular design to simplify extension and contribution.

---

## Architecture

### Node Structure

Each node consists of:

* A UDP server instance.
* A command-processing layer.
* A hash-bucket key–value engine.
* A unique identity hash (20-byte SHA-1 or OpenSSL-based digest).
* Utility subsystems (logger, networking helpers, converters, etc.).

Nodes are long-running processes designed for continuous communication and storage operations.

---

### Storage Engine

The storage subsystem is based on:

* A fixed number of **buckets** (default: `1024`)
* Each bucket is protected by a lock for thread-safe operations.
* Each bucket stores keys/values in a linked list or dynamically allocated chain.

Key operations:

* `put(key, value)`
* `get(key)`
* `delete(key)`

Hashing uses a simple but efficient modulus over the number of buckets.

---

### Networking Layer

Nodes communicate via UDP. The networking layer provides:

* A non-blocking event loop.
* Worker threads for parallel request handling.
* A callback-based request dispatch model.
* Logging hooks for monitoring incoming events.

---

### Packet Protocol

Incoming UDP data is parsed into `packet_t` structures. Each packet includes:

* Version
* Command type
* Request ID
* Payload

Commands currently implemented:

* `CMD_PING` – round-trip health check
* `CMD_ERROR` – protocol/parsing failure response

Packets are serialized/deserialized via `packet_to_bytes` and `packet_from_bytes`.

---

### Concurrency Model

The server spawns a configurable number of worker threads (default: 8). Each worker processes packets independently. Bucket-level locking ensures thread safety in the storage engine.

Worker tuning recommendations:

* Set workers ≈ number of CPU cores.
* For heavy I/O workloads, multiply by 2–4.
* For CPU-bound workloads, match CPU core count precisely.

---

### Node Identity & Hashing

Node identity is derived from:

* MAC address
* Server port
* A secure cryptographic hash (OpenSSL recommended)

This ensures:

* Global uniqueness
* Stability across restarts
* Compatibility with consistent hashing models

Hash is stored in both binary (`node->hash`) and hex string (`node->hash_hex_string`) formats.

---

## Build and Run

### Prerequisites

* `gcc` or `clang`
* `make`
* `openssl` development libraries (`libssl-dev` / `openssl-devel`)
* `nasm` (for optional assembly modules)
* Linux environment (recommended)

---

### Compiling

```
make
```

This builds all C and NASM files and produces `build/main.out`.

---

### Running

```
make run
```

or directly:

```
./build/main.out
```

---

### Debugging

Run with GDB:

```
make debug
```

Run with Valgrind:

```
make valgrind
```

Clean the project:

```
make clean
```

Rebuild from scratch:

```
make rebuild
```

---

## Configuration

Node configuration is done through `node_config_t`. Current options include:

* `port` — UDP port to bind and listen on
* `bucket_count` — size of the internal hash table (future extension)

Configuration is passed during node creation.

---

## Developer Guide

### Project Layout

```
src/
    core/          – global state, signal handling
    helpers/       – logger, network helper, hashing, conversions
    node/          – main node logic, request dispatch, commands
    packet/        – serialization/deserialization logic
    storage/       – key–value engine
include/           – public headers
build/             – compiled artifacts
```

---

### Coding Standards

To maintain clarity and consistency:

1. Keep functions small and single-purpose.
2. Use explicit memory allocation and deallocation.
3. Always handle errors gracefully.
4. Document each function with a concise header comment.
5. Avoid hidden global state except where intended (`global_node`).

---

### Extending the Protocol

To add a new command:

1. Add a new entry in `packet/types.h`.
2. Add a handler function in `node/commands.c`.
3. Route it in `node_request_handler`.
4. Update documentation accordingly.

New commands should follow this structure:

```
size_t command_example(packet_t *packet, char **output);
```

Return value is the number of bytes written into `*output`.

---

### Adding New Node Commands

1. Define command type (`CMD_XYZ`)
2. Implement handler logic
3. Add routing in the main switch-case
4. Ensure error handling consistency
5. Add test packets for validation

---

### Modifying the Storage System

Storage improvements may include:

* Changing the bucket count
* Using dynamic rehashing
* Adding TTL expiration
* Replacing linked lists with hopscotch hashing or Robin Hood hashing
* Using lock-free techniques

All modifications should preserve thread-safety.

---

## Editing This README

Other developers can extend or modify this README by following these guidelines:

1. Keep the document **fully structured** according to the existing Table of Contents.
2. When adding new sections, update both the content and the Table of Contents.
3. Use **clear, direct, and technical language**, avoiding unnecessary descriptions.
4. Whenever introducing new subsystems, include:

   * Purpose
   * Design overview
   * Relevant code modules
   * Contribution notes
5. Follow standard Markdown formatting.
6. Keep sections logically ordered; do not mix unrelated content.

It is recommended to review commit history for previous README update patterns before writing new documentation.

---

## License

?
