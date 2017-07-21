# TitanSDK

## Introduction

TitanSDK is a cross-platform, fiber-based asynchronous io library written in C, with various useful features:
* [cross-platform, less dependency](#cpld)
* [fiber-based asynchronous io](#fbai): doing asynchronous io without splitting you code flow
* unified system api: wrapped natvie api(windows api, posix, etc.)
* flexibile log system: create and manage your log system and have customized log format
* command line interface: configure system or query system status
* XML processing
* unit testing framework
* unified cryptography API
* common network protocol(dns, ssl, etc.)
* timer triggered event
* more feature are being developed

### <a name="cpld"></a>cross-platform, less dependency

Code based on TitanSDK can be built on different platforms via CMake. All features are supported on platforms below:
* Windows (Windows 7 and later)
* Linux
* Mac OS
* *iOS (developing)*
* *Android (developing)*

Building TitanSDK does not need to install other libraries.

### <a name="fbai"></a>fiber-based asynchronous io

TitanSDK makes it possible to do asynchronous io without splitting you code flow

```C
tt_skt_recv(socket, buf, len, &recvd, NULL, NULL)
```

Calling io function would block the calling fiber and current thread would execute another available fiber, the blocking fiber would be activated once io is done.
