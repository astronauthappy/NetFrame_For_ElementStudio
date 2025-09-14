//--------------------------------------------------------------------
//	Window.cpp.
//	04/25/2025.				created by dyx.
//	04/25/2025.				last modified.
//--------------------------------------------------------------------


#include"fundation.h"
#include<Windows.h>
#include<windowsx.h>
#include<iostream>
#include<format>
#include<list>
#include<thread>




#pragma comment (lib,"Ws2_32")
#define MY_WACMD  WM_USER +0x1
#define MY_WADATA WM_USER +0x2

// declarations
LRESULT WndProcdure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// function defines
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, wchar_t* lpCmdLine, int iCmdShow)
{
	wchar_t szAppName[] = L"WinApp";
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProcdure;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = wcex.hIcon;
	if (!RegisterClassEx(&wcex))
	{
		return -1;
	}
	HWND hWnd = NULL;
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		szAppName,
		L"App",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);


	Cmd_Socket = Init_CmdSocket();
	Data_Socket = Init_DataSocket();


	int  event = FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE | FD_CLOSE;
	WSAAsyncSelect(Cmd_Socket, hWnd, MY_WACMD, event);
	WSAAsyncSelect(Data_Socket, hWnd, MY_WADATA, event);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT WndProcdure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static  RECT  rect = { 100,100,rect.left + 100, rect.top + 100 };
	HDC hDC = NULL;
	PAINTSTRUCT ps{ 0 };
	HBITMAP hbitmap = NULL;
	static bool is_drop; //鼠标左键是否落下
	static bool is_dragging;
	static POINT point;
	static int mouse_x, mouse_y;

	switch (message)
	{
	case WM_CREATE:
	{
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (is_drop)
		{
			HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 225));
			FillRect(hdc, &rect, hBrush);
			DeleteObject(hBrush);
		}
		else
		{
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		}


		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{

		point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (PtInRect(&rect, point))//判断指针是否在rectangle内
		{
			is_drop = true;
			
			InvalidateRect(hWnd, nullptr, true);
		}

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (is_drop)
		{
			POINT now_pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			mouse_x = now_pt.x;
			mouse_y = now_pt.y;

			int x = mouse_x - point.x;
			int y = mouse_y - point.y;

			if (x != 0 || y != 0)
			{
				is_dragging = true;
			}
			point = now_pt;
			rect.left = rect.left + x;
			rect.top = rect.top + y;
			rect.right = rect.left + 100;
			rect.bottom = rect.top + 100;

			if (!cmd_list.empty() && is_dragging == true && mouse_flag==true)
			{
				for (auto i : cmd_list)
				{
					cmd_header.Cmd_Cat = cmd_header_cat::XY;
					cmd_header = Cmd_XY(std::to_wstring(rect.left), std::to_wstring(rect.top)); //to_wstring  int -> wstring
					CmdSend(cmd_header, i);
				}
			}
			InvalidateRect(hWnd, nullptr, true);
		}

		is_dragging = false;


		return 0;
	}
	case MY_WACMD:
	{
		auto event = WSAGETSELECTEVENT(lParam);
		switch (event)
		{
		case FD_ACCEPT:
		{
			sockaddr_in client_addr;
			int addrlen{ sizeof(client_addr) };
			auto accepted_socket = ::accept(Cmd_Socket, reinterpret_cast<sockaddr*> (&client_addr), &addrlen);
			if (accepted_socket == INVALID_SOCKET)
			{
				::WSACleanup();
				int i = WSAGetLastError();
			}

			int  event = FD_CLOSE | FD_READ | FD_WRITE | FD_CLOSE;
			WSAAsyncSelect(accepted_socket, hWnd, MY_WACMD, event);

			cmd_list.push_back(accepted_socket);
			/*cmd_header.Cmd_Cat = cmd_header_cat::XY;
			CmdSend(cmd_header, (SOCKET)wParam);*/
			return 0;
		}
		case FD_READ:
		{
			CmdRecv(cmd_header, (SOCKET)wParam);
			switch (cmd_header.Cmd_Cat)
			{
			case cmd_header_cat::curk_info:
			{
				if (curk_key == false)
				{
					cmd_header.Cmd_Cat = cmd_header_cat::curk_fail;
					CmdSend(cmd_header, (SOCKET)wParam);
				}
				else
				{
					CmdHeader cmdheader;
					cmdheader = key_list.back();
					CmdSend(cmd_header, (SOCKET)wParam);

				}
				return 0;
			}
			case cmd_header_cat::k_ok:
			{
				if (curk_key == false)
					PostMessage(hWnd,WM_MOUSEMOVE,  wParam, lParam);
				return 0;
			}
			}
		}
		}
	}
	case MY_WADATA:
	{
		auto event = WSAGETSELECTEVENT(lParam);
		switch (event)
		{
		case FD_ACCEPT:
		{
			sockaddr_in client_addr;
			int addrlen{ sizeof(client_addr) };
			auto accepted_socket = ::accept(Data_Socket, reinterpret_cast<sockaddr*> (&client_addr), &addrlen);
			if (accepted_socket == INVALID_SOCKET)
			{
				::WSACleanup();
				int i = WSAGetLastError();
			}

			int  event = FD_CLOSE | FD_READ | FD_WRITE | FD_CLOSE;
			WSAAsyncSelect(accepted_socket, hWnd, MY_WADATA, event);

			data_list.push_back(accepted_socket);
			return 0;
		}
		case FD_READ:
		{

			DataRecv(data_header, (SOCKET)wParam);
			std::thread j([&](void)->void{
				mouse_flag = false;
				std::fstream data_file(L"Data_file.txt", std::fstream::in | std::fstream::binary);
				if (data_file)
				{

					DataHeader  dataheader;
					dataheader.Data_Cat = data_header_cat::F_send;
					dataheader.file_id = data_header.file_id;
					data_file.seekg(0, data_file.end);
					dataheader.file_size = data_file.tellg();
					DataSend(dataheader, (SOCKET)wParam);
					Send_File(dataheader.file_size, (SOCKET)wParam);
				}
				data_file.close();
				mouse_flag = true;
			});
			j.detach();
			/*switch (data_header.Data_Cat)
			{
			case data_header_cat::ASK:
			{
				
					mouse_flag = false;
					std::fstream data_file(L"Data_file.txt", std::fstream::in | std::fstream::binary);
					if (data_file)
					{

						DataHeader  dataheader;
						dataheader.Data_Cat = data_header_cat::F_send;
						dataheader.file_id = data_header.file_id;
						data_file.seekg(0, data_file.end);
						dataheader.file_size = data_file.tellg();
						DataSend(dataheader, (SOCKET)wParam);
						Send_File(dataheader.file_size, (SOCKET)wParam);
					}
					data_file.close();
					mouse_flag = true;
			
				break;
			}

			}
			break;*/
			break;
		}
		}
	}
	case WM_LBUTTONUP: {
		if (is_drop) {
			is_drop = false;
			
		}
		InvalidateRect(hWnd, nullptr, true);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		curk_key = true;
		if(!data_list.empty())
		{
			std::fstream data_file("Data_file.txt", std::fstream::in | std::fstream::binary);
			if (!data_file)
				return 0;
			cmd_header = Cmd_XY(std::to_wstring(rect.left), std::to_wstring(rect.top));
			Send_k_info();
			data_file.close();
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}






