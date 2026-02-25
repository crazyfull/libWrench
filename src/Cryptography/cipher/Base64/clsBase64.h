#ifndef CLSBASE64_H
#define CLSBASE64_H

#define MAX_BASE65_SIZE 256

class CString;
class clsBase64
{
public:
    clsBase64();
    static void toBase64(const char* input, int inputSize, CString *Out);
    static void fromBase64(const char* input, CString *Out);
};

#endif // CLSBASE64_H
