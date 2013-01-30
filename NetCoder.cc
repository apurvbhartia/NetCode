/* -*-  Mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*- */
/* NetCoder.cc	-- Yin Zhang Tue Oct 23 2007
 *
 * My implementation of random linear codes
 *
 * $Header: /u/lili/cvsroot/OPRCoding/NetCode/NetCoder.cc,v 1.6 2008/11/25 01:36:01 yzhang Exp $
 */

#include "NetCoder.h"
#include <stdlib.h>

/* static members of NetCoder */
bool NetCoder::initialized = false;
uint8 NetCoder::mul_tbl[256][256];
uint8 NetCoder::inv_tbl[256];

NetCoder::NetCoder(int arg_grp_size, int arg_data_len)
{
  int i;
  grp_size = arg_grp_size;
  data_len = arg_data_len;
  pkt_len = grp_size + data_len;
  num_pkts = 0;

  // Allocate buffer
  pkt_lst = new uint8*[grp_size];
  for (i = 0; i < grp_size; i++)
    pkt_lst[i] = new uint8[pkt_len];

  d_coeffs = new uint8[grp_size];
  InitTables();
}

void NetCoder::InitTables()
{
    // Initialize mul_tbl and inv_tbl
  if (!initialized) {
    initialized = true;

    GaloisField* gf8 = new GaloisField(8);
    uint64 i, j;

    // initialize multiplication table
    for (i = 0; i < 256; i++)
      for (j = 0; j < 256; j++)
        mul_tbl[i][j] = gf8->Mul(i,j);

    // initialize inverse table
    inv_tbl[0] = 0;
    for (i = 1; i < 256; i++)
      inv_tbl[i] = gf8->Inv(i);
    
    delete gf8;
  }
}

NetCoder::~NetCoder()
{
  int i;
  for (i = 0; i < grp_size; i++)
    delete[] pkt_lst[i];
  delete[] pkt_lst;
  delete[] d_coeffs;
}

uint8* NetCoder::Encode(int n, int plen, uint8**pkts, unsigned char *coeffs)
{
  printf("Encode\n"); fflush(stdout);
  int i, k;
  uint8 *mtbl, *pi, *pkt;

  if (n == 0)
    return(0);

  pkt = new uint8[plen];
  if (n == 1) {
    memcpy(pkt, pkts[0], plen);
    return pkt;
  }

  // initialize pkt to 0
  memset(pkt, 0, plen);

  for (i = 0; i < n; i++) {
    coeffs[i] = uint8(rand() & 0xff);
    mtbl = mul_tbl[coeffs[i]];
    pi = pkts[i];
    for (k = 0; k < plen; k++)
      // pkt[k] ^= Mul(coeffs[i], pkts[i][k])
      pkt[k] ^= mtbl[pi[k]];
  }

  printf("coeffs[0]: %d, coeffs[1]: %d\n", coeffs[0], coeffs[1]); fflush(stdout);
  return pkt;
}

uint8* NetCoder::Encode()
{
  return Encode(num_pkts, pkt_len, pkt_lst, d_coeffs);
}

int NetCoder::AddPacket(uint8 *pkt)
{
  if (num_pkts == grp_size)
    // No need to add any more packet
    return(num_pkts);

  // copy the packet
  memcpy(pkt_lst[num_pkts], pkt, pkt_len);
  num_pkts++;

  // perform Gaussian elimination
  num_pkts = GaussianElimination(num_pkts, grp_size, pkt_len, pkt_lst);
  return num_pkts;  
}

int NetCoder::GetNumPktsWithZeroCoef(int ncoef)
{
  int i, j;
  i = 0;
  j = 0;
  while ((i < num_pkts) && (j < ncoef)) {
    if (pkt_lst[i][j])
      i++;  // go to next row
    else
      j++;  // go to next column
  }
  return(num_pkts - i);
}

