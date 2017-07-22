# TitanSDK

TitanSDK is a cross-platform, fiber-based io library written in C, with various useful features.

## Features
* [cross-platform, few external dependencies](#cpfed)
* [fiber-based io](#fbai)
* [inter-fiber communication, fiber timer](#ifcft)
* [unified system api](#usa)
* [flexibile log system](#fls)
* [command line interface](#cli)
* [XML processing](#xml)
* [cryptography](#crypto)
* [network protocol](#net)

### <a name="cpfed"></a>cross-platform, few external dependencies

Via CMake, applications based on TitanSDK can be built on:
* Windows (Windows 7 and later)
* Linux
* Mac OS
* *iOS (developing)*
* *Android (developing)*

Building TitanSDK does not need to install other library.

### <a name="fbai"></a>fiber-based io

> A fiber is a unit of execution that must be manually scheduled by the application.

TitanSDK makes it possible to do io without blocking current thread.
```C
tt_skt_recv(socket, buf, len, &recvd, NULL, NULL);
```
Fiber calling io functions would be paused and resumed once io is done. When one fiber is paused, thread would execute another available fiber.

fiber-based io includes:
  * file io
  * socket io
  * inter process io

### <a name="ifcft"></a>inter-fiber communication, fiber timer

A fiber can send event to another fiber. The destination fiber would be resumed and return the received event.

A fiber can start timer and it would be resumed when the timer expired.

```C
tt_skt_recv(socket, buf, len, &recvd, &p_fiber_event, &p_fiber_timer);
// p_fiber_event is the received event if it's not NULL
// p_fiber_timer is the expired timer if it's not NULL
```

### <a name="usa"></a>unified system api

Native system api(windows api, posix, etc.) are wrapped as unified apis, including:
  * thread, process
  * mutex, semaphore, read write lock, spin lock, atomic operation
  * dll(shared library)

### <a name="fls"></a>flexibile log system

Application can create and manage log system with configurable log format and multiple log destination.

### <a name="cli"></a>command line interface

Cross-platform command line interface, by which user can do configuration or show system status.

Developer can also add new entry into command line interface.

### <a name="xml"></a>XML processing

Parsing xml file and access xml data in DOM style, support XPATH.

### <a name="crypto"></a>cryptography

supported cryptography:
  * public-key cryptography: RSA, DH, ECDH, ECDSA
  * symmetric cryptography: AES, Camellia, DES/3DES, Blowfish, RC4 with ECB/CBC/CFB/CTR/GCM mode
  * message digest: MD2, MD4, MD5, SHA1, SHA224, SHA256, SHA384, SHA512, RIPEMD160
  * HMAC: HMAC-MDx, HMAC-SHAx, HMAC-RIPEMD160
  * random number generator

### <a name="net"></a>network protocol

supported network protocol:
  * SSL/TLS
  * DNS
