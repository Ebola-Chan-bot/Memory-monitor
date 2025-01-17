#pragma once
#include<stdint.h>
namespace 内核 {
	public ref struct 数据收集器集
	{
		static property uint16_t 警报GB
		{
			uint16_t get();
			void set(uint16_t value);
		}
		static property uint16_t 强杀GB
		{
			uint16_t get();
			void set(uint16_t value);
		}
		static property System::TimeSpan 采样间隔
		{
			System::TimeSpan get();
			void set(System::TimeSpan value);
		}
		static 数据收集器集();
	};
}
