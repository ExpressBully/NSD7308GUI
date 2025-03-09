#ifndef PORT_MANAGER_H
#define PORT_MANAGER_H

class PortData;

class PortManager
{
public:
    static PortManager& GetInstance();

    PortManager(const PortManager&) = delete;
    PortManager& operator=(const PortManager&) = delete;

    // 端口管理接口
    void RefreshPortList();
    void SetCurrentPort(const std::string& portName);
    const std::string& GetCurrentPort() const noexcept;
    const std::vector<std::string>& GetPortList() const noexcept;

    // 数据操作接口
    void SendData(const std::string& data);
    void ReceiveData(std::string& data);

private:
    PortManager();
    ~PortManager() = default;

    HANDLE CreatePortHandle(const std::string& portName) const;
    DCB GetDefaultPortConfig() const;

    mutable std::mutex mMutex;
    std::string mCurrentPort;
    std::vector<std::string> mPortList;
    std::unique_ptr<PortData> mActivePort;
};

#endif // PORT_MANAGER_H