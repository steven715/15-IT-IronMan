#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <fstream>
#include <execinfo.h>
#include <string.h>
#include <time.h>

const int MAX_STACK_FRAMES = 128;

void GetTimeString(char *psz)
{
    if (NULL == psz)
    {
        return;
    }

    time_t t = time(NULL);
    tm *now = localtime(&t);
    sprintf(psz, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
}

void sig_crash(int sig)
{
    FILE *fd = fopen("crashlog.log", "a");    

    if (NULL == fd)
    {
        exit(0);
    }
    try
    {
        char szLine[512] = {
            0,
        };
        time_t t = time(NULL);
        tm *now = localtime(&t);
        int nLen1 = sprintf(szLine,
                            "#########################################################\n[%04d-%02d-%02d %02d:%02d:%02d][crash signal number:%d]\n",
                            now->tm_year + 1900,
                            now->tm_mon + 1,
                            now->tm_mday,
                            now->tm_hour,
                            now->tm_min,
                            now->tm_sec,
                            sig);
        fwrite(szLine, strlen(szLine), 1, fd);

        char szTmp[4096];
        char szTime[256];
        GetTimeString(szTime);
#ifdef __linux
        void* pStacks[MAX_STACK_FRAMES];
		size_t nStackDepth;      
		char **SymbolAry;

		nStackDepth = (int)backtrace(pStacks, MAX_STACK_FRAMES);

		SymbolAry = backtrace_symbols(pStacks, nStackDepth);
		if (SymbolAry == NULL)
		{
			snprintf(szTmp, sizeof(szTmp), "no backtrace_symbols\n", szTime);
			fwrite(szTmp, strlen(szTmp), 1, fd);
		}
		else
		{
			for (int j = 0; j < nStackDepth; j++)
			{
				if (SymbolAry[j])
				{
					snprintf(szTmp, sizeof(szTmp), "%s\n", SymbolAry[j]);
					fwrite(szTmp, strlen(szTmp), 1, fd);
				}
			}

			free(SymbolAry);
	    }
#endif // __linux
    }
    catch (...)
    {
        //
    }
    fflush(fd);
    fclose(fd);
    fd = NULL;
    exit(0);
}

int main()
{
    // 紀錄要偵測的錯誤信號及回調函式
    signal(SIGSEGV, sig_crash);
    signal(SIGABRT, sig_crash);

    int *a = NULL;
    a[10] = 0; // crash
    
    return 0;
}