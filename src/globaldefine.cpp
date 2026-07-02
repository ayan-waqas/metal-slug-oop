#include "project.h"

int screen_x = 1920;
int screen_y = 1080;
int cell_size = 64;
int height = 14;
int width = 110;
float gravity = 1.0f;
RenderWindow* window = nullptr;
float terminal_Velocity = 15.0f;
float deltaTime = 1.0f / 60.0f;
Font hudFont;
Font scoreFont;




MyString MyString::fromInt(int n) {
    char buffer[16];
    int idx = 0;
    bool negative = false;

    if (n < 0)
    {
        negative = true;
        n = -n;
    }

    if (n == 0)
    {
        buffer[idx] = '0';
        idx += 1;
    }
    else
    {
        char temp[16];
        int t = 0;
        while (n > 0)
        {
            temp[t] = '0' + (n % 10);
            t += 1;
            n /= 10;
        }
        while (t > 0)
        {
            t -= 1;
            buffer[idx] = temp[t];
            idx += 1;
        }
    }
    buffer[idx] = '\0';

    if (negative)
    {
        MyString result = "-";
        result += buffer;
        return result;
    }
    return MyString(buffer);
}

MyString MyString::fromFloat(float f, int decimals) {
    if (decimals < 0)
    {
        decimals = 0;
    }
    if (decimals > 6)
    {
        decimals = 6;
    }

    bool negative = false;
    if (f < 0.0f)
    {
        negative = true;
        f = -f;
    }

    int intPart = (int)f;
    float fracPart = f - (float)intPart;

    int multiplier = 1;
    for (int i = 0; i < decimals; i++)
    {
        multiplier *= 10;
    }
    int fracInt = (int)(fracPart * multiplier + 0.5f);

    if (fracInt >= multiplier)
    {
        intPart += 1;
        fracInt = 0;
    }

    MyString result;
    if (negative)
    {
        result = "-";
    }
    result += MyString::fromInt(intPart);

    if (decimals > 0)
    {
        result += ".";
        char fracBuf[16];
        int idx = 0;
        char temp[16];
        int t = 0;
        int fi = fracInt;
        while (t < decimals)
        {
            temp[t] = '0' + (fi % 10);
            t += 1;
            fi /= 10;
        }
        while (t > 0)
        {
            t -= 1;
            fracBuf[idx] = temp[t];
            idx += 1;
        }
        fracBuf[idx] = '\0';
        result += fracBuf;
    }

    return result;
}

int myStrLen(const char* str) {

    int count = 0;
    if (str == nullptr)
    {
        return 0;
    }
    while (*str != '\0')
    {
        count += 1;
        str += 1;
    }
    return count;
}

MyString::MyString() {
    len = 0;
    data = new char[1];
    data[0] = '\0';
}

MyString::MyString(const char* str) {
    if (str == nullptr)
    {
        len = 0;
        data = new char[1];
        data[0] = '\0';
    }
    else
    {
        len = myStrLen(str);
        data = new char[len + 1];
        for (int i = 0; i < len; i++)
        {
            data[i] = str[i];
        }
        data[len] = '\0';
    }
}

MyString::MyString(const MyString& other) {
    len = other.len;
    data = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        data[i] = other.data[i];
    }
    data[len] = '\0';
}

MyString::~MyString() {
    delete[] data;
}

MyString& MyString::operator=(const MyString& other) {
    if (this == &other)
    {
        return *this;
    }
    delete[] data;
    len = other.len;
    data = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        data[i] = other.data[i];
    }
    data[len] = '\0';
    return *this;
}

MyString& MyString::operator=(const char* str) {
    delete[] data;
    if (str == nullptr)
    {
        len = 0;
        data = new char[1];
        data[0] = '\0';
    }
    else
    {
        len = myStrLen(str);
        data = new char[len + 1];
        for (int i = 0; i < len; i++)
        {
            data[i] = str[i];
        }
        data[len] = '\0';
    }
    return *this;
}

MyString MyString::operator+(const MyString& other) const {
    MyString result;
    delete[] result.data;
    result.len = len + other.len;
    result.data = new char[result.len + 1];

    for (int i = 0; i < len; i++)
    {
        result.data[i] = data[i];
    }
    for (int i = 0; i < other.len; i++)
    {
        result.data[len + i] = other.data[i];
    }
    result.data[result.len] = '\0';
    return result;
}

MyString MyString::operator+(const char* str) const {
    MyString temp(str);
    return *this + temp;
}

