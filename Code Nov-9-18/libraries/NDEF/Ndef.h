#ifndef Ndef_h
#define Ndef_h

#include <string.h>
/* NOTE: To use the Ndef library in your code, don't include Ndef.h
   See README.md for details on which files to include in your sketch.
*/

#include <Arduino.h>

#define NULL (void *)0

void PrintHex(const byte *data, const long numBytes);
void PrintHexChar(const byte *data, const long numBytes);
void DumpHex(const byte *data, const long numBytes, const int blockSize);
String returnHexString(const byte * data, const long numBytes);

#endif