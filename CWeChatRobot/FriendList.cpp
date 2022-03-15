#include "pch.h"

struct WxFriendAddrStruct {
	DWORD wxIdAddr;
	DWORD wxNumberAddr;
	DWORD wxNickNameAddr;
	DWORD wxRemarkAddr;
};

struct WxFriendStruct {
	wchar_t* wxId;
	wchar_t* wxNumber;
	wchar_t* wxNickName;
	wchar_t* wxRemark;
};

WxFriendStruct* WxFriendList;

void ReadFriendMessageByAddress(WxFriendAddrStruct* lpWxFriendAddr, WxFriendStruct* lpWxFriend) {
	DWORD length = 0;
	DWORD bufferaddr = 0;

	ReadProcessMemory(hProcess, (LPCVOID)(lpWxFriendAddr->wxIdAddr + 0x4), &length, sizeof(DWORD), 0);
	if (length) {
		lpWxFriend->wxId = new wchar_t[length + 1];//(wchar_t*)malloc(sizeof(wchar_t) * (length + 1));
		if (lpWxFriend->wxId)
		{
			ZeroMemory(lpWxFriend->wxId, sizeof(wchar_t) * (length + 1));
			ReadProcessMemory(hProcess, (LPCVOID)lpWxFriendAddr->wxIdAddr, &bufferaddr, sizeof(DWORD), 0);
			ReadProcessMemory(hProcess, (LPCVOID)bufferaddr, lpWxFriend->wxId, length * sizeof(wchar_t), 0);
		}
	}
	else {
		lpWxFriend->wxId = L"null";
	}

	ReadProcessMemory(hProcess, (LPCVOID)(lpWxFriendAddr->wxNumberAddr + 0x4), &length, sizeof(DWORD), 0);
	if (length) {
		lpWxFriend->wxNumber = new wchar_t[length + 1];//(wchar_t*)malloc(sizeof(wchar_t) * (length + 1));
		if (lpWxFriend->wxNumber)
		{
			ZeroMemory(lpWxFriend->wxNumber, sizeof(wchar_t) * (length + 1));
			ReadProcessMemory(hProcess, (LPCVOID)lpWxFriendAddr->wxNumberAddr, &bufferaddr, sizeof(DWORD), 0);
			ReadProcessMemory(hProcess, (LPCVOID)bufferaddr, lpWxFriend->wxNumber, length * sizeof(wchar_t), 0);
		}
	}
	else {
		lpWxFriend->wxNumber = L"null";
	}

	ReadProcessMemory(hProcess, (LPCVOID)(lpWxFriendAddr->wxNickNameAddr + 0x4), &length, sizeof(DWORD), 0);
	if (length) {
		lpWxFriend->wxNickName = new wchar_t[length + 1]; //(wchar_t*)malloc(sizeof(wchar_t) * (length + 1));
		if (lpWxFriend->wxNickName)
		{
			ZeroMemory(lpWxFriend->wxNickName, sizeof(wchar_t) * (length + 1));
			ReadProcessMemory(hProcess, (LPCVOID)lpWxFriendAddr->wxNickNameAddr, &bufferaddr, sizeof(DWORD), 0);
			ReadProcessMemory(hProcess, (LPCVOID)bufferaddr, lpWxFriend->wxNickName, length * sizeof(wchar_t), 0);
		}
	}
	else {
		lpWxFriend->wxNickName = L"null";
	}

	ReadProcessMemory(hProcess, (LPCVOID)(lpWxFriendAddr->wxRemarkAddr + 0x4), &length, sizeof(DWORD), 0);
	if (length) {
		lpWxFriend->wxRemark = new wchar_t[length + 1]; //(wchar_t*)malloc(sizeof(wchar_t) * (length + 1));
		if (lpWxFriend->wxRemark)
		{
			ZeroMemory(lpWxFriend->wxRemark, sizeof(wchar_t) * (length + 1));
			ReadProcessMemory(hProcess, (LPCVOID)lpWxFriendAddr->wxRemarkAddr, &bufferaddr, sizeof(DWORD), 0);
			ReadProcessMemory(hProcess, (LPCVOID)bufferaddr, lpWxFriend->wxRemark, length * sizeof(wchar_t), 0);
		}
	}
	else {
		lpWxFriend->wxRemark = L"null";
	}
}

