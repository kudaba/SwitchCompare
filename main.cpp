#include "Bootstrap.h"
#include "ChartJSPrinter.h"

//////////////////////////////////////////////////////////////////////////
// Dense Enum for use in dense switch and for counts of each pass
//////////////////////////////////////////////////////////////////////////
struct Command
{
	enum
	{
#define CODE_CHUNK(name, ...) name##1,
#include "code.inl"
#undef CODE_CHUNK
		Count1,
		Reset1 = Count1 - 1, // ensure that next set of commands are in order

#define CODE_CHUNK(name, ...) name##2,
#include "code.inl"
#undef CODE_CHUNK
		Count2,
		Reset2 = Count2 - 1, // ensure that next set of commands are in order

#define CODE_CHUNK(name, ...) name##3,
#include "code.inl"
#undef CODE_CHUNK
		Count3,
	};
};

//////////////////////////////////////////////////////////////////////////
// Enum for use in sparse switch
//////////////////////////////////////////////////////////////////////////
struct CommandSparse
{
	enum
	{
#define CODE_CHUNK(name, ...) \
name##1, \
name##1Skip = name##1 + 3,
#include "code.inl"
#undef CODE_CHUNK
		Count1,
		Next1 = Count1 + 57,

#define CODE_CHUNK(name, ...) \
name##2, \
name##2Skip = name##2 + 5,
#include "code.inl"
#undef CODE_CHUNK
		Count2,
		Next2 = Count2 + 99,

#define CODE_CHUNK(name, ...) \
name##3, \
name##3Skip = name##3 + 7,
#include "code.inl"
#undef CODE_CHUNK
		Count3,
	};
};


//////////////////////////////////////////////////////////////////////////
// Functions to call command as a dense switch case
//////////////////////////////////////////////////////////////////////////
static int ExecuteDenseSwitch1(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case Command::name##1: return 1 + (__VA_ARGS__) + 1;
#include "code.inl"
#undef CODE_CHUNK
	}
}

static int ExecuteDenseSwitch2(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case Command::name##1: return 1 + (__VA_ARGS__) + 1;\
	case Command::name##2: return 2 + (__VA_ARGS__) + 2;
#include "code.inl"
#undef CODE_CHUNK
	}
}

static int ExecuteDenseSwitch3(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case Command::name##1: return 1 + (__VA_ARGS__) + 1;\
	case Command::name##2: return 2 + (__VA_ARGS__) + 2;\
	case Command::name##3: return 3 + (__VA_ARGS__) + 3;
#include "code.inl"
#undef CODE_CHUNK
	}
}

//////////////////////////////////////////////////////////////////////////
// Functions to call command as a sparse switch case
//////////////////////////////////////////////////////////////////////////
static int ExecuteSparseSwitch1(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case CommandSparse::name##1: return 1 + (__VA_ARGS__) + 1;
#include "code.inl"
#undef CODE_CHUNK
	}
}

static int ExecuteSparseSwitch2(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case CommandSparse::name##1: return 1 + (__VA_ARGS__) + 1;\
	case CommandSparse::name##2: return 2 + (__VA_ARGS__) + 2;
#include "code.inl"
#undef CODE_CHUNK
	}
}

static int ExecuteSparseSwitch3(int command, int left, int right)
{
	switch (command)
	{
	default:
#define CODE_CHUNK(name, ...) \
	case CommandSparse::name##1: return 1 + (__VA_ARGS__) + 1;\
	case CommandSparse::name##2: return 2 + (__VA_ARGS__) + 2;\
	case CommandSparse::name##3: return 3 + (__VA_ARGS__) + 3;
#include "code.inl"
#undef CODE_CHUNK
	}
}

//////////////////////////////////////////////////////////////////////////
// Functions for individual commands
//////////////////////////////////////////////////////////////////////////
#define CODE_CHUNK(name, ...) \
static int name##CommandFunc1(int left, int right) { return 1 + (__VA_ARGS__) + 1; } \
static int name##CommandFunc2(int left, int right) { return 2 + (__VA_ARGS__) + 2; } \
static int name##CommandFunc3(int left, int right) { return 3 + (__VA_ARGS__) + 3; }
#include "code.inl"
#undef CODE_CHUNK

