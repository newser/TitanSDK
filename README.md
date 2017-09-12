# TitanSDK - A cross-platform, fiber-based framework

TitanSDK tends to be a cross-platform framework helping developer focus on their specific application development rather than rewriting common functionalities including:
- [IO](#io)
- fiber
- timer management
- system api
- logging
- CLI(command line interface)
- XML
- cryptography
- SSL/TLS
- DNS

## Supported platform
| Platform | CI Status |
| --- | --- |
| Linux | [![Linux](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| macOS | [![macOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| iOS | [![iOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Android | [![Android](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Windows | [![Windows](https://ci.appveyor.com/api/projects/status/github/newser/TitanSDK?svg=true)](https://ci.appveyor.com/project/newser/titansdk) |

## Release

- [2017-09-12] 0.1.0

## Features

#### <a name="io"/>IO
- socket io
- ipc io
- file io

## Cross-platform

Almost all features of TitanSDK can run cross platforms, implemented features are:
- **Io**:
  - socket io
  - regular file io
  - inter-process io
- **Inter-fiber event**: send and receive inter-fiber events
- **Timer**: create timer and be notified when timer expires
- **Unified system api**: wrapped native system api(windows api, posix, etc.) as unified apis
- **Flexibile log system**: configurable log format and various log output style
- **Command line interface**: configure or show status via cross-platform CLI
- **XML**: DOM style XML accessing, support XPath
- **Cryptography**: public-key, symmetric-key, message digest, HMAC, random number generation, etc.
- **Network protocol**:
  - SSL/TLS
  - DNS

## Fiber

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

TitanSDK solves some of questions above by making use of fiber. 

Each module can run in a fiber, each fiber can:
- send/receive fiber event
- do one kind of io(inter-fiber, socket, inter-process, inter-thread)
- manage timers

An example, assume an application requires:
- interact with another process
- do network io
- handling received request
- sending response to a specified network address

Such application can have two fibers, one for receiving request from another process and process request, another for sending response

TitanSDK implements various **fiber-based** features:





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


