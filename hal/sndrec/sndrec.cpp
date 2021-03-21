#include "../../src/fcs80.hpp"

struct DataHeader {
    char riff[4];
    unsigned int fsize;
    char wave[4];
    char fmt[4];
    unsigned int bnum;
    unsigned short fid;
    unsigned short ch;
    unsigned int sample;
    unsigned int bps;
    unsigned short bsize;
    unsigned short bits;
    char data[4];
    unsigned int dsize;
};

struct BufferChain {
    short buffer[65536];
    size_t size;
    struct BufferChain* next;
};

static struct BufferChain* fs_head = NULL;
static struct BufferChain* fs_tail = NULL;

static void addChain(short* buffer, size_t size)
{
    struct BufferChain* data = (struct BufferChain*)malloc(sizeof(struct BufferChain));
    memcpy(&data->buffer, buffer, size);
    data->size = size;
    data->next = NULL;
    if (fs_tail) {
        fs_tail->next = data;
        fs_tail = data;
    } else {
        fs_tail = data;
        fs_head = data;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        puts("sndrec rom_file wave_file frame_count");
        return 1;
    }
    FCS80 fcs80;
    printf("Loading ROM file: %s\n", argv[1]);
    if (!fcs80.loadRomFile(argv[1])) {
        puts("loadRom error");
        exit(-1);
    }
    printf("Open output wave file: %s\n", argv[2]);
    FILE* wav = fopen(argv[2], "wb");
    if (!wav) {
        puts("wave file open error");
        exit(-1);
    }

    /* initialize wave header */
    struct DataHeader dh;
    memset(&dh, 0, sizeof(dh));
    strncpy(dh.riff, "RIFF", 4);
    strncpy(dh.wave, "WAVE", 4);
    strncpy(dh.fmt, "fmt ", 4);
    strncpy(dh.data, "data", 4);
    dh.bnum = 16;
    dh.fid = 1;
    dh.ch = 2;
    dh.sample = 44100;
    dh.bps = 176400;
    dh.bsize = 2;
    dh.bits = 16;
    dh.dsize = sizeof(dh) - 8;

    // write prefix empty data
    {
        short noSound[4410];
        memset(noSound, 0, sizeof(noSound));
        addChain(noSound, sizeof(noSound));
        dh.dsize += sizeof(noSound);
    }

    // recording (specified frames)
    for (int frame = 0; frame < atoi(argv[3]); frame++) {
        fcs80.tick(0, 0);
        size_t size;
        short* buffer = fcs80.dequeSoundBuffer(&size);
        addChain(buffer, size);
        dh.dsize += size;
    }

    // fadeout (60 frames)
    for (int frame = 0; frame < 60; frame++) {
        fcs80.tick(0, 0);
        size_t size;
        short* buffer = fcs80.dequeSoundBuffer(&size);
        for (int i = 0; i < size; i++) {
            buffer[i] = (short)(buffer[i] * ((59 - frame) / 60.0));
        }
        addChain(buffer, size);
        dh.dsize += size;
    }

    // write wave file
    fwrite(&dh, 1, sizeof(dh), wav);
    struct BufferChain* next;
    for (struct BufferChain* cur = fs_head; cur; cur = next) {
        next = cur->next;
        fwrite(cur->buffer, 1, cur->size, wav);
        free(cur);
    }
    fclose(wav);
    printf("wrote %d bytes\n", dh.dsize + 8);
    puts("succeed");
    return 0;
}
