#pragma once
//--------------------------------------------------------------------
//	S_Client.h
//	04/27/2025.				created by dyx.
//	04/27/2025.				last modified.
//--------------------------------------------------------------------


#include"header.h"

	SOCKET Init_CmdSocket();
	SOCKET Init_DataSocket();
	CmdHeader& Cmd_XY(std::wstring m,std::wstring n);
	bool   CmdSend(CmdHeader& cmd_header, SOCKET socket);
	bool   CmdRecv(CmdHeader& cmd_header, SOCKET socket);
	bool   DataSend(DataHeader& data_header, SOCKET socket);
	bool   DataRecv(DataHeader& data_header, SOCKET socket);
	void   Send_k_info();
	bool   Send_File(long long size, SOCKET socket);
	bool   Send_block(SOCKET accepted_socket, const char* block, long long length);
	bool   Read_block(SOCKET socket,  char* block, long long len);

	static CmdHeader                          cmd_header;
	static DataHeader                         data_header;
	static SOCKET                             Cmd_Socket;
	static SOCKET                             Data_Socket;
	static std::list<SOCKET>                  cmd_list;
	static std::list<SOCKET>                  data_list;
	static bool                               mouse_flag=true;
	static bool                               curk_key{false}; //作为关键帧的标志
	static std::vector<CmdHeader>            key_list;
	
