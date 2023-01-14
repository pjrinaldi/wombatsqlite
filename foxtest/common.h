#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <bitset>

//std::string ConvertUnixTimeToHuman(uint32_t unixtime);

void ReadContent(std::ifstream* rawcontent, uint8_t* tmpbuf, uint64_t offset, uint64_t size);
void ReadContent(std::ifstream* rawcontent, int8_t* tmpbuf, uint64_t offset, uint64_t size);
void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8);
void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8);
void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8);
//void ReturnUint(uint64_t* tmp, uint8_t* tmp8, unsigned int length);
//void ReturnInt(int64_t* tmp, int8_t* tmp8, unsigned int length);

#endif
