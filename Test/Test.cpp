// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "TFactory.h"

int main()
{
	TClassFactory fac = TClassFactory::CreateInstance();
}
