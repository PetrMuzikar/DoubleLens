#ifndef TEST_HPP_INCLUDED
#define TEST_HPP_INCLUDED

#include <string>
#include <fstream>

void testVec();
void testLens();
void testInteg();
void testImages();
void testInputRays();
void testSPG();
void testIRS();
void testSetup();
void testML();

void debugRaysToPixel(const std::string& prefix);
void debug(const std::string& prefix);
void debugRays(const std::string& prefix);
void run(const std::string& prefix);

void watchPixels(const std::string& prefix);

void imageOfSourcePlane(int argc, char* argv[], const std::string& prefix);
void shooting(int argc, char* argv[], const std::string& prefix);
void combineData(int argc, char* argv[], const std::string& prefix);
void rays(int argc, char* argv[], const std::string& prefix);
void magPoint(int argc, char* argv[], const std::string& prefix);

#endif // TEST_HPP_INCLUDED
