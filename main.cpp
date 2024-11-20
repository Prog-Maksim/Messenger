#include "server.h"
#include "client.h"
#include "utils.h"

int main() {
    setlocale(LC_ALL, "");
    asio::io_context io_context;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int port = load_port();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, 6);
    std::wcout << L"Прослушивание TCP сообщений на порту: " << port << "    *" << std::endl;
    std::wcout << L"Чтобы узнать подробнее напишите /info         *" << std::endl;
    std::wcout << L"Если нужна помощь с командами напишите /help  *" << std::endl;
    std::wcout << L"***********************************************" << std::endl;
    SetConsoleTextAttribute(hConsole, 15);

    std::thread server_thread([&io_context, &port]() {
        start_server(io_context, port);
    });

    start_client(io_context, port);

    server_thread.detach();
    std::wcout << L"Перезапуск с новым портом" << std::endl;
    main();
    return 0;
}
