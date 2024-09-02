/*
MIT License

Copyright (c) 2017 Sambit Samal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#define _ASSERT(cond) ((!(cond)) ? _assert(#cond,__FILE__,__LINE__) : _noassert())

#include "BigDecimal.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <cctype>
using namespace std;
static int _scale = 100;

static const string ONE ("1");
static const string TWO ("2");
static const string ZERO ("0");
static const string TEN("10");
static void _noassert() { }
static string getLeftOfDot(string &value)
{
    size_t dot = value.find('.');
    if(dot != string::npos)
    {
        if(dot == 0)
            return string("0");
        if(dot == 1 && value[0] == '-')
            return string("-0");
        return value.substr(0, dot);
    }
    else
    {
        return value;
    }
}
static void _assert(const char *assertion, const char *file, int line)
{
    cerr<<"Critical Error in: "<<assertion<<", File '"<<file<<"' in line "<<line<<"."<<endl;
    exit(-1);
}
string trimTrailingZeros(string input)
{
    if(input.find(".") != string::npos)
    {
        string result = "";
        string inp(input.rbegin(),input.rend());
        result = inp.erase(0,min(inp.find_first_not_of('0'),inp.size()-1));
        if(result.at(0) == '.')
        {
            result = result.erase(0,1);
        }
        return string(result.rbegin(),result.rend());
    }
    else
    {
        return input;
    }
}
//parse a number into parts, returns scale on success and -1 on error
static int parse_number (const string &s, int &lsign, int &lint, int &ldot, int &lfrac, int &lscale)
{
    int i = 0;
    lsign = 1;
    if (s[i] == '-' || s[i] == '+')
    {
        if (s[i] == '-')
        {
            lsign = -1;
        }
        i++;
    }
    int len = (int)s.length();
    if (i >= len)
    {
        return -1;
    }
    lint = i;

    while (i < len && '0' <= s[i] && s[i] <= '9')
    {
        i++;
    }
    ldot = i;

    lscale = 0;
    if (i < len && s[i] == '.')
    {
        lscale = (int)s.length() - i - 1;
        i++;
    }
    lfrac = i;

    while (i < len && '0' <= s[i] && s[i] <= '9')
    {
        i++;
    }

    if (i < len)
    {
        return -1;
    }

    while (s[lint] == '0' && lint + 1 < ldot)
    {
        lint++;
    }
//  while (lscale > 0 && s[lfrac + lscale - 1] == '0') {
//    lscale--;
//  }
    if (lscale == 0 && lfrac > ldot)
    {
        lfrac--;
        _ASSERT (lfrac == ldot);
    }

    if (lsign < 0 && (lscale == 0 && s[lint] == '0'))
    {
        lsign = 1;
    }
    return lscale;
}

static string _zero (int scale)
{
    if (scale == 0)
    {
        return ZERO;
    }
    string result (scale + 2, '0');
    result[1] = '.';
    return result;
}

static int _compareTo (const char *lhs, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rint, int rdot, int rfrac, int rscale, int scale)
{
    int llen = ldot - lint;
    int rlen = rdot - rint;

    if (llen != rlen)
    {
        return (llen < rlen ? -1 : 1);
    }

    for (int i = 0; i < llen; i++)
    {
        if (lhs[lint + i] != rhs[rint + i])
        {
            return (lhs[lint + i] < rhs[rint + i] ? -1 : 1);
        }
    }

    int i;
    for (i = 0; (i < lscale || i < rscale) && i < scale; i++)
    {
        int lchar = (i < lscale ? lhs[lfrac + i] : '0');
        int rchar = (i < rscale ? rhs[rfrac + i] : '0');
        if (lchar != rchar)
        {
            return (lchar < rchar ? -1 : 1);
        }
    }

    return 0;
}

static string _round (char *lhs, int lint, int ldot, int lfrac, int lscale, int scale, int sign, bool add_trailing_zeroes, bool round_last = false)
{
    while (lhs[lint] == '0' && lint + 1 < ldot)
    {
        lint++;
    }

    _ASSERT (lint > 0 && lscale >= 0 && scale >= 0);

    if (sign < 0 && lhs[lint] == '0')
    {
        sign = 1;
        for (int i = 0; i < lscale; i++)
        {
            if (lhs[lfrac + i] != '0')
            {
                sign = -1;
                break;
            }
        }
    }

    if(round_last)
    {
        if (lscale > scale)
        {
            while (scale > 0 && lhs[lfrac + scale - 1] == '9' && lhs[lfrac + scale] >= '5')
            {
                scale--;
            }
            lscale = scale;
            if (lhs[lfrac + scale] >= '5')
            {
                if (scale > 0)
                {
                    lhs[lfrac + scale - 1]++;
                }
                else
                {
                    lfrac--;
                    _ASSERT (lfrac == ldot);

                    int i;
                    lhs[lint - 1] = '0';
                    for (i = 0; lhs[ldot - i - 1] == '9'; i++)
                    {
                        lhs[ldot - i - 1] = '0';
                    }
                    lhs[ldot - i - 1]++;
                    if (ldot - i - 1 < lint)
                    {
                        lint = ldot - i - 1;
                    }
                }
            }
        }

        while (lscale > 0 && lhs[lfrac + lscale - 1] == '0')
        {
            lscale--;
        }
    }
    else
    {
        if (lscale > scale)
        {
            lscale = scale;
        }
    }

    if (lscale == 0 && lfrac > ldot)
    {
        lfrac--;
        _ASSERT (lfrac == ldot);
    }

    if (sign < 0)
    {
        lhs[--lint] = '-';
    }

    if (lscale == scale || !add_trailing_zeroes)
    {
        return string (string(lhs + lint).substr(0, lfrac + lscale - lint));
    }
    else
    {
        string result (string(lhs + lint).substr(0, lfrac + lscale - lint));
        if (lscale == 0)
        {
            result+='.';
        }
        for(int kI = 0; kI < scale - lscale; ++kI)
            result+='0';
        return result;
    }
}

static string add_positive (const char *lhs, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign)
{
    int llen = ldot - lint;
    int rlen = rdot - rint;

    int resint, resdot, resfrac, resscale;

    int result_len = max (llen, rlen) + 1;
    int result_scale = max (lscale, rscale);
    int result_size = result_len + result_scale + 3;
    string result(result_size, '0');

    int i, um = 0;
    int cur_pos = result_size;
    int was_frac = 0;
    for (i = result_scale - 1; i >= 0; i--)
    {
        if (i < lscale)
        {
            um += lhs[lfrac + i] - '0';
        }
        if (i < rscale)
        {
            um += rhs[rfrac + i] - '0';
        }

        if (um != 0 || was_frac)
        {
            result[--cur_pos] = (char)(um % 10 + '0');
            um /= 10;
            was_frac = 1;
        }
    }
    resscale = result_size - cur_pos;
    resfrac = cur_pos;
    if (was_frac)
    {
        result[--cur_pos] = '.';
    }
    resdot = cur_pos;

    for (int i = 0; i < result_len; i++)
    {
        if (i < llen)
        {
            um += lhs[ldot - i - 1] - '0';
        }
        if (i < rlen)
        {
            um += rhs[rdot - i - 1] - '0';
        }

        result[--cur_pos] = (char)(um % 10 + '0');
        um /= 10;
    }
    resint = cur_pos;
    _ASSERT (cur_pos > 0);

    return _round ((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static string subtract_positive (const char *lhs, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign)
{
    int llen = ldot - lint;
    int rlen = rdot - rint;

    int resint, resdot, resfrac, resscale;

    int result_len = llen;
    int result_scale = max (lscale, rscale);
    int result_size = result_len + result_scale + 3;
    string result (result_size, '0');

    int i, um = 0, next_um = 0;
    int cur_pos = result_size;
    int was_frac = 0;
    for (i = result_scale - 1; i >= 0; i--)
    {
        um = next_um;
        if (i < lscale)
        {
            um += lhs[lfrac + i] - '0';
        }
        if (i < rscale)
        {
            um -= rhs[rfrac + i] - '0';
        }
        if (um < 0)
        {
            next_um = -1;
            um += 10;
        }
        else
        {
            next_um = 0;
        }

        if (um != 0 || was_frac)
        {
            result[--cur_pos] = (char)(um + '0');
            was_frac = 1;
        }
    }
    resscale = result_size - cur_pos;
    resfrac = cur_pos;
    if (was_frac)
    {
        result[--cur_pos] = '.';
    }
    resdot = cur_pos;

    for (int i = 0; i < result_len; i++)
    {
        um = next_um;
        um += lhs[ldot - i - 1] - '0';
        if (i < rlen)
        {
            um -= rhs[rdot - i - 1] - '0';
        }
        if (um < 0)
        {
            next_um = -1;
            um += 10;
        }
        else
        {
            next_um = 0;
        }

        result[--cur_pos] = (char)(um + '0');
    }
    resint = cur_pos;
    _ASSERT (cur_pos > 0);

    return _round ((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static string multiply_positive (const char *lhs, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign)
{
    int llen = ldot - lint;
    int rlen = rdot - rint;

    int resint, resdot, resfrac, resscale;

    int result_len = llen + rlen;
    int result_scale = lscale + rscale;
    int result_size = result_len + result_scale + 3;
    string result (result_size, '0');

    int *res = (int *)malloc(sizeof (int) * result_size);
    memset(res, 0, sizeof (int) * result_size);
    for (int i = -lscale; i < llen; i++)
    {
        int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
        for (int j = -rscale; j < rlen; j++)
        {
            int y = (j < 0 ? rhs[rfrac - j - 1] : rhs[rdot - j - 1]) - '0';
            res[i + j + result_scale] += x * y;
        }
    }
    for (int i = 0; i + 1 < result_size; i++)
    {
        res[i + 1] += res[i] / 10;
        res[i] %= 10;
    }

    int cur_pos = result_size;
    for (int i = 0; i < result_scale; i++)
    {
        result[--cur_pos] = (char)(res[i] + '0');
    }
    resscale = result_size - cur_pos;
    resfrac = cur_pos;
    if (result_scale > 0)
    {
        result[--cur_pos] = '.';
    }
    resdot = cur_pos;

    for (int i = result_scale; i < (result_len + result_scale); i++)
    {
        result[--cur_pos] = (char)(res[i] + '0');
    }
    resint = cur_pos;
    _ASSERT (cur_pos > 0);

    free(res);

    char *data = (char*)malloc((result.length()+1)*sizeof(char));
    if (data)
    {
        sprintf(data, result.c_str());
    }
    string ret = _round (data, resint, resdot, resfrac, resscale, scale, sign, 0);
    free(data);

    return ret;
}

static string divide_positive (const char *lhs, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign)
{
    int llen = ldot - lint;
    int rlen = rdot - rint;

    int resint, resdot = -1, resfrac = -1, resscale;

    int result_len = max (llen + rscale - rlen + 1, 1);
    int result_scale = scale;
    int result_size = result_len + result_scale + 3;

    if (rscale == 0 && rhs[rint] == '0')
    {
        cerr << ("Division By ZERO")<< endl<< endl;
        return ZERO;
    }

    int dividend_len = llen + lscale;
    int divider_len = rlen + rscale;
    int *dividend = (int *)malloc(sizeof (int) * (result_size + dividend_len + divider_len));
    int *divider = (int *)malloc(sizeof (int) * divider_len);
    memset(dividend, 0, sizeof (int) * (result_size + dividend_len + divider_len));
    memset(divider, 0, sizeof (int) * divider_len);

    for (int i = -lscale; i < llen; i++)
    {
        int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
        dividend[llen - i - 1] = x;
    }

    for (int i = -rscale; i < rlen; i++)
    {
        int x = (i < 0 ? rhs[rfrac - i - 1] : rhs[rdot - i - 1]) - '0';
        divider[rlen - i - 1] = x;
    }

    int divider_skip = 0;
    while (divider_len > 0 && divider[0] == 0)
    {
        divider++;
        divider_skip++;
        divider_len--;
    }
    _ASSERT (divider_len > 0);

    int cur_pow = llen - rlen + divider_skip;
    int cur_pos = 2;

    if (cur_pow < -scale)
    {
        divider -= divider_skip;
        divider_len += divider_skip;
        free(dividend);
        free(divider);
        return _zero (scale);
    }

    string result (result_size, '0');
    resint = cur_pos;
    if (cur_pow < 0)
    {
        result[cur_pos++] = '0';
        resdot = cur_pos;
        result[cur_pos++] = '.';
        resfrac = cur_pos;
        for (int i = -1; i > cur_pow; i--)
        {
            result[cur_pos++] = '0';
        }
    }

    int beg = 0, real_beg = 0;
    while (cur_pow >= -scale)
    {
        char dig = '0';
        while (true)
        {
            if (real_beg < beg && dividend[real_beg] == 0)
            {
                real_beg++;
            }

            bool less = false;
            if (real_beg == beg)
            {
                for (int i = 0; i < divider_len; i++)
                {
                    if (dividend[beg + i] != divider[i])
                    {
                        less = (dividend[beg + i] < divider[i]);
                        break;
                    }
                }
            }
            if (less)
            {
                break;
            }

            for (int i = divider_len - 1; i >= 0; i--)
            {
                dividend[beg + i] -= divider[i];
                if (dividend[beg + i] < 0)
                {
                    dividend[beg + i] += 10;
                    dividend[beg + i - 1]--;
                }
            }
            dig++;
        }

        result[cur_pos++] = dig;

        if (cur_pow == 0)
        {
            resdot = cur_pos;
            if (scale > 0)
            {
                result[cur_pos++] = '.';
            }
            resfrac = cur_pos;
        }
        cur_pow--;
        beg++;
    }
    resscale = cur_pos - resfrac;

    divider -= divider_skip;
    divider_len += divider_skip;
    free(dividend);
    free(divider);

    char *data = (char*)malloc((result.length()+1)*sizeof(char));
    if (data)
    {
        sprintf(data, result.c_str());
    }

    string ret = _round (data, resint, resdot, resfrac, resscale, scale, sign, 0);
    free(data);

    return ret;
}


static string _add (const char *lhs, int lsign, int lint, int ldot, int lfrac, int lscale, const char *rhs, int rsign, int rint, int rdot, int rfrac, int rscale, int scale)
{
    if (lsign > 0 && rsign > 0)
    {
        return add_positive (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, 1);
    }

    if (lsign > 0 && rsign < 0)
    {
        if (_compareTo (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, 1000000000) >= 0)
        {
            return subtract_positive (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, 1);
        }
        else
        {
            return subtract_positive (rhs, rint, rdot, rfrac, rscale, lhs, lint, ldot, lfrac, lscale, scale, -1);
        }
    }

    if (lsign < 0 && rsign > 0)
    {
        if (_compareTo (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, 1000000000) <= 0)
        {
            return subtract_positive (rhs, rint, rdot, rfrac, rscale, lhs, lint, ldot, lfrac, lscale, scale, 1);
        }
        else
        {
            return subtract_positive (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, -1);
        }
    }

    if (lsign < 0 && rsign < 0)
    {
        return add_positive (lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, -1);
    }

    _ASSERT (0);
    return ZERO; //Is dummy...
}

void BigDecimal::setscale (int scale)
{
    if (scale < 0)
    {
        _scale = 0;
    }
    else
    {
        _scale = scale;
    }
}

string BigDecimal::divide (const string &lhs, const string &rhs,int scale)
{
    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }
    if (scale < 0)
    {
        cerr << "Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<<endl;
        scale = 0;
    }
    if (lhs.empty())
    {
        return _zero (scale);
    }
    if (rhs.empty())
    {
        cerr << "Division By Empty "<<  rhs.c_str() << endl;
        return _zero (scale);
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<  lhs.c_str() <<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) < 0)
    {
        cerr << "\""<< rhs.c_str() <<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    return trimTrailingZeros(divide_positive (lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, scale, lsign * rsign));
}

string BigDecimal::modulus (const string &lhs, const string &rhs,int scale)
{
    if (lhs.empty())
    {
        return ZERO;
    }
    if (rhs.empty())
    {
        cerr << "Modulo By empty "<<endl;
        return ZERO;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) != 0)
    {
        cerr << "\""<< lhs.c_str() <<"\" Is Not A Integer"<< endl;
        return ZERO;
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) != 0)
    {
        cerr << "\""<< rhs.c_str() <<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    long long mod = 0;
    for (int i = rint; i < rdot; i++)
    {
        mod = mod * 10 + rhs[i] - '0';
    }

    if (rdot - rint > 18 || mod == 0)
    {
        cerr << "\""<< rhs.c_str() <<"\" Is Not A Non Zero Integer Less Than 1e18 By Absolute Value"<< endl;
        return ZERO;
    }

    long long res = 0;
    for (int i = lint; i < ldot; i++)
    {
        res = res * 2;
        if (res >= mod)
        {
            res -= mod;
        }
        res = res * 5 + lhs[i] - '0';
        while (res >= mod)
        {
            res -= mod;
        }
    }

    char buffer[20];
    int cur_pos = 20;
    do
    {
        buffer[--cur_pos] = (char)(res % 10 + '0');
        res /= 10;
    }
    while (res > 0);

    if (lsign < 0)
    {
        buffer[--cur_pos] = '-';
    }

    return string(trimTrailingZeros(string(buffer + cur_pos).substr(0, 20 - cur_pos)));
}

string BigDecimal::pow (const string &lhs, const string &rhs,int scale)
{
    if (lhs.empty())
    {
        return ZERO;
    }
    if (rhs.empty())
    {
        return ONE;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) != 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) != 0)
    {
        cerr << "\""<< rhs.c_str() <<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    long long deg = 0;
    for (int i = rint; i < rdot; i++)
    {
        deg = deg * 10 + rhs[i] - '0';
    }

    if (rdot - rint > 18 || (rsign < 0 && deg != 0))
    {
        cerr << "\""<< rhs.c_str() <<"\" Is Not A Non Negative Integer Less Than 1e18"<< endl;
        return ZERO;
    }

    if (deg == 0)
    {
        return ONE;
    }

    string result = ONE;
    string mul = lhs;
    while (deg > 0)
    {
        if (deg & 1)
        {
            result = multiply (result, mul, 0);
        }
        mul = multiply (mul, mul, 0);
        deg >>= 1;
    }

    return trimTrailingZeros(result);
}

string BigDecimal::add (const string &lhs, const string &rhs, int scale)
{
    if (lhs.empty())
    {
        return add (ZERO, rhs, scale);
    }
    if (rhs.empty())
    {
        return add (lhs, ZERO, scale);
    }

//  if (scale == SHRT_MAX) {
//    scale = _scale;
//  }
    if (scale < 0)
    {
        cerr <<"Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<< endl;
        scale = 0;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<< lhs.c_str() << "\" Is Not A Number"<< endl;
        return _zero (scale);
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) < 0)
    {
        cerr << "\""<< rhs.c_str() << "\" Is Not A Number"<< endl;
        return _zero (scale);
    }

    return trimTrailingZeros(_add (lhs.c_str(), lsign, lint, ldot, lfrac, lscale, rhs.c_str(), rsign, rint, rdot, rfrac, rscale, max (lscale, rscale)));
}

string BigDecimal::subtract (const string &lhs, const string &rhs, int scale)
{
    if (lhs.empty())
    {
        return subtract (ZERO, rhs, scale);
    }
    if (rhs.empty())
    {
        return subtract (lhs, ZERO, scale);
    }

//  if (scale == SHRT_MAX) {
//    scale = _scale;
//  }
    if (scale < 0)
    {
        cerr <<"Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<< endl;
        scale = 0;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return _zero (scale);
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) < 0)
    {
        cerr << "\""<<rhs.c_str()<<"\" Is Not A Number"<< endl;
        return _zero (scale);
    }

    rsign *= -1;

    return trimTrailingZeros(_add (lhs.c_str(), lsign, lint, ldot, lfrac, lscale, rhs.c_str(), rsign, rint, rdot, rfrac, rscale, max (lscale, rscale)));
}

string BigDecimal::multiply (const string &lhs, const string &rhs, int scale)
{
    if (lhs.empty())
    {
        return multiply (ZERO, rhs, scale);
    }
    if (rhs.empty())
    {
        return multiply (lhs, ZERO, scale);
    }

    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }
    if (scale < 0)
    {
        cerr <<"Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<< endl;
        scale = 0;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) < 0)
    {
        cerr << "\""<<rhs.c_str()<<"\" Is Not A Number"<< endl;
        return ZERO;
    }

    return trimTrailingZeros(multiply_positive (lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, lscale + rscale, lsign * rsign));
}

int BigDecimal::compareTo (const string &lhs, const string &rhs, int scale)
{
    if (lhs.empty())
    {
        return BigDecimal::compareTo (ZERO, rhs, scale);
    }
    if (rhs.empty())
    {
        return BigDecimal::compareTo (lhs, ZERO, scale);
    }

    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }
    if (scale < 0)
    {
        cerr << "Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<< endl;
        scale = 0;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return 0;
    }

    int rsign, rint, rdot, rfrac, rscale;
    if (parse_number (rhs, rsign, rint, rdot, rfrac, rscale) < 0)
    {
        cerr << "\""<<rhs.c_str()<<"\" Is Not A Number"<< endl;
        return 0;
    }

    if (lsign != rsign)
    {
        return (lsign - rsign) / 2;
    }

    return (1 - 2 * (lsign < 0)) * _compareTo (lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, scale);
}

string BigDecimal::round (const string &lhs, int scale)
{
    if (lhs.empty())
    {
        return BigDecimal::round (ZERO, scale);
    }

    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }

    if (scale < 0)
    {
        cerr << "Scale ("<<to_string(scale).c_str()<<") Cant Be Negative!!!"<< endl;
        scale = 0;
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return "0";
    }

    int len = (int)lhs.size();
    string result(len + 1, '0');
    for(int i = len-1; i>=lint; --i)
    {
        result[i+1] = lhs[i];
    }

    char *data = (char*)malloc((result.length()+1)*sizeof(char));
    if (data)
    {
        sprintf(data, result.c_str());
    }
    string ret = _round (data, lint+1, ldot+1, lfrac+1, lscale, scale, lsign, 1, 1);
    free(data);
    return ret;
}

string BigDecimal::ln(const string &lhs, int scale)
{
    char decision;
    if (lhs.empty())
    {
        return BigDecimal::ln(ZERO, scale);
    }

    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }

    if (scale < 0)
    {
        cerr << "Scale (" << to_string(scale).c_str() << ") Cant Be Negative!!!" << endl;
        scale = 0;
    }
    do
    {
        cout<<"Larger scale of (" << to_string(scale).c_str() << ") will take more time to compute. \nPress \"Y/y\" to change"<< endl;
        cin >> decision;
    }
    while(  toupper(decision) !='Y' && toupper(decision) !='N');
    if(toupper(decision) == 'Y')
    {
        cout<<"Set a new scale : ";
        cin>>scale;
        BigDecimal::setscale(scale);
    }
    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number(lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\"" << lhs.c_str() << "\" Is Not A Number" << endl;
        return "0";
    }
    string old_sum = ZERO;
    string eq = BigDecimal::divide((BigDecimal::subtract(lhs,ONE)),(BigDecimal::add(lhs,ONE)));
    string eq_2 = BigDecimal::multiply(eq,eq);
    string denom = ONE;
    string frac = eq;
    string term = frac;                 // denom start from 1.0
    string new_sum = term;

    while ( new_sum != old_sum )
    {
        old_sum = new_sum;
        denom = BigDecimal::add(denom,TWO);
        frac = BigDecimal::multiply(frac,eq_2);
        new_sum = BigDecimal::add(new_sum,BigDecimal::divide(frac,denom));
        cout<<old_sum<<"\n";
        cout<<new_sum<<"\n";
    }
    return BigDecimal::multiply(TWO,new_sum);

}
string BigDecimal::log2 (const string &lhs, int scale)
{
    if (lhs.empty())
    {
        return BigDecimal::round (ZERO, scale);
    }

    if (scale == SHRT_MAX)
    {
        scale = _scale;
    }
    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return "0";
    }
    if(lsign < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Cannot Be A Negative Number"<< endl;
        return "0";
    }
    return (BigDecimal::compareTo(lhs,ONE) > 0)? string(BigDecimal::add(ONE,BigDecimal::log(BigDecimal::divide(lhs,TEN)))):ZERO;
}

string fact(string a)
{
    string i("1");
    string fact("1");
    while(BigDecimal::compareTo(i,a) <= 0)
    {
        fact=BigDecimal::multiply(fact,to_string(i));
        i = BigDecimal::add(i,ONE);
    }
    return fact;
}

//string BigDecimal::sin(const string &lhs, int scale)
//{
//	int i,j;
//	string sum_sin("0");
//	for(i=0,j=0;i<20;i=i+2,j++)
// 	{
// 		string p = BigDecimal::divide(BigDecimal::pow(lhs,to_string(i+1)),fact(i+1));
// 		string q = BigDecimal::multiply(power(-1,j),p);
//		sum_sin = BigDecimal::add(sum_sin,q);
// 		//sum_sin+=pow(-1,j)*pow(num,i+1)/f(i+1);
//	}
//	return sum_sin;
//}
string BigDecimal::sin(const string &lhs, int scale)
{
    string sum("0"),n,d,t,i("0"),j("0");
    while(BigDecimal::compareTo(i,lhs) <= 0)
    {
        if(BigDecimal::modulus(i,"2") != "0")
        {
            j = BigDecimal::add(j,ONE);
            n = BigDecimal::pow(lhs,i);
            d = fact(i);
            t = BigDecimal::divide(n,d);
            if(BigDecimal::modulus(j,"2") != "0")
            {
                sum = BigDecimal::subtract(sum,t);
            }
            else
            {
                sum = BigDecimal::add(sum,t);
            }
        }
        i = BigDecimal::add(i,ONE);
    }
    return sum;
}

string BigDecimal::log (const string &lhs, int scale)
{
    if (lhs.empty())
    {
        return round (ZERO, scale);
    }

    int lsign, lint, ldot, lfrac, lscale;
    if (parse_number (lhs, lsign, lint, ldot, lfrac, lscale) < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Is Not A Number"<< endl;
        return _zero(scale);
    }
    if(lsign < 0)
    {
        cerr << "\""<<lhs.c_str()<<"\" Cannot Be A Negative Number"<< endl;
        return _zero(scale);
    }
    return string(BigDecimal::divide(BigDecimal::ln(lhs,0),BigDecimal::ln(to_string("10"),0),0));
}

string BigDecimal::stringToHex(string &lhs,int caps)
{
    long int i = 1;
    int temp;
    lhs = getLeftOfDot(lhs);
    string quotient = lhs,hexoutput("");
    temp = atoi(modulus(quotient,to_string("16"),0).c_str());
//	while(compareTo(quotient,ZERO,0) != 0)
//	{
//		temp = atoi(modulus(quotient,to_string("16"),0).c_str());
//		if(temp < 10)
//			temp = temp + 48;
//		else
//			temp = temp + 55;
//		hexoutput[i++] = temp;
//		quotient = divide(quotient,to_string("16"),0);
//	}
    cout<<temp;
    return hexoutput;

}
