#ifndef PORT_DATA_H
#define PORT_DATA_H

#include <Windows.h>

class PortData
{
public:
    PortData();
    PortData(HANDLE hPort, DCB& dcb);
    ~PortData();

    void ResetPort(HANDLE hPort, DCB& dcb);

    void SendData(const std::string& data);
    void SendData(const char* data, int len);
    void ReceiveData(std::string& data);
    void ReceiveData(char* data, int len);

protected:
    bool OpenPort();
    bool ClosePort();
private:
    HANDLE mhPort;
    DCB mDcb;
};

#endif // PORT_DATA_H