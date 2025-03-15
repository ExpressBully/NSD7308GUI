#ifndef PORT_MANAGER_H
#define PORT_MANAGER_H

#include <vector>
#include <mutex>
#include <memory>
#include <string>
#include <functional>
#include <Windows.h>

class PortData;

class PortManager
{
public:
    using DataReceivedCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    static PortManager& GetInstance();

    PortManager(const PortManager&) = delete;
    PortManager& operator=(const PortManager&) = delete;

    void RefreshPortList();
    bool SetCurrentPort(const std::string& portName);
    const std::string& GetCurrentPort() const noexcept;
    const std::vector<std::string>& GetPortList() const noexcept;

    void SendData(const std::string& data);
    void ReceiveData(std::string& data);

    void SetDataReceivedCallback(DataReceivedCallback callback);
    void SetErrorCallback(ErrorCallback callback);

    bool IsPortOpen() const;

private:
    PortManager();
    ~PortManager();

    HANDLE CreatePort(const std::string& portName) const;
    DCB GetDefaultPortConfig() const;
private:
    mutable std::mutex mMutex;
    std::string mCurrentPort;
    std::vector<std::string> mPortList;
    std::unique_ptr<PortData> mActivePort;
};

#endif // PORT_MANAGER_H