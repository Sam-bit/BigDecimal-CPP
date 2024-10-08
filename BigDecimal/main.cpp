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
#include <iostream>
#include "BigDecimal.h"
#include <stdio.h>

using namespace std;
int main(int argc, char** argv) {
	BigDecimal C;
	BigDecimal A("64345443243223456776535253647568768574635423567465564242343231");
	BigDecimal B("14234324257568698789685764354657468576758746352435675486597695857463343243");
	cout<<"A = "<<A.toString()<<endl;
	cout<<"B = "<<B.toString()<<endl;
	C = A + B;
	cout << "A + B = " << C.toString()<<endl;
	C = A - B;
	cout << "A - B = " << C.toString()<<endl;
	C = A * B;
	cout << "A * B = " << C.toString()<<endl;
	C = A / B;
	cout << "A / B = " << C.toString()<<endl;
	return 0;
}

