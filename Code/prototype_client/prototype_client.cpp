﻿// prototype_client.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "prototype_client_app.h"

int main()
{
    using namespace XP;

    std::locale::global(std::locale("korean"));
    std::wcout.imbue(std::locale("korean"));

    AppConfig appConfig(TEXT("PrototypeClientApp"), TEXT("prototype_client_log4cplus.property"));
    appConfig.SetAddress(TEXT("127.0.0.1"));
    appConfig.SetPort(1280);

    if (g_PrototypeClientApp.Initialize(std::move(appConfig)))
    {
        g_PrototypeClientApp.AsyncRun();

        std::array<wchar_t, 512> inputMessage = { 0 };
        while (std::wcin.getline(inputMessage.data(), inputMessage.size()))
        {
            std::wstring message(inputMessage.data());
            if (message == TEXT("quit"))
            {
                LOG_INFO(LOG_FILTER_CLIENT, "quit client.");
                break;
            }

            g_PrototypeClientApp.SendMessage(message);
            inputMessage.fill(L'\0');
        }
    }

    return 0;
}
