#include "t_arr.h"

#include "arr.h"
#include "file.h"
#include "print.h"

#include "test.h"

#define TEST_FILE "t_arr.txt"

TEST(t_arr_init_free)
{
	START;

	arr_t arr = { 0 };

	EXPECT_EQ(arr_init(NULL, 0, sizeof(int)), NULL);
	EXPECT_EQ(arr_init(&arr, 0, sizeof(int)), NULL);
	EXPECT_NE(arr_init(&arr, 1, sizeof(int)), NULL);

	EXPECT_NE(arr.data, NULL);
	EXPECT_EQ(arr.cap, 1);
	EXPECT_EQ(arr.cnt, 0);
	EXPECT_EQ(arr.size, sizeof(int));

	arr_free(&arr);
	arr_free(NULL);

	EXPECT_EQ(arr.data, NULL);
	EXPECT_EQ(arr.cap, 0);
	EXPECT_EQ(arr.cnt, 0);
	EXPECT_EQ(arr.size, 0);

	END;
}

TEST(t_arr_add)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 1, sizeof(int));

	EXPECT_EQ(arr_add(NULL), ARR_END);
	EXPECT_NE(arr_add(&arr), ARR_END);
	EXPECT_NE(arr_add(&arr), ARR_END);
	EXPECT_EQ(arr.cnt, 2);
	EXPECT_EQ(arr.cap, 2);

	arr_free(&arr);

	END;
}

TEST(t_arr_get)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 1, sizeof(int));

	EXPECT_EQ(arr_get(NULL, ARR_END), NULL);
	EXPECT_EQ(arr_get(&arr, ARR_END), NULL);
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;

	EXPECT_EQ(*(int *)arr_get(&arr, 0), 1);

	arr_free(&arr);

	END;
}

TEST(t_arr_set)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 1, sizeof(int));

	int value = 1;

	uint a0 = arr_add(&arr);

	EXPECT_EQ(arr_set(NULL, ARR_END, NULL), NULL);
	EXPECT_EQ(arr_set(&arr, ARR_END, NULL), NULL);
	EXPECT_EQ(arr_set(&arr, a0, NULL), NULL);
	EXPECT_EQ(arr_set(&arr, ARR_END, &value), NULL);
	EXPECT_NE(arr_set(&arr, a0, &value), NULL);

	EXPECT_EQ(*(int *)arr_get(&arr, 0), value);

	arr_free(&arr);

	END;
}

TEST(t_arr_app)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 2, sizeof(int));

	EXPECT_EQ(arr_app(NULL, NULL), ARR_END);
	EXPECT_EQ(arr_app(&arr, NULL), ARR_END);

	const int v0 = 1;
	const int v1 = 2;

	uint i0 = arr_app(&arr, &v0);
	uint i1 = arr_app(&arr, &v1);

	EXPECT_EQ(i0, 0);
	EXPECT_EQ(i1, 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 2);
	EXPECT_EQ(arr.cnt, 2);
	EXPECT_EQ(arr.cap, 2);

	arr_free(&arr);

	END;
}

TEST(t_arr_index)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 2, sizeof(int));

	const int value0 = 0;
	const int value1 = 1;

	*(int *)arr_get(&arr, arr_add(&arr)) = value0;
	*(int *)arr_get(&arr, arr_add(&arr)) = value1;

	EXPECT_EQ(arr_index(NULL, NULL), ARR_END);
	EXPECT_EQ(arr_index(&arr, NULL), ARR_END);
	EXPECT_EQ(arr_index(&arr, &value0), 0);
	EXPECT_EQ(arr_index(&arr, &value1), 1);

	arr_free(&arr);

	END;
}

static int index_cmp_cb(const void *value1, const void *value2)
{
	return *(int *)value1 == *(int *)value2;
}

TEST(t_arr_index_cmp)
{
	START;

	arr_t arr = { 0 };
	arr_init(&arr, 2, sizeof(int));

	const int value0 = 0;
	const int value1 = 1;
	const int value2 = 2;

	*(int *)arr_get(&arr, arr_add(&arr)) = value0;
	*(int *)arr_get(&arr, arr_add(&arr)) = value1;

	EXPECT_EQ(arr_index_cmp(NULL, NULL, NULL), ARR_END);
	EXPECT_EQ(arr_index_cmp(&arr, NULL, NULL), ARR_END);
	EXPECT_EQ(arr_index_cmp(&arr, &value0, NULL), ARR_END);
	EXPECT_EQ(arr_index_cmp(&arr, &value2, index_cmp_cb), ARR_END);
	EXPECT_EQ(arr_index_cmp(&arr, &value0, index_cmp_cb), 0);
	EXPECT_EQ(arr_index_cmp(&arr, &value1, index_cmp_cb), 1);

	arr_free(&arr);

	END;
}

