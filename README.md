# TitanSDK - A cross-platform, fiber-based framework

| Platform | CI Status |
| --- | --- |
| Linux | [![Linux](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| macOS | [![macOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| iOS | [![iOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Android | [![Android](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Windows | [![Windows](https://ci.appveyor.com/api/projects/status/github/newser/TitanSDK?svg=true)](https://ci.appveyor.com/project/newser/titansdk) |

TitanSDK implements various fiber-based features.

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

The major advantages(also this sdk's purposes) are:
- higher concurrency
- easier coding
- less race condition
- better architecture
- fewer dependencies
- flexible configuration
- cross platform, rich features

## Higher concurrency
Traditional application gains concurrency by multi-thread, a typical case is network server, which may create a thread serving each connection. A thread generally occupies megabytes while a fiber only uses kilobytes(128K by default and can be adjusted to be less), thus an application can have far more fibers than threads and be more concurrent.

## Easier coding
Some other high concurrent applications make use of asynchronous programming, which has to split code flow into a chain of callbacks，while this make coding difficult，handling errors or releasing resources or rolling back operations in a callback chain are never easy tasks. With fiber, code flow can be paused when doing an io and resumed when io finishes, code context is reserved，programming is completely same as normal but application still benefit from asynchronous programming model.

Below is a fiber-based dns query operation:
```C
tt_dns_rrlist_t *rrl = tt_dns_get_aaaa("google.com");
```
After calling the dns api, current fiber is paused, dns querying is then ongoing, other available fibers are scheduled to execute, once dns response is received, the paused fiber is resumed and a list of AAAA resource records is returned.

## Less race condition
Traditional multi-threaded applications can now turn to multi-fibered, functions running in different fibers have no race condition issue, free to access shared resource.

## Better architecture
Modules of traditional software communicate via function call which is tightly coupled. With fiber, modules can run independently in different fibers and communicate via inter-fiber events, decoupled modules and make a better architecture.

## Fewer dependencies
No need to find external source to compile, no need to install external libraries, TitanSDK only rely on built-in libraris, such as libpthread on Linux, ws2_32 on Windows, etc.

## Flexible configuration
Many systems need be configurable. TitanSDK provides APIs for defining configuration options, and these options can be configured via console CLI, configuring via ssh CLI and web page is to be implemented.

## Cross platform, rich features
Code can be compiled on different platforms(Windows, Linux, macOS, iOS, Android) without modification. Lots of common/infra features have been implemented or integrated, developer can focus on specific application without caring trivial things like how to create threads on different platforms, how to create a ssl connection, how to do xml querying, etc.

- **Fiber-based io**:
  - socket io
  - regular file io
  - inter-process io
- **Inter-fiber event**: send and receive inter-fiber events
- **Fiber-based timer**: create timer and be resumed when timer expires
- **Unified system api**: wrapped native system api(windows api, posix, etc.) as unified apis
- **Flexibile log system**: configurable log format and various log output style
- **Command line interfac**e: configure or show status via cross-platform CLI
- **XML**: DOM style XML accessing, support XPath
- **Cryptography**: public-key, symmetric-key, message digest, HMAC, random number generation, etc.
- **Network protocol**:
  - SSL/TLS
  - DNS

# Release

- [2017-09-12] 0.1.0

