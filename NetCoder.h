/* -*-  Mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*- */
/* NetCoder.h	-- Yin Zhang
 *
 * My implementation of random linear codes
 *
 * $Header: /u/lili/cvsroot/OPRCoding/NetCode/NetCoder.h,v 1.5 2008/11/25 01:36:01 yzhang Exp $
 */

#ifndef netcoder_h
#define netcoder_h

#include "util.h"
#include "GaloisField.h"

class NetCoder
{
public:
  // constructor
  NetCoder(int grp_size, int data_len);
  // destructor
  ~NetCoder();

  // add one packet
  int AddPacket(uint8 *pkt);

  // reset num_pkts to 0
  void Reset()   { num_pkts = 0; }

  // encode based on a given list of packets
  uint8* Encode(int n, int plen, uint8 **pkts, unsigned char *coeffs);

  // encode based on the set of packets in the current coder
  uint8* Encode();

  // decode based on a given list of packets
  uint8** Decode(int n, int gsize, int plen, uint8 **pkts, bool GE_done = false, bool copy_pkts = true);
  
  // decode based on the set of packets in the current coder
  uint8** Decode();

  // get the number of packets whose first few coefficients are all 0s
  // needed by InterCoder.
  int GetNumPktsWithZeroCoef(int ncoef);
  
  static void Test(int grp_size = 8, int data_len = 20);
  static void SpeedTest(int grp_size = 8, int data_len = 20);
  
protected:
  inline uint8 Mul(uint8 i, uint8 j) const { return mul_tbl[i][j]; }
  inline uint8 Inv(uint8 i) const          { return inv_tbl[i]; }
  int GaussianElimination(int n, int gsize, int plen, uint8 **pkts);
  void BackSubstitution(int n, int gsize, int plen, uint8 **pkts);
  void InitTables();
  
  // tables for computing mulplication and inverse on GF(256)
  static uint8 mul_tbl[256][256];
  static uint8 inv_tbl[256];
  static bool initialized;

public:  
  // set of packets
  int num_pkts;
  int grp_size;
  int data_len;
  int pkt_len;
  uint8 **pkt_lst;  
  uint8 *d_coeffs;
};


#endif