TEST(t_arr_add_all)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr, 4, sizeof(int));
	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	EXPECT_EQ(arr_add_all(NULL, &arr1), NULL);
	EXPECT_EQ(arr_add_all(&arr, NULL), NULL);
	EXPECT_NE(arr_add_all(&arr, &arr0), NULL);
	EXPECT_NE(arr_add_all(&arr, &arr1), NULL);

	EXPECT_EQ(arr.cnt, 4);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 3), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(t_arr_add_unique)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr, 3, sizeof(int));
	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	EXPECT_EQ(arr_add_unique(NULL, &arr1), NULL);
	EXPECT_EQ(arr_add_unique(&arr, NULL), NULL);
	EXPECT_NE(arr_add_unique(&arr, &arr0), NULL);
	EXPECT_NE(arr_add_unique(&arr, &arr1), NULL);

	EXPECT_EQ(arr.cnt, 3);
	EXPECT_EQ(arr.cap, 3);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(t_arr_merge_all)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };
	arr_t arrs = { 0 };

	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));
	arr_init(&arrs, 2, sizeof(long long));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	EXPECT_EQ(arr_merge_all(NULL, &arr0, &arr1), NULL);
	EXPECT_EQ(arr_merge_all(&arr, NULL, &arr1), NULL);
	EXPECT_EQ(arr_merge_all(&arr, &arr0, NULL), NULL);
	EXPECT_EQ(arr_merge_all(&arr, &arrs, &arr1), NULL);
	EXPECT_EQ(arr_merge_all(&arr, &arr0, &arrs), NULL);
	EXPECT_NE(arr_merge_all(&arr, &arr0, &arr1), NULL);

	EXPECT_EQ(arr.cnt, 4);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 3), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);
	arr_free(&arrs);

	END;
}

TEST(t_arr_merge_unique)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };
	arr_t arrs = { 0 };

	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));
	arr_init(&arrs, 2, sizeof(long long));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	EXPECT_EQ(arr_merge_unique(NULL, &arr0, &arr1), NULL);
	EXPECT_EQ(arr_merge_unique(&arr, NULL, &arr1), NULL);
	EXPECT_EQ(arr_merge_unique(&arr, &arr0, NULL), NULL);
	EXPECT_EQ(arr_merge_unique(&arr, &arrs, &arr1), NULL);
	EXPECT_EQ(arr_merge_unique(&arr, &arr0, &arrs), NULL);
	EXPECT_NE(arr_merge_unique(&arr, &arr0, &arr1), NULL);

	EXPECT_EQ(arr.cnt, 3);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);
	arr_free(&arrs);

	END;
}

TEST(t_arr_foreach)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));
	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;

	int *value = NULL;

	int i = 0;
	arr_foreach(&arr, value)
	{
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 2);

	arr_free(&arr);

	END;
}

static int print_arr(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret + 1;
}

TEST(t_arr_print)
{
	START;

	//arr_t arr = { 0 };
	//arr_init(&arr, 1, sizeof(int));

	//*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	//*(int *)arr_get(&arr, arr_add(&arr)) = 1;
	//*(int *)arr_get(&arr, arr_add(&arr)) = 2;

	EXPECT_EQ(arr_print(NULL, NULL, NULL, 0), 0);
	//EXPECT_EQ(arr_print(&arr, NULL, NULL, 0), 0);
	//EXPECT_EQ(arr_print(&arr, file, NULL, 0), 0);
	//EXPECT_EQ(arr_print(&arr, file, print_arr, 0), 3);

	{
		//file_reopen(TEST_FILE, "wb+", file);
		//EXPECT_EQ(arr_print(&arr, file, NULL, 0), 0);
		//EXPECT_EQ(arr_print(&arr, file, print_arr, 0), 3);

		//char buf[128] = { 0 };
		//file_read_ft(file, buf, sizeof(buf));

		/*const char exp[] = "0\n"
				   "1\n"
				   "2\n";*/
		//EXPECT_STR(buf, exp);
	}

	//arr_free(&arr);

	END;
}

STEST(t_arr)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_arr_init_free);
	RUN(t_arr_add);
	RUN(t_arr_get);
	RUN(t_arr_set);
	RUN(t_arr_app);
	RUN(t_arr_index);
	RUN(t_arr_index_cmp);
	RUN(t_arr_add_all);
	RUN(t_arr_add_unique);
	RUN(t_arr_merge_all);
	RUN(t_arr_merge_unique);
	RUN(t_arr_foreach);
	RUN(t_arr_print);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
