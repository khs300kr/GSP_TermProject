#include "define.h"
#include "Global.h"
#include "DBFunc.h"
#include "PacketFunc.h"

#define POS_LEN 300
#define LEVEL_LEN 10
#define EXP_LEN 51200

WCHAR sql_buf[255];
wstring sql_query;

SQLLEN cbID{}, cbX{}, cbY{}, cbLevel{}, cbExp{}, cbHP{}, cbATT{}, cbGold{};
SQLWCHAR szID[MAX_ID_LEN]{};
int iXPos{}, iYPos{}, iLevel{}, iExp{}, iHP{}, iATT{}, iGold{};

void Init_DB(void)
{
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
			}
		}
	}
}

void Client_Login(WCHAR id[], int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2012182008", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
		wstring user_id{ id };
		sql_query = L"EXEC dbo.client_login " + user_id;
	
		retcode = SQLExecDirect(hstmt, (wchar_t*)sql_query.c_str(), SQL_NTS);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			// Bind columns 1, 2, and 3  
			retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szID, MAX_ID_LEN, &cbID);
			retcode = SQLBindCol(hstmt, 2, SQL_INTEGER, &iXPos, POS_LEN, &cbX);
			retcode = SQLBindCol(hstmt, 3, SQL_INTEGER, &iYPos, POS_LEN, &cbY);
			retcode = SQLBindCol(hstmt, 4, SQL_INTEGER, &iLevel, POS_LEN, &cbLevel);
			retcode = SQLBindCol(hstmt, 5, SQL_INTEGER, &iExp, EXP_LEN, &cbExp);
			retcode = SQLBindCol(hstmt, 6, SQL_INTEGER, &iHP, EXP_LEN, &cbHP);
			retcode = SQLBindCol(hstmt, 7, SQL_INTEGER, &iATT, EXP_LEN, &cbATT);
			retcode = SQLBindCol(hstmt, 8, SQL_INTEGER, &iGold, EXP_LEN, &cbGold);
			// Fetch and print each row of data. On an error, display a message and exit.  

			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				wcout << "connect ID : " << szID << endl;
				cout << iXPos << endl;
				cout << iYPos << endl;
				cout << iLevel << endl;
				cout << iExp << endl;
				cout << iHP << endl;
				cout << iATT << endl;
				cout << iGold << endl;
				// Send Login Success
				// 초기위치
				g_Clients[ci].m_iX = iXPos;
				g_Clients[ci].m_iY = iYPos;
				g_Clients[ci].m_Level = iLevel;
				g_Clients[ci].m_Exp = iExp;
				g_Clients[ci].m_HP = iHP;
				g_Clients[ci].m_ATT = iATT;
				g_Clients[ci].m_Gold = iGold;

				// 위치 하기.
				SendPutPlayerPacket(ci, ci);
				SendCharDBinfo(ci, ci);

				std::unordered_set<int> local_my_view_list;

				for (int i = 0; i < MAX_USER; ++i)
				{
					if (g_Clients[i].m_bConnect == true)
					{
						if (i != ci)
						{
							if (Is_Close(i, ci) == true) {
								SendPutPlayerPacket(ci, i);
								local_my_view_list.insert(i);
								SendPutPlayerPacket(i, ci);
								// 나한테 넣는다. (시야리스트)
								g_Clients[ci].view_list.insert(i);

								// 나한테만이 아니라 상대방한테도 넣어줘야한다.
								g_Clients[i].vl_lock.lock();						//lock
								g_Clients[i].view_list.insert(ci);
								g_Clients[i].vl_lock.unlock();						//unlock
																					// 이렇게하면 처음에 근처에있는 친구들만 보인다.
							}
						}
					}
					g_Clients[ci].vl_lock.lock();					// lock
					for (auto p : local_my_view_list) g_Clients[ci].view_list.insert(p);
					g_Clients[ci].vl_lock.unlock();					//unlock
				}// for loop
			}
			else
			{
				SendLoginFail(ci, ci);
			}
		}

		// Process data  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			SQLCancel(hstmt);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		}
		SQLDisconnect(hdbc);
	}
}

void Client_Logout(WCHAR id[],WORD x, WORD y, BYTE Level, WORD Exp, WORD HP, WORD ATT, int gold, int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2012182008", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
		
		wstring user_id{ id };
		sql_query = L"EXEC dbo.client_logout " + user_id + L","
			+ to_wstring(x) + L"," + to_wstring(y) + L"," + to_wstring(Level) + L","
			+ to_wstring(Exp) + L"," + to_wstring(HP) + L"," + to_wstring(ATT) + L","
			+ to_wstring(gold);

		retcode = SQLExecDirect(hstmt, (wchar_t*)sql_query.c_str(), SQL_NTS);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				cout << "Logout\n";
			}
		}

		// Process data  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			SQLCancel(hstmt);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		}
		SQLDisconnect(hdbc);
	}
}

void Close_DB(void)
{
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}