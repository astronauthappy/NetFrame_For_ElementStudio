#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<format>
#include<string>


#include"../Net_Work_Server/fundation.h"

#pragma  comment(lib,"Ws2_32")


int main()
{

	//step 0 
	WSADATA Wsa_data;
	int err = 0;

	if (::WSAStartup(MAKEWORD(2, 2), &Wsa_data) != 0)
	{
		int i = ::WSAGetLastError();
		return 1;
	}


	SOCKET cmd_socket = ::socket(AF_INET, SOCK_STREAM, 0);         //cmd socket
	if (cmd_socket == INVALID_SOCKET)
		return 1;

	SOCKET data_socket = ::socket(AF_INET, SOCK_STREAM, 0);         //data socket
	if (data_socket == INVALID_SOCKET)
		return 1;

	sockaddr_in cmd_server;
	cmd_server.sin_family = AF_INET;
	int net_buf1;
	if (::InetPtonW(AF_INET, L"127.0.0.1", &net_buf1) != 1)          //cmd socket
		return -1;
	cmd_server.sin_addr.S_un.S_addr = net_buf1;
	cmd_server.sin_port = htons(9008);

	if (::connect(cmd_socket, reinterpret_cast<sockaddr*> (&cmd_server), sizeof(cmd_server)) == SOCKET_ERROR)
		return 1;


	sockaddr_in data_server;
	data_server.sin_family = AF_INET;
	int net_buf2;
	if (::InetPtonW(AF_INET, L"127.0.0.1", &net_buf2) != 1)          //data socket
		return -1;
	data_server.sin_addr.S_un.S_addr = net_buf2;
	data_server.sin_port = htons(9009);

	if (::connect(data_socket, reinterpret_cast<sockaddr*> (&data_server), sizeof(data_server)) == SOCKET_ERROR)
		return 1;


	DataHeader data_header;

	CmdHeader cmd_header;
	cmd_header.Cmd_Cat = cmd_header_cat::curk_info;
	CmdSend(cmd_header, cmd_socket);
	int File_ID = -1;
	std::vector<char> buffer(BLOCK_SIZE);

	
	while (1)
	{

_cmd:
		if (!CmdRecv(cmd_header, cmd_socket))
		{
			break;
		}
		switch (cmd_header.Cmd_Cat)
		{
		case cmd_header_cat::curk_fail:
		{
			/*cmd_header.Cmd_Cat = cmd_header_cat::XY;
			CmdSend(cmd_header, cmd_socket);          *///默认索取普通帧
			break;
		}
		case cmd_header_cat::k_info:
		{
			//传输关键帧
			data_header.Data_Cat = data_header_cat::ASK;
			data_header.file_id = cmd_header.file_id;
			File_ID = cmd_header.file_id;
			data_header.file_size = 0;
			DataSend(data_header, data_socket);

goto _data;
			break;
		}
		case cmd_header_cat::XY:
		{
			/*if (cmd_header.file_id == -1)
			{
				break;
			}*/
			std::wcout << L"X: " << cmd_header.x << L" Y: " << cmd_header.y << std::endl;
			break;
		}
		}

	}

_data:
	if (DataRecv(data_header, data_socket))
	{
		switch (data_header.Data_Cat)
		{
		case data_header_cat::F_send:
		{
			if (data_header.file_id == File_ID)
			{
				std::fstream fs(L"./aa.txt", std::fstream::binary | std::fstream::trunc | std::fstream::out);
				long long size = data_header.file_size;
				int cnt = size / BLOCK_SIZE;
				long long last_block = size - cnt * BLOCK_SIZE;
				while (cnt > 0)
				{
					if (Read_block(data_socket, buffer.data(), BLOCK_SIZE))
					{
						fs.write(buffer.data(), BLOCK_SIZE);
						--cnt;
					}
					else
					{
						fs.close();
						return 0;
					}
				}
				if (last_block > 0)
				{
					if (Read_block(data_socket, buffer.data(), last_block))
					{
						fs.write(buffer.data(), last_block);
						
					}
					else
					{
						fs.close();
						return 0;
					}
				}
				fs.close();
				std::wcout << L"file_id: " << cmd_header.file_id << std::endl;
				std::wcout << L"XY: " << cmd_header.x << L"," << cmd_header.y << std::endl;
				cmd_header.Cmd_Cat = cmd_header_cat::k_ok;
				CmdSend(cmd_header, cmd_socket);
				break;
			}
		}
		}
goto _cmd;
	}

	closesocket(cmd_socket);
	cmd_socket = INVALID_SOCKET;

	::WSACleanup();


	return 0;
}