#ifndef SWAP_H
#define SWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define ConvBBBB_LE(i) (i)
#define ConvBB_LE(i) (i)
#define ConvWW_LE(i) (i)

#define ConvBBBB_BE(i) SwapBBBB(i)
#define ConvBB_BE(i) SwapBB(i)
#define ConvWW_BE(i) SwapWW(i)

unsigned int SwapBBBB(unsigned int i);
unsigned int SwapBB(unsigned int i);
unsigned int SwapWW(unsigned int i);

#ifdef __cplusplus
}
#endif

#endif

