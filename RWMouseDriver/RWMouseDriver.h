//
//  RWMouseDriver.h
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/24/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MacTypes.h"

@interface RWMouseDriver : NSObject
void handleReport(UInt8 *buffer, UInt16 length);

@end
