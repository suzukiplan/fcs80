//
//  VideoView.m
//  EmuBoard
//
//  Created by 鈴木　洋司　 on 2018/12/30.
//  Copyright © 2018年 SUZUKIPLAN. All rights reserved.
//

#import "EmuBoard.h"
#import "VideoLayer.h"
#import "VideoView.h"
#import "constants.h"
#import "fcs80def.h"
#include <ctype.h>

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* context);
extern unsigned char emu_key;

@interface VideoView ()
@property (nonatomic) VideoLayer* videoLayer;
@property CVDisplayLinkRef displayLink;
@property NSInteger score;
@property BOOL isGameOver;
@end

@implementation VideoView

+ (Class)layerClass
{
    return [VideoLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame]) != nil) {
        [self setWantsLayer:YES];
        _score = -1;
        _isGameOver = -1 ? YES : NO;
        _videoLayer = [VideoLayer layer];
        [self setLayer:_videoLayer];
        CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
        CVDisplayLinkSetOutputCallback(_displayLink, MyDisplayLinkCallback, (__bridge void*)self);
        CVDisplayLinkStart(_displayLink);
    }
    return self;
}

- (void)releaseDisplayLink
{
    if (_displayLink) {
        CVDisplayLinkStop(_displayLink);
        CVDisplayLinkRelease(_displayLink);
        _displayLink = nil;
    }
}

- (void)vsync
{
    emu_vsync();
    [self.videoLayer drawVRAM];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)keyDown:(NSEvent*)event
{
    unichar c = [event.charactersIgnoringModifiers characterAtIndex:0];
    //NSLog(@"keyDown: %04X", tolower(c));
    switch (tolower(c)) {
        case 0xF703: emu_key |= FCS80_JOYPAD_RI; break;
        case 0xF702: emu_key |= FCS80_JOYPAD_LE; break;
        case 0xF701: emu_key |= FCS80_JOYPAD_DW; break;
        case 0xF700: emu_key |= FCS80_JOYPAD_UP; break;
        case 0x007A: emu_key |= FCS80_JOYPAD_T1; break;
        case 0x0078: emu_key |= FCS80_JOYPAD_T2; break;
        case 0x0061: emu_key |= FCS80_JOYPAD_SE; break;
        case 0x0073: emu_key |= FCS80_JOYPAD_ST; break;
        case 0x0064: emu_dump(); break;
        case 0x006C: [self _openSaveData]; break;
    }
}

- (void)keyUp:(NSEvent*)event
{
    unichar c = [event.charactersIgnoringModifiers characterAtIndex:0];
    switch (tolower(c)) {
        case 0xF703: emu_key &= ~FCS80_JOYPAD_RI; break;
        case 0xF702: emu_key &= ~FCS80_JOYPAD_LE; break;
        case 0xF701: emu_key &= ~FCS80_JOYPAD_DW; break;
        case 0xF700: emu_key &= ~FCS80_JOYPAD_UP; break;
        case 0x007A: emu_key &= ~FCS80_JOYPAD_T1; break;
        case 0x0078: emu_key &= ~FCS80_JOYPAD_T2; break;
        case 0x0061: emu_key &= ~FCS80_JOYPAD_SE; break;
        case 0x0073: emu_key &= ~FCS80_JOYPAD_ST; break;
    }
}

- (void)_openSaveData
{
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    panel.allowsMultipleSelection = NO;
    panel.canChooseDirectories = NO;
    panel.canCreateDirectories = YES;
    panel.canChooseFiles = YES;
    panel.allowedFileTypes = @[ @"dat" ];
    [panel beginWithCompletionHandler:^(NSModalResponse result) {
        if (!result) return;
        NSData* data = [NSData dataWithContentsOfURL:panel.URL];
        if (!data) return;
        emu_loadState(data.bytes, data.length);
    }];
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* context)
{
    [(__bridge VideoLayer*)context performSelectorOnMainThread:@selector(vsync) withObject:nil waitUntilDone:NO];
    return kCVReturnSuccess;
}

@end
