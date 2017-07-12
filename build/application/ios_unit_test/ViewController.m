//
//  ViewController.m
//  titansdk-ios-unit-test
//
//  Created by haniu on 9/11/15.
//  Copyright (c) 2015 titansdk-org. All rights reserved.
//

#import "ViewController.h"

static UITextView *ios_text_view;

@interface ViewController ()

- (IBAction)run_titansdk_ut:(UIButton *)sender;
@property (retain, nonatomic) IBOutlet UIButton *run_button;

@property (retain, nonatomic) IBOutlet UITextView *text_view;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    [_text_view setEditable:FALSE];
    
    if (ios_text_view == NULL)
    {
        ios_text_view = _text_view;
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)titansdk_ut:(void*)param
{
    char *argv[] = { "app_unit_test" };
    
    extern int app_ut_main(int argc, char* argv[]);
    app_ut_main(1, argv);
}

- (IBAction)run_titansdk_ut:(UIButton *)sender
{
    [_run_button setTitle:@"running" forState:UIControlStateDisabled];
    [_run_button setEnabled:FALSE];
    
    NSThread *thread = [[NSThread alloc]initWithTarget:self
                                              selector:@selector(titansdk_ut:)
                                                object:nil];
    if (thread != NULL)
    {
        [thread start];
    }
}

- (void)dealloc
{
    [_text_view release];
    [_run_button release];
    [super dealloc];
}

@end

void ios_display(const char *str)
{
    NSString *nstr = [[NSString alloc] initWithUTF8String:(char*)str];
    if (nstr == NULL)
    {
        return;
    }
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [[[ios_text_view textStorage] mutableString] appendString:nstr];
        CFRelease(nstr);
    });
}

const char *get_app_path()
{
    static char app_path[256] = {0};
    
    if (app_path[0] == 0)
    {
        [[[NSBundle mainBundle] executablePath] getCString:app_path
                                                 maxLength:sizeof(app_path) - 1
                                                  encoding:NSUTF8StringEncoding];
    }
    
    return app_path;
}