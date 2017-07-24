# TitanSDK - A cross-platform, fiber-based io library

TitanSDK implements fiber-based io, which is a kind of asynchronous io without splitting application code flow.

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

Whenever calls a fiber-based io function, the calling fiber is paused, io is then ongoing, meanwhile other available fibers are scheduled to execute, once io finishes, the paused fiber is resumed and result is returned.

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
- **Fiber-based io**
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
