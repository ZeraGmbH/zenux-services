#ifndef DISKREADWRITEVALUES_H
#define DISKREADWRITEVALUES_H

#include <QtGlobal>

// hard coded for now
// * all /sys/block/<devicename>/queue/hw_sector_size reported 512
// * /sys/block/<devicename>/queue/hw_sector_size reported requires root
constexpr quint64 sectorSizeAllDevices = 512;

struct DiskValues {
    quint64 totalReadBytes = 0;
    quint64 totalWriteBytes = 0;
};

#endif // DISKREADWRITEVALUES_H
