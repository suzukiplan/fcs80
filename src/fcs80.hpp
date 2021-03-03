/**
 * FAIRY COMPUTER SYSTEM 80 - Machine Emulator
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

#include "ay8910.hpp"
#include "fcs80video.hpp"
#include "z80.hpp"

#define VGS80_CPU_CLOCK_PER_SEC 3579545
#define VGS80_VDP_CLOCK_PER_SEC 4024320 /* 256 * 262 * 60 Hz */
#define VGS80_PSG_CLOCK_PER_SEC 44100

class FCS80 {
    private:
        unsigned char* rom;
        size_t romSize;
        short soundBuffer[65536];
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
            this->cpu = new Z80([](void* arg, unsigned short addr) { return ((FCS80*)arg)->readMemory(addr); }, [](void* arg, unsigned short addr, unsigned char value) { return ((FCS80*)arg)->writeMemory(addr, value); }, [](void* arg, unsigned char port) { return ((FCS80*)arg)->inPort(port); }, [](void* arg, unsigned char port, unsigned char value) { return ((FCS80*)arg)->outPort(port, value); }, this);
            this->cpu->setConsumeClockCallback([](void* arg, int clocks) { ((FCS80*)arg)->consumeClock(clocks); });
            this->vdp = new FCS80Video(this, [](void* arg) { ((FCS80*)arg)->cpu->requestBreak(); }, [](void* arg) { ((FCS80*)arg)->cpu->generateIRQ(0x07); });
            this->psg = new AY8910();
            this->rom = NULL;
            this->romSize = 0;
            this->reset();
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
            memset(this->soundBuffer, 0, sizeof(this->soundBuffer));
            this->soundCursor = 0;
        }

        bool loadRom(void* rom, size_t size) {
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
            this->vdp->rom = this->rom;
            this->vdp->romSize = this->romSize;
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

    private:
        inline void consumeClock(int clocks) {
            this->vdp->ctx.bobo += clocks * VGS80_VDP_CLOCK_PER_SEC;
            while (VGS80_CPU_CLOCK_PER_SEC <= this->vdp->ctx.bobo) {
                this->vdp->tick();
                this->vdp->ctx.bobo -= VGS80_CPU_CLOCK_PER_SEC;
            }
            this->psg->ctx.bobo += clocks * VGS80_PSG_CLOCK_PER_SEC;
            while (VGS80_CPU_CLOCK_PER_SEC <= this->psg->ctx.bobo) {
                short* l = &this->soundBuffer[this->soundCursor];
                this->soundCursor++;
                short* r = &this->soundBuffer[this->soundCursor];
                this->soundCursor++;
                this->psg->tick(l, r, 81);
                this->vdp->ctx.bobo -= VGS80_CPU_CLOCK_PER_SEC;
            }
        }

        inline unsigned char readMemory(unsigned short addr) {
            if (addr < 0x8000) {
                int ptr = this->ctx.romBank[addr / 0x2000] * 0x2000;
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
                case 0xE0: return this->ctx.romBank[0];
                case 0xE1: return this->ctx.romBank[1];
                case 0xE2: return this->ctx.romBank[2];
                case 0xE3: return this->ctx.romBank[3];
                case 0xF0: return this->vdp->ctx.bgBank;
                case 0xF1: return this->vdp->ctx.fgBank;
                case 0xF2: return this->vdp->ctx.spriteBank;
                default: return 0xFF;
            }
        }

        inline void outPort(unsigned char port, unsigned char value) {
            switch (port) {
                case 0xA0: this->psg->latch(value); break;
                case 0xA1: this->psg->write(value); break;
                case 0xE0: this->ctx.romBank[0] = value; break;
                case 0xE1: this->ctx.romBank[1] = value; break;
                case 0xE2: this->ctx.romBank[2] = value; break;
                case 0xE3: this->ctx.romBank[3] = value; break;
                case 0xF0: this->vdp->ctx.bgBank = value; break;
                case 0xF1: this->vdp->ctx.fgBank = value; break;
                case 0xF2: this->vdp->ctx.spriteBank = value; break;
            }
        }
};

#endif
