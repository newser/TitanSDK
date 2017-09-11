//
//  main.m
//  titansdk-ios-unit-test
//
//  Created by haniu on 9/11/15.
//  Copyright (c) 2015 titansdk-org. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "AppDelegate.h"

int main(int argc, char * argv[])
{
    if (argc > 0 && strcmp(argv[0], "tsk_unit_test") == 0)
    {
        extern int app_ut_main(int argc, char* argv[]);
        return app_ut_main(argc, argv);
    }
    
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
