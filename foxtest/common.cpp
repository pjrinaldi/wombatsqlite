#include "common.h"

/*
std::string ConvertUnixTimeToHuman(uint32_t unixtime)
{
    time_t timet = (time_t)unixtime;
    struct tm* tmtime = gmtime(&timet);
    char hchar[100];
    strftime(hchar, 100, "%m/%d/%Y %I:%M:%S %p UTC", tmtime);
    
    return std::string(hchar);
}
*/

void ReadContent(std::ifstream* rawcontent, uint8_t* tmpbuf, uint64_t offset, uint64_t size)
{
    rawcontent->seekg(offset);
    rawcontent->read((char*)tmpbuf, size);
}

void ReadContent(std::ifstream* rawcontent, int8_t* tmpbuf, uint64_t offset, uint64_t size)
{
    rawcontent->seekg(offset);
    rawcontent->read((char*)tmpbuf, size);
}

void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8)
{
    *tmp32 = __builtin_bswap32((uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24);
}

void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8)
{
    *tmp16 = __builtin_bswap16((uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8);
}

void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8)
{
    *tmp64 = __builtin_bswap64((uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56);
}

/*
void ReturnUint(uint64_t* tmp, uint8_t* tmp8, unsigned int length)
{
    for(unsigned int i=0; i < length; i++)
        *tmp |= (uint64_t)tmp8[i] << i * 8;
}

void ReturnInt(int64_t* tmp, int8_t* tmp8, unsigned int length)
{
    for(unsigned int i=0; i < length; i++)
        *tmp |= (int64_t)tmp8[i] << i * 8;
}
*/