int NetCoder::GaussianElimination(int n, int gsize, int plen, uint8**pkts)
{
  int i, j, k, u, nzi, rank;
  uint8 *tmp, *pi, *pu, *mtbl;

  i = 0;
  j = 0;
  while ((i < n) && (j < gsize)) {
    // Find pivot in column j, starting in row i
    nzi = i;
    if (pkts[nzi][j] == 0) {
      for (k = i+1; k < n; k++) {
        if (pkts[k][j] != 0) {
          nzi = k;
          break;
        }
      }
    }

    if (pkts[nzi][j] != 0) {
      // swap rows i and nzi, but do not change the value of i
      // now pkts[i][j] will contain the old value of pkts[nzi][j]
      if (i != nzi) {
        tmp = pkts[i];
        pkts[i] = pkts[nzi];
        pkts[nzi] = tmp;
      }

      // divide each entry in row i by pkts[i][j]
      // now pkts[i][j] will have the value 1
      pi = pkts[i];
      if (pi[j] != 1) {
        mtbl = mul_tbl[inv_tbl[pi[j]]];
        for (k = j; k < pkt_len; k++)
          // pkts[i][k] = Mul(Inv(pkts[i][j]), pkts[i][k])
          pi[k] = mtbl[pi[k]];
      }

      for (u = i+1; u < n; u++) {
        // subtract pkts[u][j] * row i from row u
        pu = pkts[u];
        if (pu[j] != 0) {
          mtbl = mul_tbl[pu[j]];
          for (k = j; k < pkt_len; k++) {
            // pkts[u][k] ^= Mul(pkts[u][j], pkts[i][k]);
            pu[k] ^= mtbl[pi[k]];
          }
        }
      }
      // move to the next row
      i++;
    }
    // move to the next column
    j++;
  }

  rank = i;
  return(rank);
}

void NetCoder::BackSubstitution(int n, int gsize, int plen, uint8 **pkts)
{
  int i, j, k;
  uint8 *pi, *pj, *mtbl;
  
  if (n < gsize)
    return;

  // we now have a upper-triangular matrix with an all-1 diagonal
  for (i = n-1; i > 0; i--) {
    pi = pkt_lst[i];
    for (j = 0; j < i; j++) {
      // subtract pkt_lst[j][i] * row i from row j
      pj = pkt_lst[j];
      if (pj[i] != 0) {
        mtbl = mul_tbl[pj[i]];
        for (k = j+1; k <= i; k++)
          // pkt_lst[j][k] ^= Mul(pkt_lst[j][i], pkt_lst[i][k])
          pj[k] ^= mtbl[pi[k]];
        for (k = gsize; k < plen; k++)
          // pkt_lst[j][k] ^= Mul(pkt_lst[j][i], pkt_lst[i][k])
          pj[k] ^= mtbl[pi[k]];          
      }
    }
  }
}

uint8** NetCoder::Decode(int n, int gsize, int plen, uint8 **pkts, bool GE_done, bool copy_pkts)
{
  int i, rank;
  uint8 **pkts_copy;

  if (n < gsize)
    return(0);

  // copy the packets if needed
  if (!copy_pkts)
    pkts_copy = pkts;
  else{
    pkts_copy = new uint8*[n];
    for (i = 0; i < n; i++) {
      pkts_copy[i] = new uint8[plen];
      memcpy(pkts_copy[i], pkts[i], plen);
    }
  }

  // perform gaussian elimination if needed
  if (GE_done)
    rank = n;
  else
    rank = GaussianElimination(n, gsize, plen, pkts_copy);
  
  if (rank < gsize) {
    if (copy_pkts) {
      for (i = 0; i < n; i++)
        delete[] pkts_copy[i];
      delete[] pkts_copy;
    }
    return(0);
  }

  BackSubstitution(n, gsize, plen, pkts_copy);

  if (copy_pkts)
    for (i = gsize; i < n; i++)
      delete[] pkts_copy[i];

  return pkts_copy;
}

uint8** NetCoder::Decode()
{
  return Decode(num_pkts, grp_size, pkt_len, pkt_lst, true, false);
}

