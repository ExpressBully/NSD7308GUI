#include <iostream>
#include "UART_DEC.h"
#include "imgui/imgui.h"
#include <windows.h>
#include <setupapi.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <initguid.h> // 必须包含此头文件以使用 DEFINE_GUID
#include <ntddser.h>  // 包含串口设备的 GUID 定义

#pragma comment(lib, "setupapi.lib")

// 全局变量定义
std::vector<std::string> usb_ports;
int selected_usb_index = -1;
bool ConnectFlag = false;
bool is_scanning = true;
std::mutex usb_mutex;
HANDLE hSerial = INVALID_HANDLE_VALUE; // 串口句柄

// 获取已连接的串口列表
std::vector<std::string> GetAvailablePorts() {
    std::vector<std::string> ports;

    // 枚举 COM1 到 COM15
    for (int i = 1; i <= 255; i++) {
        std::string port_name = "\\\\.\\COM" + std::to_string(i);
        HANDLE hPort = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hPort != INVALID_HANDLE_VALUE) {
            ports.push_back("COM" + std::to_string(i));
            CloseHandle(hPort);
        }
    }

    return ports;
}

// 通过串口发送和接收数据的函数
// 通过串口发送和接收数据的函数
bool SendAndReceiveData(const std::string& port) {
    // 打开串口
    std::string port_name = "\\\\.\\" + port;
    hSerial = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open port: " << port << std::endl;
        return false;
    }

    // 配置串口参数
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Failed to get serial port state." << std::endl;
        CloseHandle(hSerial);
        return false;
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Failed to set serial port state." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // 设置超时
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Failed to set serial port timeouts." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // 发送二进制数据
    uint8_t send_data = 0b10000000; // 二进制数据 10000000
    DWORD bytes_written;
    if (!WriteFile(hSerial, &send_data, sizeof(send_data), &bytes_written, nullptr)) {
        std::cerr << "Failed to write to serial port." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // 接收二进制数据
    uint8_t receive_data = 0;
    DWORD bytes_read;
    if (!ReadFile(hSerial, &receive_data, sizeof(receive_data), &bytes_read, nullptr)) {
        std::cerr << "Failed to read from serial port." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // 关闭串口
    CloseHandle(hSerial);

    // 检查接收到的数据
    if (receive_data == 0b00000001) { // 二进制数据 00000001
        return true;
    }
    else {
        return false;
    }
}

// 串口设备扫描线程
void USBScanThread() {
    while (is_scanning) {
        auto detected_ports = GetAvailablePorts(); // 检测串口

        // 使用互斥锁保护共享数据
        {
            std::lock_guard<std::mutex> lock(usb_mutex);
            usb_ports = detected_ports;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 每 0.5s 检测一次
    }
}


UART_DEC::UART_DEC()
{
}

UART_DEC::~UART_DEC()
{
}


void UART_DEC::Render()
{
    static std::thread usb_scan_thread; // USB 扫描线程
    static bool is_thread_started = false; // 标记线程是否已启动

    // 启动 USB 扫描线程（仅一次）
    if (!is_thread_started) {
        usb_scan_thread = std::thread(USBScanThread);
        is_thread_started = true;
    }

    // 设置窗口位置和大小
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);

    // 开始窗口
    ImGui::Begin("USB Connection", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // 下拉框：选择 USB 端口
    {
        std::lock_guard<std::mutex> lock(usb_mutex); // 保护共享数据
        if (ImGui::BeginCombo("USB Ports", selected_usb_index >= 0 ? usb_ports[selected_usb_index].c_str() : "Select a port")) {
            for (int i = 0; i < usb_ports.size(); i++) {
                bool is_selected = (selected_usb_index == i);
                if (ImGui::Selectable(usb_ports[i].c_str(), is_selected)) {
                    selected_usb_index = i; // 更新选中的 USB 端口索引
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus(); // 设置默认焦点
                }
            }
            ImGui::EndCombo();
        }
    }

    // Connect 按钮
    if (ImGui::Button("Connect")) {
        if (selected_usb_index >= 0 && selected_usb_index < usb_ports.size()) {
            std::string selected_port = usb_ports[selected_usb_index];
            bool success = SendAndReceiveData(selected_port); // 发送并接收数据
            if (success) {
                ConnectFlag = true; // 连接成功
                ImGui::OpenPopup("Connection Status");
            }
            else {
                ConnectFlag = false; // 连接失败
                ImGui::OpenPopup("Connection Status");
            }
        }
    }

    // Disconnect 按钮
    ImGui::SameLine();
    if (ImGui::Button("Disconnect")) {
        ConnectFlag = false; // 断开连接
        ImGui::OpenPopup("Connection Status");
    }

    // 连接状态弹窗
    if (ImGui::BeginPopupModal("Connection Status", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ConnectFlag) {
            ImGui::Text("Connection Successful!");
        }
        else {
            ImGui::Text("Connection Failed!");
        }
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // 结束窗口
    ImGui::End();
}

