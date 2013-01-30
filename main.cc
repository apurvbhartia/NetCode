/* -*-  Mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*- */
/* main.cc	-- Yin Zhang Mon Jun 21 2004
 *
 * 
 */

#include <stdlib.h>

#include "util.h"
#include "GaloisField.h"
#include "NetCoder.h"

int main(int argc, char **argv)
{
  srand(1);

  // Test Galois Field
  GaloisField *gf = new GaloisField(8);
  gf->Test();

  NetCoder::Test(2,10);
  //NetCoder::SpeedTest(16,1500);

  //InterCoder::Test(3,6,10);
}
