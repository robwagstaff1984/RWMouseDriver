//
//  RWMouseDriver.m
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/24/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//

#import "RWMouseDriver.h"

//You are writing a driver for a new mouse. All incoming data is processed in a function named handleReport();
//
//void handleReport(UInt8 *buffer, UInt16 length);
//
//A mouse packet has the following structure:


//B0: report format
//B1: reserved
//B2: dX (bits[7:0])
//B3: dY (bits[7:0])
//B4[1:0]: Left/Right button states
//B4[3:2]: dX (bits[9:8])
//B4[5:4]: dY (bits[9:8])
//B4[7:6]: timestamp (ms) bits[1:0]
//B5: timestamp (ms) bits[9:2]
//B6: timestamp (ms) bits[17:10]

//1) Write one or more functions that extract the mouse deltas, button state, and timestamp from the device.
//2) The 18-bit packet timestamp reflects a device timestamp in milliseconds. It will roll over after a few minutes. The host can accomodate a 32-bit timestamp that is updated with values from the incoming packet. Write code that detects the timestamp rollover in the incoming packets, and extends it to 32 bits.

#define MOUSE_REPORT_FORMAT_DEFAULT 255
#define MOUSE_RESERVED_DEFAULT 255

#define MOUSE_LEFT_RIGHT_DEFAULT_STATE 3

#define AND_BIT_MASK_ZERO_TO_ONE (1 << 2) - 1
#define AND_BIT_MASK_TWO_TO_SEVEN (1 << 8) - (1 << 2);

typedef unsigned char BYTE;

struct mouseDriverPacket
{
    unsigned int B0 : 8;
    unsigned int B1 : 8;
    unsigned int B2 : 8;
    unsigned int B3 : 8;
    unsigned int B4a : 2;
    unsigned int B4b : 2;
    unsigned int B4c : 2;
    unsigned int B4d : 2;
    unsigned int B5 : 8;
    unsigned int B6 : 8;
};

@implementation RWMouseDriver

- (instancetype)init
{
    self = [super init];
    if (self) {
        
        BYTE inputBuffer[3];
        setTestDataForInputBuffer(inputBuffer);
        
        BYTE* inputBufferPtr = &inputBuffer[0];
        UInt16 length = 17;
        
        handleReport(inputBufferPtr, length);
    }
    return self;
}


void handleReport(UInt8 *buffer, UInt16 length) {
    struct mouseDriverPacket mouseDriverPacket = createDefaultMouseDriverPacket();
    extractMouseDriverPacketDataFromBuffer(buffer, &mouseDriverPacket);
    printMouseDriverPacket(mouseDriverPacket);
}

void extractMouseDriverPacketDataFromBuffer(UInt8 *buffer, struct mouseDriverPacket* mouseDriverPacket) {
    BYTE firstByte = buffer[0];
    BYTE secondByte = buffer[1];
    
    mouseDriverPacket->B2 = firstByte;
    mouseDriverPacket->B3 = firstByte;
    mouseDriverPacket->B4b = getFirstTwoBitsForByte(secondByte);
    mouseDriverPacket->B4c = getFirstTwoBitsForByte(secondByte);
    mouseDriverPacket->B4d = getFirstTwoBitsForByte(firstByte);
    mouseDriverPacket->B5 = combineBytesTwoToNine(buffer);
    mouseDriverPacket->B6 = combineBytesTenToSeventeen(buffer);
}


BYTE getFirstTwoBitsForByte(BYTE byte) {
    return byte & AND_BIT_MASK_ZERO_TO_ONE;
}

BYTE getLastSixBitsForByte(BYTE byte) {
    return byte & AND_BIT_MASK_TWO_TO_SEVEN;
}

BYTE combineBytes(BYTE leastSignificantByteSection, UInt8 leastLength, BYTE mostSignificantByteSection, UInt8 mostLength) {
    leastSignificantByteSection = leastSignificantByteSection >> mostLength;
    mostSignificantByteSection = mostSignificantByteSection << leastLength;
    return leastSignificantByteSection | mostSignificantByteSection;
}

BYTE combineBytesTwoToNine(UInt8* buffer) {
    BYTE bitsTwoToSeven = getLastSixBitsForByte(buffer[0]);
    BYTE bitsEightToNine = getFirstTwoBitsForByte(buffer[1]);
    return combineBytes(bitsTwoToSeven, 6, bitsEightToNine, 2);
}

BYTE combineBytesTenToSeventeen(UInt8* buffer) {
    BYTE bitsTenToFifteen = getLastSixBitsForByte(buffer[1]);
    BYTE bitsSixteenToSeventeen = getFirstTwoBitsForByte(buffer[2]);
    return combineBytes(bitsTenToFifteen, 6, bitsSixteenToSeventeen, 2);
}

#pragma mark - setup
void setTestDataForInputBuffer(BYTE inputBuffer[3]) {
    inputBuffer[0] = 0x52;
    inputBuffer[1] = 0x52;
    inputBuffer[2] = 0x3;
}

struct mouseDriverPacket createDefaultMouseDriverPacket() {
    struct mouseDriverPacket mouseDriverPacket;
    mouseDriverPacket.B0 = MOUSE_REPORT_FORMAT_DEFAULT;
    mouseDriverPacket.B1 = MOUSE_RESERVED_DEFAULT;
    mouseDriverPacket.B2 = 0;
    mouseDriverPacket.B3 = 0;
    mouseDriverPacket.B4a = MOUSE_LEFT_RIGHT_DEFAULT_STATE;
    mouseDriverPacket.B4b = 0;
    mouseDriverPacket.B4c = 0;
    mouseDriverPacket.B4d = 0;
    mouseDriverPacket.B5 = 0;
    mouseDriverPacket.B6 = 0;
    return mouseDriverPacket;
}

void printMouseDriverPacket(struct mouseDriverPacket mouseDriverPacket ) {
    NSLog(@"B0: %u\n,", mouseDriverPacket.B0);
    NSLog(@"B1: %u\n,", mouseDriverPacket.B1);
    NSLog(@"B2: %u\n,", mouseDriverPacket.B2);
    NSLog(@"B3: %u\n,", mouseDriverPacket.B3);
    NSLog(@"B4a: %u\n,", mouseDriverPacket.B4a);;
    NSLog(@"B4b: %u\n,", mouseDriverPacket.B4b);
    NSLog(@"B4c: %u\n,", mouseDriverPacket.B4c);
    NSLog(@"B4d: %u\n,", mouseDriverPacket.B4d);
    NSLog(@"B5: %u\n,", mouseDriverPacket.B5);
    NSLog(@"B6: %u\n,", mouseDriverPacket.B6);
}

@end
