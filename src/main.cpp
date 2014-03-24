//
//  main.cpp
//  FinanceDashboard
//
//  Created by Yinxia on 24/03/14.
//  Copyright (c) 2014 Yinxia. All rights reserved.
//

#include <iostream>
#include "quandl.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";
    quandl qq;
    qq.auth("FJiuZvG62tiyYqvjxTs4");
    qq.get("NSE/OIL");
    
    return 0;
}

