/*
 * quandl.h
 *
 *  Created on: 30/09/2013
 *      Author: Zhiwei Fu
 */

/* This programme is free software. It is developed by Dr Zhiwei Fu as a product
 contributing to quandl.com.
 
 This programme is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY, without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef QUANDL_H_
#define QUANDL_H_
#include <iostream>
#include <fstream> //for perror()
#include <string> // string for c++
#include <string.h> // for memset()/bzero() For C
#include <sys/socket.h>
#include <netinet/in.h> // for sockaddr_in, hotons
#include <arpa/inet.h> // for inet_addr
#include <netdb.h> //for gethostbyname(), gethostbyaddr()
#include <netinet/tcp.h> // for TCP_NODELAY
#include <time.h> // time.h

using namespace std;

// This void function "download" is to download a file from internet.
// It has been defined in the same file.
void download(string, string, string);

class quandl {
	string AuthCode;
public:
    quandl();
    virtual ~quandl();
    // To get the authentication token.
    void auth(string);
    
    //To download file from the website defined by the first
    //argument.
    void get(string);
    //		void get(string, string);
    //		void get(string, string, string);
    //		void get(string, strinauthenticationg, string, string);
    //		void get(string, string, string, string, string);
    //		void get(string, string, string, string, string, string);
    //		void get(string, string, string, string, string, string,
    //			string);
    // All parameters are prescribed by users.
    // 1. Quandl code;
    // 2. Ascending/descending order;
    // 3. Start date;
    // 4. End date;
    // 5. Transformation;
    // 6. collapse;
    // 7. Rows;
    // 8. Output type
    void get(string, string, string, string, string, string, string,
             string);
};

quandl::quandl() {
	// TODO Auto-generated constructor stub
}

quandl::~quandl() {
	// TODO Auto-generated destructor stub
}

// To store the token in "AuthCode", which is a public variable in the class.
void quandl::auth(string code){
	AuthCode = code;
}

// To determine the website address by the token stored in "code"
// and call the function "download"
void quandl::get(string code){
	//Set optional variables by default
	string order = "asc";
	string type = "csv";
    
	string website = "http://www.quandl.com/api/v1/datasets/" + code
    + "." + type + "?sort_order=" + order;
	if(AuthCode.length() == 0){
		cout << "It would appear you are\'nt using an authentication"
        << " token. Please visit http://www.quandl.com/help/c++"
        << " or your usage may be limited.\n";
	}
	else{
		website += "&auth_token=" + AuthCode;
	}
    
	string FileName;
	size_t iLength = code.length();
	for (int i=0; i < iLength; i++){
		if(code.substr(i, 1) == "/"){
			FileName = code.substr(i+1, iLength - i -1);
			break;
		}
	}
    cout<<"Downloading data of "<< code <<"."<<type<<endl;
    cout<<"From "<<website<<endl;
    cout<<"To "<<FileName<<endl;
    	download(website, FileName, type);
	return;
}

// There are 7 optional arguments compared to the one above.
void quandl::get(string code, string order, string StartDate, string EndDate,
                 string transformation, string collapse, string rows, string type){
    
	string website = "http://www.quandl.com/api/v1/datasets/" + code
    + "." + type + "?sort_order=" + order;
	if(AuthCode.length() == 0){
		cout << "It would appear you are\'nt using an authentication"
        << " token. Please visit http://www.quandl.com/help/c++"
        << " or your usage may be limited.\n";
	}
	else{
		website += "&auth_token=" + AuthCode;
	}
    
	website += "&trim_start=" + StartDate;
	website += "&trim_end=" + EndDate;
	website += "&transformation=" + transformation;
	website += "&collapse=" + collapse;
	website += "&rows=" + rows;
    
	string FileName;
	size_t iLength = code.length();
	for (int i=0; i < iLength; i++){
		if(code.substr(i, 1) == "/"){
			FileName = code.substr(i+1, iLength - i -1);
			break;
		}
	}
    
	download(website, FileName, type);
}

// The "download" function is developed with a general purpose of downloading
// files from internet. It is based on the socket programming.
void download(string website, string FileName, string FileType){
	struct sockaddr_in ServAddr;
	struct hostent *ServInf;
	int sockfd;
	size_t iMessage = 1048576; //1 MB for the buff storage
	char message[iMessage];
	string request = "GET ";
	ofstream fid;
	size_t iStart, iEnd, iLength;
    
	//Get a socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("   *** Error when calling socket()! ***\n");
		return;
	}
    
	//To define the server's address.
	memset(&ServAddr, 0, sizeof(ServAddr));
	string host;
	iLength = website.length();
	iStart = 0; iEnd = iLength - 1;
	if(website.substr(0,8) == "https://") iStart = 8;
	else if(website.substr(0,7) == "http://") iStart = 7;
	for(unsigned long i = iStart; i<iLength; i++){
        if(website.substr(i, 1)=="/"){
            iEnd = i - 1;
            break;}
	}
	host = website.substr(iStart, iEnd - iStart + 1);
	ServInf = gethostbyname(host.c_str());
	if(ServInf == NULL){
		perror("   *** The hostname is not recognized! ***\n");
		return;
	}
	memcpy((char *)&ServAddr.sin_addr.s_addr, (char *)ServInf->h_addr,
	       ServInf->h_length);
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(80);
    
	// To set the socket with options.
	int BDP =100*1024;
	int ret = 0;
    /*	//int flags = fcntl(sockfd, F_GETFL, 0);
     //ret = fcntl( sockfd, F_SETFL, O_NONBLOCK); //must MSG_DONTWAIT,
     //     &~O_NONBLOCK
     if (ret != 0) {
     printf("Couldn't setsockopt(O_NONBLOCK)\n");
     return; // ?exit(-1);
     }
     //setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTime,
     //sizeof(iTime)); //work for non-blocking only
     //int nZero=0;
     //setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&nZero,
     //sizeof(nZero));
     */
	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&BDP,
                     sizeof(BDP));
    cout<<"DEBUG: ret ="<<ret<<endl;
	if (ret == -1) {
        printf("   *** Couldn't setsockopt(SO_RCVBUF)! ***\n");
        return;
	}
	int on = 1;
	ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
	if (ret == -1) {
        printf("   *** Couldn't setsockopt(TCP_NODELAY)! ***\n");
        return;
	}
    cout<<"DEBUG: ret ="<<ret<<endl;

	// To connect to the server.
	ret = connect(sockfd, (struct sockaddr *)&ServAddr, sizeof(ServAddr));
	if(ret == -1){
		perror("*** Connection fails! ***\n");
		return;
	}
    cout<<"DEBUG: ret ="<<ret<<endl;
	// To send the request.
	request += website + " \r\n";
	//write(sockfd,request.c_str(),request.length());
	send(sockfd, request.c_str(), request.length(), 0);
    
	// To initiate the file.
    cout<<"DEBUG: openning file "<<(FileName + "." + FileType).c_str()<<endl;
	fid.open((FileName + "." + FileType).c_str());
	if(fid.fail()){
        cerr << "*** Error when openning file! ***\n" << endl;
        return;
    }
	// To read dataflow for the file.
	memset(&message, 0, iMessage);
	ssize_t iRecv = 1; iStart = 1;
	string TempStr;
	while (iRecv != 0 && iRecv != -1){
		iRecv = recv(sockfd, message, iMessage, MSG_WAITALL);
		fid.write(message, iRecv);
	}
    
    // To close the downloaded file
	fid.close();
	if(iRecv == -1){
		perror("*** Error when calling recv()! ***\n");
		return;
	}
	shutdown(sockfd, 0);
}

#endif /* QUANDL_H_ */
