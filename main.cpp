#include <iostream>
#include "BigDecimal.h"
#include <stdio.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
using namespace std;
int main(int argc, char** argv) {
	BigDecimal C;
	cout<<"Enter A : ";
	cin>>str1;
	cout<<"Enter B : ";
	cin>>str2;
	BigDecimal A("6556465465565466545675435678976543567897654345678907654345678957656578654676565656565656565656565656565656565656565656");
	BigDecimal B("123456789987654321234567890987654322221234567888888876543234566666666666666234567543266666666666666666666666666666666");
	C = A * B;
	cout<<"\nC = "<<C.toString();
}
