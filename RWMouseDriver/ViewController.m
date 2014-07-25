//
//  ViewController.m
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/24/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//

#import "ViewController.h"
#import "mouseDriver.h"

@interface ViewController ()

@end

@implementation ViewController
            
- (void)viewDidLoad {
    [super viewDidLoad];
    
    UInt8 inputBuffer[3];
    setTestDataForInputBuffer(inputBuffer);
    
    BYTE* inputBufferPtr = &inputBuffer[0];
    UInt16 length = 17;
    handleReport(inputBufferPtr, length);
}

void setTestDataForInputBuffer(BYTE inputBuffer[3]) {
    inputBuffer[0] = 0x52;
    inputBuffer[1] = 0x52;
    inputBuffer[2] = 0x3;
}


@end
