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
#include "scc.hpp"

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
        SCC* scc;

        struct Context {
            int bobo;
            unsigned char ram[0x4000];
            unsigned char romBank[4];
            unsigned char cpuBoostFlag;
            unsigned char reserved[3];
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
            this->scc = new SCC();
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
            delete this->scc;
            delete this->psg;
            delete this->vdp;
            delete this->cpu;
        }

        void reset() {
            memset(&this->ctx, 0, sizeof(this->ctx));
            for (int i = 0; i < 4; i++) this->ctx.romBank[i] = i;
            this->vdp->reset();
            this->psg->reset(4);
            this->scc->reset();
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
            if (romSize != fread(rom, 1, romSize, fp)) {
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
            this->psg->setPads(pad1, pad2);
            this->cpu->execute(0x7FFFFFFF);
        }

        unsigned short* getDisplay() { return this->vdp->display; }
        size_t getDisplaySize() { return 240 * 192 * 2; }

        short* dequeSoundBuffer(size_t* size) {
            *size = 2 * this->soundCursor;
            this->soundCursor = 0;
            return this->soundBuffer;
        }

        size_t getStateSize() { return sizeof(this->ctx) + sizeof(this->cpu->reg) + sizeof(this->psg->ctx) + sizeof(this->vdp->ctx) + sizeof(this->scc->ctx); }
 
        void saveState(void* buffer) {
            unsigned char* bufferPtr = (unsigned char*)buffer;
            memcpy(bufferPtr, &this->ctx, sizeof(this->ctx));
            bufferPtr += sizeof(this->ctx);
            memcpy(bufferPtr, &this->cpu->reg, sizeof(this->cpu->reg));
            bufferPtr += sizeof(this->cpu->reg);
            memcpy(bufferPtr, &this->psg->ctx, sizeof(this->psg->ctx));
            bufferPtr += sizeof(this->psg->ctx);
            memcpy(bufferPtr, &this->vdp->ctx, sizeof(this->vdp->ctx));
            bufferPtr += sizeof(this->vdp->ctx);
            memcpy(bufferPtr, &this->scc->ctx, sizeof(this->scc->ctx));
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
            bufferPtr += sizeof(this->vdp->ctx);
            memcpy(&this->scc->ctx, bufferPtr, sizeof(this->scc->ctx));
        }

    private:
        inline void consumeClock(int clocks) {
            if (this->ctx.cpuBoostFlag) {
                // Normally, consumeClock is called back at an interval of 4Hz or 3Hz (fixed at 4Hz in the case of GBZ80),
                // but by always fixing it to 1Hz during cpuBoostFlag is not zero,
                // so the CPU execution speed can be accelerated by 4 to 3 times.
                clocks = 1;
            }
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
                this->scc->tick(l, r, 81);
                this->psg->ctx.bobo -= FCS80_CPU_CLOCK_PER_SEC;
            }
        }

        inline unsigned char readMemory(unsigned short addr) {
            if (addr < 0x8000) {
                int ptr = this->ctx.romBank[addr / 0x2000] * 0x2000 + (addr & 0x1FFF);
                return (this->romSize <= ptr) ? 0xFF : this->rom[ptr];
            } else if (addr < 0xC000) {
                if (0x9800 <= addr && addr < 0x9900) {
                    return this->scc->read(addr);
                } else {
                    return this->vdp->read(addr);
                }
            } else {
                return this->ctx.ram[addr & 0x3FFF];
            }
        }

        inline void writeMemory(unsigned short addr, unsigned char value) {
            if (addr < 0x8000) {
                this->ctx.romBank[addr / 0x2000] = value;
            } else if (addr < 0xC000) {
                if (0x9800 <= addr && addr < 0x9900) {
                    this->scc->write(addr, value);
                } else {
                    this->vdp->write(addr, value);
                }
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
                case 0xC1: return this->ctx.cpuBoostFlag;
                case 0xD0: return this->psg->read(0);
                case 0xD1: return this->psg->read(1);
                case 0xD2: return this->psg->read(2);
                case 0xD3: return this->psg->read(3);
                case 0xD4: return this->psg->read(4);
                case 0xD5: return this->psg->read(5);
                case 0xD6: return this->psg->read(6);
                case 0xD7: return this->psg->read(7);
                case 0xD8: return this->psg->read(8);
                case 0xD9: return this->psg->read(9);
                case 0xDA: return this->psg->read(10);
                case 0xDB: return this->psg->read(11);
                case 0xDC: return this->psg->read(12);
                case 0xDD: return this->psg->read(13);
                case 0xDE: return this->psg->read(14);
                case 0xDF: return this->psg->read(15);
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
                    int addr = this->cpu->reg.pair.A;
                    addr *= 0x2000;
                    if (addr + 0x2000 <= this->romSize) memcpy(&this->vdp->ctx.ram[0x2000], &this->rom[addr], 0x2000);
                    else memset(&this->vdp->ctx.ram[0x2000], 0xFF, 0x2000);
                    break;
                }
                case 0xC1: this->ctx.cpuBoostFlag = value; break;
                case 0xD0: this->psg->write(0, value); break;
                case 0xD1: this->psg->write(1, value); break;
                case 0xD2: this->psg->write(2, value); break;
                case 0xD3: this->psg->write(3, value); break;
                case 0xD4: this->psg->write(4, value); break;
                case 0xD5: this->psg->write(5, value); break;
                case 0xD6: this->psg->write(6, value); break;
                case 0xD7: this->psg->write(7, value); break;
                case 0xD8: this->psg->write(8, value); break;
                case 0xD9: this->psg->write(9, value); break;
                case 0xDA: this->psg->write(10, value); break;
                case 0xDB: this->psg->write(11, value); break;
                case 0xDC: this->psg->write(12, value); break;
                case 0xDD: this->psg->write(13, value); break;
                case 0xDE: this->psg->write(14, value); break;
                case 0xDF: this->psg->write(15, value); break;
            }
        }
};

#endif
