#include "t_cutils_c.h"

#include "file.h"
#include "print.h"

#define TEST_FILE "t_print.txt"

TEST(t_c_printf)
{
	START;

	EXPECT_EQ(c_printf(NULL), 0);
	EXPECT_EQ(c_printf(""), 0);

	c_wprintf(L"");
	EXPECT_EQ(c_printf("\n"), 1);

	END;
}

TESTP(t_c_fprintf, FILE *file)
{
	START;

	EXPECT_EQ(c_fprintf(NULL, NULL), 0);

	file_reopen(TEST_FILE, "r", file);
	EXPECT_EQ(c_fprintf(file, ""), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_fprintf(file, NULL), 0);
		EXPECT_EQ(c_fprintf(file, "test"), 4);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "test");
	}

	END;
}

TEST(t_c_sprintf)
{
	START;

	char buf[16] = { 0 };

	EXPECT_EQ(c_sprintf(NULL, 1, 0, ""), 0);
	EXPECT_EQ(c_sprintf(buf, 1, 0, NULL), 0);
#if defined(C_LINUX)
	EXPECT_EQ(c_sprintf(buf, 1, 0, "abcd"), 0);
#endif
	EXPECT_EQ(c_sprintf(buf, sizeof(buf), 0, "%s", "a"), 1);

	EXPECT_STR(buf, "a");

	END;
}

TEST(t_c_wprintf)
{
	START;

	EXPECT_EQ(c_wprintf(NULL), 0);
	EXPECT_EQ(c_wprintf(L""), 0);

	c_printf("");
	EXPECT_EQ(c_wprintf(L"\n"), 1);

	END;
}

TEST(t_c_swprintf)
{
	START;

	wchar buf[16] = { 0 };

	EXPECT_EQ(c_swprintf(NULL, 1, 0, L""), 0);
	EXPECT_EQ(c_swprintf(buf, 1, 0, NULL), 0);
	EXPECT_EQ(c_swprintf(buf, sizeof(buf), 0, L"%ls", L"a"), 1);

	EXPECT_WSTR(buf, L"a");

	END;
}

TESTP(t_c_fflush, FILE *file)
{
	START;

	EXPECT_EQ(c_fflush(NULL), 1);
	EXPECT_EQ(c_fflush(file), 0);

	END;
}

TEST(t_c_setmode)
{
	START;

	EXPECT_EQ(c_setmode(NULL, 0), 0);

	int mode = c_setmodew(stdout);

	c_setmode(stdout, mode);

	END;
}

TESTP(t_c_ur, FILE *file)
{
	START;

	EXPECT_EQ(c_ur(NULL, 0, 0, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_ur(c_fprintv_cb, 0, 0, file), sizeof("└─") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "└─");
	}

	END;
}

TESTP(t_c_v, FILE *file)
{
	START;

	EXPECT_EQ(c_v(NULL, 0, 0, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_v(c_fprintv_cb, 0, 0, file), sizeof("│ ") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "│ ");
	}

	END;
}

TESTP(t_c_vr, FILE *file)
{
	START;

	EXPECT_EQ(c_vr(NULL, 0, 0, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_vr(c_fprintv_cb, 0, 0, file), sizeof("├─") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "├─");
	}

	END;
}

TEST(t_c_sprintv_cb)
{
	START;

	char buf[16] = { 0 };

	va_list empty = { 0 };
	EXPECT_EQ(c_sprintv_cb(NULL, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_sprintv_cb(buf, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_sprintv_cb(buf, 1, 0, NULL, NULL), 0);
#if defined(C_LINUX)
	EXPECT_EQ(c_sprintv_cb(buf, 1, 0, "Test", empty), 0);
	EXPECT_STR(buf, "");
	EXPECT_EQ(c_sprintv_cb(buf, 4, 2, "Test", empty), 0);
	EXPECT_STR(buf, "");
#endif
	EXPECT_EQ(c_sprintv_cb(buf, 5, 0, "Test", empty), 4);
	EXPECT_STR(buf, "Test");
	EXPECT_EQ(c_sprintv_cb(buf, 10, 4, "Test", empty), 4);
	EXPECT_STR(buf, "TestTest");

	END;
}

TESTP(t_c_fprintv_cb, FILE *file)
{
	START;

	EXPECT_EQ(c_fprintv_cb(NULL, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_fprintv_cb(file, 0, 0, NULL, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		va_list empty = { 0 };
		EXPECT_EQ(c_fprintv_cb(file, 0, 0, "Test", empty), 4);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "Test");
	}

	END;
}

TEST(t_c_swprintv_cb)
{
	START;

	wchar buf[32] = { 0 };

	va_list empty = { 0 };
	EXPECT_EQ(c_swprintv_cb(NULL, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_swprintv_cb(buf, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_swprintv_cb(buf, 1, 0, NULL, NULL), 0);
#if defined(C_LINUX)
	EXPECT_EQ(c_swprintv_cb(buf, 1, 0, L"Test", empty), 0);
	EXPECT_WSTR(buf, L"");
	EXPECT_EQ(c_swprintv_cb(buf, 4, 2, L"Test", empty), 0);
	EXPECT_WSTR(buf, L"");
#endif
	EXPECT_EQ(c_swprintv_cb(buf, 32, 0, L"Test", empty), 4);
	EXPECT_WSTR(buf, L"Test");
	EXPECT_EQ(c_swprintv_cb(buf, 64, 4, L"Test", empty), 4);
	EXPECT_WSTR(buf, L"TestTest");

	END;
}

TESTP(t_c_fwprintv_cb, FILE *file)
{
	START;

	EXPECT_EQ(c_fwprintv_cb(NULL, 0, 0, NULL, NULL), 0);
	EXPECT_EQ(c_fwprintv_cb(file, 0, 0, NULL, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		va_list empty = { 0 };
		EXPECT_EQ(c_fwprintv_cb(file, 0, 0, L"Test", empty), 4);

		wchar buf[64] = { 0 };
		file_read_ft(file, (char *)buf, sizeof(buf));

		//EXPECT_WSTR(buf, L"Test"); //TODO: Read wchar from file
	}

	END;
}

STEST(t_print)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_c_printf);
	RUNP(t_c_fprintf, file);
	RUN(t_c_sprintf);
	RUN(t_c_wprintf);
	RUN(t_c_swprintf);
	RUNP(t_c_fflush, file);
	RUN(t_c_setmode);
	RUNP(t_c_ur, file);
	RUNP(t_c_v, file);
	RUNP(t_c_vr, file);
	RUN(t_c_sprintv_cb);
	RUNP(t_c_fprintv_cb, file);
	RUN(t_c_swprintv_cb);
	RUNP(t_c_fwprintv_cb, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