void NetCoder::Test(int grp_size, int data_len)
{
  int i, j;
  int pkt_len;
  uint8* pkts[2*grp_size];

  pkt_len = grp_size + data_len;
  for (i = 0; i < grp_size; i++) {
    pkts[i] = new uint8[pkt_len];
    for (j = grp_size; j < pkt_len; j++)
      pkts[i][j] = uint8(rand() & 0xff);
    for (j = 0; j < grp_size; j++)
      pkts[i][j] = 0;
    pkts[i][i] = 1;
  }

  NetCoder *encoder = new NetCoder(grp_size, data_len);
  for (i = 0; i < grp_size; i++)
     encoder->AddPacket(pkts[i]);

  for (i = 0; i < grp_size; i++)
    pkts[grp_size+i] = encoder->Encode();

  printf("original content:\n");
  for (i = 0; i < grp_size; i++) {
    printf("%3d ", i);
    //for (j = 0; j < data_len; j++)
      //printf("%02X ", int(pkts[i][grp_size+j]));
    for(j = 0; j < pkt_len; j++)
	printf("%02X ", int(pkts[i][j]));
    printf("\n");
  }
  
  printf("encoded content:\n");
  for (i = 0; i < grp_size; i++) {
    printf("%3d ", i);
    //for (j = 0; j < data_len; j++)
      //printf("%02X ", int(pkts[i+grp_size][grp_size+j]));
    for(j = 0; j < pkt_len; j++)
        printf("%02X ", int(pkts[i+grp_size][j]));
    printf("\n");
  }

#if 1
  printf("------------\n"); fflush(stdout);
  uint8* pkts1[2*grp_size];
  pkt_len = grp_size + data_len;
  for (i = 0; i < grp_size; i++) {
    pkts1[i] = new uint8[pkt_len];
    for (j = grp_size; j < pkt_len; j++)
      pkts1[i][j] = uint8(rand() & 0xff);
    for (j = 0; j < grp_size; j++)
      pkts1[i][j] = 0;
    pkts1[i][i] = 1;
  }

  printf("reencoder\n"); fflush(stdout);
  NetCoder *reEncoder = new NetCoder(grp_size, data_len);
  for(i = 0; i < grp_size; i++) {
      reEncoder->AddPacket(pkts[grp_size+i]);			// add encoded pkt
      memcpy(pkts1[i], reEncoder->Encode(), pkt_len);
      for (j = 0; j < pkt_len; j++)
	printf("%02X ", int(pkts1[i][j]));
      printf("\n");	
      //pkts1[i] = reEncoder->Encode();
  }
  printf("re_encoded content:\n");
  for (i = 0; i < grp_size; i++) {
    printf("%3d ", i);
    for (j = 0; j < pkt_len; j++)
      printf("%02X ", int(pkts1[i][j]));
    printf("\n");
  }

  NetCoder *redecoder = new NetCoder(grp_size, data_len);
  for(i = 0; i < grp_size; i++)
      redecoder->AddPacket(pkts1[i]);
  
  uint8** res1 = redecoder->Decode();

  printf("redecoded content:\n");
  for (i = 0; i < grp_size; i++) {
    printf("%3d ", i);
    for (j = 0; j < pkt_len; j++)
      printf("%02X ", int(res1[i][j]));
    printf("\n");
  }

#endif

  


  NetCoder *decoder = new NetCoder(grp_size, data_len);
  for (i = 0; i < grp_size; i++)
     decoder->AddPacket(pkts[i+grp_size]);
  
  uint8** res = decoder->Decode();

  printf("decoded content:\n");
  for (i = 0; i < grp_size; i++) {
    printf("%3d ", i);
    for (j = 0; j < pkt_len; j++)
      printf("%02X ", int(res[i][j]));
    printf("\n");
  }  
}

void NetCoder::SpeedTest(int grp_size, int data_len)
{
  int i, j, k, n, pkt_len;

  pkt_len = grp_size + data_len;
  n = grp_size;
  uint8 *pkts[2*n];
  uint8 coeffs[n];

  for (i = 0; i < n; i++) {
    pkts[i] = new uint8[pkt_len];
    for (j = grp_size; j < pkt_len; j++)
      pkts[i][j] = uint8(rand() & 0xff);
    for (j = 0; j < grp_size; j++)
      pkts[i][j] = 0;
    pkts[i][i] = 1;
  }

  NetCoder *coder = new NetCoder(grp_size, data_len);
  for (i = 0; i < n; i++)
    pkts[n+i] = coder->Encode(n, pkt_len, pkts, coeffs);
  
  int runs = 10000;
  double t0 = current_time();
  for (k = 0; k < runs; k++) {
    for (i = 0; i < n; i++)
      delete[] coder->Encode(n, pkt_len, pkts, coeffs);
  }
  double t1 = current_time();
  printf("Encode() speed test: data_len = %d grp_size = %d time/pkt = %.3f msec BW = %.3f Mbps\n",
         data_len, grp_size, (t1-t0)*1000/runs/grp_size, data_len*8*runs*grp_size/(t1-t0)/1e6);

  t0 = current_time();
  for (k = 0; k < runs; k++) {
    delete[] coder->Decode(n, grp_size, pkt_len, pkts + grp_size, false, true);
  }
  t1 = current_time();
  printf("Decode() speed test: data_len = %d grp_size = %d time/pkt = %.3f msec BW = %.3f Mbps\n",
         data_len, grp_size, (t1-t0)*1000/runs/grp_size, data_len*8*runs*grp_size/(t1-t0)/1e6);

}
