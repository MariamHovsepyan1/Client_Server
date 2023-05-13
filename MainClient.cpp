 /*
	UDP Client routine
*/

#include "UDPSocket.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <string>
#include <cmath>
#include <time.h>
#include <vector>
#include <cctype>

//using namespace std;
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
	//	SOCKET s;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);

	//struct sockaddr_in si_other1;
	//int slen1 = sizeof(si_other1);

	char buf[BUFLEN];
	char message[BUFLEN];

	std::string decod_string, ncode_string;

	unsigned short srv_port = 0;
	char srv_ip_addr[40];
	memset(srv_ip_addr, 0, 40);


	//create socket
	UDPSocket client_sock;

	//setup address structure
	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;

	if (1 == argc)
	{
		si_other.sin_port = htons(PORT);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
		printf("1: Server - addr=%s , port=%d\n", SERVER, PORT);
	}
	else if (2 == argc)
	{
		si_other.sin_port = htons(atoi(argv[1]));
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
		printf("2: argv[0]: Server - addr=%s , port=%d\n", SERVER, atoi(argv[1]));
	}
	else
	{
		si_other.sin_port = htons(atoi(argv[2]));
		si_other.sin_addr.S_un.S_addr = inet_addr(argv[1]);
		printf("3: Server - addr=%s , port=%d\n", argv[1], atoi(argv[2]));
	}

	while (1)
	{
		memset(buf, '\0', BUFLEN);
		memset(message, '\0', BUFLEN);

		printf("Enter message: ");
		gets_s(message, BUFLEN);
		std::string decod_string(message);

		int n, e, d;
		generateKeys(n, e, d);

		std::string key = std::to_string(n);
		std::string encryptedKey = caesarEncrypt(key, 4);
		std::string decryptedKey = caesarDecrypt(encryptedKey, 4);
		std::vector<long> encryptedMessage = encrypt(decod_string, n, e);
        decod_string = vectorToString(encryptedMessage);
		strcpy_s(message, decod_string.c_str());
		client_sock.SendDatagram(message, (int)strlen(message), (struct sockaddr*)&si_other, slen);
		memset(message, '\0', BUFLEN);

		client_sock.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
		std::string ncode_string(buf);
        std::vector<long> nCode = stringToVector(ncode_string);
        ncode_string = decrypt(nCode, n, d);
		strcpy_s(buf, ncode_string.c_str());
		printf("Answer: %s\n", buf);
		memset(buf, '\0', BUFLEN);
	}
	return 0;
}

