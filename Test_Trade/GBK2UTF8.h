//
// Created by boer on 2021/7/18.
//

#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_GBK2UTF8_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_GBK2UTF8_H


#include <locale>
#include<vector>
#include <codecvt>
#ifdef _MSC_VER
const static std::locale g_loc("zh-CN");
#else
const static std::locale g_loc("zh_CN.GB18030");
#endif

std::string gbk2utf8(const char* str)
{
    const std::string &gb2312(str);
    std::vector<wchar_t> wstr(gb2312.size());
    wchar_t *wstrEnd = nullptr;
    const char *gbEnd = nullptr;
    mbstate_t state = {};
    int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
                                               (g_loc).in(state,gb2312.data(), gb2312.data() + gb2312.size(),
                                                          gbEnd,wstr.data(), wstr.data() + wstr.size(), wstrEnd);
    if (std::codecvt_base::ok == res)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t > > cutf8;
        std::string result = cutf8.to_bytes(std::wstring(wstr.data(), wstrEnd));
        return result;
    }
    else {
        std::string result;
        return result;
    }
}
//重载
std::string gbk2utf8(const char str)
{
    const std::string &gb2312(&str);
    std::vector<wchar_t> wstr(gb2312.size());
    wchar_t *wstrEnd = nullptr;
    const char *gbEnd = nullptr;
    mbstate_t state = {};
    int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
                                               (g_loc).in(state,gb2312.data(), gb2312.data() + gb2312.size(),
                                                          gbEnd,wstr.data(), wstr.data() + wstr.size(), wstrEnd);
    if (std::codecvt_base::ok == res)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
        std::string result = cutf8.to_bytes(std::wstring(wstr.data(), wstrEnd));
        return result;
    }
    else {
        std::string result;
        return result;
    }
}
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_GBK2UTF8_H