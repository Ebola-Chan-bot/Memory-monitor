#include "pch.h"
#include "内核.h"
#include<pla.h>
#include<atlsafe.h>
#include<sstream>
static CComPtr<IDataCollectorSet> DCS;
static CComPtr<IDataCollectorCollection> DCC;
static CComPtr<IAlertDataCollector> 警报数据收集器;
static const CComBSTR 数据收集器集名称{ L"埃博拉酱" };
static constexpr uint8_t 警报阈值索引 = 0;
static constexpr uint8_t 强杀阈值索引 = 1;
static constexpr CommitMode 提交模式 = static_cast<CommitMode>(plaCreateOrModify | plaUpdateRunningInstance);
static uint16_t 获取阈值GB(uint8_t 索引)
{
	CComSafeArray<BSTR>所有阈值;
	警报数据收集器->get_AlertThresholds(所有阈值.GetSafeArrayPtr());
	//wcstoull(wcschr(所有阈值[索引], L'>') + 1, nullptr, 10) >> 30;
	DISP_E_ARRAYISLOCKED
	return 20;
}
static void 设置阈值GB(uint8_t 索引, uint16_t 阈值)
{
	std::wstringstream 阈值文本{ L"\\Memory\\Committed Bytes>",std::ios::app };
	阈值文本 << (阈值 << 30ull);
	CComSafeArray<BSTR>所有阈值;
	警报数据收集器->get_AlertThresholds(所有阈值.GetSafeArrayPtr());
	所有阈值[索引] = 阈值文本.str().c_str();
	警报数据收集器->put_AlertThresholds(所有阈值);
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
		static const CComBSTR 警报名称{ L"内存监视器" };
		for (CComVariant 索引{ static_cast<BYTE>(0) }; 索引.bVal < 数据收集器个数; ++索引.bVal)
		{
			DCC->get_Item(索引, &数据收集器);
			CComBSTR 名称;
			数据收集器->get_Name(&名称);
			if (名称 == 警报名称)
			{
				警报数据收集器 = 数据收集器;
				数据收集器.Release();
				break;
			}
			数据收集器.Release();
		}
		if (!警报数据收集器)
		{
			DCC->CreateDataCollector(plaAlert, &数据收集器);
			警报数据收集器 = 数据收集器;
			constexpr wchar_t 字符串[] = L"\\Memory\\Committed Bytes>549755813888";
			CComSafeArray<BSTR>默认阈值{ 2 };
			默认阈值[0] = 字符串;
			默认阈值[1] = 字符串;
			警报数据收集器->put_AlertThresholds(默认阈值);
			警报数据收集器->put_SampleInterval(-1);
			警报数据收集器->put_Name(警报名称);
			警报数据收集器->put_Task(CComBSTR{ L"埃博拉酱的内存监视器" });
		}
	}
	uint16_t 数据收集器集::警报GB::get()
	{
		return 获取阈值GB(警报阈值索引);
	}
	void 数据收集器集::警报GB::set(uint16_t value)
	{
		设置阈值GB(警报阈值索引, value);
	}
	uint16_t 数据收集器集::强杀GB::get()
	{
		return 获取阈值GB(警报阈值索引);
	}
	void 数据收集器集::强杀GB::set(uint16_t value)
	{
		设置阈值GB(警报阈值索引, value);
	}
	System::TimeSpan 数据收集器集::采样间隔::get()
	{
		unsigned long 采样间隔;
		警报数据收集器->get_SampleInterval(&采样间隔);
		return System::TimeSpan::FromSeconds(static_cast<double>(采样间隔));
	}
	void 数据收集器集::采样间隔::set(System::TimeSpan value)
	{
		警报数据收集器->put_SampleInterval(value.TotalSeconds);
		CComPtr<IValueMap>值映射;
		DCS->Commit(数据收集器集名称, NULL, 提交模式, &值映射);
	}
}