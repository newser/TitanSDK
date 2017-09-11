# TitanSDK - A cross-platform, fiber-based framework

| Platform | CI Status |
| --- | --- |
| Linux | [![Linux](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| macOS | [![macOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| iOS | [![iOS](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Android | [![Android](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK) |
| Windows | [![Windows](https://ci.appveyor.com/api/projects/status/github/newser/TitanSDK?svg=true&passingText=windows%20passing&failingText=windows%20failing)](https://ci.appveyor.com/project/newser/titansdk) |

TitanSDK implements various fiber-based features, the major advantages(also my purposes) is reducing code complexity but keeping concurrency:
- keep concurrency: 
- reduce code complexity: no need to write multithread program, 
- 

which originally need to "wait for some result" and thus would block caller, examples are waiting for network connection, reading disk files, waiting for RPC results and so on. 

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

With fiber technology these features can be done asynchronously but without splitting application code flow. Whenever calls a fiber-based API, the calling fiber is paused, operation is then ongoing, meanwhile other available fibers are scheduled to execute, once operation finishes, the paused fiber is resumed and result is returned.

Network protocol implementation basing on fiber-based io has same advantage:
```C
tt_dns_rrlist_t *rrl = tt_dns_get_aaaa("googe.com");
```
After calling the dns api, current fiber is paused, dns querying is then ongoing, other available fibers are scheduled to execute, once dns response is received, the paused fiber is resumed and a list of AAAA resource records is returned.

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