void FreeWxFriend(int index) {
	delete[] WxFriendList[index].wxId;
	WxFriendList[index].wxId = NULL;
	if (StrCmpW(WxFriendList[index].wxNumber, L"null") != 0) {
		delete[] WxFriendList[index].wxNumber;
		WxFriendList[index].wxNumber = NULL;
	};
	if (StrCmpW(WxFriendList[index].wxNickName, L"null") != 0) {
		delete[] WxFriendList[index].wxNickName;
		WxFriendList[index].wxNickName = NULL;
	};
	if (StrCmpW(WxFriendList[index].wxRemark, L"null") != 0) {
		delete[] WxFriendList[index].wxRemark;
		WxFriendList[index].wxRemark = NULL;
	};
}

std::wstring GetFriendList() {
	DWORD GetFriendListInitAddr = GetWeChatRobotBase() + GetFriendListInitOffset;
	DWORD GetFriendListRemoteAddr = GetWeChatRobotBase() + GetFriendListRemoteOffset;
	DWORD GetFriendListFinishAddr = GetWeChatRobotBase() + GetFriendListFinishOffset;
	DWORD FriendCount = 0;
	DWORD dwId, dwHandle = 0;
	// ��ȡ�����б��ĳ���
	HANDLE hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetFriendListInitAddr, NULL, 0, &dwId);
	if (hThread) {
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &FriendCount);
		CloseHandle(hThread);
	}
	// ��ȡ�����һ�����ѵ�����ָ��Ľṹ���׵�ַ
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetFriendListRemoteAddr, 0, 0, &dwId);
	if (hThread) {
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &dwHandle);
		CloseHandle(hThread);
	}
	
	WxFriendAddrStruct WxFriendAddr = { 0 };
	// ���ݺ���������ʼ��ȫ�ֱ���
	WxFriendList = new WxFriendStruct[FriendCount];
	if (dwHandle) {
		for (unsigned int i = 0; i < FriendCount; i++) {
			WxFriendList[i] = { 0 };
			ZeroMemory(&WxFriendAddr, sizeof(WxFriendAddrStruct));
			ReadProcessMemory(hProcess, (LPCVOID)dwHandle, &WxFriendAddr, sizeof(WxFriendAddrStruct), 0);
			ReadFriendMessageByAddress(&WxFriendAddr, &WxFriendList[i]);
			// ������һ���������ݵĽṹ��
			dwHandle += sizeof(WxFriendAddrStruct);
		}
	}
	else {
		return L"[]";
	}
	// ���΢�Ž��̿ռ��еĻ���
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetFriendListFinishAddr, NULL, 0, &dwId);
	if (hThread) {
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	
	wstring message = L"[";
	// ����ṹ��������
	for (unsigned int i = 0; i < FriendCount; i++) {
		message = message + L"{\"wxid\":\"" + WxFriendList[i].wxId + L"\",";
		message = message + L"\"wxNumber\":\"" + WxFriendList[i].wxNumber + L"\",";
		message = message + L"\"wxNickName\":\"" + WxFriendList[i].wxNickName + L"\",";
		message = message + L"\"wxRemark\":\"" + WxFriendList[i].wxRemark + L"\"}";
		if(i != FriendCount - 1)
			message = message + L",";
		FreeWxFriend(i);
	}
	message = message + L"]";
	// �ͷ�ȫ�ֱ���
	delete[] WxFriendList;
	WxFriendList = NULL;
	return message;
}