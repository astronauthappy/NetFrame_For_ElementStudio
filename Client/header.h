#pragma once
#include<string>

constexpr unsigned BLOCK_SIZE{ 512 * 1024 };

enum cmd_header_cat
{
	XY,                  // 当前为普通帧
	curk_info,           // 索取关键帧
	curk_fail,           // 关键帧未生成
	k_info,              // 关键帧已生成
	k_ok,                // 关键帧收取完成
	send_info,           // 正在发送关键帧
	CLR,                 // 清空通道
};
typedef class Cmd_header
{
public:
	cmd_header_cat       Cmd_Cat;
	int                  file_id;
	std::wstring         x;
	std::wstring         y;
} CmdHeader;


enum data_header_cat
{
	F_send,
	ASK,
};

typedef class Data_header
{
public:
	int                  file_id;
	data_header_cat      Data_Cat;
	long long            file_size;
} DataHeader;
