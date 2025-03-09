#include <iostream>
#include "Application.h"


int main()
{

    Application app;
    app.Run();

    //// 停止 USB 扫描线程
    //is_scanning = false;
    //if (usb_scan_thread.joinable()) {
    //    usb_scan_thread.join();
    //}
    return 0;
}