//////////////////////////////////////////////////////////////////////////
// Command interface with vtables
//////////////////////////////////////////////////////////////////////////
class CommandInterface
{
public:
	virtual int Execute(int left, int right) const = 0;
};

#define CODE_CHUNK(name, ...) \
class name##CommandInterface1 : public CommandInterface \
{ \
public: \
	int Execute(int left, int right) const override { return 0 + (__VA_ARGS__) + 1; } \
};\
name##CommandInterface1 g_##name##CommandInterface1; \
class name##CommandInterface2 : public CommandInterface \
{ \
public: \
	int Execute(int left, int right) const override { return 2 + (__VA_ARGS__) + 2; } \
};\
name##CommandInterface2 g_##name##CommandInterface2; \
class name##CommandInterface3 : public CommandInterface \
{ \
public: \
	int Execute(int left, int right) const override { return 3 + (__VA_ARGS__) + 3; } \
};\
name##CommandInterface3 g_##name##CommandInterface3;
#include "code.inl"
#undef CODE_CHUNK

//////////////////////////////////////////////////////////////////////////
// Holding tanks of functions and interfaces and global state
//////////////////////////////////////////////////////////////////////////
typedef int(*CommandFunc)(int left, int right);
CommandFunc g_Funcs[Command::Count3];
CommandInterface* g_Interfaces[Command::Count3];

std::function<int(int, int)> g_StdFuncs[Command::Count3];

int indexMap[Command::Count3];
int indexMapSparse[Command::Count3];

