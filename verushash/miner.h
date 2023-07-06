#ifndef MINER_H
#define MINER_H
#define bswap_16(value)  \
  ((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
  (((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
  (uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) \
  (((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \
      << 32) | \
  (uint64_t)bswap_32((uint32_t)((value) >> 32)))


#  define htole16(x) (x)
#  define htole32(x) (x)
#  define htole64(x) (x)
#  define le32toh(x) (x)
#  define le64toh(x) (x)
#  define be32toh(x) bswap_32(x)
#  define be64toh(x) bswap_64(x)
#  define htobe32(x) bswap_32(x)
#  define htobe64(x) bswap_64(x)

#endif