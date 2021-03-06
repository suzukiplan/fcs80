/**
 * FAIRY COMPUTER SYSTEM 80 - Core Emulator
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 * 
 * Copyright (c) 2020 Yoji Suzuki.
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

#ifndef INCLUDE_FCS80_HPP
#define INCLUDE_FCS80_HPP

#include "fcs80def.h"
#include "ay8910.hpp"
#include "fcs80video.hpp"
#include "z80.hpp"

class FCS80 {
    private:
        unsigned char* rom;
        size_t romSize;
        short soundBuffer[0x10000];
        unsigned short soundCursor;

    public:
        Z80* cpu;
        AY8910* psg;
        FCS80Video* vdp;

        struct Context {
            int bobo;
            unsigned char ram[0x4000];
            unsigned char romBank[4];
        } ctx;

        FCS80() {
            this->cpu = new Z80([](void* arg, unsigned short addr) {
                return ((FCS80*)arg)->readMemory(addr);
            }, [](void* arg, unsigned short addr, unsigned char value) {
                return ((FCS80*)arg)->writeMemory(addr, value);
            }, [](void* arg, unsigned char port) {
                return ((FCS80*)arg)->inPort(port);
            }, [](void* arg, unsigned char port, unsigned char value) {
                return ((FCS80*)arg)->outPort(port, value);
            }, this);
            this->cpu->setConsumeClockCallback([](void* arg, int clocks) {
                ((FCS80*)arg)->consumeClock(clocks);
            });
            this->vdp = new FCS80Video(this, [](void* arg) {
                ((FCS80*)arg)->cpu->requestBreak();
            }, [](void* arg) {
                ((FCS80*)arg)->cpu->generateIRQ(0x07);
            });
            this->psg = new AY8910();
            this->rom = NULL;
            this->romSize = 0;
            this->reset();
            /*this->cpu->setDebugMessage([](void* arg, const char* msg) {
                FCS80* fcs80 = (FCS80*)arg;
                printf("line:%03d px:%03d %s\n", fcs80->vdp->ctx.countV, fcs80->vdp->ctx.countH, msg);
            });*/
            /*this->cpu->addBreakPoint(0x0024, [](void* arg) {
                puts("0024");
            });*/
        }

        ~FCS80() {
            delete this->psg;
            delete this->vdp;
            delete this->cpu;
        }

        void reset() {
            memset(&this->ctx, 0, sizeof(this->ctx));
            for (int i = 0; i < 4; i++) this->ctx.romBank[i] = i;
            this->vdp->reset();
            this->psg->reset(1);
            memset(&this->cpu->reg, 0, sizeof(this->cpu->reg));
            this->cpu->reg.SP = 0xFFFF;
            memset(this->soundBuffer, 0, sizeof(this->soundBuffer));
            this->soundCursor = 0;
        }

        bool loadRom(const void* rom, size_t size) {
            if (this->rom) free(this->rom);
            this->rom = NULL;
            size -= size % 0x2000;
            if (0 < size) {
                this->rom = (unsigned char*)malloc(size);
                if (!this->rom) {
                    this->romSize = 0;
                    return false;
                }
                memcpy(this->rom, rom, size);
            }
            this->romSize = size;
            return true;            
        }

        bool loadRomFile(const char* romFile) {
            FILE* fp = fopen(romFile, "rb");
            if (!fp) return false;
            if (-1 == fseek(fp, 0, SEEK_END)) {
                fclose(fp);
                return false;
            }
            long romSize = ftell(fp);
            if (romSize < 1|| -1 == fseek(fp, 0, SEEK_SET)) {
                fclose(fp);
                return false;
            }
            void* rom = malloc(romSize);
            if (!rom) {
                fclose(fp);
                return false;
            }
            if (romSize != fread(rom, romSize, 1, fp)) {
                fclose(fp);
                free(rom);
                return false;
            }
            bool result = this->loadRom(rom, romSize);
            free(rom);
            return result;
        }

        void tick(unsigned char pad1, unsigned char pad2)
        {
            this->psg->ctx.reg[14] = pad1;
            this->psg->ctx.reg[15] = pad2;
            this->cpu->execute(0x7FFFFFFF);
        }

        unsigned short* getDisplay() { return this->vdp->display; }
        size_t getDisplaySize() { return 240 * 192 * 2; }

        short* dequeSoundBuffer(size_t* size) {
            *size = 2 * this->soundCursor;
            this->soundCursor = 0;
            return this->soundBuffer;
        }

        size_t getStateSize() { return sizeof(this->ctx) + sizeof(this->cpu->reg) + sizeof(this->psg->ctx) + sizeof(this->vdp->ctx); }
 
        void saveState(void* buffer) {
            unsigned char* bufferPtr = (unsigned char*)buffer;
            memcpy(bufferPtr, &this->ctx, sizeof(this->ctx));
            bufferPtr += sizeof(this->ctx);
            memcpy(bufferPtr, &this->cpu->reg, sizeof(this->cpu->reg));
            bufferPtr += sizeof(this->cpu->reg);
            memcpy(bufferPtr, &this->psg->ctx, sizeof(this->psg->ctx));
            bufferPtr += sizeof(this->psg->ctx);
            memcpy(bufferPtr, &this->vdp->ctx, sizeof(this->vdp->ctx));
        }

        void loadState(const void* buffer) {
            const unsigned char* bufferPtr = (const unsigned char*)buffer;
            this->reset();
            memcpy(&this->ctx, bufferPtr, sizeof(this->ctx));
            bufferPtr += sizeof(this->ctx);
            memcpy(&this->cpu->reg, bufferPtr, sizeof(this->cpu->reg));
            bufferPtr += sizeof(this->cpu->reg);
            memcpy(&this->psg->ctx, bufferPtr, sizeof(this->psg->ctx));
            bufferPtr += sizeof(this->psg->ctx);
            memcpy(&this->vdp->ctx, bufferPtr, sizeof(this->vdp->ctx));
            this->vdp->refreshDisplay();
        }

    private:
        inline void consumeClock(int clocks) {
            this->vdp->ctx.bobo += clocks * FCS80_VDP_CLOCK_PER_SEC;
            while (0 < this->vdp->ctx.bobo) {
                this->vdp->tick();
                this->vdp->ctx.bobo -= FCS80_CPU_CLOCK_PER_SEC;
            }
            this->psg->ctx.bobo += clocks * FCS80_PSG_CLOCK_PER_SEC;
            while (0 < this->psg->ctx.bobo) {
                short* l = &this->soundBuffer[this->soundCursor];
                this->soundCursor++;
                short* r = &this->soundBuffer[this->soundCursor];
                this->soundCursor++;
                this->psg->tick(l, r, 81);
                this->psg->ctx.bobo -= FCS80_CPU_CLOCK_PER_SEC;
            }
        }

        inline unsigned char readMemory(unsigned short addr) {
            if (addr < 0x8000) {
                int ptr = this->ctx.romBank[addr / 0x2000] * 0x2000 + (addr & 0x1FFF);
                return (this->romSize <= ptr) ? 0xFF : this->rom[ptr];
            } else if (addr < 0xC000) {
                return this->vdp->read(addr);
            } else {
                return this->ctx.ram[addr & 0x3FFF];
            }
        }

        inline void writeMemory(unsigned short addr, unsigned char value) {
            if (addr < 0x8000) {
                this->ctx.romBank[addr / 0x2000] = value;
            } else if (addr < 0xC000) {
                this->vdp->write(addr, value);
            } else {
                this->ctx.ram[addr & 0x3FFF] = value;
            }
        }

        inline unsigned char inPort(unsigned char port) {
            switch (port) {
                case 0xA2: return this->psg->read();
                case 0xB0: return this->ctx.romBank[0];
                case 0xB1: return this->ctx.romBank[1];
                case 0xB2: return this->ctx.romBank[2];
                case 0xB3: return this->ctx.romBank[3];
                default: return 0xFF;
            }
        }

        inline void outPort(unsigned char port, unsigned char value) {
            switch (port) {
                case 0xA0: this->psg->latch(value); break;
                case 0xA1: this->psg->write(value); break;
                case 0xB0: this->ctx.romBank[0] = value; break;
                case 0xB1: this->ctx.romBank[1] = value; break;
                case 0xB2: this->ctx.romBank[2] = value; break;
                case 0xB3: this->ctx.romBank[3] = value; break;
                case 0xC0: {
                    unsigned short to = this->cpu->reg.pair.B;
                    to <<= 8;
                    to |= this->cpu->reg.pair.C;
                    unsigned short from = value;
                    from <<= 8;
                    unsigned short size = this->cpu->reg.pair.D;
                    size <<= 8;
                    size |= this->cpu->reg.pair.E;
                    for (int n = 0; n < size; n++, from++, to++) this->writeMemory(to, this->readMemory(from));
                }
            }
        }
};

#endif
