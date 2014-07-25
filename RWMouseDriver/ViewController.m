//
//  ViewController.m
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/24/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//

#import "ViewController.h"
#import "RWMouseDriver.h"

@interface ViewController ()

@end

@implementation ViewController
            
- (void)viewDidLoad {
    [super viewDidLoad];
    RWMouseDriver* mouseDriver = [[RWMouseDriver alloc] init];
    
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
