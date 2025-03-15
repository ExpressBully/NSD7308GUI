#ifndef PORT_DATA_H
#define PORT_DATA_H

#include <Windows.h>
#include <string>
#include <functional>

class PortData
{
public:
    using DataReceivedCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    PortData();
    PortData(HANDLE hPort, DCB& dcb);
    ~PortData();

    void ResetPort(HANDLE hPort, DCB& dcb);

    void SendData(const std::string& data);
    void SendData(const char* data, int len);
    void ReceiveData(std::string& data);
    void ReceiveData(char* data, int len);

    void SetDataReceivedCallback(DataReceivedCallback callback);
    void SetErrorCallback(ErrorCallback callback);

    bool IsOpen() const;

protected:
    bool OpenPort();
    bool ClosePort();

private:
    HANDLE mhPort;
    DCB mDcb;
    DataReceivedCallback mDataReceivedCallback;
    ErrorCallback mErrorCallback;
};

#endif // PORT_DATA_H