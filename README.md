# TitanSDK

[![Linux](https://www.travis-ci.org/newser/TitanSDK.svg?branch=master)](https://www.travis-ci.org/newser/TitanSDK)
[![Windows](https://ci.appveyor.com/api/projects/status/github/newser/TitanSDK?svg=true)](https://ci.appveyor.com/project/newser/titansdk)

TitanSDK is a cross-platform, fiber-based, rich feature framework.

## Cross-platform

TitanSDK can be built on:
- Linux
- macOS
- iOS
- Android
- Windows

## Fiber-based

> A fiber is a particularly lightweight thread of execution. see [Fiber - Wikipedia](https://en.wikipedia.org/wiki/Fiber_(computer_science))

TitanSDK integrated fiber implementation, which can create or switch fibers on all supported platforms.

Fiber APIs are transparent to user, user does not need explicitly yield fiber or resume fiber. The fiber APIs are used by io operations internally, i.e. yielding fiber after staring an asynchronous io, or resuming fiber once io is completed. Such io style is called **fiber-based** here.

Features based on fiber-based io are also fiber-based, see an example:
```C
tt_dns_rrlist_t *rrl = tt_dns_get_aaaa("google.com");
```
After calling the dns api, current fiber is paused, dns querying is then ongoing, other available fibers are scheduled to execute, once dns response is received, the paused fiber is resumed and a list of AAAA resource records is returned.

## Rich feature

TitanSDK helps developer focus on their specific application development rather than rewriting common functionalities.

#### IO
- fiber-based socket io
- fiber-based regular file io
- fiber-based inter-process io

#### Inter-fiber event
- send and receive inter-fiber events
- wait for fiber events immediately after sending events

#### Timer
- create timer and be notified when timer expires

#### Unified system api
- wrapped native system api(windows api, posix, etc.) as unified apis

#### Logging
- configurable log format and various log output style

#### CLI(Command line interface)
- configure or show status via cross-platform CLI

#### XML
- DOM style XML accessing, support XPath

#### Cryptography
- public-key, symmetric-key, message digest, HMAC, random number generation, etc.

#### SSL

#### TLS

## Framework

TitanSDK provides a better way of architect software system:
- each module runs in a fiber. fibers can be in same or different threads, i.e. module A's fiber and B's are in a thread, while module C's fiber runs in another thread.
- modules can communicate with each other, via sending and receiving fiber events.
- module can do io and receives fiber events at same time.

#### versus multi-thread:
- more concurrent: fiber consumes less resource than thread, system could have far more fibers than threads.
- less race condition: fibers in same thread do not have race condition issue.

#### versus asynchronous io:
- easier coding: no need to split code flow into a callback chain, coding and debugging are mush easier.

#### an example application:

Requirements:
- receive data from another process
- receive data from network
- handle received data and send response to source

The example application implementation could have three modules in three fibers below:
```C
tt_task_create(&t, NULL);
tt_task_add_fiber(&t, "ipc", fiber_ipc, NULL, NULL);
tt_task_add_fiber(&t, "net", fiber_net, NULL, NULL);
tt_task_add_fiber(&t, "core", fiber_core, NULL, NULL);
tt_task_run_local(&t);
```
Note all these three modules run in same thread and never block thread, fiber would be paused when it needs doing io and another available fiber is resumed, if all fibers are paused, the thread does nothing untill some io is done and corresponding fiber is resumed.

The "ipc" fiber receives request from another process and send to "core" fiber, it also receives events from "core" fiber and send response to another process:
```C
tt_result_t fiber_ipc(IN void *param)
{
    tt_fiber_t *core_fb = tt_fiber_find("core");
    while (TT_OK(tt_ipc_recv(&ipc, buf, len, &recvd, &fiber_ev, &timer))) {
        if (recvd != 0) {
            // received ipc data, send a event to core fiber
            tt_fiber_send_ev(core_fb, ...);
        } 
        
        if (fiber_ev != NULL) {
            // received event from core fiber, send through ipc
            tt_ipc_send(&ipc, ...);
        } 
        
        if (timer != NULL) {
            // handle timer events
        }
    }
}
```

The "net" fiber receives request from network and send to "core" fiber, it also receives events from "core" fiber and send response to network:
```C
tt_result_t fiber_net(IN void *param)
{
    tt_fiber_t *core_fb = tt_fiber_find("core");
    while (TT_OK(tt_skt_recv(&skt, buf, len, &recvd, &fiber_ev, &timer))) {
        if (recvd != 0) {
            // received network data, send a event to core fiber
            tt_fiber_send_ev(core_fb, ...);
        } 
        
        if (fiber_ev != NULL) {
            // received event from core fiber, send through network
            tt_skt_send(&skt, ...);
        } 
        
        if (timer != NULL) {
            // handle timer events
        }
    }
}
```

The "core" fiber receives events from "net" and "ipc" fiber, and sends response events to them:
```C
tt_result_t fiber_core(IN void *param)
{
    tt_fiber_t *core_fb = tt_current_fiber();
    while (TT_OK(tt_fiber_recv(core_fb, TT_TRUE, &fiber_ev, &timer))) {
        if (fiber_ev != NULL) {
            // handle request
            tt_fiber_send_ev(fiber_ev->src, resp_ev, TT_FALSE);
        }
        
        if (timer != NULL) {
            // handle timer events
        }
    }
}
```

# Release

- [2017-09-12] 0.1.0

# License

