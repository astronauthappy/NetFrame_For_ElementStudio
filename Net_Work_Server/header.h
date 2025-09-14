#pragma once
#include<WS2tcpip.h>
#include<winsock2.h>
#include<list>
#include<map>
#include<string>
#include<fstream>
#include<vector>

#pragma comment (lib,"Ws2_32")

constexpr unsigned BLOCK_SIZE{ 1024 };

enum cmd_header_cat
{
	XY,                  // 当前为普通帧
	curk_fail,           // 关键帧未生成
	k_info,              // 关键帧已生成 
	CLR,                 // 清空通道
	k_ok,                // 关键帧收取完成
	send_info,           // 正在发送关键
	curk_info,           // 索取关键帧
};

typedef class Cmd_header
{
public:
	cmd_header_cat       Cmd_Cat;
	int                  file_id = -1;
	std::wstring         x;
	std::wstring         y;
	long long            Guid=0;
} CmdHeader;


enum data_header_cat
{
	F_send,
	ASK,
};

typedef class Data_header
{
public:
	int                  file_id = -1;
	data_header_cat      Data_Cat;
	long long            file_size;
} DataHeader;
