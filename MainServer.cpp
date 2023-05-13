/*
	UDP Server
*/

#include <stdio.h>
#include "UDPServer.h"
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <string>
#include <cmath>
#include <time.h>
#include <vector>
#include <cctype>

#ifndef _WIN32
using SOCKET = int
#define WSAGetLastError() 1
#else
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

//#define SERVER "127.0.0.1"	//ip address of udp server
#define BUFLEN 	1024		    //Max length of buffer
#define PORT    8888			//The port on which to listen for incoming data

void getTwoRandomPrimes(int& prime1, int& prime2)
{
    int primes[70] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
                      31, 37, 41, 43, 47, 53, 59, 61, 67,
                      71, 73, 79, 83, 89, 97, 101, 103,
                      107, 109, 113, 127, 131, 137, 139,
                      149, 151, 157, 163, 167, 173, 179,
                      181, 191, 193, 197, 199, 211, 223,
                      227, 229, 233, 239, 241, 251, 257,
                      263, 269, 271, 277, 281, 283, 293,
                      307, 311, 313, 317, 331, 337, 347, 349 };

    srand(time(0));
    int i1, i2;
    i1 = rand() % 70;
    do {
        i2 = rand() % 70;
    } while (i1 == i2);

    prime1 = primes[i1];
    prime2 = primes[i2];

    std::cout << "prime1 = " << prime1 << std::endl;
    std::cout << "prime2 = " << prime2 << std::endl;
}

int GCD(int a, int b)
{
    int temp;

    while (b)
    {
        temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

void generateKeys(int& n, int& e, int& d)
{
    int prime1 = 11, prime2 = 13;
    int phi;

    n = prime1 * prime2;
    phi = (prime1 - 1) * (prime2 - 1);

    e = 2;
    while (GCD(e, phi) != 1)
        ++e;

    // d = (k*Φ(n) + 1) / e for some integer k
    d = 2;
    while ((d * e) % phi != 1)
        ++d;

    //std::cout << "n = " << n << std::endl;
    //std::cout << "e = " << e << std::endl;
    //std::cout << "d = " << d << std::endl;
}

long encryptChar(char ch, int n, int e)
{
    int c = (int)ch;
    long encryptedChar = 1;
    while (e--)
    {
        encryptedChar *= c;
        encryptedChar %= n;
    }
    return encryptedChar;
}

char decryptChar(long c, int n, int d)
{
    long decryptedChar = 1;
    while (d--)
    {
        decryptedChar *= c;
        decryptedChar %= n;
    }
    return (char)decryptedChar;
}

std::vector<long> encrypt(std::string message, int n, int e)
{
    std::vector<long> result;
    for (char& ch : message)
        result.push_back(encryptChar(ch, n, e));
    return result;
}

std::string decrypt(std::vector<long> encryptedMessage, int n, int d)
{
    std::string result;
    for (long& num : encryptedMessage)
        result += decryptChar(num, n, d);
    return result;
}

std::string vectorToString(std::vector<long> vec)
{
    std::string result;
    for (long& num : vec)
        result += std::to_string(num) += " ";
    return result;
}

std::vector<long> stringToVector(std::string message)
{
    std::vector<long> vec;

    int i = 0;
    while (message[i] != '\0')
    {
        vec.push_back(atol(&message[i]));
        while (message[i] != '\0' && isdigit(message[i]))
            ++i;
        while (message[i] != '\0' && isblank(message[i]))
            ++i;
    }
    return vec;
}

std::string caesarEncrypt(std::string message, int key)
{
    for (char& ch : message)
    {
        ch += key;
        if (ch > '9')
            ch = ch - '9' + '0' - 1;
    }
    return message;
}

std::string caesarDecrypt(std::string message, int key)
{
    for (char& ch : message)
    {
        ch -= key;
        if (ch < '0')
            ch = ch + '9' - '0' + 1;
    }
    return message;
}

int main(int argc, char* argv[])
{
	struct sockaddr_in si_other;
	//struct sockaddr_in si_other1;
	unsigned short srvport;
	int slen;
	char buf[BUFLEN];
	char msg[BUFLEN];
	int i, k = 0;

	std::string decod_string, ncode_string;

	srvport = (1 == argc) ? PORT : atoi(argv[1]);

	UDPServer server(srvport);
	slen = sizeof(si_other);

	while (1)
	{
		memset(msg, '\0', BUFLEN);
		memset(buf, '\0', BUFLEN);

		printf("Waiting for data...   ");
		server.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
		std::string ncode_string(buf);

        int n, e, d;
        generateKeys(n, e, d);

        std::string key = std::to_string(n);
        //std::cout << "\nThe key:\n" << key << std::endl;
        std::string encryptedKey = caesarEncrypt(key, 4);
        //std::cout << "\nEncrypted key:\n" << encryptedKey << std::endl;
        std::string decryptedKey = caesarDecrypt(encryptedKey, 4);
        //std::cout << "\nDecrypted key:\n" << decryptedKey << std::endl;

        std::vector<long> nCode = stringToVector(ncode_string);
        ncode_string = decrypt(nCode, n, d);
		strcpy_s(buf, ncode_string.c_str());
		printf("%s ", buf);
		memset(buf, '\0', BUFLEN);

		printf("\nAnswer : ");
		gets_s(msg, BUFLEN);
		std::string decod_string(msg);
        std::vector<long> encryptedMessage = encrypt(decod_string, n, e);
        decod_string = vectorToString(encryptedMessage);
		strcpy_s(msg, decod_string.c_str());
		server.SendDatagram(msg, (int)strlen(msg), (struct sockaddr*)&si_other, slen);
		memset(msg, '\0', BUFLEN);
	}
	return 0;
}
