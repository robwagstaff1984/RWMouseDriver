//
//  mouseDriver.h
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/25/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//
#import "MacTypes.h"
typedef unsigned char BYTE;

void handleReport(UInt8 *buffer, UInt16 length);