static void InitCommands()
{
#define CODE_CHUNK(name, ...) \
g_Funcs[Command::name##1] = name##CommandFunc1; \
g_Funcs[Command::name##2] = name##CommandFunc2; \
g_Funcs[Command::name##3] = name##CommandFunc3; \
\
g_StdFuncs[Command::name##1] = name##CommandFunc1; \
g_StdFuncs[Command::name##2] = name##CommandFunc2; \
g_StdFuncs[Command::name##3] = name##CommandFunc3; \
\
g_Interfaces[Command::name##1] = &g_##name##CommandInterface1; \
g_Interfaces[Command::name##2] = &g_##name##CommandInterface2; \
g_Interfaces[Command::name##3] = &g_##name##CommandInterface3;\
\
indexMap[Command::name##1] = Command::name##1;\
indexMap[Command::name##2] = Command::name##2;\
indexMap[Command::name##3] = Command::name##3;\
indexMapSparse[Command::name##1] = CommandSparse::name##1;\
indexMapSparse[Command::name##2] = CommandSparse::name##2;\
indexMapSparse[Command::name##3] = CommandSparse::name##3;

#include "code.inl"
#undef CODE_CHUNK
}

//////////////////////////////////////////////////////////////////////////
// Wrapper functions to normalize calls and ensure outside mechanics compile the same
//////////////////////////////////////////////////////////////////////////
__declspec(noinline)
int RunSwitch1(int index, int left, int right)
{
	return ExecuteDenseSwitch1(index, left, right);
}
__declspec(noinline)
int RunSwitch2(int index, int left, int right)
{
	return ExecuteDenseSwitch2(index, left, right);
}
__declspec(noinline)
int RunSwitch3(int index, int left, int right)
{
	return ExecuteDenseSwitch3(index, left, right);
}

__declspec(noinline)
int RunSwitchSparse1(int index, int left, int right)
{
	return ExecuteSparseSwitch1(index, left, right);
}
__declspec(noinline)
int RunSwitchSparse2(int index, int left, int right)
{
	return ExecuteSparseSwitch2(index, left, right);
}
__declspec(noinline)
int RunSwitchSparse3(int index, int left, int right)
{
	return ExecuteSparseSwitch3(index, left, right);
}

__declspec(noinline)
int RunFunc(int index, int left, int right)
{
	return g_Funcs[index](left, right);
}

__declspec(noinline)
int RunStdFunc(int index, int left, int right)
{
	return g_StdFuncs[index](left, right);
}

__declspec(noinline)
int RunInterface(int index, int left, int right)
{
	return g_Interfaces[index]->Execute(left, right);
}

// Noop to get a baseline
__declspec(noinline)
int RunNoop(int index, int left, int right)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Call wrapper
//////////////////////////////////////////////////////////////////////////
template<int count>
__int64 ExecutePass(Parameter const* param, int(*pass)(int, int, int), int const* indexMap)
{
	int value = 0;
	const long long loopCount = ((Int64Parameter const*)param)->GetValue();
	for (long long i = 0; i < loopCount; ++i)
	{
		int index = indexMap[i % count];
		value = pass(index, value, 1 + (i % 10));
	}
	return value;
}

__int64 ExecutePass1(Parameter const* param, int(*pass)(int, int, int), int const* indexMap)
{
	return ExecutePass<Command::Count1>(param, pass, indexMap);
}
__int64 ExecutePass2(Parameter const* param, int(*pass)(int, int, int), int const* indexMap)
{
	return ExecutePass<Command::Count2>(param, pass, indexMap);
}
__int64 ExecutePass3(Parameter const* param, int(*pass)(int, int, int), int const* indexMap)
{
	return ExecutePass<Command::Count2>(param, pass, indexMap);
}

class ExampleHarness : public TestHarness
{
	unique_ptr<TestSuite const> TestHarness::CreateTest() const override
	{
		InitCommands();

		//////////////////////////////////////////////////////////////////////////
		// Debug mode will run fewer operations and skip noop but also print the actual
		// pass output to ensure that all passes are evaluating identical results
		//////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
		long long run = 50000;
		TestConfig passConfig(1, 1, 0);
#else
		long long run = 100000;
		TestConfig passConfig(0, 10, 0);
#endif

		TestSuite* suite = new TestSuite("Test Suite");
		suite->DisableSummary();

		for (int i = 1; i <= 5; ++i)
			suite->AddAutoParameter((long long)run * (i * 5));

		char passName1[256];
		char passName2[256];
		char passName3[256];

		{
			char const* passName = "Switch V Functor V VTable (%d Commands)";
			sprintf_s(passName1, passName, Command::Count1);
			sprintf_s(passName2, passName, Command::Count2);
			sprintf_s(passName3, passName, Command::Count3);
		}

		passConfig.Performance.Sort = PassConfig::RelativeValue1Min;


		suite->SetPassConfig(passName1, passConfig);
		suite->SetPassConfig(passName2, passConfig);
		suite->SetPassConfig(passName3, passConfig);

		{
			unique_ptr<CodeTest> test(new CodeTest("Dense Switch"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunSwitch1, indexMap); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunSwitch2, indexMap); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunSwitch3, indexMap); });
			suite->AddTest(move(test));
		}

		{
			unique_ptr<CodeTest> test(new CodeTest("Sparse Switch"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunSwitchSparse1, indexMapSparse); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunSwitchSparse2, indexMapSparse); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunSwitchSparse3, indexMapSparse); });
			suite->AddTest(move(test));
		}

		{
			unique_ptr<CodeTest> test(new CodeTest("Function Pointer"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunFunc, indexMap); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunFunc, indexMap); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunFunc, indexMap); });
			suite->AddTest(move(test));
		}

#if !defined(_DEBUG)
		{
			unique_ptr<CodeTest> test(new CodeTest("STD Function"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunStdFunc, indexMap); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunStdFunc, indexMap); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunStdFunc, indexMap); });
			suite->AddTest(move(test));
		}
#endif

		{
			unique_ptr<CodeTest> test(new CodeTest("Virtual Method"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunInterface, indexMap); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunInterface, indexMap); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunInterface, indexMap); });
			suite->AddTest(move(test));
		}

#if !defined(_DEBUG)
		{
			unique_ptr<CodeTest> test(new CodeTest("noop"));
			test->SetPass(passName1, [](Parameter const* param) { return ExecutePass1(param, RunNoop, indexMap); });
			test->SetPass(passName2, [](Parameter const* param) { return ExecutePass2(param, RunNoop, indexMap); });
			test->SetPass(passName3, [](Parameter const* param) { return ExecutePass3(param, RunNoop, indexMap); });
			suite->AddTest(move(test));
		}
#endif

		return unique_ptr<TestSuite const>(suite);
	}

	void PrintTest(TestResults const& results) const override
	{
		ChartJSPrinter printer;
		printer.OneRowPerTest = true;
		printer.OutputPath = "bin";
		printer.PrintResults(results);
		printer.Open();
	}
};

int main()
{
	Bootstrap::RunTests(ExampleHarness());
	return 0;
}