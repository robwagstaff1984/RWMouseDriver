//
//  mouseDriver.c
//  RWMouseDriver
//
//  Created by Robert Wagstaff on 7/25/14.
//  Copyright (c) 2014 Robert Wagstaff. All rights reserved.
//

#include <stdio.h>
#import "mouseDriver.h"

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
#define AND_BIT_MASK_ZERO_TO_SEVENTEEN (1 << 18) - 1
#define AND_BIT_MASK_EIGHTEEN_TO_THIRTYONE (~0) - (1 << 18) + 1

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

struct mouseDriverPacket createDefaultMouseDriverPacket();
void extractMouseDriverPacketDataFromBuffer(UInt8 *buffer, struct mouseDriverPacket* mouseDriverPacket);
void printMouseDriverPacket(struct mouseDriverPacket mouseDriverPacket);
void update32BitTimestamp (UInt8 *buffer);


/******************
 ASSUMPTION: bits[7:0] refers to the first 8 bits in the buffer, bits[17:10] refers to the 11-18 bit of the buffer
        B0: refers to the first byte of the output MouseDriverPacket
        B1: refers to the second byte of the output MouseDriverPacket
 
 IMPORTANT FOOT NOTE!: Upon final review of this coding challenge I believe I have misinterpreted the question. I now believe B0, B1, B2 etc referes to the bytes of the input data, but I assumed this was the format the data is to be stored in.
    I incorrectly believed bits[7:0] was refering to the buffer instead of the first 8 bits of dX and dY. It's too late to recode this solution, but I believe the misinterpreted question I solved has very similar logic to the actual question. Please take this into account while reviewing my code.
 
 How I would have solved the problem if I started again: 
 1) loop through buffer until length extracting each byte until I have all 7 bytes of info
 2) Use bitwise AND masks to extract the 4 parts of B4
 3) Use bitshifting to add the bits correctly for DX/DY and the timestamp as their data is stored partially in multiple bytes
 
 
 
 
 This function will extract the bytes from buffer and extract the relevant mouse driver information using bitwise operations. The function will then add this data into the mouseDriverPacket bitfield at the required positions.
 *****************/
void handleReport(UInt8 *buffer, UInt16 length) {
    struct mouseDriverPacket mouseDriverPacket = createDefaultMouseDriverPacket();
    extractMouseDriverPacketDataFromBuffer(buffer, &mouseDriverPacket);
    printMouseDriverPacket(mouseDriverPacket);
    update32BitTimestamp(buffer);
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
    printf("B0: %u\n", mouseDriverPacket.B0);
    printf("B1: %u\n", mouseDriverPacket.B1);
    printf("B2: %u\n", mouseDriverPacket.B2);
    printf("B3: %u\n", mouseDriverPacket.B3);
    printf("B4a: %u\n", mouseDriverPacket.B4a);;
    printf("B4b: %u\n", mouseDriverPacket.B4b);
    printf("B4c: %u\n", mouseDriverPacket.B4c);
    printf("B4d: %u\n", mouseDriverPacket.B4d);
    printf("B5: %u\n", mouseDriverPacket.B5);
    printf("B6: %u\n", mouseDriverPacket.B6);
}

UInt32 latest18BitTimeStampFromBuffer(UInt8 *buffer) {
    UInt32 leastSignificantTimestampByte = buffer[0];
    UInt32 middleSignificantTimestampByte = buffer[1];
    UInt32 mostSignificantTimestampByte = getFirstTwoBitsForByte(buffer[2]);
    
    middleSignificantTimestampByte = middleSignificantTimestampByte << 8;
    mostSignificantTimestampByte = mostSignificantTimestampByte << 16;
    
    UInt32 latestTimestamp = leastSignificantTimestampByte | middleSignificantTimestampByte | mostSignificantTimestampByte;
    return latestTimestamp;
}

void update32BitTimestamp (UInt8 *buffer) {
    static UInt32 storedLargeTimestamp = 0;
    
    UInt32 latestTimestamp = latest18BitTimeStampFromBuffer(buffer);
    UInt32 firstEighteenBitsOfStoredLargeTimstamp = storedLargeTimestamp & AND_BIT_MASK_ZERO_TO_SEVENTEEN;
    
    //if Timestamp rollover has occured;
    if(latestTimestamp < firstEighteenBitsOfStoredLargeTimstamp) {
        storedLargeTimestamp += 1 << 18;
        storedLargeTimestamp = (storedLargeTimestamp & AND_BIT_MASK_EIGHTEEN_TO_THIRTYONE) + latestTimestamp;
    } else {
        storedLargeTimestamp = latestTimestamp;
    }
}




