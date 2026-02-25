#include "clsBase64.h"
//#include "cencode.h"
#include "base64.h"
#include "clsCString.h"
#include <string.h>

clsBase64::clsBase64()
{

}

void clsBase64::toBase64(const char *input, int inputSize, CString *Out)
{
    if(!input)
        return;

    if( inputSize == 0 || inputSize == -1)
        return;

    //
    int len = Base64encode_len(inputSize);

    //buffer
    char *buf = new char[len];
    memset(buf, 0, len);

    //tobase 64
    int ret = Base64encode(buf, input, inputSize);
    //LOG("Base64encode %d, len: %d", ret, len);

    //
    if(ret > 0){
        Out->SetDataWithNewPointer(buf, len-1);
    }else{
        delete[] buf;
    }

}

void clsBase64::fromBase64(const char *input, CString *Out)
{
    if(!input)
        return;

    int len = Base64decode_len(input);
    char *buf = new char[len+1];
    memset(buf, 0, len);
    int ret = Base64decode(buf, input);


    if(ret > 0){
        Out->SetDataWithNewPointer(buf, ret);
    }else{
        delete[] buf;
    }
    /*
    std::ostringstream out;
    for (int i = 0; i < len; i++) {
        out << buf[i];
    }
*/

}



