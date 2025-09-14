//--------------------------------------------------------------------
//	S_Client.cpp.
//	04/27/2025.				created by dyx.
//	09/14/2025.				last modified.
//--------------------------------------------------------------------

#include"fundation.h"
#pragma comment (lib,"Ws2_32")

// CmdSocket  &&  DataSOcket
	//step 0 
WSADATA Wsa_data;
auto wVersionRequested = MAKEWORD(2, 2);
int err = WSAStartup(wVersionRequested, &Wsa_data);

//--------------------------------------------------------------------
SOCKET Init_CmdSocket()
{
	//step 1 :create a socket
	Cmd_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Cmd_Socket == INVALID_SOCKET)
		int i = WSAGetLastError();

	//step 2: bind to a local address and port
	//     2.1 create a ipv4 struct
	sockaddr_in server;
	server.sin_family = AF_INET;
	int net_buf;
	if (::InetPtonW(AF_INET, /*L"10.12.73.234"*/ L"127.0.0.1", &net_buf) != 1)
		int i = WSAGetLastError();

	server.sin_addr.S_un.S_addr = net_buf;
	server.sin_port = htons(9008);
	//    2.2 bind ipv4 information  to  socket
	if (::bind(Cmd_Socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == SOCKET_ERROR)
	{
		::WSACleanup();
		int i = WSAGetLastError();
	}

	if (::listen(Cmd_Socket, SOMAXCONN) == SOCKET_ERROR)//将listen_socket置于监听状态
	{
		::WSACleanup();
		int i = WSAGetLastError();
	}
	return Cmd_Socket;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
SOCKET Init_DataSocket()
{
	//step 1 :create a socket
	Data_Socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (Data_Socket == INVALID_SOCKET)
		int i = WSAGetLastError();

	//step 2: bind to a local address and port
	//     2.1 create a ipv4 struct
	sockaddr_in server;
	server.sin_family = AF_INET;
	int net_buf;
	if (::InetPtonW(AF_INET, L"127.0.0.1"/* L"172.20.10.2"*/, &net_buf) != 1)
		int i = WSAGetLastError();

	server.sin_addr.S_un.S_addr = net_buf;
	server.sin_port = htons(9009);
	//    2.2 bind ipv4 information  to  socket
	if (::bind(Data_Socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == SOCKET_ERROR)
	{
		::WSACleanup();
		int i = WSAGetLastError();
	}
	if (::listen(Data_Socket, SOMAXCONN) == SOCKET_ERROR)//将listen_socket置于监听状态
	{
		::WSACleanup();
		int i = WSAGetLastError();
	}
	return Data_Socket;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
CmdHeader& Cmd_XY(std::wstring m, std::wstring n)
{
	cmd_header.x = m;
	cmd_header.y = n;
	return cmd_header;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool CmdSend(CmdHeader & cmd_header, SOCKET socket)
{
	long long remainder = sizeof(cmd_header);
	long long offset = 0;
	while (remainder > 0)
	{
		auto send_byte = ::send(socket,
			reinterpret_cast<const char*>(&cmd_header) + offset,
			remainder,
			0);
		if (send_byte < 0)
			return false;
		else
		{
			remainder -= send_byte;
			offset += send_byte;
		}
	}
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool CmdRecv(CmdHeader& cmd_header, SOCKET socket)
{
	long long remainder = sizeof(cmd_header);
	long long offset = 0;
	while (remainder > 0)
	{
		auto receive_header = ::recv(socket,
			reinterpret_cast<char*>(&cmd_header) + offset,
			remainder,
			0);
		if (receive_header <= 0)
			return false;
		else
		{
			remainder -= receive_header;
			offset += receive_header;
		}
	}
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------

bool DataSend(DataHeader& data_header, SOCKET socket)
{
	long long remainder = sizeof(data_header);
	long long offset = 0;
	while (remainder > 0)
	{
		auto send_byte = ::send(socket,
			reinterpret_cast<const char*>(&data_header) + offset,
			remainder,
			0);
		if (send_byte < 0)
			return false;
		else
		{
			remainder -= send_byte;
			offset += send_byte;
		}
	}
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------

bool DataRecv(DataHeader& data_header, SOCKET socket)
{
	long long remainder = sizeof(data_header);
	long long offset = 0;
	while (remainder > 0)
	{
		auto receive_header = ::recv(socket,
			reinterpret_cast<char*>(&data_header) + offset,
			remainder,
			0);
		if (receive_header <= 0)
			return false;
		else
		{
			remainder -= receive_header;
			offset += receive_header;
		}
	}
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------

void Send_k_info()
{
	static int num = 0;
	cmd_header.Cmd_Cat = cmd_header_cat::k_info;
	cmd_header.file_id = num;
	key_list.emplace_back(cmd_header);
	if (!cmd_list.empty())
	{
		for (auto i : cmd_list)
		{
			CmdSend(cmd_header, i);
		}
	}

}
//--------------------------------------------------------------------

//--------------------------------------------------------------------

bool Send_File(long long size, SOCKET socket)
{
	/*char* buf = new char[BLOCK_SIZE];*/
	std::vector<char> buffer(BLOCK_SIZE);
	static long long offset=0;
	long long remainder = size;
	std::fstream fs(L"Data_file.txt", std::fstream::in | std::fstream::binary);
	while (remainder > 0)
	{
		fs.seekg(offset, std::fstream::beg);
		fs.read(buffer.data(), remainder>BLOCK_SIZE ? BLOCK_SIZE : remainder);
		auto sizeblock= fs.gcount();
		if (sizeblock <0)
			break;
		Send_block(socket, buffer.data(), sizeblock);
		offset += sizeblock;
		remainder-= sizeblock;
	}
	

	
	fs.close();
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool Send_block(SOCKET socket, const char* block, long long length)
{
	long long remainder = length;
	long long offset = 0;
	while (remainder > 0)
	{
		auto send_byte = ::send(socket,
			block + offset,
			remainder,
			0);
		if (send_byte < 0)
			return false;
		else
		{
			remainder -= send_byte;
			offset += send_byte;
		}
	}
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool Read_block(SOCKET socket,  char* block, long long len)
{
	/*long long reminder = len;
	long long offset = 0;

	while (reminder > 0)
	{
		auto send_byte = ::recv(socket,
			block + offset,
			reminder,
			0);
		if (send_byte <= 0)
			return false;
		else
		{
			reminder -= send_byte;
			offset += send_byte;
		}
	}*/
	::recv(socket,
		block,
		len,
		0);
	return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------