MyString& MyString::operator+=(const MyString& other) {
    int newLen = len + other.len;
    char* newData = new char[newLen + 1];

    for (int i = 0; i < len; i++)
    {
        newData[i] = data[i];
    }
    for (int i = 0; i < other.len; i++)
    {
        newData[len + i] = other.data[i];
    }
    newData[newLen] = '\0';

    delete[] data;
    data = newData;
    len = newLen;
    return *this;
}

MyString& MyString::operator+=(const char* str) {
    MyString temp(str);
    *this += temp;
    return *this;
}

MyString& MyString::operator+=(char c) {
    char* newData = new char[len + 2];
    for (int i = 0; i < len; i++)
    {
        newData[i] = data[i];
    }
    newData[len] = c;
    newData[len + 1] = '\0';

    delete[] data;
    data = newData;
    len += 1;
    return *this;
}

bool MyString::operator==(const MyString& other) const {
    if (len != other.len)
    {
        return false;
    }
    for (int i = 0; i < len; i++)
    {
        if (data[i] != other.data[i])
        {
            return false;
        }
    }
    return true;
}

bool MyString::operator==(const char* str) const {
    if (str == nullptr)
    {
        return false;
    }
    int otherLen = myStrLen(str);
    if (len != otherLen)
    {
        return false;
    }
    for (int i = 0; i < len; i++)
    {
        if (data[i] != str[i])
        {
            return false;
        }
    }
    return true;
}

bool MyString::operator!=(const MyString& other) const {
    return !(*this == other);
}

bool MyString::operator!=(const char* str) const {
    return !(*this == str);
}

char& MyString::operator[](int index) {
    if (index < 0 || index >= len)
    {
        return data[0];
    }
    return data[index];
}

char MyString::operator[](int index) const {
    if (index < 0 || index >= len)
    {
        return '\0';
    }
    return data[index];
}

int MyString::length() const {
    return len;
}

bool MyString::isEmpty() const {
    return len == 0;
}

const char* MyString::c_str() const {
    return data;
}

void MyString::clear() {
    delete[] data;
    len = 0;
    data = new char[1];
    data[0] = '\0';
}

void MyString::toUpper() {
    for (int i = 0; i < len; i++)
    {
        if (data[i] >= 97 && data[i] <= 122)
        {
            data[i] -= 32;
        }
    }
}

void MyString::toLower() {
    for (int i = 0; i < len; i++)
    {
        if (data[i] >= 65 && data[i] <= 90)
        {
            data[i] += 32;
        }
    }
}

int MyString::toInt() const {
    if (len <= 0)
    {
        return 0;
    }

    int result = 0;
    int sign = 1;
    int start = 0;

    if (data[0] == '-')
    {
        sign = -1;
        start = 1;
    }
    else if (data[0] == '+')
    {
        start = 1;
    }

    for (int i = start; i < len; i++)
    {
        if (data[i] >= '0' && data[i] <= '9')
        {
            result = result * 10 + (data[i] - '0');
        }
        else
        {
            break;
        }
    }
    return result * sign;
}

float MyString::toFloat() const {
    if (len <= 0)
    {
        return 0.0f;
    }

    int sign = 1;
    int start = 0;
    if (data[0] == '-')
    {
        sign = -1;
        start = 1;
    }
    else if (data[0] == '+')
    {
        start = 1;
    }

    int dotPos = -1;
    for (int i = start; i < len; i++)
    {
        if (data[i] == '.')
        {
            dotPos = i;
            break;
        }
    }

    int intPart = 0;
    int intEnd = (dotPos == -1) ? len : dotPos;
    for (int i = start; i < intEnd; i++)
    {
        if (data[i] >= '0' && data[i] <= '9')
        {
            intPart = intPart * 10 + (data[i] - '0');
        }
    }

    float decPart = 0.0f;
    if (dotPos != -1)
    {
        float divisor = 10.0f;
        for (int i = dotPos + 1; i < len; i++)
        {
            if (data[i] >= '0' && data[i] <= '9')
            {
                decPart += (data[i] - '0') / divisor;
                divisor *= 10.0f;
            }
            else
            {
                break;
            }
        }
    }

    return sign * ((float)intPart + decPart);
}

MyString MyString::substring(int start, int count) const {
    if (start < 0 || start >= len || count <= 0)
    {
        return MyString();
    }
    if (start + count > len)
    {
        count = len - start;
    }

    char* buffer = new char[count + 1];
    for (int i = 0; i < count; i++)
    {
        buffer[i] = data[start + i];
    }
    buffer[count] = '\0';

    MyString result(buffer);
    delete[] buffer;
    return result;
}

int MyString::indexOf(char c) const {
    for (int i = 0; i < len; i++)
    {
        if (data[i] == c)
        {
            return i;
        }
    }
    return -1;
}