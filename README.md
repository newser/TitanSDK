# TitanSDK - A cross-platform, fiber-based framework

| Platform | CI Status |
| --- | --- |
| Linux | [![Linux](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| macOS | [![macOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| iOS | [![iOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Android | [![Android](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Windows | [![Windows](https://ci.appveyor.com/api/projects/status/github/newser/TitanSDK?svg=true&passingText=windows%20passing&failingText=windows%20failing)](https://ci.appveyor.com/project/newser/titansdk) |

TitanSDK implements various fiber-based features.

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

The major advantages(also its purposes) are:
- high concurrency
- easy coding
- less race condition
- better architecture

## High concurrency
Traditional application gains concurrency by multi-thread, a typical case is network server, which may create a thread serving each connection. Each thread generally occupies megabytes while a fiber only uses kilobytes(128K by default and can be adjusted to be less), so that application can have far more fibers than threads and thus more concurrent.

## Easy coding
Some application make use of asynchronous io for high concurrency, it can only have one thread(or one thread per cpu), when application needs do io(or any operation that would block caller), it provides a callback to be executed once io finishes. Such mechanism gains highest concurrecy but the key issue is programming a callback chain for a complex case is very difficult, imaging how hard it is to handle errors and release resources and roll back operations in a callback chain. With fiber, application can be paused when doing an io and resumed when io finishes, does not need any callback.

Below is a fiber-based dns query operation:
```C
tt_dns_rrlist_t *rrl = tt_dns_get_aaaa("googe.com");
```
After calling the dns api, current fiber is paused, dns querying is then ongoing, other available fibers are scheduled to execute, once dns response is received, the paused fiber is resumed and a list of AAAA resource records is returned.

## Less race condition
Lots of application does not really need be multi-threaded but they have to

## Better architecture

# Release

- [2017-08-01] 0.1.0

# Features
- **Cross-platform**: built on Windows, Linux, macOS, iOS(developing), Android(developing) via CMake
- **Few dependencies**: only require pre-installed libraries(libpthread, ws2_32, etc.)
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
