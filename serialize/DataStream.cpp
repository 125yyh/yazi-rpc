#include "DataStream.h"
using namespace yazi::serialize;

DataStream::DataStream() : m_pos(0)
{
    m_byteorder = byteorder();
}

DataStream::DataStream(const string & str) : m_pos(0)
{
    m_byteorder = byteorder();
    m_buf.clear();
    reserve(str.size());
    write(str.data(), str.size());
}

DataStream::~DataStream()
{
}

void DataStream::reserve(int len)
{
    int size = m_buf.size();
    int cap = m_buf.capacity();
    if (size + len > cap)
    {
        while (size + len > cap)
        {
            if (cap == 0)
            {
                cap = 1;
            }
            else
            {
                cap *= 2;
            }
        }
        m_buf.reserve(cap);
    }
}

DataStream::ByteOrder DataStream::byteorder()
{
    int n = 0x12345678;
    char str[4];
    memcpy(str, &n, sizeof(int));
    if (str[0] == 0x12)
    {
        return ByteOrder::BigEndian;
    }
    else
    {
        return ByteOrder::LittleEndian;
    }
}

void DataStream::show() const
{
    int size = m_buf.size();
    std::cout << "data size = " << size << std::endl;
    int i = 0;
    while (i < size)
    {
        switch ((DataType)m_buf[i])
        {
        case DataType::BOOL:
            if ((int)m_buf[++i] == 0)
            {
                std::cout << "false";
            }
            else
            {
                std::cout << "true";
            }
            ++i;
            break;
        case DataType::CHAR:
            std::cout << m_buf[++i];
            ++i;
            break;
        case DataType::INT32:
            std::cout << *((int32_t *)(&m_buf[++i]));
            i += 4;
            break;
        case DataType::INT64:
            std::cout << *((int64_t *)(&m_buf[++i]));
            i += 8;
            break;
        case DataType::FLOAT:
            std::cout << *((float *)(&m_buf[++i]));
            i += 4;
            break;
        case DataType::DOUBLE:
            std::cout << *((double *)(&m_buf[++i]));
            i += 8;
            break;
        case DataType::STRING:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
                std::cout << string(&m_buf[i], len);
                i += len;
            }
            else
            {
                throw std::logic_error("parse string error");
            }
            break;
        case DataType::VECTOR:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse vector error");
            }
            break;
        case DataType::MAP:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse map error");
            }
            break;
        case DataType::SET:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse set error");
            }
            break;
        case DataType::CUSTOM:
            break;
        default:
            break;
        }
    }
    std::cout << std::endl;
}

void DataStream::write(const char * data, int len)
{
    reserve(len);
    int size = m_buf.size();
    m_buf.resize(m_buf.size() + len);
    std::memcpy(&m_buf[size], data, len);
}

void DataStream::write(bool value)
{
    char type = DataType::BOOL;
    write((char *)&type, sizeof(char));
    write((char *)&value, sizeof(char));
}

void DataStream::write(char value)
{
    char type = DataType::CHAR;
    write((char *)&type, sizeof(char));
    write((char *)&value, sizeof(char));
}

void DataStream::write(int32_t value)
{
    char type = DataType::INT32;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int32_t);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(int32_t));
}

void DataStream::write(int64_t value)
{
    char type = DataType::INT64;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int64_t);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(int64_t));
}

void DataStream::write(float value)
{
    char type = DataType::FLOAT;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(float);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(float));
}

void DataStream::write(double value)
{
    char type = DataType::DOUBLE;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(double);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(double));
}

void DataStream::write(const char * value)
{
    char type = DataType::STRING;
    write((char *)&type, sizeof(char));
    int len = strlen(value);
    write(len);
    write(value, len);
}

void DataStream::write(const string & value)
{
    char type = DataType::STRING;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    write(value.data(), len);
}

void DataStream::write(const Serializable & value)
{
    value.serialize(*this);
}


void DataStream::write_args()
{
}

bool DataStream::read(char * data, int len)
{
    std::memcpy(data, (char *)&m_buf[m_pos], len);
    m_pos += len;
    return true;
}

bool DataStream::read(bool & value)
{
    if (m_buf[m_pos] != DataType::BOOL)
    {
        return false;
    }
    ++m_pos;
    value = m_buf[m_pos];
    ++m_pos;
    return true;
}

bool DataStream::read(char & value)
{
    if (m_buf[m_pos] != DataType::CHAR)
    {
        return false;
    }
    ++m_pos;
    value = m_buf[m_pos];
    ++m_pos;
    return true;
}

bool DataStream::read(int32_t & value)
{
    if (m_buf[m_pos] != DataType::INT32)
    {
        return false;
    }
    ++m_pos;
    value = *((int32_t *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int32_t);
        std::reverse(first, last);
    }
    m_pos += 4;
    return true;
}

bool DataStream::read(int64_t & value)
{
    if (m_buf[m_pos] != DataType::INT64)
    {
        return false;
    }
    ++m_pos;
    value = *((int64_t *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int64_t);
        std::reverse(first, last);
    }
    m_pos += 8;
    return true;
}

bool DataStream::read(float & value)
{
    if (m_buf[m_pos] != DataType::FLOAT)
    {
        return false;
    }
    ++m_pos;
    value = *((float *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(float);
        std::reverse(first, last);
    }
    m_pos += 4;
    return true;
}

bool DataStream::read(double & value)
{
    if (m_buf[m_pos] != DataType::DOUBLE)
    {
        return false;
    }
    ++m_pos;
    value = *((double *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(double);
        std::reverse(first, last);
    }
    m_pos += 8;
    return true;
}

bool DataStream::read(string & value)
{
    if (m_buf[m_pos] != DataType::STRING)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    if (len < 0)
    {
        return false;
    }
    value.assign((char *)&(m_buf[m_pos]), len);
    m_pos += len;
    return true;
}

bool DataStream::read(Serializable & value)
{
    return value.unserialize(*this);
}


bool DataStream::read_args()
{
    return true;
}

const char * DataStream::data() const
{
    return m_buf.data();
}

int DataStream::size() const
{
    return m_buf.size();
}

void DataStream::clear()
{
    m_buf.clear();
}

void DataStream::reset()
{
    m_pos = 0;
}

void DataStream::save(const string & filename)
{
    ofstream fout(filename);
    fout.write(data(), size());
    fout.flush();
    fout.close();
}

void DataStream::load(const string & filename)
{
    ifstream fin(filename);
    stringstream ss;
    ss << fin.rdbuf();
    const string & str = ss.str();
    m_buf.clear();
    reserve(str.size());
    write(str.data(), str.size());
}

DataStream & DataStream::operator << (bool value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (char value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (int32_t value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (int64_t value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (float value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (double value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const char * value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const string & value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const Serializable & value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator >> (bool & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (char & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (int32_t & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (int64_t & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (float & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (double & value)
{
    read(value);
    return *this; 
}

DataStream & DataStream::operator >> (string & value)
{
    read(value);
    return *this; 
}

DataStream & DataStream::operator >> (Serializable & value)
{
    read(value);
    return *this;
}