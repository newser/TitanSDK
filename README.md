# TitanSDK

## Introduction

TitanSDK is a cross-platform, fiber-based asynchronous io library written in C, with various useful features:
* [cross-platform, less dependency](#cpld)
* [fiber-based asynchronous io](#fbai)
* [unified system api](#usa)
* flexibile log system: create and manage your log system and have customized log format
* command line interface: configure system or query system status
* XML processing
* unit testing framework
* unified cryptography API
* common network protocol(dns, ssl, etc.)
* timer triggered event
* more feature are being developed

### <a name="cpld"></a>cross-platform, less dependency

Code based on TitanSDK can be built on different platforms via CMake. **All features are supported** on platforms below:
* Windows (Windows 7 and later)
* Linux
* Mac OS
* *iOS (developing)*
* *Android (developing)*

Building TitanSDK does not need to install other libraries.

### <a name="fbai"></a>fiber-based asynchronous io

TitanSDK makes it possible to do asynchronous io without splitting your code flow
```C
tt_skt_recv(socket, buf, len, &recvd, NULL, NULL)
```
Fiber calling io functions would pause(but current thread would execute another fiber) and continue once io is done. All asynchronous operations are transparent to caller.

fiber-based asynchronous io includes:
* file io
* socket io
* inter process io

Network io based on socket io are also fiber-based asynchronous
```C
tt_dns_rrlist_t *rrl = tt_dns_get_a("google.com")
```
Calling fiber would pause during dns querying and continue once dns query result can be returned

### <a name="usa"></a>unified system api

Native system api(windows api, posix, etc.) are wrapped as unified apis.

```C
tt_thread_create(func, arg, NULL)
```
this function can create thread on all supported platforms

unified system api includes:
* thread, process
* mutex, semaphore, read write lock, spin lock, atomic
* dll(shared library)
