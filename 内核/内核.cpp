#include "pch.h"
#include "内核.h"
#include<pla.h>
#include<atlsafe.h>
#include<sstream>
static CComPtr<IDataCollectorSet> DCS;
static CComPtr<IDataCollectorCollection> DCC;
static CComPtr<IAlertDataCollector> 警报收集器;
static CComPtr<IAlertDataCollector> 强杀收集器;
static const CComBSTR 数据收集器集名称{ L"埃博拉酱" };
static constexpr CommitMode 提交模式 = static_cast<CommitMode>(plaCreateOrModify | plaUpdateRunningInstance);
static void 警报器初始化(CComPtr<IAlertDataCollector>& 警报器, CComBSTR 名称)
{
	if (!警报器)
	{
		CComPtr<IDataCollector> 数据收集器;
		DCC->CreateDataCollector(plaAlert, &数据收集器);
		警报器 = 数据收集器;
		static CComSafeArray<BSTR> 默认阈值 = []()
			{
				CComSafeArray<BSTR>阈值{ 1 };
				阈值[0] = L"\\Memory\\Committed Bytes>549755813888";
				return 阈值;
			}();
		警报器->put_AlertThresholds(默认阈值);
		警报器->put_SampleInterval(-1);
		警报器->put_Name(名称);
		static const CComBSTR 本进程路径 = []() 
			{
				wchar_t 路径[MAX_PATH];
				GetModuleFileNameW(NULL, 路径, MAX_PATH);
				return 路径;
			}();
		警报器->put_Task(本进程路径);
		警报器->put_TaskArguments(名称);
		警报器->put_TaskUserTextArguments()
	}
}
static uint16_t 获取阈值GB(const CComPtr<IAlertDataCollector>&警报器)
{
	CComSafeArray<BSTR>警报阈值;
	警报器->get_AlertThresholds(警报阈值.GetSafeArrayPtr());
	return wcstoull(wcschr(警报阈值[0], L'>') + 1, nullptr, 10) >> 30;
}
static void 设置阈值GB(const CComPtr<IAlertDataCollector>& 警报器, uint16_t 阈值)
{
	std::wstringstream 阈值文本{ L"\\Memory\\Committed Bytes>",std::ios::app };
	阈值文本 << (阈值 << 30ull);
	static CComSafeArray<BSTR>性能计数器{ 1 };
	性能计数器[0] = 阈值文本.str().c_str();
	警报器->put_AlertThresholds(性能计数器);
	CComPtr<IValueMap>值映射;
	DCS->Commit(数据收集器集名称, NULL, 提交模式, &值映射);
}
namespace 内核
{
	static 数据收集器集::数据收集器集()
	{
		DCS.CoCreateInstance(__uuidof(DataCollectorSet));
		DCS->Query(数据收集器集名称, NULL);
		DCS->get_DataCollectors(&DCC);
		long 数据收集器个数;
		DCC->get_Count(&数据收集器个数);
		CComPtr<IDataCollector> 数据收集器;
		for (CComVariant 索引{ static_cast<BYTE>(0) }; 索引.bVal < 数据收集器个数; ++索引.bVal)
		{
			DCC->get_Item(索引, &数据收集器);
			CComBSTR 名称;
			数据收集器->get_Name(&名称);
			if (名称 == L"内存监视器_警报")
				警报收集器 = 数据收集器;
			else if (名称 == L"内存监视器_强杀")
				强杀收集器 = 数据收集器;
			数据收集器.Release();
		}
		警报器初始化(警报收集器, L"内存监视器_警报");
		警报器初始化(强杀收集器, L"内存监视器_强杀");
	}
	uint16_t 数据收集器集::警报GB::get()
	{
		return 获取阈值GB(警报收集器);
	}
	void 数据收集器集::警报GB::set(uint16_t value)
	{
		设置阈值GB(警报收集器, value);
	}
	uint16_t 数据收集器集::强杀GB::get()
	{
		return 获取阈值GB(强杀收集器);
	}
	void 数据收集器集::强杀GB::set(uint16_t value)
	{
		设置阈值GB(强杀收集器, value);
	}
	System::TimeSpan 数据收集器集::采样间隔::get()
	{
		unsigned long 采样间隔;
		警报收集器->get_SampleInterval(&采样间隔);
		return System::TimeSpan::FromSeconds(static_cast<double>(采样间隔));
	}
	void 数据收集器集::采样间隔::set(System::TimeSpan value)
	{
		警报收集器->put_SampleInterval(value.TotalSeconds);
		强杀收集器->put_SampleInterval(value.TotalSeconds);
		DCS->Commit(数据收集器集名称, NULL, 提交模式, NULL);
	}
}