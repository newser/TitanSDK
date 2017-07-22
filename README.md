# TitanSDK

## Introduction

TitanSDK is a cross-platform, fiber-based asynchronous io library written in C, with various useful features:
* [cross-platform, less dependency](#cpld)
* [fiber-based asynchronous io](#fbai)
* [unified system api](#usa)
* [flexibile log system](#fls)
* command line interface: configure system or query system status
* XML processing
* cryptography
* common network protocol(dns, ssl, etc.)
* timer triggered event

### <a name="cpld"></a>cross-platform, less dependency

Application based on TitanSDK can be built on different platforms via CMake. All features are supported on:
* Windows (Windows 7 and later)
* Linux
* Mac OS
* *iOS (developing)*
* *Android (developing)*

Building TitanSDK does not need to install other libraries.

### <a name="fbai"></a>fiber-based asynchronous io

> A fiber is a unit of execution that must be manually scheduled by the application

TitanSDK makes it possible to do asynchronous io without splitting your code flow
```C
tt_skt_recv(socket, buf, len, &recvd, NULL, NULL)
```
Fiber calling io functions would be paused and resumed once io is done. When one fiber is paused, thread would execute another available fiber. All asynchronous operations are transparent to caller.

fiber-based asynchronous io includes:
* file io
* socket io
* inter process io

Network io based on socket io are also fiber-based asynchronous
```C
tt_dns_rrlist_t *rrl = tt_dns_get_a("google.com")
```
Fiber calling tt_dns_get_a("google.com") would be paused during dns querying and resumed once dns result can be returned

### <a name="usa"></a>unified system api

Native system api(windows api, posix, etc.) are wrapped as unified apis.

```C
tt_thread_create(func, arg, NULL)
```
this function can create thread on all supported platforms

unified system api includes:
* thread, process
* mutex, semaphore, read write lock, spin lock, atomic operation
* dll(shared library)

### flexibile log system

Application can create and manage customized log system:
* create your log
  ```C
  tt_logmgr_create(&logmgr, "MyLog", NULL);
  ```
* customize error log format 
  ```C
  tt_loglyt_t *lyt = tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n");
  tt_logmgr_set_layout(&logmgr, TT_LOG_ERROR, lyt);
  ```
* configure where to output log
  ```C
  tt_logio_t *lio = tt_logio_std_create(NULL);
  tt_logmgr_append_io(logmgr, TT_LOG_ERROR, tt_s_logio_std)
  ```
