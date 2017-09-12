# TitanSDK - A cross-platform, fiber-based framework

TitanSDK tends to be a cross-platform framework helping developer focus on their specific application development rather than rewriting common functionalities including:
- [IO](#io)(socket io, file io, inter-process io)
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
