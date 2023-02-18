/**
 * FAIRY COMPUTER SYSTEM 80 - SOUND CREATIVE CHIP Emulator
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 * 
 * Copyright (c) 2023 Yoji Suzuki.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#ifndef INCLUDE_SCC_HPP
#define INCLUDE_SCC_HPP
#include <string.h>

class SCC {
public:
    bool enabled;

    struct Channel {
        signed char waveforms[32];
        int counter;
        unsigned short period;
        unsigned char volume;
        unsigned char index;
    };
    
    struct Context {
        struct Channel ch[5];
        int sw;
        int mode;
    } ctx;

    SCC() {
        this->reset();
        this->enabled = false;
    }

    void reset() { memset(&this->ctx, 0, sizeof(this->ctx)); }

    inline unsigned char read(unsigned short addr) {
        addr &= 0xFF;
        if (addr < 0x20) {
            return this->ctx.ch[0].waveforms[addr & 0x1F];
        } else if (addr < 0x40) {
            return this->ctx.ch[1].waveforms[addr & 0x1F];
        } else if (addr < 0x60) {
            return this->ctx.ch[2].waveforms[addr & 0x1F];
        } else if (addr < 0x80) {
            return this->ctx.ch[3].waveforms[addr & 0x1F];
        } else {
            return 0xFF;
        }
    }

    inline void write(unsigned short addr, unsigned char value) {
        addr &= 0xFF;
        if (addr & 0x80) {
            addr &= 0x3F;
            switch (addr) {
                case 0x00:
                case 0x02:
                case 0x04:
                case 0x06:
                case 0x08: {
                    Channel* ch = &this->ctx.ch[addr >> 1];
                    ch->period = (ch->period & 0xf00) | value;
                    break;
                }
                case 0x01:
                case 0x03:
                case 0x05:
                case 0x07:
                case 0x09: {
                    Channel* ch = &this->ctx.ch[addr >> 1];
                    ch->period = (ch->period & 0xff) | ((value & 0x0F) << 8);
                    break;
                }
                case 0x0A:
                case 0x0B:
                case 0x0C:
                case 0x0D:
                case 0x0E:
                    this->ctx.ch[addr - 0x0A].volume = value & 0x0F;
                    break;
                case 0x0F:
                    this->ctx.sw = value & 0x1F;
                    break;
            }
        } else {
            this->ctx.ch[addr >> 5].waveforms[addr & 0x1F] = (signed char)value;
        }
    }
    
    inline void setMode(unsigned char mode) {
        this->ctx.mode = mode;
    }

    inline void tick(short* left, short* right, unsigned int cycles) {
        if (!this->enabled) return;
        int mix[5];
        int i;
        int sw = this->ctx.sw;
        for(i = 0; i < 5; i++) {
            Channel* ch = &this->ctx.ch[i];
            if (ch->period) {
                ch->counter += cycles;
                while (0 <= ch->counter) {
                    ch->counter -= ch->period;
                    ch->index++;
                    ch->index &= 0x1F;
                }
            } else {
                ch->index++;
                ch->index &= 0x1F;
            }
            if (sw & 1) {
                int si = (4 == i && 0 == (this->ctx.mode & 0x20)) ? 3 : i;
                mix[i] = this->ctx.ch[si].waveforms[ch->index] * ch->volume;
            } else {
                mix[i] = 0;
            }
            sw >>= 1;
        }
        int result = (mix[0] + mix[1] + mix[2]) & 0xFFFF;
        result |= ((mix[0] + mix[3] + mix[4]) & 0xffff) << 16;
        *left = (short)((*left) + result);
        *right = (short)((*right) + result);
    }
};

#endif /* INCLUDE_SCC_HPP */
